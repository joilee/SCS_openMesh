#pragma  once

#include "../abstractModelManager.h"
#include "cityModel.h"
#include <string>

using namespace std;

class cityModelManager:public abstractModelManager
{
public:
	cityModelManager();
	~cityModelManager();
	abstractModel *loadModel(string path);
private:
};

