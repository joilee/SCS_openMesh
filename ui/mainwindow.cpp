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
	bip=new BuildingInfoPage(this);
	lpg=new localPage(this);
	mod=new meshOptionDialog(this);
	//场景数据初始化
	 MaxPoint=Vector3d(-INFINITY,-INFINITY,-INFINITY);
	 MinPoint=Vector3d(INFINITY,INFINITY,INFINITY);

	 plugin_file_path="";

	//triangleModel=NULL;
	total_Buildings.clear();

	material_ID=43;//默认是混凝土文件

	//左侧目录
	ui.treeWidget_project->setHeaderLabels(QStringList()<<QStringLiteral("项目")<<QStringLiteral("属性")); 
	modelTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("模型"));
	computeTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("电磁算法"));
	visualTW = new QTreeWidgetItem(QStringList()<<QStringLiteral("可视化"));
	ui.treeWidget_project->addTopLevelItem(modelTW);
	ui.treeWidget_project->addTopLevelItem(computeTW);
	ui.treeWidget_project->addTopLevelItem(visualTW);

	ui.treeWidget_para->setHeaderLabels(QStringList()<<QStringLiteral("站点")); 



	ui.dockWidget_2->setWidget(bip);
	ui.dockWidget_localsecene->setWidget(lpg);
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
	QString filepath=QFileDialog::getSaveFileName(this,QStringLiteral("保存为"),"",tr("*.obj"));
	if (filepath.isEmpty())
	{
		outputLog(QStringLiteral("获取保存路径失败！"));
		return;
	}
	globalContext *globalCtx=globalContext::GetInstance();
	if (globalCtx->modelPara->trianglePara==NULL||!globalCtx->modelPara->localexist)
	{
		outputLog(QStringLiteral("没有局部场景生成！"));
		return;
	}
	//obj文件输出
	QFile f(filepath);
	if(!f.open(QIODevice::WriteOnly | QIODevice::Text))  
	{  
		  QMessageBox::critical(NULL, QStringLiteral("提示"), QStringLiteral("无法创建文件"));  		
		  return;  
	} 
	QTextStream txtOutput(&f);  
	//点的编号从1开始
	txtOutput.setRealNumberPrecision(10);
	
	emxModel *triangleModel=globalCtx->modelPara->trianglePara;
	for (int i=0;i<triangleModel->NumV();i++)
	{
		Vector3d point=triangleModel->GetVertex(i);
		txtOutput<<"v "<<point.x<<" "<<point.y<<" "<<point.z<<endl;
	}
	for (int i=0;i<triangleModel->NumF();i++)
	{
		Vector3i tri=triangleModel->GetFace(i);
		txtOutput<<"f "<<tri.x+1<<" "<<tri.y+1<<" "<<tri.z+1<<endl;
	}
	txtOutput.flush();
	f.close();
	outputLog(QStringLiteral("保存obj文件成功"));
}

void MainWindow::loadMaterial(QString path)
{
	setProgress(0);
	if (path.isEmpty())
	{
		outputLog(QStringLiteral("材料路径错误！"));
		return;
	}
	material_path=path;
	globalContext *globalCtx=globalContext::GetInstance();

	globalCtx->modelPara->materialdatabase.clear();  //存放材质属性的vector容器
	setProgress(10);
	ifstream infile((path.toStdString()).c_str(),ios::in|ios::_Nocreate);
	if(!infile)
	{
		return;
	}
	string str;
	getline(infile,str);
	string unit;
	string sub_name; //创建String对象，调用默认构造函数初始化为空字符串
	while(getline(infile,str))
	{
		material new_material;
		istringstream linestream(str);
		linestream>>new_material.Id;
		linestream>>sub_name;
		while (sub_name != "thickness:")
		{
			sub_name = " "+ sub_name;
			new_material.name += sub_name;
			linestream>>sub_name;
		}
		linestream>>new_material.thickness>>unit>>new_material.color.r>>new_material.color.g>>new_material.color.b>>new_material.frequency>>new_material.dielectric>>new_material.permeability>>new_material.conductivity>>new_material.Transmission_Loss>>new_material.Reflection_Loss;
		if (unit == "m.\"")
		{
		}
		else if(unit == "cm.\"")
		{
			new_material.thickness/=100;
		}
		else if (unit == "mm.\"")
		{
			new_material.thickness/=1000;
		}
		globalCtx->modelPara->materialdatabase.push_back(new_material);
		setProgress(70);
	}
	infile.close();
	setProgress(100);
	outputLog(QStringLiteral("成功加载材质文件!"));
}

