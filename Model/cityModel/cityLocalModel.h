#pragma  once
#include "../abstractModel.h"
#include "cityScene.h"
#include "TriangleMesh/meshStruct.h"
#include "../../util/vector.h"
#include "TriangleMesh/point.h"

#define meshLength 10

using namespace  std;

/*
	@brief �ֲ�������
	@note �����ֲ�������;ֲ����档��ͬ�ڴ�������Ԫ�����㷨��������Ҫ���������ߣ�����Ҫָ�������
*/
class cityLocalModel:public abstractModel
{
public:
	cityLocalModel(Vector3d center,double range,cityScene* cityAll,string _name);
	~cityLocalModel();
	int getTriangleNum(){ return F.size() ; }
	inline Vector3d getMin(){ return MinPos; }
	inline Vector3d getMax(){ return MaxPos; }
private:
	std::vector<Vector3d> V;					// vertices
	std::vector<Vector3i> F;					// faces
	std::vector<Vector3d> NF;					// normals of faces
	/*std::vector<Vector3d> Vn;                 // normals of vertices*/
	std::vector<int>f_materialId;               //the material of face,��Ƭ��Ӧ�Ĳ��ϱ��

	void clearVandF();

	/* 
	@brief �ӽ�����͵���ģ�͵õ�������
	*/
	void generateBuildingMesh();

public:
	/*
	@brief  �ֲ�������Ա���� �������λ�õĺ���
	*/
	double getAltitude(double x, double y);

private:
	void loadLocalBuilding(Vector3d center, double range, cityScene* cityAll);
	void loadLocalGround(Vector3d center, double range, cityScene* cityAll);
	void localGetNormalMatrix();
	void getAdjPoint(vector<Pot> &adjPoints, int i, int j);
	Pot GetNormalPoint(Pot src, vector<Pot> adjPoint);
	double disP2P(Pot p, Pot q);
	Pot get_Normal(Pot p1, Pot p2, Pot p3);
	int inputMeshPtr();

	//����ģ������
	vector<Building> local_Buildings;
	Vector3d MaxPos, MinPos;

	cityGround* local_Ground;

	vector<vector<double> >normalMatrix;
	vector<vector<int> > cannyPoint;
	MESH meshCTX;
	MESH_PTR ground_pMesh;
public:
	/*
	@brief �̳и���ĺ���
	*/
	string name;
	inline string  getName(){ return name; }
	void  draw(int mode);
	void initDraw();
};

