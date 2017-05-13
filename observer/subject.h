#pragma once
#include <string>
#include <set>

class Observer;


using namespace std;
class Subject
{
protected:
	set<Observer*> observers;
public:
	string action;
	virtual void attach(Observer*) = 0;
	virtual void detach(Observer*) = 0;
	virtual void notify() = 0;
};