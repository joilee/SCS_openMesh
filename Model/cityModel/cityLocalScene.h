#pragma  once

#include "cityScene.h"
#include "../../util/vector.h"
using namespace  std;
class cityLocalScene
{
public:
	cityLocalScene(Vector3d center,double range,cityScene* cityAll);
	~cityLocalScene();

private:
	void loadLocalBuilding(Vector3d center, double range, cityScene* cityAll);
	void loadLocalGround(Vector3d center, double range, cityScene* cityAll);
	vector<Building> local_Buildings;
	Vector3d MaxPos, MinPos;
	//����ģ������
	cityGroundVector * local_Ground;


};