void MainWindow::open_material()
{
	
	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("打开材质文件"),"./",QStringLiteral("txt 材质文件 (*.txt)"));
	loadMaterial(path);
	
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
	setProgress(0);
	buildingName=_name;
	Scene2DInfoFile_paths=_v;
	SceneHeightInfoFile_paths=_h;
	ScenePlaneHeightInfoFile_path=_p;
	if (ScenePlaneHeightInfoFile_path.isEmpty())
	{
		QMessageBox::warning(this, QStringLiteral("文件导入"), QStringLiteral("请先导入海拔信息文件"));
		return;
	}
	if (Scene2DInfoFile_paths.isEmpty())
	{
		QMessageBox::warning(this, QStringLiteral("文件导入"),QStringLiteral("请先导入建筑物二维信息文件"));
		return;
	}
	if (SceneHeightInfoFile_paths.isEmpty())
	{
		QMessageBox::warning(this, QStringLiteral("文件导入"),QStringLiteral("请先导入建筑物高度信息文件"));
		return;
	}
	setProgress(20);
	//场景地面海拔文件读取
	globalContext *gctx=globalContext::GetInstance();
	readHeightGrid(ScenePlaneHeightInfoFile_path.toStdString(),gctx->modelPara->heightR,gctx->modelPara->rowNum,gctx->modelPara->colNum,gctx->modelPara->stdLen,gctx->modelPara->xmin,gctx->modelPara->ymax);
	setProgress(60);
#ifdef PRINT_MODE
	cout<<"读海拔文件完毕"<<endl;
#endif

	for (int i = 0; i < Scene2DInfoFile_paths.size();i++)
	{
		Scene2DInfoFile_path = Scene2DInfoFile_paths.at(i);
		SceneHeightInfoFile_path = SceneHeightInfoFile_paths.at(i);
		ReadScenePlanetFile(Scene2DInfoFile_path.toStdString().c_str(),SceneHeightInfoFile_path.toStdString().c_str(),MaxPoint,MinPoint);
	}
	setProgress(90);
	//cout<<"buildings num:"<<total_Buildings.size()<<endl;
	//cout<<"MinPoint:"<<MinPoint.x<<" "<<MinPoint.y<<" "<<MinPoint.z<<endl;
	//cout<<"MaxPoint:"<<MaxPoint.x<<" "<<MaxPoint.y<<" "<<MaxPoint.z<<endl;

	globalContext *globalCtx=globalContext::GetInstance();
	globalCtx->modelPara->modelexist=true;
	setModelName(0,_name);
	outputLog(QStringLiteral("已经导入整个场景"));
	bip->setValue(-1,total_Buildings.size(),MinPoint,MaxPoint);

	setProgress(100);
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
	QStringList paths = QFileDialog::getOpenFileNames(this,QStringLiteral("批量导入天线方向增益文件"),"./",QStringLiteral("txt  天线方向增益文件 (*.txt)"));
	if (paths.isEmpty())
		return;
	globalContext *glbctx=globalContext::GetInstance();
	for (int i=0;i<glbctx->cptPara->Sites.size();i++)
	{
		for (int j=0;j<glbctx->cptPara->Sites[i].Site_Antennas.size();j++)
		{
			//对每个site中每个cell从批量导入的方向增益文件中找到匹配的增益文件
			string cell_name = glbctx->cptPara->Sites[i].Site_Antennas[j].Cell_Name + ".txt";
			for (int path_id =0; path_id<paths.size();path_id++)
			{
				string path = paths[path_id].toStdString();
				if (path.find(cell_name) != std::string::npos)
				{
					//打开发射天线方向增益文件，读取各方向增益
					ifstream infile(path.c_str(),ios::in|ios::_Nocreate);  
					if(!infile)
					{
						cout << "can not open file!" << endl;
						return ;
					}

					string str,str_flag;
					getline(infile,str);
					istringstream linestream(str);
					linestream >> str_flag;
					if (str_flag == "NAME" )
					{
						getline(infile,str);
						getline(infile,str);
					}
					getline(infile,str);
					istringstream linestream1(str);
					linestream1 >> str_flag;
					if (str_flag == "Gain")
					{
						getline(infile,str);
						istringstream linestream2(str);
						linestream2 >> str_flag;
						glbctx->cptPara->Sites[i].Site_Antennas[j].initial_Gain = atof(str_flag.c_str());
						getline(infile,str);
						getline(infile,str);
						getline(infile,str);
					}
					while(getline(infile,str))
					{
						istringstream linestream3(str);
						linestream3 >> str_flag;
						vector<double> antenna_property(3); // V_angle  H_angle  attenuation
						antenna_property[0] = atof(str_flag.c_str());
						linestream3 >> antenna_property[1] >> antenna_property[2];
						glbctx->cptPara->Sites[i].Site_Antennas[j].direction_Gain.push_back(antenna_property);	
					}
					infile.close();

					break;
				}
			}
		}
	}
	QMessageBox::warning(this, QStringLiteral("多个站点方向增益文件"), QStringLiteral("加载成功"));
}

