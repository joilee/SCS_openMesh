#pragma  once
#include "../para/computePara.h"
#include <QString>

/*
	算法管理
	1.参数的导入，更改
	2.算法模块的参数设计
	3.算法dll的导入
*/

class computeManager
{
public:
	computeManager();
	~computeManager();

	/*
	@brief 打开天线参数文件
	*/
	void openTransAntenna_ParamFile(QString path);

	/*
	@brief 返回计算参数
	*/
	inline ComputePara *getComputationPara(){ return cptPara; }

	/*
	@brief
	*/
	vector<Site> &getSite();

private:
	ComputePara* cptPara;
};

