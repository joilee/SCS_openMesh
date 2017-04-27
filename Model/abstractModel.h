#ifndef ABSTRACT_MODEL
#define ABSTRACT_MODEL
 
class abstractModel
{
public:
	abstractModel();
	virtual ~abstractModel();

	void virtual draw()=0;
private:

};



#endif