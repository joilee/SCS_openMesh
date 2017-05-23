#include "context.h"


globalContext::globalContext()
{
	//cptPara=new ComputePara;
	modelPara=new ModelPara;
	visualPara=new visPara;
	modelManager = new ModelManager;
	cptManager = new computeManager;
	matManager = new scsMaterialManager;
}

globalContext::~globalContext()
{
	
	 if (modelPara!=NULL)
	 {
		 delete modelPara;
	 }
	 delete cptManager;
}