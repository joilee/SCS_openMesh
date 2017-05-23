#pragma once
#include "../abstractModel.h"
#include "cityScene.h"


class cityModel :public abstractModel
{
public:
	cityModel(string jsonPath);
	~cityModel();
	void draw(int mode);
	void initDraw();
	inline string  getName(){ return name; }
	inline cityScene* getCity(){ return city; }
private:
	string name;
	
	//����ģ�ͳ��� ���������� ����ģ��  �Լ����ߺϳɵ�������Ƭģ��

	//����ģ�ͺ���
	void LoadModel(string _name, QStringList _v, QStringList _h, string _p);

	//������ 
	cityScene *city;  //����Ľ�����ģ��

	//����


};

