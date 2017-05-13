#include "cityLocalScene.h"
#include "TriangleMesh/cannyImage.h"
#include "TriangleMesh/Mesh.h"
double cityLocalScene::disP2P(Pot p, Pot q)
{
	double res = (p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y) + (p.z - q.z)*(p.z - q.z);
	return sqrt(res);
}
//steps
int countFeaturePoint(int steps, int localRow, int localCol, vector<vector<int> >& cannyPoint)
{
	int totalVer = 0;//ͳ�Ƹ����������������
	if (localRow < 10 || localCol < 10)
	{
		steps = 2;
	}
	int stepRow = localRow / steps;
	int stepCol = localCol / steps;

	for (int n = 0; n < localCol; n++)
	{
		for (int m = localRow - 1; m >= 0; m--)
		{
			if (cannyPoint[m][n] != 0)
			{
				totalVer++;
			}
		}
	}

	//�ټ�����ϵĵ� �Ҳ������������غ�
	for (int i = 0; i <= steps; i++)
	{
		for (int j = 0; j <= steps; j++)
		{
			int m = i*stepRow;
			int n = j*stepCol;
			if (m >= localRow)m = localRow - 1;
			if (n >= localCol)n = localCol - 1;
			if (cannyPoint[m][n] == 0)
			{
				totalVer++;
			}
		}
	}
	return totalVer;
}

Pot cityLocalScene::get_Normal(Pot p1, Pot p2, Pot p3)
{
	double x1 = p2.x - p1.x;
	double y1 = p2.y - p1.y;
	double z1 = p2.z - p1.z;

	double x2 = p3.x - p2.x;
	double y2 = p3.y - p2.y;
	double z2 = p3.z - p2.z;

	double a = y1*z2 - y2*z1;
	double b = x2*z1 - x1*z2;
	double c = x1*y2 - x2*y1;

	double radix = sqrt(a*a + b*b + c*c);

	return Pot(a / radix, b / radix, c / radix);

}


void cityLocalScene::loadLocalBuilding(Vector3d  AP_position, double  LocalRange, cityScene* cityAll)
{
	//�ֲ�����ķ�Χ MinPos��MaxPos
	MinPos = AP_position - Vector3d(LocalRange / 2, LocalRange / 2, 0);
	MaxPos = AP_position + Vector3d(LocalRange / 2, LocalRange / 2, 0);
	int buildingSize = cityAll->getBuildingSize();
	for (int buildings_id = 0; buildings_id < buildingSize; buildings_id++)
	{
		bool in_range = true;
		Building tmpBuilding = cityAll->getBuildingByReference(buildings_id);
		for (int id = 0; id < tmpBuilding.upper_facePoint.size() - 1; id++) //��¼building���������ʱ����ĩ���غϣ���¼���Σ�����   .size����-1
		{
			double x1 = tmpBuilding.upper_facePoint[id].x - MinPos.x;
			double x2 = tmpBuilding.upper_facePoint[id].x - MaxPos.x;
			double y1 = tmpBuilding.upper_facePoint[id].y - MinPos.y;
			double y2 = tmpBuilding.upper_facePoint[id].y - MaxPos.y;
			if (!(x1*x2 < 0 && y1*y2 < 0)) //�жϽ������Ƿ����趨��Χ��,��ʹ��һ���㲻�ڷ�Χ�ڣ�Ҳ�ж�Ϊ�����ڲ�
			{
				in_range = false;
				break;
			}
		}
		if (in_range)
		{
			local_Buildings.push_back(cityAll->getBuildingByValue(buildings_id));
		}
	}
}

