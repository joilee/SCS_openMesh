#pragma  once

#include "abstractModel.h"
#include "abstractModelFactory.h"
#include  "cityModel/cityModelFactory.h"
#include  "../observer/modelSubject.h"
#include <map>
class ModelManager
{
public:
	ModelManager();
	~ModelManager();

	void  insertModel(abstractModel* model);
	void  deleteModel(abstractModel* model);
	void deleteModel(string name);
	void loadCityModel(string path);

	modelSubject *getModelSubject(){ return m_subject; }
	/************************************************************************/
	/* 
		@brief 将最新的状态发送到相应的观察者
	*/
	/************************************************************************/
	void sendNewState();
private:
	map<string, abstractModel*> modelMap;
	cityModelFactory *cityFac;
	modelSubject * m_subject;
};

