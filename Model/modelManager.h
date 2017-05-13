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
		@brief �����µ�״̬���͵���Ӧ�Ĺ۲���
	*/
	/************************************************************************/
	void sendNewState();
private:
	map<string, abstractModel*> modelMap;
	cityModelFactory *cityFac;
	modelSubject * m_subject;
};

