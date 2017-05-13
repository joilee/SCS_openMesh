#include "cityModelFactory.h"
abstractModel  * cityModelFactory::loadModel(string path)
{
	return (new cityModel(path));
}


cityModelFactory::cityModelFactory()
{
	cout << "Info: Model Factory initialized. " << endl;
}
cityModelFactory::~cityModelFactory()
{

}