#pragma  once
#include "../para/computePara.h"
#include <QString>

/*
	�㷨����
	1.�����ĵ��룬����
	2.�㷨ģ��Ĳ������
	3.�㷨dll�ĵ���
*/

class computeManager
{
public:
	computeManager();
	~computeManager();

	/*
	@brief �����߲����ļ�
	*/
	void openTransAntenna_ParamFile(QString path);

	/*
	@brief ���ؼ������
	*/
	inline ComputePara *getComputationPara(){ return cptPara; }

	/*
	@brief
	*/
	vector<Site> &getSite();

private:
	ComputePara* cptPara;
};

