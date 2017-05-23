#include "cityLocalModel.h"
#include "TriangleMesh/cannyImage.h"
#include "TriangleMesh/Mesh.h"
#include "../../util/emxUtilityInc.h"
#include <gl/freeglut.h>

double cityLocalModel::disP2P(Pot p, Pot q)
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

Pot cityLocalModel::get_Normal(Pot p1, Pot p2, Pot p3)
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

void cityLocalModel::clearVandF()
{
	V.clear();
	F.clear();
	f_materialId.clear();
	NF.clear();
}


void cityLocalModel::generateBuildingMesh()
{
	int concave_polygonNum = 0;
	for (int buildings_id = 0; buildings_id < local_Buildings.size(); buildings_id++)
	{
		int count = local_Buildings[buildings_id].upper_facePoint.size() - 1; //��¼building���������ʱ����ĩ���غϣ���¼���Σ�����   .size����-1
		double building_height = local_Buildings[buildings_id].height;
		int V_size = V.size();
		vector<int> upper_PointIndex; //�洢�϶���������ֵ���Ա��϶����ʷ�������Ƭʱ����

		//��Ĵ洢����
		//�϶���ĵ�
		for (int id = 0; id < count; id++)
		{
			Vector3d point = local_Buildings[buildings_id].upper_facePoint[id];
			V.push_back(point);
			upper_PointIndex.push_back(V.size() - 1);
		}
		//�µ���ĵ�
		for (int id = 0; id < count; id++)
		{
			Vector3d point = local_Buildings[buildings_id].upper_facePoint[id];
			double under_height = point.z - building_height;
			V.push_back(Vector3d(point.x, point.y, under_height));
		}

		Vector3d E1, E2, N;
		//��Ƭ�Ĳ���
		//����������ʷ�������Ƭ,�ֳ�����������
		for (int id = 0; id < count; id++)
		{
			int i1, i2, i3, i4;

			i1 = id + V_size;
			i2 = (id + 1) % count + V_size;
			i3 = (id + 1) % count + count + V_size;
			i4 = id + count + V_size;

			E1 = V[i2] - V[i1];
			E2 = V[i3] - V[i2];
			N = VectorCross(E1, E2);

			if (N.norm() > DOUBLE_EPSILON)
			{
				F.push_back(Vector3i(i1, i2, i3));
				NF.push_back(N.normalize());
			}

			E1 = V[i3] - V[i1];
			E2 = V[i4] - V[i3];
			N = VectorCross(E1, E2);

			if (N.norm() > DOUBLE_EPSILON)
			{
				F.push_back(Vector3i(i1, i3, i4));
				NF.push_back(N.normalize());
			}
		}

		//�������϶����ʷ�������Ƭ����Ƭ��������Ϊ��z�᷽��Vector3d(0,0,1),����������ǻ����⴦������ַ��http://blog.sina.com.cn/s/blog_5a6f39cf0101374h.html
		bool convex = true; //Ϊtrueʱ��͹����Σ�falseʱ�ǰ������
		vector<Vector3d> upper_face = local_Buildings[buildings_id].upper_facePoint;
		upper_face.pop_back(); //��ĩ���ظ�������Ҫɾ��ĩβ���ظ���
		int count1 = upper_face.size();

		for (int id = 0; id < count1; id++)
		{
			Vector3d v1, v2, v3;
			v1 = upper_face[id];
			v2 = upper_face[(id + 1) % count1];
			v3 = upper_face[(id + 2) % count1];
			//��v2�ǲ��ǰ���
			if (Dot(VectorCross(v2 - v1, v3 - v2), Vector3d(0, 0, -1)) < DOUBLE_EPSILON)//�ж��Ƿ�Ϊ��
			{
				//�ж���Ϊ���㣬ֱ���޳���v2
				std::vector<Vector3d>::iterator it1 = upper_face.begin() + (id + 1) % count1;
				upper_face.erase(it1);
				std::vector<int>::iterator it2 = upper_PointIndex.begin() + (id + 1) % count1;
				upper_PointIndex.erase(it2);

				count1--;
				id--;

				convex = false;
			}
		}
		if (!convex)
		{
			concave_polygonNum++;
		}

		//͹������ʷ֣�ѡ��һ�����㣬Ȼ������Ѱ���������������һ��������
		int count2 = upper_face.size();
		for (int id = 0; id <= count2 - 3; id++)
		{
			E1 = V[upper_PointIndex[id + 2]] - V[upper_PointIndex[0]];
			E2 = V[upper_PointIndex[id + 1]] - V[upper_PointIndex[id + 2]];
			N = VectorCross(E1, E2);
			if (N.norm() > DOUBLE_EPSILON)
			{
				F.push_back(Vector3i(upper_PointIndex[0], upper_PointIndex[id + 2], upper_PointIndex[id + 1]));
				NF.push_back(N.normalize());
			}
		}
	}//һ�������ﴦ�����
	cout << "info: �ֲ�������������" << local_Buildings.size() <<endl;
	cout << "info: �ֲ���������Ƭ������" << F.size() << " ��������������" << concave_polygonNum << endl;

	int V_num = V.size();
	int F_startNum = F.size();
	int NF_startNum = NF.size();
	VERTEX2D_PTR pVtx = ground_pMesh->pVerArr;
	int numV = ground_pMesh->vertex_num;
	for (int i = 3; i<numV + 3; i++)//ǰ��������bounding triangle
	{
		double x, y, z;
		x = ((VERTEX2D_PTR)(ground_pMesh->pVerArr + i))->x;
		y = ((VERTEX2D_PTR)(ground_pMesh->pVerArr + i))->y;
		z = ((VERTEX2D_PTR)(ground_pMesh->pVerArr + i))->z;
		V.push_back(Vector3d(x, y, z));
	}

	TRIANGLE_PTR pTri = ground_pMesh->pTriArr;
	int tri_index = 0;
	int* pi;
	while (pTri != NULL)
	{
		tri_index++;
		int id[3];
		pi = &(pTri->i1);
		for (int j = 0; j<3; j++)
		{
			id[j] = *pi++;
			id[j] = id[j] - 3 + V_num;
		}
		Vector3d E1, E2, N;
		E1 = V[id[1]] - V[id[0]];
		E2 = V[id[2]] - V[id[1]];
		N = VectorCross(E1, E2);
		if (N.norm() > DOUBLE_EPSILON)
		{
			F.push_back(Vector3i(id[0], id[1], id[2]));
			NF.push_back(N.normalize());
		}
		pTri = pTri->pNext;
	}
	int F_endNum = F.size();
	int NF_endNum = NF.size();
	cout << "info: ���潨ģ������� " << V.size() - V_num << "  ���������� " << F_endNum - F_startNum << " ���������� " << NF_endNum - NF_startNum << endl;
	// calculate the bounding box
	std::vector<Vector3d>::const_iterator v = V.begin();
	for (++v; v != V.end(); ++v)
	{
		MinPos = Min(MinPos, (*v));
		MaxPos = Max(MaxPos, (*v));
	}
	cout << "info: ���������ϣ�" << endl;
}

