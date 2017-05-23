#pragma once
#include <QtGui> 
#include <QTableWidget>
#include "../observer/observer.h"
class scsModelTable :public QTableWidget, public Observer
{
	Q_OBJECT
public:
	scsModelTable(QWidget* a=NULL);
	~scsModelTable();
	void update(visualModelItem*a);
private:
	 //*table;
	void clearCityItem();
	void clearLocalItem();
};

