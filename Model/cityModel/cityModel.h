#pragma once
#include "../abstractModel.h"
#include "cityScene.h"


class cityModel :public abstractModel
{
public:
	cityModel(string jsonPath);
	~cityModel();
	void draw();
	string getName(){ return name; }
private:
	string name;
	
	//城市模型场景 包括建筑物 地面模型  以及两者合成的三角面片模型

	//导入模型函数
	void LoadModel(string _name, QStringList _v, QStringList _h, string _p);

	//建筑物 
	cityScene *city;  //整体的建筑物模型



	//材料


};