/************************************************************************/
/* Ѱ�ҹ涨��Χ�ڵĵ�������
1.�洢��άդ������
2.��ȡ������
3.�����ʷ�
*/
/************************************************************************/
void cityLocalScene::loadLocalGround(Vector3d center, double range, cityScene* cityAll)
{
	
	//step1 ���ɾֲ���cityGroundģ������
	local_Ground = new cityGround(*((cityAll->getGround()->getGroundVector())[0]), center, range);

	//step2 ���оֲ����ļ���ȡ canny�㷨 �ʷ� 

	//int nv = 0;//��¼canny�㷨��ȡ��������
	int localRow = local_Ground->getRow();
	int localCol = local_Ground->getCol();

	//��ʼ�����������
	vector<vector<int> > cannyPoint;//�Ӿֲ��㿪ʼ�ĵ� ����ȫ�ֵ� 
	cannyPoint.resize(localRow, vector<int>(localCol));

	//��ʼ����������
	normalMatrix.resize(localRow, vector<double>(localCol));
	localGetNormalMatrix();

	//������canny�㷨����
	//1.
	cannyImage * tmpTest = new cannyImage(localRow, localCol);
	tmpTest->canny(normalMatrix, cannyPoint, localRow, localCol);

	//2.��ȡ�������������Ա��ڳ�ʼ������
	int  totalVer = countFeaturePoint(meshLength, localRow, localCol, cannyPoint);
	ground_pMesh = &meshCTX;
	InitMesh(ground_pMesh, totalVer);

	//3,�����Ѿ���õ������㣬������������
	int amount = inputMeshPtr();
	cout << "Info: �ֲ�������ͼ����" << amount - 3 << "����" << endl;

	double last_time, this_time;
	last_time = GetTickCount();
	IncrementalDelaunay(ground_pMesh);
	this_time = GetTickCount();
	cout << "Info: �ֲ�Delaunay�ʷ֣�����" <<ground_pMesh->vertex_num << "����" << endl;
	cout << "Info: �ʷֺ�ʱ" << (this_time - last_time) << "ms" << endl;
}
/************************************************************************/
/*���㱣����MESH_PTR �ṹ��  ����3 �����ڲ����                        */
/************************************************************************/
int cityLocalScene::inputMeshPtr()
{
	int amount = 3;
	int localRow = local_Ground->getRow();
	int localCol = local_Ground->getCol();
	int stdLen = local_Ground->getPrecesion();
	int steps = meshLength;
	double xmin = local_Ground->getXmin();
	double ymax = local_Ground->getYmax();
	if (localRow < 10 || localCol < 10)
	{
		steps = 2;
	}
	int stepRow = localRow / steps;
	int stepCol = localCol / steps;
	//ѡ��������
	//������ ���µ���
	for (int n = 0; n < localCol; n++)
	{
		for (int m = localRow - 1; m >= 0; m--)
		{
			if (cannyPoint[m][n] != 0)
			{
				double x = xmin + n*stdLen;
				double y = ymax - m *stdLen;
				double z = local_Ground->getMatrix(m, n);
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->x = x;
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->y = y;
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->z = z;
				amount++;
			}
		}
	}

	//�ټ�����ϵĵ� �Ҳ������������غ�
	for (int i = 0; i <= steps; i++)
	{
		for (int j = 0; j <= steps; j++)
		{
			//
			int rowIndex = i*stepRow;
			int colIndex = j*stepCol;

			if (rowIndex >= localRow)rowIndex = localRow - 1;
			if (colIndex >= localCol)colIndex = localCol - 1;
			int m = rowIndex;
			int n = colIndex;


			if (cannyPoint[rowIndex][colIndex] == 0)
			{
				double x = xmin + n*stdLen;
				double y = ymax - m*stdLen;
				double z = local_Ground->getMatrix(m, n);
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->x = x;
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->y = y;
				((VERTEX2D_PTR)(ground_pMesh->pVerArr + amount))->z = z;
				amount++;
			}
		}
	}
	return amount;
}

/************************************************************************/
/* �õ�һ����ķ�����
��� ԭʼ��  ��Χ���vector
���ز��� Pot��ʽ�ķ�����
*/
/************************************************************************/
Pot cityLocalScene::GetNormalPoint(Pot src, vector<Pot> adjPoint)
{

	int adjSize = adjPoint.size();
	Pot tmpNormal = Pot(0, 0, 0);
	for (int k = 0; k<adjSize - 1; k++)
	{
		double per = disP2P(src, adjPoint[k + 1]) / (disP2P(src, adjPoint[k]) + disP2P(src, adjPoint[k + 1]) + disP2P(adjPoint[k], adjPoint[k + 1]));
		Pot Nk = get_Normal(adjPoint[k], src, adjPoint[k + 1]);
		tmpNormal.x += Nk.x*per;
		tmpNormal.y += Nk.y*per;
		tmpNormal.z += Nk.z*per;
	}

	if (adjSize == 8)
	{
		double per = disP2P(src, adjPoint[0]) / (disP2P(src, adjPoint[7]) + disP2P(src, adjPoint[0]) + disP2P(adjPoint[7], adjPoint[0]));
		Pot Nk = get_Normal(adjPoint[7], src, adjPoint[0]);
		tmpNormal.x += Nk.x*per;
		tmpNormal.y += Nk.y*per;
		tmpNormal.z += Nk.z*per;
	}

	return tmpNormal;


}

