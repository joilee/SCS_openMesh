#ifndef ABSTRACT_MODEL
#define ABSTRACT_MODEL
#include <string>
using namespace std;
class abstractModel
{
public:
	abstractModel();
	virtual ~abstractModel();

	virtual string  getName();
	void virtual draw(int mode);
	void virtual initDraw();
private:

};



#endif