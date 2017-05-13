#include "mainwindow.h"
#include <QtWidgets/QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include "Antenna/receiver.h"
#include "io/io_simuPlane.h"
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	init();
	createActions();
}

MainWindow::~MainWindow()
{

}

void MainWindow::init()
{
	M_outdoorFileDialog=new outdoorFileDialog(this);
	M_computeroptionDialog=new computerOptionDialog(this);

	mod=new meshOptionDialog(this);

	//将表格嵌入到dockwidget
	modelTable = new scsModelTable;
	modelTable->showMaximized();
//	QWidget* tmpWidget = new QWidget();
	//QVBoxLayout* layout = new QVBoxLayout();
	//layout->addWidget(modelTable);
	//tmpWidget->setLayout(layout);
	ui.dockWidget_Property->setWidget(modelTable);
	globalContext *gctx = globalContext::GetInstance();
	gctx->modelManager->getModelSubject()->attach(modelTable);
	//场景数据初始化


	 plugin_file_path="";



	material_ID=43;//默认是混凝土文件

	//左侧目录
	ui.treeWidget_project->setHeaderLabels(QStringList()<<QStringLiteral("项目")<<QStringLiteral("属性")); 
	modelTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("模型"));
	computeTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("电磁算法"));
	visualTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("可视化"));
	ui.treeWidget_project->addTopLevelItem(modelTW);
	ui.treeWidget_project->addTopLevelItem(computeTW);
	ui.treeWidget_project->addTopLevelItem(visualTW);

	ui.progressBar->setRange(0,100);

	//设置对话框的默认数据
	M_computeroptionDialog->sa->lineEdit_RT_ReflectCount->setText(QString::number(2));
	M_computeroptionDialog->sa->lineEdit_RT_RefractCount->setText(QString::number(1));
	M_computeroptionDialog->sa->lineEdit_RT_RefractCount->setReadOnly(true);
	M_computeroptionDialog->sa->lineEdit_RT_sample->setText(QString::number(1));
	M_computeroptionDialog->sa->lineEdit_RT_sample_radius->setText(QString::number(1.0));


}

void  MainWindow::openOutdoorFile()
{
	if (M_outdoorFileDialog==NULL)
	{
		M_outdoorFileDialog=new outdoorFileDialog(this);
	}
	M_outdoorFileDialog->exec();

}

void MainWindow::computerOption()
{
	if (M_computeroptionDialog==NULL)
	{
		M_computeroptionDialog=new computerOptionDialog(this);
	}
	M_computeroptionDialog->exec();
}

void MainWindow::setProgress(int value)
{
	ui.progressBar->setValue(value);
}

void MainWindow:: createActions()
{
		connect(ui.action_3, SIGNAL(triggered()), this, SLOT(openOutdoorFile()));
		connect(ui.computerOption,SIGNAL(triggered()),this,SLOT(computerOption()));
		connect(M_outdoorFileDialog,SIGNAL(fileIsOK(QString,QStringList,QStringList ,QString )),this,SLOT(loadAllFile(QString,QStringList ,QStringList ,QString )));
		connect(ui.action_ShowAll,SIGNAL(triggered()),this,SLOT(showAll()));
		connect(ui.action_localscene,SIGNAL(triggered()),this,SLOT(showLocal()));
		connect(ui.action_obj,SIGNAL(triggered()),this,SLOT(loadObj()));
		connect(ui.action_9,SIGNAL(triggered()),this,SLOT(setMaterial()));
		connect(ui.action_matFile,SIGNAL(triggered()),this,SLOT(open_material()));
		connect(ui.action_SaveSimuPlane,SIGNAL(triggered()),this,SLOT(saveSimuPlane()));
		connect(ui.action_loadSimuPlane,SIGNAL(triggered()),this,SLOT(loadSimuPlane()));
		connect(ui.action_6,SIGNAL(triggered()),this,SLOT(setMeshOption()));
		connect(ui.action_startMesh,SIGNAL(triggered()),this,SLOT(meshAll()));
		connect(ui.action_saveLocal,SIGNAL(triggered()),this,SLOT(saveLocalScene()));
		connect(M_computeroptionDialog->es->loadSitesButton,SIGNAL(clicked()),this,SLOT(openTransAntenna_ParamFile()));
		connect(M_computeroptionDialog->es->loadTransAntennaButton,SIGNAL(clicked()),this,SLOT(openTransAntennas_DirGain()));
		connect(M_computeroptionDialog->fp->loadReceieverPointFile,SIGNAL(clicked()),this,SLOT(openNo_SimplaneReceiverFile()));
       	connect(ui.action_loadPlugin,SIGNAL(triggered()),this,SLOT(loadPlugin()));
		connect(ui.action_run,SIGNAL(triggered()),this,SLOT(run()));
		connect(ui.action_json,SIGNAL(triggered()),this,SLOT(quickLoadJson()));
}

