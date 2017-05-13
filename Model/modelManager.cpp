#include "modelManager.h"
#include "cityModel/cityModel.h"
#include <QDebug>
#include <iostream>
ModelManager::ModelManager()
{
	cityFac = new cityModelFactory;
	m_subject = new modelSubject;
}

ModelManager::~ModelManager()
{

}

void ModelManager::insertModel(abstractModel* model)
{
	string name = model->getName();

	if (modelMap.find(name)!=modelMap.end())
	{
		cout<<"error: Model 已经存在，或者模型重名.名字为"<<name<<". 插入失败，已经退出";
		return;
	}
	modelMap.insert(make_pair(name, model));
	cout << "success: 模型已插入至容器" << endl;

	sendNewState();
}
void ModelManager::deleteModel(abstractModel* model)
{
	map<string, abstractModel*>::iterator it;
	for (it = modelMap.begin(); it != modelMap.end();)
	{
		if (it->second == model)
		{
			modelMap.erase(it++);
			cout << "success: Model 已经删除" << endl;
		}
		else
		{
			it++;
		}
	}
	cout << "Info: 容器中模型数量为" << modelMap.size() << endl;
}
void ModelManager::deleteModel(string name)
{
	map<string, abstractModel*>::iterator it = modelMap.find(name);
	if (it!=modelMap.end())
	{
		modelMap.erase(it);
		cout << "success: Model 已经删除" << endl;
	}
	else
	{
		cout << "error: 容器中没有匹配的名字" << endl;
	}
	cout << "Info: 容器中模型数量为" << modelMap.size() << endl;
}

void ModelManager::loadCityModel(string path)
{
	abstractModel * tmp=cityFac->loadModel(path);
	cout << "success: 模型已经读入" << endl;
	insertModel(tmp);
}

void ModelManager::sendNewState()
{
	map<string, abstractModel*>::iterator it = modelMap.begin();
	vector<bool> exist;//记录是否有城市场景和局部场景
	while (it!=modelMap.end())
	{
		cout << typeid(*(it->second)).name() << endl;
		if ( typeid(*(it->second))==typeid(cityModel))
		{
			cityModel * city = dynamic_cast<cityModel*>(it->second);
			int cityNum = city->getCity()->getBuildingSize();
			int concave = city->getCity()->getConcaveNum();
			Vector3d MinPos = city->getCity()->getMinPoint();
			Vector3d MaxPos = city->getCity()->getMaxPoint();
			m_subject->visItem->setBuildingNum(cityNum);
			m_subject->visItem->setConcaveNum(concave);
			m_subject->visItem->setcityMax(MaxPos);
			m_subject->visItem->setcityMin(MinPos);
			m_subject->visItem->modelList[0] = city->getName();
		}
		it++;
	}
	m_subject->notify();
}