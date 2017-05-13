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
	int totalVer = 0;//统计该区域的特征点数量
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

	//再计算边上的点 且不能与特征点重合
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
	//局部区域的范围 MinPos、MaxPos
	MinPos = AP_position - Vector3d(LocalRange / 2, LocalRange / 2, 0);
	MaxPos = AP_position + Vector3d(LocalRange / 2, LocalRange / 2, 0);
	int buildingSize = cityAll->getBuildingSize();
	for (int buildings_id = 0; buildings_id < buildingSize; buildings_id++)
	{
		bool in_range = true;
		Building tmpBuilding = cityAll->getBuildingByReference(buildings_id);
		for (int id = 0; id < tmpBuilding.upper_facePoint.size() - 1; id++) //记录building顶面点坐标时，首末点重合，记录两次，所以   .size（）-1
		{
			double x1 = tmpBuilding.upper_facePoint[id].x - MinPos.x;
			double x2 = tmpBuilding.upper_facePoint[id].x - MaxPos.x;
			double y1 = tmpBuilding.upper_facePoint[id].y - MinPos.y;
			double y2 = tmpBuilding.upper_facePoint[id].y - MaxPos.y;
			if (!(x1*x2 < 0 && y1*y2 < 0)) //判断建筑物是否在设定范围内,即使有一个点不在范围内，也判定为不在内部
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
/* 寻找规定范围内的地面数据
1.存储二维栅格数据
2.提取特征点
3.三角剖分
*/
/************************************************************************/
void cityLocalScene::loadLocalGround(Vector3d center, double range, cityScene* cityAll)
{
	
	//step1 生成局部的cityGround模型数据
	local_Ground = new cityGround(*((cityAll->getGround()->getGroundVector())[0]), center, range);

	//step2 进行局部的文件提取 canny算法 剖分 

	//int nv = 0;//记录canny算法提取的特征点
	int localRow = local_Ground->getRow();
	int localCol = local_Ground->getCol();

	//初始化特征点矩阵
	vector<vector<int> > cannyPoint;//从局部点开始的点 不是全局的 
	cannyPoint.resize(localRow, vector<int>(localCol));

	//初始化向量矩阵
	normalMatrix.resize(localRow, vector<double>(localCol));
	localGetNormalMatrix();

	//以下是canny算法部分
	//1.
	cannyImage * tmpTest = new cannyImage(localRow, localCol);
	tmpTest->canny(normalMatrix, cannyPoint, localRow, localCol);

	//2.获取特征点数量，以便于初始化网格
	int  totalVer = countFeaturePoint(meshLength, localRow, localCol, cannyPoint);
	ground_pMesh = &meshCTX;
	InitMesh(ground_pMesh, totalVer);

	//3,根据已经获得的特征点，构建三角网格
	int amount = inputMeshPtr();
	cout << "Info: 局部场景地图共有" << amount - 3 << "个点" << endl;

	double last_time, this_time;
	last_time = GetTickCount();
	IncrementalDelaunay(ground_pMesh);
	this_time = GetTickCount();
	cout << "Info: 局部Delaunay剖分，共有" <<ground_pMesh->vertex_num << "个点" << endl;
	cout << "Info: 剖分耗时" << (this_time - last_time) << "ms" << endl;
}
/************************************************************************/
/*将点保存在MESH_PTR 结构中  方案3 采用内部插点                        */
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
	//选择特征点
	//从左到右 从下到上
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

	//再计算边上的点 且不能与特征点重合
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
/* 得到一个点的法向量
入参 原始点  周围点的vector
返回参数 Pot形式的法向量
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
/* 获取栅格中每个点的法向量的水平分量 ,求出九宫格构成8个三角面平均的法向量的θ角
入参: 
出参: normals(θ角的形式)

备注：地图一定是要大于3*3的，不然会出问题。
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
			Pot source = Pot(tmpX, tmpY, tmpZ);//该点的三维坐标
			vector<Pot> adjPoints;
			getAdjPoint(adjPoints,i,j);
			Pot normalPoint = GetNormalPoint(source, adjPoints);//求得该点的法向量
			double theta = acos(normalPoint.z / normalPoint.getLength());//获得theta值
			normalMatrix[i][j] = theta;
		}
	}
}
/************************************************************************/
/* 
根据点分布的九种情况进行处理

入参：i,j
出参：vector<Pot> adjPoints
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
	if (i == 0 && j == 0)//case1: 如果左上角点不存在
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
	{ //从该点的上面一个点计算
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
/* 从一个整个城市模型场景中，构建一个局部场景         
	
*/
/************************************************************************/
cityLocalScene::cityLocalScene(Vector3d  AP_position, double  LocalRange, cityScene* cityAll)
{
	loadLocalBuilding(AP_position, LocalRange, cityAll);
	loadLocalGround(AP_position, LocalRange, cityAll);
	cout << "Info: 地面场景构建完成" << endl;
}
cityLocalScene::~cityLocalScene()
{
}
