#pragma  once

#include "../abstractModelFactory.h"
#include "cityModel.h"
#include <string>

using namespace std;

class cityModelFactory :public abstractModelFactory
{
public:
	cityModelFactory();
	~cityModelFactory();
	abstractModel *loadModel(string path);
private:
};

