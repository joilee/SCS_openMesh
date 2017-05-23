#include "scsMaterial.h"

class scsMaterialManager
{
public:
	scsMaterialManager();
	~scsMaterialManager();

	/*
	@brief 从文件中添加材料
	*/
	void addMatertial(string path);
private:
	vector<scsMaterial> materialdatabase;
};

