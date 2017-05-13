#pragma  once

#include "cityScene.h"
#include "TriangleMesh/meshStruct.h"
#include "../../util/vector.h"
#include "TriangleMesh/point.h"

#define meshLength 10

using namespace  std;
class cityLocalScene
{
public:
	cityLocalScene(Vector3d center,double range,cityScene* cityAll);
	~cityLocalScene();

private:
	void loadLocalBuilding(Vector3d center, double range, cityScene* cityAll);
	void loadLocalGround(Vector3d center, double range, cityScene* cityAll);
	void localGetNormalMatrix();
	void getAdjPoint(vector<Pot> &adjPoints, int i, int j);
	Pot GetNormalPoint(Pot src, vector<Pot> adjPoint);
	double disP2P(Pot p, Pot q);
	Pot get_Normal(Pot p1, Pot p2, Pot p3);
	int inputMeshPtr();

	//地面模型数组
	vector<Building> local_Buildings;
	Vector3d MaxPos, MinPos;

	cityGround* local_Ground;

	vector<vector<double> >normalMatrix;
	vector<vector<int> > cannyPoint;
	MESH meshCTX;
	MESH_PTR ground_pMesh;
};

