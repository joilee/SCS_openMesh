#include "computeManager.h"
#include "../Context/context.h"
#include <fstream>
#include <QMessageBox>
#include <util/stringUtil.h>
computeManager::computeManager()
{
	cptPara = new ComputePara;
}

computeManager::~computeManager()
{
	delete cptPara;
}

void computeManager:: openTransAntenna_ParamFile(QString path)
{
	if (path.isEmpty())
		return;
	cptPara->Sites.clear();
	ifstream infile((path.toStdString()).c_str(), ios::in | ios::_Nocreate);
	if (!infile)
	{
		cout << "error:can not open file!" << endl;
		return;
	}

	globalContext *globalCtx = globalContext::GetInstance();
	string str, str_flag;
	getline(infile, str);//跳过第一行
	while (getline(infile, str))
	{
		istringstream linestream(str);
		vector<string>parameters;
		string parameter;
		while (getline(linestream, parameter, ','))
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
		//从全局中获取
		double z = atof(height.c_str()) + globalCtx->modelManager->getFirstCity()->getCity()->getAltitude(x, y);

		TransAntenna new_antenna;
		new_antenna.Cell_Name = CellName;
		new_antenna.PCI = atoi(PCI.c_str());
		new_antenna.frequency = 1750; //单位为MHZ，设置了一个默认
		new_antenna.trans_power = 12.2; //单位为dBm
		new_antenna.wire_loss = 0.5;
		new_antenna.enlarge_power = 0;
		new_antenna.position = Vector3d(x, y, z);
		new_antenna.polor_direction = Vector3d(0, 0, 1);

		new_antenna.phi = 0;
		new_antenna.theta = 0;

		if (new_antenna.polor_direction.norm() < 1e-10)
		{
			QMessageBox::warning(NULL, QStringLiteral("发射天线属性设置"), QStringLiteral("请输入正确的极化方向"));
			cout << "error : 天线文件读取错误。已返回！" << endl;
			return;
		}	
		//针对新获得的cell，检测是否存在一个已知的site中，如果是，则插入，否则新建
		bool newsite = true;   //是否需要新建一个site
		int current_sitename = stof(SiteName.c_str());
		for (int i = 0; i < cptPara->Sites.size(); i++)
		{
			if (current_sitename == cptPara->Sites[i].Site_Name)
			{
				newsite = false;
				cptPara->Sites[i].Site_Antennas.push_back(new_antenna);
				break;
			}
		}
		if (newsite)//如果是新的小区
		{
			Site new_site;
			new_site.Site_Name = current_sitename;
			new_site.Site_Antennas.push_back(new_antenna);
			cptPara->Sites.push_back(new_site);
		}
	}
	infile.close();
	cout << "info:读取天线文件成功" << endl;
}

vector<Site> & computeManager::getSite()
{
	if (cptPara->Sites.size()!=0)
	{
		return cptPara->Sites;
	}
	else
	{
		//是不是不能返回局部变量的引用
		cptPara->Sites.clear();
		return  cptPara->Sites;
	}
}