void MainWindow::openTransAntenna_ParamFile()
{
	globalContext *globalCtx=globalContext::GetInstance();
	globalCtx->cptPara->Sites.clear();

	site_roots1.clear();
	site_roots2.clear();
	ui.treeWidget_para->clear();
	M_computeroptionDialog->es->sitesTreewidget->clear();

	//检测是否导入场景
	if (!globalCtx->modelPara->localexist&&!globalCtx->modelPara->modelexist)
	{
		QMessageBox::warning(this, QStringLiteral("发射天线设置"), QStringLiteral("请先加载场景"));
		return;
	}
	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("导入发射天线（站点）参数信息文件"),"./",QStringLiteral("csv 发射天线（站点）参数信息文件 (*.csv)"));
	if (path.isEmpty())
		return;

	//打开发射天线(站点)参数信息文件
	ifstream infile((path.toStdString()).c_str(),ios::in|ios::_Nocreate);  
	if(!infile)
	{
		cout << "can not open file!" << endl;
		return ;
	}
	globalContext *gctx=globalContext::GetInstance();
	string str,str_flag;
	getline(infile,str);//跳过第一行
	while(getline(infile,str))
	{
		istringstream linestream(str);
		vector<string>parameters;
		string parameter;
		while (getline(linestream,parameter,','))
		{
			parameters.push_back(parameter);
		}
		string SiteName = Trim(parameters[0]);
		string CellName = Trim(parameters[1]);
		string PCI = Trim(parameters[3]);
		string height = Trim(parameters[8]);
		string str_x = Trim(parameters[14]);
		string str_y = Trim(parameters[15]);
		double x = atof(str_x.c_str());
		double y = atof(str_y.c_str());
		double z = atof(height.c_str()) + getPointAltitude(gctx->modelPara->heightR[0],x,y,gctx->modelPara->rowNum[0],gctx->modelPara->colNum[0],gctx->modelPara->stdLen[0],gctx->modelPara->xmin[0],gctx->modelPara->ymax[0]);

		TransAntenna new_antenna;
		new_antenna.Cell_Name = CellName;
		new_antenna.PCI = atoi(PCI.c_str());
		new_antenna.frequency = 1750; //单位为MHZ，设置了一个默认
		new_antenna.trans_power = 12.2; //单位为dBm
		new_antenna.wire_loss = 0.5;
		new_antenna.enlarge_power = 0;
		new_antenna.position = Vector3d(x,y,z);
		new_antenna.polor_direction = Vector3d(0,0,1);

		new_antenna.phi = 0;
		new_antenna.theta = 0;

		if(new_antenna.polor_direction.norm() < 1e-10)
		{
			QMessageBox::warning(this, QStringLiteral("发射天线属性设置"), QStringLiteral("请输入正确的极化方向"));
			return;
		}

		QTreeWidgetItem * site_root1, *cell_leaf1,*site_root2, *cell_leaf2;
		int current_sitename = stof(SiteName.c_str());

		//针对新获得的cell，检测是否存在一个已知的site中，如果是，则插入，否则新建
		bool newsite = true;   //是否需要新建一个site
		for (int i=0;i<globalCtx->cptPara->Sites.size();i++)
		{
			if (current_sitename==globalCtx->cptPara->Sites[i].Site_Name)
			{
				newsite=false;
				globalCtx->cptPara->Sites[i].Site_Antennas.push_back(new_antenna);
				string cell_name = "Cell"+CellName;

				cell_leaf1 = new QTreeWidgetItem(site_roots1[i],QStringList(QString::fromStdString(cell_name)));
				site_roots1[i]->addChild(cell_leaf1);

				cell_leaf2 = new QTreeWidgetItem(site_roots2[i],QStringList(QString::fromStdString(cell_name)));
				cell_leaf2->setCheckState(0,Qt::Unchecked);
				site_roots2[i]->addChild(cell_leaf2);
				break;
			}
		}
		if (newsite)
		{
			Site new_site;
			new_site.Site_Name = current_sitename;
			new_site.Site_Antennas.push_back(new_antenna);	
			globalCtx->cptPara->Sites.push_back(new_site);		
			string site_name = "Site"+SiteName;
			string cell_name = "Cell"+CellName;

			site_root1 = new QTreeWidgetItem(ui.treeWidget_para,QStringList(QString::fromStdString(site_name)));
			cell_leaf1 = new QTreeWidgetItem(site_root1,QStringList(QString::fromStdString(cell_name)));		
			site_root1->addChild(cell_leaf1);
			site_roots1.push_back(site_root1);

			site_root2 = new QTreeWidgetItem(M_computeroptionDialog->es->sitesTreewidget,QStringList(QString::fromStdString(site_name)));
			cell_leaf2 = new QTreeWidgetItem(site_root2,QStringList(QString::fromStdString(cell_name)));
			cell_leaf2->setCheckState(0,Qt::Unchecked);
			site_root2->addChild(cell_leaf2);
			site_roots2.push_back(site_root2);

			ui.treeWidget_para->addTopLevelItem(site_root1);
			M_computeroptionDialog->es->sitesTreewidget->addTopLevelItem(site_root2);
		}

	}
	infile.close();
	outputLog(QStringLiteral("加载站点数据成功！"));
}