void cityLocalModel::loadLocalBuilding(Vector3d  AP_position, double  LocalRange, cityScene* cityAll)
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
void cityLocalModel::loadLocalGround(Vector3d center, double range, cityScene* cityAll)
{
	
	//step1 ���ɾֲ���cityGroundģ������
	local_Ground = new cityGround(*((cityAll->getGround()->getGroundVector())[0]), center, range);

	//step2 ���оֲ����ļ���ȡ canny�㷨 �ʷ� 

	//int nv = 0;//��¼canny�㷨��ȡ��������
	int localRow = local_Ground->getRow();
	int localCol = local_Ground->getCol();

	//��ʼ�����������
	//vector<vector<int> > cannyPoint;//�Ӿֲ��㿪ʼ�ĵ� ����ȫ�ֵ� 
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
int cityLocalModel::inputMeshPtr()
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
Pot cityLocalModel::GetNormalPoint(Pot src, vector<Pot> adjPoint)
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
void cityLocalModel::localGetNormalMatrix()
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
void cityLocalModel::getAdjPoint(vector<Pot> &adjPoints,int i,int j)
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
cityLocalModel::cityLocalModel(Vector3d  AP_position, double  LocalRange, cityScene* cityAll,string _name)
{
	name = _name + "_Local_Model";
	loadLocalBuilding(AP_position, LocalRange, cityAll);
	loadLocalGround(AP_position, LocalRange, cityAll);
	cout << "Info: ���泡���������" << endl;
	clearVandF();
	generateBuildingMesh();

}
cityLocalModel::~cityLocalModel()
{
}

void cityLocalModel::initDraw()
{
	
}

void cityLocalModel::draw(int mode)
{
	
}

double cityLocalModel:: getAltitude(double x, double y)
{
	if (local_Ground == NULL)
	{
		cout << "error : �����ھֲ�ģ��" << endl;
		return -1000;
	}
  return local_Ground->getPointAltitude(x, y);
}