#include "cityScene.h"
#include "../../util/emxUtilityInc.h"
#include <limits>
#include <fstream>
Building::Building()
{
	
}

Building::~Building()
{
	upper_facePoint.clear();
}

void cityScene::readGround(string p)
{
	//导入海拔信息
	ground = new cityGroundVector(p);
}

cityScene::cityScene(QStringList _v, QStringList _h,string _p)
{
	concave_num = 0;
	MinPoint = Vector3d(numeric_limits<double>::max(), numeric_limits<double>::max(), numeric_limits<double>::max());
	MaxPoint = Vector3d(numeric_limits<double>::min(), numeric_limits<double>::min(), numeric_limits<double>::min());
	readGround(_p);
	cout << "Info: 地面模型读入成功。" << endl;
	QString Scene2DInfoFile_path, SceneHeightInfoFile_path;

	for (size_t i = 0; i < _v.size();i++)
	{
		Scene2DInfoFile_path =_v.at(i);
		SceneHeightInfoFile_path = _h.at(i);
		readBuilding(Scene2DInfoFile_path.toStdString().c_str(), SceneHeightInfoFile_path.toStdString().c_str());
	}
}
cityGroundVector *cityScene::getGround()
{
	return ground;
}

void cityScene::readBuilding(const char*filename_2D, const char*filename_Height)
{
	//场景vector格式文件读取
	ifstream infile1(filename_2D, ios::in | ios::_Nocreate);
	if (!infile1)
	{
		cout << "can not open file1!" << endl;
		return;
	}
	ifstream infile2(filename_Height, ios::in | ios::_Nocreate);
	if (!infile2)
	{
		cout << "can not open file2!" << endl;
		return;
	}
	string str1, str2;
	infile1.seekg(4, ios::beg);
	infile2.seekg(3, ios::beg);
	while (getline(infile1, str1))
	{
		int building_id1 = 0;
		string sign1 = " ";
		int point_num = 0;
		istringstream linestream1(str1);
		linestream1 >> building_id1 >> sign1 >> point_num;

		int building_id2 = 0;
		string sign2 = " ";
		double height = 0;
		getline(infile2, str2);
		istringstream linestream2(str2);
		linestream2 >> building_id2 >> sign2 >> height;

		if (point_num > 0)
		{
			Building building_info;
			building_info.height = height;

			for (int i = 0; i < point_num; i++)
			{
				double x, y;
				getline(infile1, str1);
				istringstream linestream4(str1);
				linestream4 >> x >> y;

				double building_altitude = getAltitude(x, y); //建筑物各个点按照实际获取对应海拔值
				double upper_height = height + building_altitude;
				double under_height = building_altitude;
				building_info.upper_facePoint.push_back(Vector3d(x, y, upper_height));
				MaxPoint = Max(MaxPoint, Vector3d(x, y, upper_height));
				MinPoint = Min(MinPoint, Vector3d(x, y, under_height));
			}
			total_Buildings.push_back(building_info);
		}
	}
	infile1.close();
	infile2.close();
}

double cityScene:: getAltitude(double x, double y)
{
	if (ground->existed())
	{
		return ground->getAltitudeFrom0(x, y);
	}
	else
	{
		cout << "error: ground is not existed!" << endl;
		return 0;
	}
}