void MainWindow::openNo_SimplaneReceiverFile()
{
	QString path = QFileDialog::getOpenFileName(this,QStringLiteral("打开非仿真面设置的接收点文件"),"./",QStringLiteral("csv 非仿真面接收点文件 (*.csv)"));
	if (path.isEmpty())
		return;

	globalContext *glbctx=globalContext::GetInstance();

	glbctx->cptPara->No_SimPlanePoint.clear();  //存放接收点信息的vector容器

	ifstream infile((path.toStdString()).c_str(),ios::in|ios::_Nocreate);
	if(!infile)
	{
		cout << "can not open file!" << endl;
		return ;
	}

	string str;
	getline(infile,str);
	globalContext *gctx=globalContext::GetInstance();
	while(getline(infile,str))
	{
		istringstream linestream(str);
		vector<string>parameters;
		string parameter;
		while (getline(linestream,parameter,','))
		{
			parameters.push_back(parameter);
		}
		string str_x = Trim(parameters[0]);
		string str_y = Trim(parameters[1]);
		string str_z = Trim(parameters[2]);
		string PCI = Trim(parameters[3]);
		double x = atof(str_x.c_str());
		double y = atof(str_y.c_str());
		double z = atof(str_z.c_str()) + getPointAltitude(gctx->modelPara->heightR[0],x,y,gctx->modelPara->rowNum[0],gctx->modelPara->colNum[0],gctx->modelPara->stdLen[0],gctx->modelPara->xmin[0],gctx->modelPara->ymax[0]);
		no_simplaneReceiver receiver;
		receiver.position = Vector3d(x,y,z);
		receiver.PCI = atoi(PCI.c_str());
		glbctx->cptPara->No_SimPlanePoint.push_back(receiver);
	}
	infile.close();

	glbctx->cptPara->no_simplane=true;

	outputLog(QStringLiteral("非仿真面接收点位置设置文件加载成功。共")+QString::number(glbctx->cptPara->No_SimPlanePoint.size())+QStringLiteral("个接收点"));
	QMessageBox::warning(this, QStringLiteral("非仿真面接收点位置设置文件"), QStringLiteral("加载成功"));

	return;
}

