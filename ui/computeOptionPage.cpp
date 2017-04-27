
#include "computeOptionPage.h"
#include <qwidget.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

//构造函数
emitSource::emitSource(QWidget* parent)
{
	angleOfNorth=90;

	//first group layout
	firstGroup=new QGroupBox(tr("emit source"));

	//firstLayout
	QLabel  *angleLable=new QLabel(QStringLiteral("正北方向在场景坐标系中的phi角度[0,360]"));
	angleEdit= new QLineEdit;
	angleEdit->setText("90");
	QHBoxLayout *firstLayout=new QHBoxLayout;
	firstLayout->addWidget(angleLable);
	firstLayout->addWidget(angleEdit);

	QLabel  *zhandian=new QLabel(QStringLiteral("导入站点参数信息"));
	loadSitesButton=new QPushButton(QStringLiteral("添加"));
	QHBoxLayout *secondLayout=new QHBoxLayout;
	secondLayout->addWidget(zhandian);
	secondLayout->addWidget(loadSitesButton);

	sitesTreewidget=new QTreeWidget;
	QStringList headers;
	headers <<QStringLiteral("小区(site*)") ;
	sitesTreewidget->setHeaderLabels(headers);


	QLabel *zengyi=new QLabel(QStringLiteral("天线方向增益文件"));
	loadTransAntennaButton=new QPushButton(QStringLiteral("添加"));
	QHBoxLayout *thirdLayout=new QHBoxLayout;
	thirdLayout->addWidget(zengyi);
	thirdLayout->addWidget(loadTransAntennaButton);

	AntennaList=new QListWidget;
	
	QVBoxLayout *total=new QVBoxLayout;
	total->addLayout(firstLayout);
	total->addLayout(secondLayout);
	total->addWidget(sitesTreewidget);
	total->addLayout(thirdLayout);
	total->addWidget(AntennaList);

	firstGroup->setLayout(total);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(firstGroup);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}



double emitSource::getAngle()
{
	return angleOfNorth;
}

void fieldpoint::getFieldPoint(double &lx,double &ly,double &rx,double &ry,double &pre,double &alti)
{
	lx=leftupXinput->text().toDouble();
	ly=leftupYinput->text().toDouble();
	rx=rightbottomXinput->text().toDouble();
	ry=rightbottomYinput->text().toDouble();
	pre=Precisioninput->text().toDouble();
	alti=Altitudeinput->text().toDouble();
}
fieldpoint::fieldpoint(QWidget* parent)
{
	//初始化成员变量
	RecePoint="";
	leftupX=0;
	leftupY=0;
	rightbottomX=0;
	rightbottomY=0;
	predictPrecision=0;
	predictAltitude=0;

	//布局
	QGroupBox *firstgroupbox=new QGroupBox(QStringLiteral("仿真区域（单位：m）"));
	//QLabel* label1=new QLabel(QStringLiteral("场景（Xmin，Xmax）"));
	//QLabel* label2=new QLabel(QStringLiteral("场景（Ymin，Ymax）"));
	//QLabel* label3=new QLabel(QStringLiteral("场景（Zmin，Zmax）"));

	//Xmin=new QLineEdit;
	//Xmin->setText(0);
	//Xmax=new QLineEdit;
	//Xmax->setText(0);
	//Ymin=new QLineEdit;
	//Ymin->setText(0);
	//Ymax=new QLineEdit;
	//Ymax->setText(0);
	//Zmin=new QLineEdit;
	//Zmin->setText(0);
	//Zmax=new QLineEdit;
	//Zmax->setText(0);

	/*QHBoxLayout *h1=new QHBoxLayout;
	h1->addWidget(label1);
	h1->addWidget(Xmin);
	h1->addWidget(Xmax);

	QHBoxLayout *h2=new QHBoxLayout;
	h2->addWidget(label2);
	h2->addWidget(Ymin);
	h2->addWidget(Ymax);

	QHBoxLayout *h3=new QHBoxLayout;
	h3->addWidget(label3);
	h3->addWidget(Zmin);
	h3->addWidget(Zmax);*/

	QLabel* label4=new QLabel(QStringLiteral("导入非仿真面接收点位置"));
	QHBoxLayout *h4=new QHBoxLayout;
	filePath=new QLineEdit;
	loadReceieverPointFile=new QPushButton(QStringLiteral("导入"));
	h4->addWidget(label4);
	h4->addWidget(filePath);
	h4->addWidget(loadReceieverPointFile);

	QLabel* label5=new QLabel(QStringLiteral("左上角（X，Y）"));
	leftupXinput=new QLineEdit;
	leftupYinput=new QLineEdit;
	QHBoxLayout *h5=new QHBoxLayout;
	h5->addWidget(label5);
	h5->addWidget(leftupXinput);
	h5->addWidget(leftupYinput);

	QLabel* label6=new QLabel(QStringLiteral("右下角（X，Y）"));
	rightbottomXinput=new QLineEdit;
	rightbottomYinput=new QLineEdit;
	QHBoxLayout*h6=new QHBoxLayout;
	h6->addWidget(label6);
	h6->addWidget(rightbottomXinput);
	h6->addWidget(rightbottomYinput);

	QLabel *label7=new QLabel(QStringLiteral("预测精度"));
	Precisioninput=new QLineEdit;
	QHBoxLayout *h7=new QHBoxLayout;
	h7->addWidget(label7);
	h7->addWidget(Precisioninput);

	QLabel *label8=new QLabel(QStringLiteral("预测高度"));
	Altitudeinput=new QLineEdit;
	QHBoxLayout *h8=new QHBoxLayout;
	h8->addWidget(label8);
	h8->addWidget(Altitudeinput);

	QVBoxLayout *total=new QVBoxLayout;
	/*total->addLayout(h1);
	total->addLayout(h2);
	total->addLayout(h3);*/
	total->addLayout(h4);
	total->addLayout(h5);
	total->addLayout(h6);
	total->addLayout(h7);
	total->addLayout(h8);

	firstgroupbox->setLayout(total);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(firstgroupbox);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

	leftupXinput->setText("0");
	leftupYinput->setText("0");
	rightbottomXinput->setText("0");
	rightbottomYinput->setText("0");
	Precisioninput->setText("0");
	Altitudeinput->setText("0");
}

