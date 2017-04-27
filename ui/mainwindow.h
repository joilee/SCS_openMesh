#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_mainwindow.h"
#include "outdoorFile.h"
#include "InfoPage.h"
#include "computerOptionDialog.h"
#include "geometry/emxModel.h"
#include "rasterHeight/Mesh.h"
#include "rasterHeight/TriMesh.h"
#include "meshOption.h"
#include "plugin/PluginInterface.h"
#include <QtWidgets/QMessageBox>
#include "Context/context.h"

#include "Model/abstractModel.h"
#include "Model/abstractModelManager.h"
#include "Model/cityModel/cityModel.h"
#include "Model/cityModel/cityModelManager.h"


class QAction; 
struct building;

using namespace std;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


public:
	emxModel* getEmxModel();
	void setModelName(int id,QString name);//设置treewidget中的名字
	void outputLog(QString source);
	void setProgress(int value);

private slots:
	void openOutdoorFile();
	void computerOption();
	void loadAllFile(QString _name,QStringList _v,QStringList _h,QString _p);
	void showAll();
	void showLocal();
	void openTransAntenna_ParamFile();
	void openTransAntennas_DirGain();
	void openNo_SimplaneReceiverFile();
	void loadObj();
	void setMaterial();
	void open_material();
	void setMeshOption();
	void meshAll();
	void saveLocalScene();
	void loadPlugin();
	void deletePlugin();
	void run();
	void quickLoadJson();
	void saveSimuPlane();
	void loadSimuPlane();
	//内部界面控件
private:
	Ui::MainWindowClass ui;
	QTreeWidgetItem* modelTW;
	QTreeWidgetItem* computeTW;
	QTreeWidgetItem* visualTW;

	vector <QTreeWidgetItem*> site_roots1;
	vector <QTreeWidgetItem*> site_roots2;

	outdoorFileDialog*  M_outdoorFileDialog;
	computerOptionDialog* M_computeroptionDialog;
	BuildingInfoPage * bip;
	localPage *lpg;
	meshOptionDialog *mod;
	//内部变量
 private:
	 QString material_path;
	 int material_ID;
	 int getDefaultMaterial();
	 void loadMaterial(QString path);
	 /************************************************************************/
	 /* 整个场景 建筑物                                                                     */
	 /************************************************************************/
	 QString buildingName;
	 vector<building> total_Buildings;  //整体模型
	 Vector3d MaxPoint,MinPoint;
	

	 QStringList Scene2DInfoFile_paths;//建筑物地面
	 QStringList SceneHeightInfoFile_paths;//建筑物高度
	 QString ScenePlaneHeightInfoFile_path;//地面海拔

	 QString Scene2DInfoFile_path;//临时变量
	 QString SceneHeightInfoFile_path;//临时变量




	 /************************************************************************/
	 /* 局部场景 三角面片                                                                     */
	 /************************************************************************/
	 QString objName;
	 QString OBJFile_path;
	 //emxModel * triangleModel;//局部模型，存储obj模型或者建筑物加地面的三角面片,局部模型
	 
	 /************************************************************************/
	 /* plugin 参数                                                                     */
	 /************************************************************************/
	 QString plugin_file_path;

	 //

	 abstractModel* cityScene;
	 abstractModelManager* cityManager;

	 //读建筑物以及高度
	 void ReadScenePlanetFile(const char*filename_2D, const char*filename_Height, Vector3d& MaxPoint, Vector3d& MinPoint);
	 void LocalGround(MESH_PTR pMesh,Vector3d AP_position, double LocalRange);//
	 void LocalBuilding(vector< building> &Local_buildings, Vector3d AP_position, double LocalRange);
	 void init();
	 void createActions();
};
#endif // MAINWINDOW_H
