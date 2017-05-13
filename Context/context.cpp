#include "context.h"


globalContext::globalContext()
{
	cptPara=new ComputePara;
	modelPara=new ModelPara;
	visualPara=new visPara;
	modelManager = new ModelManager;
}

globalContext::~globalContext()
{
	
	 if (modelPara!=NULL)
	 {
		 delete modelPara;
	 }
}