simuArgument::simuArgument(QWidget* parent)
{
	reflectNum=0;
	refractNum=0;
	diffractionNum=0;
	isDiffraction=false;

	QGroupBox *firstgroupbox=new QGroupBox(QStringLiteral("仿真参数"));
	QLabel *label1=new QLabel(QStringLiteral("反射次数"));
	QLabel*label2=new QLabel(QStringLiteral("透射次数"));
	QLabel*label2_5=new QLabel(QStringLiteral("透射损耗"));
	QLabel*label3=new QLabel(QStringLiteral("绕射"));
	QLabel*label4=new QLabel(QStringLiteral("绕射次数"));
	QLabel*label5=new QLabel(QStringLiteral("采样密度"));
	QLabel*label6=new QLabel(QStringLiteral("采样球半径"));
	QLabel*label7=new QLabel(QStringLiteral("Beam数量"));

	lineEdit_RT_ReflectCount=new QLineEdit;
	lineEdit_RT_RefractCount=new QLineEdit;
	lineEdit_RT_Diffraction=new QLineEdit;
	comboBox_transmission=new QComboBox;
	comboBox_diffraction=new QComboBox;
	lineEdit_RT_sample=new QLineEdit;
	lineEdit_RT_sample_radius=new QLineEdit;
	lineEdit_RT_sampleBeamNum=new QLineEdit;


	comboBox_diffraction->insertItem(0,QStringLiteral("不考虑绕射"));
	comboBox_diffraction->insertItem(1,QStringLiteral("考虑绕射"));

	comboBox_transmission->insertItem(0,QStringLiteral("经验值"));
	comboBox_transmission->insertItem(1,QStringLiteral("理论公式计算"));

	QHBoxLayout* h1=new QHBoxLayout;
	h1->addWidget(label1);
	h1->addWidget(lineEdit_RT_ReflectCount);

	QHBoxLayout* h2=new QHBoxLayout;
	h2->addWidget(label2);
	h2->addWidget(lineEdit_RT_RefractCount);

	QHBoxLayout *h2_5=new QHBoxLayout;
	h2_5->addWidget(label2_5);
	h2_5->addWidget(comboBox_transmission);

	QHBoxLayout *h3=new QHBoxLayout;
	h3->addWidget(label3);
	h3->addWidget(comboBox_diffraction);

	QHBoxLayout*h4=new QHBoxLayout;
	h4->addWidget(label4);
	h4->addWidget(lineEdit_RT_Diffraction);

	QHBoxLayout*h5=new QHBoxLayout;
	h5->addWidget(label5);
	h5->addWidget(lineEdit_RT_sample);

	QHBoxLayout*h6=new QHBoxLayout;
	h6->addWidget(label6);
	h6->addWidget(lineEdit_RT_sample_radius);

	QHBoxLayout*h7=new QHBoxLayout;
	h7->addWidget(label7);
	h7->addWidget(lineEdit_RT_sampleBeamNum);

	QVBoxLayout *total=new QVBoxLayout;
	total->addLayout(h1);
	total->addLayout(h2);
	total->addLayout(h2_5);
	total->addLayout(h3);
	total->addLayout(h4);
	total->addLayout(h5);
	total->addLayout(h6);
	total->addLayout(h7);

	firstgroupbox->setLayout(total);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(firstgroupbox);
	mainLayout->addStretch(1);


	lineEdit_RT_Diffraction->setText("0");
	lineEdit_RT_sampleBeamNum->setText("0");

	setLayout(mainLayout);
}

void simuArgument::getSimuArgu(double &reflect,double &refract,double &diffract,bool &isDiff,int transIndex,double &sample,double &radius,double &beamNum)
{
	refract=lineEdit_RT_ReflectCount->text().toDouble();
	refract=lineEdit_RT_ReflectCount->text().toDouble();
	diffract=lineEdit_RT_RefractCount->text().toDouble();
	if (comboBox_diffraction->currentIndex()==0)
	{
		isDiff=false;
	}else
		isDiff=true;
	transIndex=comboBox_transmission->currentIndex();
	sample=lineEdit_RT_sample->text().toDouble();
	radius=lineEdit_RT_sample_radius->text().toDouble();
	beamNum=lineEdit_RT_sampleBeamNum->text().toDouble();
}