void MainWindow::ReadScenePlanetFile(const char*filename_2D, const char*filename_Height, Vector3d& MaxPoint, Vector3d& MinPoint)  //filename_2D为二维信息文件，filename_Height为高度信息文件;filename_altitude为海拔信息文件，MaxPoint为场景中最大点坐标，MinPoint为场景中最小点坐标
{
	//场景vector格式文件读取
	ifstream infile1(filename_2D,ios::in|ios::_Nocreate);  
	if(!infile1)
	{
		cout << "can not open file1!" << endl;
		return ;
	}
	ifstream infile2(filename_Height,ios::in|ios::_Nocreate);  
	if(!infile2)
	{
		cout << "can not open file2!" << endl;
		return ;
	}
	globalContext *gctx=globalContext::GetInstance();
	string str1,str2;
	infile1.seekg(4,ios::beg);
	infile2.seekg(3,ios::beg);
	while(getline(infile1,str1))
	{
		int building_id1 = 0;
		string sign1 = " ";
		int point_num = 0;
		istringstream linestream1(str1);
		linestream1>>building_id1>>sign1>>point_num;

		int building_id2 = 0;
		string sign2= " ";
		double height = 0;
		getline(infile2,str2);
		istringstream linestream2(str2);
		linestream2>>building_id2>>sign2>>height;

		if (point_num > 0)
		{
			building building_info;
			building_info.height = height;
			
			for (int i = 0; i < point_num; i++)
			{
				double x,y;
				getline(infile1,str1);
				istringstream linestream4(str1);
				linestream4>>x>>y;

				double building_altitude = getPointAltitude(gctx->modelPara->heightR[0],x,y,gctx->modelPara->rowNum[0],gctx->modelPara->colNum[0],gctx->modelPara->stdLen[0],gctx->modelPara->xmin[0],gctx->modelPara->ymax[0]);; //建筑物各个点按照实际获取对应海拔值
				double upper_height = height+building_altitude;
				double under_height = building_altitude;
				building_info.upper_facePoint.push_back(Vector3d(x,y,upper_height));
				MaxPoint = Max(MaxPoint,Vector3d(x,y,upper_height));
				MinPoint = Min(MinPoint,Vector3d(x,y,under_height));
			} 
			total_Buildings.push_back(building_info);	
		}			
	}
	infile1.close();
	infile2.close();
}

/************************************************************************/
/*    展示全部场景                                                                                       */
/************************************************************************/
void MainWindow::showAll()
{
	setProgress(0);
	globalContext *globalCtx=globalContext::GetInstance();
	if (!globalCtx->modelPara->modelexist)
	{
		QMessageBox::warning(this, QStringLiteral("场景展示"),QStringLiteral("请先导入文件"));
		return;
	}
	outputLog(QStringLiteral("努力加载整个场景中......"));	
	ui.ModelView->setMaterial(globalCtx->modelPara->materialdatabase);
	ui.ModelView->setBuilding(total_Buildings,MaxPoint,MinPoint);
	ui.ModelView->updateMesh();
	ui.ModelView->updateGL();
	setProgress(20);
	outputLog(QStringLiteral("已经显示建筑物场景."));
	setProgress(100);
}

void MainWindow::showLocal()
{
	setProgress(0);
		globalContext *globalCtx=globalContext::GetInstance();
	if (globalCtx->modelPara->materialdatabase.size()==0)
	{
		QMessageBox::warning(this,  QStringLiteral("场景展示"), QStringLiteral("请先导入并编辑生成最终场景中所需材质属性"));
		return;
	}

	if (!globalCtx->modelPara->localexist)
	{
		QMessageBox::warning(this, QStringLiteral("局部场景展示"),QStringLiteral("请先导入文件"));
		return;
	}
	outputLog(QStringLiteral("努力加载局部场景中......"));
	setProgress(0);
	Vector3d MaxPointLocal,MinPointLocal;
	globalContext *gctx=globalContext::GetInstance();
	emxModel *triangleModel=gctx->modelPara->trianglePara;
	triangleModel->GetBBox(MinPointLocal,MaxPointLocal);
	

	ui.simuArea->setMaterial(globalCtx->modelPara->materialdatabase);
	ui.simuArea->drawTriangleScene=1;
	ui.simuArea->defaultMaterial=material_ID;
	ui.simuArea->setTriangleModel(triangleModel);
	ui.simuArea->updateMesh();
	ui.simuArea->updateGL();  //函数updateGL是QT自带的函数
	outputLog(QStringLiteral("已经显示仿真区域"));
}

