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

	void setDefault(int i);
	int getDefaultMaterial();
private:
	vector<scsMaterial> materialdatabase;
	int material_ID;
};

