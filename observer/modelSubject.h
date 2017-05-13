#pragma once

#include "subject.h"
#include "../visualData/visualModelItem.h"
class modelSubject:public Subject
{
public:
	explicit modelSubject();
	~modelSubject();
	void attach(Observer*);
	void detach(Observer*);
	void notify();
	//模型信息展示项
	visualModelItem *visItem;
private:

};

