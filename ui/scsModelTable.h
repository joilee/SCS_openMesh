#pragma once
#include <QtGui> 
#include <QTableWidget>
#include "../observer/observer.h"
class scsModelTable:public QWidget,public Observer
{
	Q_OBJECT
public:
	scsModelTable(QWidget* a=NULL);
	~scsModelTable();
	void update(visualModelItem*a);
private:
	QTableWidget *table;
	void clearCityItem();
	void clearLocalItem();
};