/************************************************************************/
/* 导入obj文件                                                                     */
/************************************************************************/
void MainWindow::loadObj()
{
	setProgress(0);
	 OBJFile_path = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开obj文件"),"./",QString::fromLocal8Bit("obj 文件 (*.obj)"));
	if (OBJFile_path.isEmpty())
		return;
	globalContext *gctx=globalContext::GetInstance();
	emxModel* triangleModel=gctx->modelPara->trianglePara;
	if (triangleModel)
	{
		delete triangleModel;
	}
	setProgress(10);
	triangleModel = new emxModel(OBJFile_path.toStdString().c_str());
	setProgress(90);
	outputLog(QStringLiteral("成功读取obj文件"));
	if (triangleModel->NumF()>triangleModel->NumMaterialF())
	{
		outputLog(QStringLiteral("材质信息不完整，请在剖分选项中指定统一材料编号"));
	}
	globalContext *globalCtx=globalContext::GetInstance();
	globalCtx->modelPara->localexist=true;

	//右侧边栏显示参数
	Vector3d MaxPointLocal,MinPointLocal;
	triangleModel->GetBBox(MinPointLocal,MaxPointLocal);
	lpg->setParametre(triangleModel->NumF(),MinPointLocal,MaxPointLocal);
	setModelName(1,OBJFile_path);
	setProgress(100);
	gctx->modelPara->trianglePara=triangleModel;
	return;
}

void MainWindow::setMeshOption()
{
	if (mod==NULL)
	{
		mod=new meshOptionDialog(this);
	}
	mod->exec();
}

/************************************************************************/
/* 剖分选中的地面和建筑物                                                                     */
/************************************************************************/
void MainWindow::meshAll()
{
	setProgress(0);
	globalContext *gctx=globalContext::GetInstance();
	emxModel *triangleModel=gctx->modelPara->trianglePara;
	if (triangleModel!=NULL)
	{
		if (triangleModel->NumV())
		{
			outputLog(QStringLiteral("局部场景已有点集"));
		}
		outputLog(QStringLiteral("局部场景已有模型，请先删除。"));
		return;
	}else if (total_Buildings.size()==0)
	{
		outputLog(QStringLiteral("没有导入建筑物文件，请先导入相关文件。"));
		return;
	}
	Vector3d center;
	double range;
	//要不要加一个判断，防止错误？
	if (mod->inputFlag)
	{
		mod->getValue(center,range);
	}else
	{
		outputLog(QStringLiteral("没有设置剖分参数"));
		return;
	}
	setProgress(5);

	LocalBuilding(gctx->modelPara->Local_buildings,center, range);
	outputLog(QStringLiteral("选定区域内建筑物数量：")+QString::number(gctx->modelPara->Local_buildings.size()));

	setProgress(50);
	MESH meshCTX;
	MESH_PTR ground_pMesh = &meshCTX;
	LocalGround( ground_pMesh,center,range);
	setProgress(70);
	gctx->modelPara->trianglePara = new emxModel(gctx->modelPara->Local_buildings, ground_pMesh);
	setProgress(80);
	gctx->modelPara->trianglePara->GenerateEdge(gctx->modelPara->Local_buildings);
	setProgress(90);
	gctx->modelPara->groundMesh=meshCTX;
	//右侧边栏显示参数
	Vector3d MaxPointLocal,MinPointLocal;
	gctx->modelPara->trianglePara->GetBBox(MinPointLocal,MaxPointLocal);
	globalContext *globalCtx=globalContext::GetInstance();
	globalCtx->modelPara->localexist=true;
	lpg->setParametre(gctx->modelPara->trianglePara->NumF(),MinPointLocal,MaxPointLocal);
	setProgress(100);
}

