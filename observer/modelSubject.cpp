#include "modelSubject.h"
#include "../observer/observer.h"

void modelSubject::attach(Observer*a)
{
	observers.insert(a);
	cout << "添加观察者成功 "<<a->name << endl;
}
void modelSubject::detach(Observer*a)
{
	set<Observer*>::iterator it=observers.begin();
	while (it!=observers.end())
	{
		if ((*it)==a)
		{
			observers.erase(it);
			break;
		}
		++it;
	}
}

void modelSubject::notify()
{
	cout << "subject开始发送消息" << endl;
	set<Observer*>::iterator it = observers.begin();
	while (it != observers.end())
	{
		(*it)->update(this->visItem);
		it++;
	}
}

modelSubject::modelSubject()
{
	visItem = new visualModelItem;
}

modelSubject::~modelSubject()
{
}