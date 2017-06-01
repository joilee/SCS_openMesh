#ifndef MODELPARA_H
#define  MODELPARA_H
#include <cityModule/cityScene.h>
#include <mesh/meshStruct.h>
#include <util/vector.h>
#include <mesh/point.h>
#include <material/scsMaterial.h>

class ModelPara
{
public:
	ModelPara();
	~ModelPara();

public:
	//城市场景 包含： 建筑物数组、地面栅格模型
	cityScene *localScene;

	//材料数组，来自于文件
	vector<scsMaterial> materialdatabase;

	//地面三角网格
	MESH_PTR ground_pMesh;

	//所有的点、面、材料编号
	std::vector<Vector3d> V;					// vertices
	std::vector<Vector3i> F;					// faces
	std::vector<Vector3d> NF;					 // normals of faces
	/*std::vector<Vector3d> Vn;                 // normals of vertices*/
	std::vector<int>f_materialId;                //the material of face,面片对应的材料编号

	/************************************************************************/
	/* 得到一个点对应的高度                                                                     */
	/************************************************************************/
	double getPointAltitude(double x, double y);
};





#endif