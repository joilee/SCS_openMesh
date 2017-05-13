#include "scsModelTable.h"
#include <QHeaderView>
#include <sstream>  

scsModelTable::scsModelTable(QWidget* a ) :Observer("scsModelTable"), QWidget(a)
{
	//QWidget::showMaximized();
	table = new QTableWidget(this);
	QStringList headers;
	headers << "属性" << "值";
	table->setHorizontalHeaderLabels(headers);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setVisible(false);   //隐藏列表头  
	table->setRowCount(15);
	table->setColumnCount(2);
	table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("建筑物数量")));
	table->setItem(1, 0, new QTableWidgetItem(QStringLiteral("凹建筑物")));
	table->setItem(2, 0, new QTableWidgetItem(QStringLiteral("城市场景Xmin")));
	table->setItem(3, 0, new QTableWidgetItem(QStringLiteral("城市场景Xmax")));
	table->setItem(4, 0, new QTableWidgetItem(QStringLiteral("城市场景Ymin")));
	table->setItem(5, 0, new QTableWidgetItem(QStringLiteral("城市场景Ymax")));
	table->setItem(6, 0, new QTableWidgetItem(QStringLiteral("城市场景Zmin")));
	table->setItem(7, 0, new QTableWidgetItem(QStringLiteral("城市场景Zmax")));
	table->setItem(8, 0, new QTableWidgetItem(QStringLiteral("局部场景三角面片数量")));
	table->setItem(9, 0, new QTableWidgetItem(QStringLiteral("局部场景Xmin")));
	table->setItem(10, 0, new QTableWidgetItem(QStringLiteral("局部场景Xmax")));
	table->setItem(11, 0, new QTableWidgetItem(QStringLiteral("局部场景Ymin")));
	table->setItem(12, 0, new QTableWidgetItem(QStringLiteral("局部场景Ymax")));
	table->setItem(13, 0, new QTableWidgetItem(QStringLiteral("局部场景Zmin")));
	table->setItem(14, 0, new QTableWidgetItem(QStringLiteral("局部场景Zmax")));

}

scsModelTable::~scsModelTable()
{

}

void scsModelTable::clearCityItem()
{
	table->setItem(0, 1, new QTableWidgetItem(QString("")));
	table->setItem(1, 1, new QTableWidgetItem(QString("")));
	table->setItem(2, 1, new QTableWidgetItem(QString("")));
	table->setItem(3, 1, new QTableWidgetItem(QString("")));
	table->setItem(4, 1, new QTableWidgetItem(QString("")));
	table->setItem(5, 1, new QTableWidgetItem(QString("")));
	table->setItem(6, 1, new QTableWidgetItem(QString("")));
	table->setItem(7, 1, new QTableWidgetItem(QString("")));
}

void scsModelTable::clearLocalItem()
{
	table->setItem(8, 1, new QTableWidgetItem(QString("")));
	table->setItem(9, 1, new QTableWidgetItem(QString("")));
	table->setItem(10, 1, new QTableWidgetItem(QString("")));
	table->setItem(11, 1, new QTableWidgetItem(QString("")));
	table->setItem(12, 1, new QTableWidgetItem(QString("")));
	table->setItem(13, 1, new QTableWidgetItem(QString("")));
	table->setItem(14, 1, new QTableWidgetItem(QString("")));
}

void scsModelTable::update(visualModelItem*a)
{
	cout << "ModelTable 接收到更新的信号" << endl;
	if (a->getFlag()[0]!="")
	{
		cout << table->rowCount() << " " << table->columnCount() << endl;
	//	table->setItem(0, 1, new QTableWidgetItem(a->getBuildingNum()));
		table->setItem(0, 1, new QTableWidgetItem(QString::number(a->getBuildingNum())));
		table->setItem(1, 1, new QTableWidgetItem(QString::number(a->getConcaveNum())));
		table->setItem(2, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMin()[0], 0, 'f', 3)));
		table->setItem(3, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMin()[1], 0, 'f', 3)));
		table->setItem(4, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMin()[2], 0, 'f', 3)));
		table->setItem(5, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMax()[0], 0, 'f', 3)));
		table->setItem(6, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMax()[1], 0, 'f', 3)));
		table->setItem(7, 1, new QTableWidgetItem(QString("%1").arg(a->getcityMax()[2], 0, 'f', 3)));
	}
	else
	{
		clearCityItem();
	}

	if (a->getFlag()[1] != "")
	{
		table->setItem(8, 1, new QTableWidgetItem(a->getLocalFaceNum()));
		table->setItem(9, 1, new QTableWidgetItem(a->getlocalMin()[0]));
		table->setItem(10, 1, new QTableWidgetItem(a->getlocalMin()[1]));
		table->setItem(11, 1, new QTableWidgetItem(a->getlocalMin()[2]));
		table->setItem(12, 1, new QTableWidgetItem(a->getlocalMax()[0]));
		table->setItem(13, 1, new QTableWidgetItem(a->getlocalMax()[1]));
		table->setItem(14, 1, new QTableWidgetItem(a->getlocalMax()[2]));
	}
	else
	{
		clearLocalItem();
	}
	table->show();
	cout << "ModelTable数据已经更新" << endl;
}