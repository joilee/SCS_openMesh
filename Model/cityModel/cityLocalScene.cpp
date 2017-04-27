#include "cityLocalScene.h"

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
	MESH_PTR ground_pMesh = &meshCTX;
	//step1 �ҵ���Ӧ���к���
	//��Χ��  xmin xmax ymin ymax
	vector<double> bounding = cityAll->getGround()->getBoundingBoxByIndex(0);
	int upRow = (gctx->modelPara->ymax[0] - MaxPos[1]) / gctx->modelPara->stdLen[0];
	int downRow = (gctx->modelPara->ymax[0] - MinPos[1]) / gctx->modelPara->stdLen[0] + 1;
	int leftCol = (MinPos[0] - gctx->modelPara->xmin[0]) / gctx->modelPara->stdLen[0];
	int rightCol = (MaxPos[0] - gctx->modelPara->xmin[0]) / gctx->modelPara->stdLen[0] + 1;
	int area[4] = { upRow, downRow, leftCol, rightCol };

}
/************************************************************************/
/* ��һ����������ģ�ͳ����У�����һ���ֲ�����         
	
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
