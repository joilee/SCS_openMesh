#pragma  once
#include "../../util/vector.h"
#include <QStringList>
#include <vector>
#include <string>
#include "cityGroundVector.h"
using namespace std;

//����һ��������

class Building
{
public:
	Building();
	~Building();
	vector<Vector3d> upper_facePoint;  //�������϶��������
	double height;  //������߶ȣ����϶����z�����ȥ�߶ȼ��ɻ���µ����Ӧ��ĸ߶�z����
};

/*
��������
���������������顢����ģ��
*/
class cityScene
{
	public:
		cityScene(QStringList _v, QStringList _h,string _p);
		cityGroundVector *getGround();
		double getAltitude(double x, double y);
		vector<Building>& getTotal_Building(){ return total_Buildings; }
		int getBuildingSize(){ return total_Buildings.size(); }
		const Building& getBuildingByReference(int id){ return total_Buildings[id]; }
		Building getBuildingByValue(int id){ return total_Buildings[id]; }
		Vector3d getMaxPoint(){ return MaxPoint; }
		Vector3d getMinPoint(){ return MinPoint; }
	private:
		vector<Building> total_Buildings;
		Vector3d MaxPoint, MinPoint;
		void readBuilding(const char*filename_2D, const char*filename_Height);
		void readGround(string p);

		//����ģ������
		cityGroundVector * ground;
};