void MainWindow::saveSimuPlane()
{
	QString filepath=QFileDialog::getSaveFileName(this,QStringLiteral("保存为"),"",tr("*.sp"));
	if (filepath.isEmpty())
	{
		outputLog(QStringLiteral("获取保存路径失败！"));
		return;
	}
	globalContext *globalCtx=globalContext::GetInstance();
	if (globalCtx->visualPara->vis_AP_EFieldArrays.size()==0)
	{
		outputLog(QStringLiteral("没有仿真面生成！"));
		return;	
	}
	saveSimuPlaneResult(globalCtx->visualPara->vis_AP_EFieldArrays,globalCtx->visualPara->veticalNum,globalCtx->visualPara->horizonNum,filepath.toStdString());
}
void MainWindow::loadSimuPlane()
{
	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("打开仿真面文件"),"./",QStringLiteral("sp 仿真面文件 (*.sp)"));
	if (path.isEmpty())
	{
		outputLog(QStringLiteral("仿真面路径错误！"));
		return;
	}
	globalContext *globalCtx=globalContext::GetInstance();
	loadSimuPlaneResult(globalCtx->visualPara->vis_AP_EFieldArrays,globalCtx->visualPara->veticalNum,globalCtx->visualPara->horizonNum ,path);
	ui.simuPlane->setSimPlane(globalCtx->visualPara->vis_AP_EFieldArrays,globalCtx->visualPara->horizonNum,globalCtx->visualPara->veticalNum);
	ui.simuArea->setSimPlane(globalCtx->visualPara->vis_AP_EFieldArrays,globalCtx->visualPara->horizonNum,globalCtx->visualPara->veticalNum);
	outputLog(QStringLiteral("显示结果"));
}

void MainWindow::saveLocalScene()
{
	
}

void MainWindow::loadMaterial(QString path)
{

}

void MainWindow::open_material()
{
	
	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("打开材质文件"),"./",QStringLiteral("txt 材质文件 (*.txt)"));
	loadMaterial(path);
	
}

void MainWindow::setMeshOption()
{

}

void MainWindow::setMaterial()
{
	bool ok;
	int i = QInputDialog::getInt(this, QStringLiteral("材质信息"),
		QStringLiteral("编号:"), 10, 0, 78, 1, &ok);
	if (ok)
		material_ID=i;
	outputLog(QString(QStringLiteral("设置材质编号为：")+QString::number(material_ID,10)));
}

int  MainWindow::getDefaultMaterial(){
	return material_ID;
}

/************************************************************************/
/* 根据路径，读取文件，存放到对应的变量中                                              */
/************************************************************************/
void MainWindow::loadAllFile(QString _name,QStringList _v,QStringList _h,QString _p){
	
}

void MainWindow::outputLog(QString source)
{
	ui.textBrowser->append(source);
}

//index==0为城市场景 index==1为obj
void MainWindow::setModelName(int index,QString name)
{
	 QTreeWidgetItem *child;  
	 QStringList columItemList;
	 if (index==0)
	 {
		 columItemList<<QStringLiteral("城市场景")<<name;
	 }else if (index==1)
	 {
		 columItemList<<QStringLiteral("局部场景")<<name;
	 }
	 
	 child=new QTreeWidgetItem(columItemList);
	 QTreeWidgetItem* temp=ui.treeWidget_project->itemAt(0,0);
	 temp->addChild(child);
}

string Trim(string &str)   //提取不包含空格、制表符、回车、换行符的字符串
{
	str.erase(0,str.find_first_not_of(" \t\n\r"));    

	str.erase(str.find_last_not_of(" \t\n\r") + 1);

	return str;
}

void MainWindow::openTransAntennas_DirGain()
{

}

void MainWindow::openTransAntenna_ParamFile()
{

}

void MainWindow::openNo_SimplaneReceiverFile()
{

}


/************************************************************************/
/*    展示全部场景                                                                                       */
/************************************************************************/
void MainWindow::showAll()
{

}

void MainWindow::showLocal()
{
	
}

/************************************************************************/
/* 导入obj文件                                                                     */
/************************************************************************/
void MainWindow::loadObj()
{
	
}

/************************************************************************/
/* 剖分选中的地面和建筑物                                                                     */
/************************************************************************/
void MainWindow::meshAll()
{
	
}






void MainWindow::loadPlugin()
{
	plugin_file_path = QFileDialog::getOpenFileName(this,QStringLiteral("加载算法插件"),"./dll_plugins/","*.dll");  
	if (!plugin_file_path.isEmpty())
	{
		outputLog(QStringLiteral("插件加载成功"));
	}else
	{
		outputLog(QStringLiteral("插件加载失败"));
	}
}

void MainWindow::deletePlugin()
{
	
}

void MainWindow::run()
{
	QObject* object ;
	QPluginLoader loader( plugin_file_path); 

	globalContext *gctx=globalContext::GetInstance();
	if ((object=loader.instance())!=NULL)
	{
		ComputeInterface * pluginTemp=qobject_cast<ComputeInterface*>(object);
		if (pluginTemp)
		{
			outputLog(QStringLiteral("开始运行计算函数"));	
			pluginTemp->runAlgorithm(gctx->modelPara,gctx->cptPara,gctx->visualPara);
		    outputLog(QStringLiteral("结束计算"));
			ui.simuPlane->setSimPlane(gctx->visualPara->vis_AP_EFieldArrays,gctx->visualPara->horizonNum,gctx->visualPara->veticalNum);
			outputLog(QStringLiteral("显示结果"));
		}
	}
}


/************************************************************************/
/* 读取json文件    参考 http://bbs.itheima.com/thread-331656-1-1.html */
/************************************************************************/
void MainWindow::quickLoadJson()
{

	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("快速导入场景"),"./",QStringLiteral("场景文件 (*.json)"));
	globalContext *gctx = globalContext::GetInstance();
	gctx->modelManager->loadCityModel(path.toStdString());


}