/************************************************************************/
/* ��ȡդ����ÿ����ķ�������ˮƽ���� ,����Ź��񹹳�8��������ƽ���ķ������ĦȽ�
���: 
����: normals(�Ƚǵ���ʽ)

��ע����ͼһ����Ҫ����3*3�ģ���Ȼ������⡣
added by lg
*/
/************************************************************************/
void cityLocalScene::localGetNormalMatrix()
{
	double xminLocal = local_Ground->getXmin();
	double ymaxLocal = local_Ground->getYmax();
	double stdlen = local_Ground->getPrecesion();
	for (size_t i = 0; i < local_Ground->getRow(); i++)
	{
		for (size_t j = 0; j <local_Ground->getCol(); j++)
		{
			double tmpX = xminLocal+ j*stdlen;
			double tmpY = ymaxLocal - i*stdlen;
			double tmpZ = local_Ground->getMatrix(i, j);
			Pot source = Pot(tmpX, tmpY, tmpZ);//�õ����ά����
			vector<Pot> adjPoints;
			getAdjPoint(adjPoints,i,j);
			Pot normalPoint = GetNormalPoint(source, adjPoints);//��øõ�ķ�����
			double theta = acos(normalPoint.z / normalPoint.getLength());//���thetaֵ
			normalMatrix[i][j] = theta;
		}
	}
}
/************************************************************************/
/* 
���ݵ�ֲ��ľ���������д���

��Σ�i,j
���Σ�vector<Pot> adjPoints
*/
/************************************************************************/
void cityLocalScene::getAdjPoint(vector<Pot> &adjPoints,int i,int j)
{
	double xmin = local_Ground->getXmin();
	double ymax = local_Ground->getYmax();
	double stdlen = local_Ground->getPrecesion();
	double tmpX = xmin + j*stdlen;
	double tmpY = ymax - i*stdlen;
	int col = local_Ground->getCol();
	int row = local_Ground->getRow();
	if (i == 0 && j == 0)//case1: ������Ͻǵ㲻����
	{
		Pot P1 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i,j+1));
		Pot P2 = Pot(tmpX + stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j + 1));
		Pot P3 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
	}
	else if (i == 0 && j<col - 1 && j>0) //case2:
	{
		Pot P1 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i,j+1));
		Pot P2 = Pot(tmpX + stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j + 1));
		Pot P3 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j ));
		Pot P4 = Pot(tmpX - stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j - 1));
		Pot P5 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
		adjPoints.push_back(P4);
		adjPoints.push_back(P5);

	}
	else if (i == 0 && j == col - 1)//case3
	{
		Pot P1 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j ));
		Pot P2 = Pot(tmpX - stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j-1));
		Pot P3 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
	}
	else if (0 < i&&i < row - 1 && j == 0)//case4
	{
		Pot P1 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j ));
		Pot P2 = Pot(tmpX + stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j + 1));
		Pot P3 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i, j + 1));
		Pot P4 = Pot(tmpX + stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j +1));
		Pot P5 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j ));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
		adjPoints.push_back(P4);
		adjPoints.push_back(P5);
	}
	else if (0 < i&&i < row - 1 && 0 < j&&j < col - 1)//case5
	{ //�Ӹõ������һ�������
		Pot P1 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j ));
		Pot P2 = Pot(tmpX + stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j + 1));
		Pot P3 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i, j + 1));
		Pot P4 = Pot(tmpX + stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j + 1));
		Pot P5 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j));
		Pot P6 = Pot(tmpX - stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j - 1));
		Pot P7 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		Pot P8 = Pot(tmpX - stdlen, tmpY + stdlen,local_Ground->getMatrix(i-1, j - 1));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
		adjPoints.push_back(P4);
		adjPoints.push_back(P5);
		adjPoints.push_back(P6);
		adjPoints.push_back(P7);
		adjPoints.push_back(P8);
	}
	else if (0 < i&&i < row - 1 && j == col - 1)//case6
	{
		Pot P1 = Pot(tmpX, tmpY - stdlen, local_Ground->getMatrix(i+1, j));
		Pot P2 = Pot(tmpX - stdlen, tmpY - stdlen, local_Ground->getMatrix(i+1, j - 1));
		Pot P3 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		Pot P4 = Pot(tmpX - stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j - 1));
		Pot P5 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
		adjPoints.push_back(P4);
		adjPoints.push_back(P5);
	}
	else if (j == 0 && i == row - 1)//case7
	{
		Pot P1 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j));
		Pot P2 = Pot(tmpX + stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j+1));
		Pot P3 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i, j + 1));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
	}
	else if (i == row - 1 && 0 < j&&j < col - 1)//case8
	{
		Pot P1 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		Pot P2 = Pot(tmpX - stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j - 1));
		Pot P3 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j));
		Pot P4 = Pot(tmpX + stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j+1));
		Pot P5 = Pot(tmpX + stdlen, tmpY, local_Ground->getMatrix(i, j + 1));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
		adjPoints.push_back(P4);
		adjPoints.push_back(P5);
	}
	else if (i == row - 1 && j == col - 1)//case9
	{
		Pot P1 = Pot(tmpX - stdlen, tmpY, local_Ground->getMatrix(i, j - 1));
		Pot P2 = Pot(tmpX - stdlen, tmpY + stdlen, local_Ground->getMatrix(i-1, j - 1));
		Pot P3 = Pot(tmpX, tmpY + stdlen, local_Ground->getMatrix(i-1, j));
		adjPoints.push_back(P1);
		adjPoints.push_back(P2);
		adjPoints.push_back(P3);
	}
}

/************************************************************************/
/* ��һ����������ģ�ͳ����У�����һ���ֲ�����         
	
*/
/************************************************************************/
cityLocalScene::cityLocalScene(Vector3d  AP_position, double  LocalRange, cityScene* cityAll)
{
	loadLocalBuilding(AP_position, LocalRange, cityAll);
	loadLocalGround(AP_position, LocalRange, cityAll);
	cout << "Info: ���泡���������" << endl;
}
cityLocalScene::~cityLocalScene()
{
}
