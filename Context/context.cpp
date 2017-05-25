#include "context.h"


globalContext::globalContext()
{

	visualPara=new visPara;
	modelManager = new ModelManager;
	cptManager = new computeManager;
	matManager = new scsMaterialManager;
}

globalContext::~globalContext()
{
	

	 delete cptManager;
}