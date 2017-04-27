#include "cityModelManager.h"
abstractModel  * cityModelManager::loadModel(string path)
{
	return (new cityModel(path));
}
cityModelManager::cityModelManager()
{
	cout << "Info: Model Manager initialized. " << endl;
}
cityModelManager::~cityModelManager()
{

}