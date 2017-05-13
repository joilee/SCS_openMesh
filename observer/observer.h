
#pragma once
#include <string>
#include "../visualData/visualModelItem.h"
using namespace std;

class Subject;
//³éÏó¹Û²ìÕß
class Observer
{
public:
	Observer(string name)
	{
		this->name = name;
	}
	string getName(){ return name; }
	string name;
	virtual void update(visualModelItem*a) = 0;
};