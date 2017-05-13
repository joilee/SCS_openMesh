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
		cout<<"error: Model �Ѿ����ڣ�����ģ������.����Ϊ"<<name<<". ����ʧ�ܣ��Ѿ��˳�";
		return;
	}
	modelMap.insert(make_pair(name, model));
	cout << "success: ģ���Ѳ���������" << endl;

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
			cout << "success: Model �Ѿ�ɾ��" << endl;
		}
		else
		{
			it++;
		}
	}
	cout << "Info: ������ģ������Ϊ" << modelMap.size() << endl;
}
void ModelManager::deleteModel(string name)
{
	map<string, abstractModel*>::iterator it = modelMap.find(name);
	if (it!=modelMap.end())
	{
		modelMap.erase(it);
		cout << "success: Model �Ѿ�ɾ��" << endl;
	}
	else
	{
		cout << "error: ������û��ƥ�������" << endl;
	}
	cout << "Info: ������ģ������Ϊ" << modelMap.size() << endl;
}

void ModelManager::loadCityModel(string path)
{
	abstractModel * tmp=cityFac->loadModel(path);
	cout << "success: ģ���Ѿ�����" << endl;
	insertModel(tmp);
}

void ModelManager::sendNewState()
{
	map<string, abstractModel*>::iterator it = modelMap.begin();
	vector<bool> exist;//��¼�Ƿ��г��г����;ֲ�����
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