void MainWindow::LocalGround(MESH_PTR pMesh,Vector3d AP_position, double LocalRange)
{
	//局部区域的范围 MinPos、MaxPos
	Vector3d MinPos = AP_position - Vector3d(LocalRange/2, LocalRange/2, 0);
	Vector3d MaxPos = AP_position + Vector3d(LocalRange/2, LocalRange/2, 0);

	//step0 检测数据有没有读 没读取则报错退出 
	if(ScenePlaneHeightInfoFile_path.isEmpty())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("场景展示"),QString::fromLocal8Bit("请先导入场景的海拔信息文件"));
		return;
	}
	globalContext *gctx=globalContext::GetInstance();
	//step1 找到对应的行和列
	int upRow=(gctx->modelPara->ymax[0]-MaxPos[1])/gctx->modelPara->stdLen[0];
	int downRow= (gctx->modelPara->ymax[0]-MinPos[1])/gctx->modelPara->stdLen[0]+1;
	int leftCol=(MinPos[0]-gctx->modelPara->xmin[0])/gctx->modelPara->stdLen[0];
	int rightCol=(MaxPos[0]-gctx->modelPara->xmin[0])/gctx->modelPara->stdLen[0]+1;
	int area[4]={upRow,downRow,leftCol,rightCol};

	//step2 进行局部的文件提取 canny算法 剖分 
	vector<vector<int> > cannyPoint;//从局部点开始的点 不是全局的 
	int nv=0;
	int localRow=downRow-upRow+1;
	int localCol=rightCol-leftCol+1;
	cannyPoint.resize(localRow,vector<int>(localCol));

	height2canny(gctx->modelPara->heightR[0],cannyPoint,gctx->modelPara->rowNum[0],gctx->modelPara->colNum[0],gctx->modelPara->stdLen[0],gctx->modelPara->xmin[0],gctx->modelPara->ymax[0],area);

	int  totalVer=getFeaturePoint3(stepLength,localRow,localCol,cannyPoint);

	InitMesh(pMesh, totalVer);

	int amount=setMeshPtr3( pMesh,stepLength,localRow,localCol,cannyPoint,gctx->modelPara->heightR[0],gctx->modelPara->xmin[0],gctx->modelPara->ymax[0],gctx->modelPara->stdLen[0],upRow,leftCol);

	outputLog(QStringLiteral("整张地图共有")+QString::number(nv,10)+QStringLiteral("特征点。"));
	outputLog(QStringLiteral("本区域插入点数量")+QString::number(amount-3,10));
	double last_time, this_time;
	last_time = GetTickCount();
	IncrementalDelaunay(pMesh);
	this_time = GetTickCount();

	outputLog(QStringLiteral("局部地面建模共产生点")+QString::number(pMesh->vertex_num,10));
	outputLog(QStringLiteral("剖分耗时")+QString::number((this_time - last_time)/1000)+QStringLiteral("s"));
}

/************************************************************************/
/* 返回选定范围内的建筑物                                                                          */
/************************************************************************/
void MainWindow::LocalBuilding(vector< building> &Local_buildings, Vector3d AP_position, double LocalRange)
{
	//局部区域的范围 MinPos、MaxPos
	Vector3d MinPos = AP_position - Vector3d(LocalRange/2, LocalRange/2, 0);
	Vector3d MaxPos = AP_position + Vector3d(LocalRange/2, LocalRange/2, 0);
	for (int buildings_id = 0; buildings_id < total_Buildings.size(); buildings_id++)
	{
		bool in_range = true;
		for (int id = 0; id < total_Buildings[buildings_id].upper_facePoint.size()-1; id++) //记录building顶面点坐标时，首末点重合，记录两次，所以   .size（）-1
		{
			double x1 = total_Buildings[buildings_id].upper_facePoint[id].x - MinPos.x;
			double x2 = total_Buildings[buildings_id].upper_facePoint[id].x - MaxPos.x;
			double y1 = total_Buildings[buildings_id].upper_facePoint[id].y - MinPos.y;
			double y2 = total_Buildings[buildings_id].upper_facePoint[id].y - MaxPos.y;
			if ( !(x1*x2 < 0 && y1*y2 < 0)) //判断建筑物是否在设定范围内,即使有一个点不在范围内，也判定为不在内部
			{
				in_range = false;
				break;
			}
		}
		if (in_range)
		{
			Local_buildings.push_back(total_Buildings[buildings_id]);
		}		
	}
	outputLog(QStringLiteral("建筑物数量为")+QString::number(Local_buildings.size()));
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
	cityManager = new cityModelManager();
	cityScene = cityManager->loadModel(path.toStdString());

}