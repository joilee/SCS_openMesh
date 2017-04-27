#include "cityLocalScene.h"

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
	MESH_PTR ground_pMesh = &meshCTX;
	//step1 找到对应的行和列
	//包围盒  xmin xmax ymin ymax
	vector<double> bounding = cityAll->getGround()->getBoundingBoxByIndex(0);
	int upRow = (gctx->modelPara->ymax[0] - MaxPos[1]) / gctx->modelPara->stdLen[0];
	int downRow = (gctx->modelPara->ymax[0] - MinPos[1]) / gctx->modelPara->stdLen[0] + 1;
	int leftCol = (MinPos[0] - gctx->modelPara->xmin[0]) / gctx->modelPara->stdLen[0];
	int rightCol = (MaxPos[0] - gctx->modelPara->xmin[0]) / gctx->modelPara->stdLen[0] + 1;
	int area[4] = { upRow, downRow, leftCol, rightCol };

}
/************************************************************************/
/* 从一个整个城市模型场景中，构建一个局部场景         
	
*/
/************************************************************************/
cityLocalScene::cityLocalScene(Vector3d  AP_position, double  LocalRange, cityScene* cityAll)
{
	loadLocalBuilding(AP_position, LocalRange, cityAll);
	loadLocalGround(AP_position, LocalRange, cityAll);

}
cityLocalScene::~cityLocalScene()
{
}
