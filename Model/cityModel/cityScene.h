#pragma  once
#include "../../util/vector.h"
#include <QStringList>
#include <vector>
#include <string>
#include "cityGroundVector.h"
using namespace std;

//单独一个建筑物

class Building
{
public:
	Building();
	~Building();
	vector<Vector3d> upper_facePoint;  //建筑物上顶面点坐标
	double height;  //建筑物高度，用上顶面点z坐标减去高度即可获得下底面对应点的高度z坐标
};

/*
建筑物类
包含：建筑物数组、地面模型
*/
class cityScene
{
	public:
		cityScene(QStringList _v, QStringList _h,string _p);
		cityGroundVector *getGround();
		double getAltitude(double x, double y);
		vector<Building>& getTotal_Building(){ return total_Buildings; }
		inline int getBuildingSize(){ return total_Buildings.size(); }
		inline int getConcaveNum(){ return concave_num; }
		const Building& getBuildingByReference(int id){ return total_Buildings[id]; }
		Building getBuildingByValue(int id){ return total_Buildings[id]; }
		inline Vector3d getMaxPoint(){ return MaxPoint; }
		inline Vector3d getMinPoint(){ return MinPoint; }
	private:
		vector<Building> total_Buildings;
		int concave_num;
		Vector3d MaxPoint, MinPoint;
		void readBuilding(const char*filename_2D, const char*filename_Height);
		void readGround(string p);

		//地面模型数组
		cityGroundVector * ground;
};