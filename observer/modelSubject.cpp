#include "modelSubject.h"
#include "../observer/observer.h"

void modelSubject::attach(Observer*a)
{
	observers.insert(a);
	cout << "��ӹ۲��߳ɹ� "<<a->name << endl;
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
	cout << "subject��ʼ������Ϣ" << endl;
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