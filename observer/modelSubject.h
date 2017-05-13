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
	//ģ����Ϣչʾ��
	visualModelItem *visItem;
private:

};

