#include "scsMaterial.h"

class scsMaterialManager
{
public:
	scsMaterialManager();
	~scsMaterialManager();

	/*
	@brief ���ļ�����Ӳ���
	*/
	void addMatertial(string path);
private:
	vector<scsMaterial> materialdatabase;
};

