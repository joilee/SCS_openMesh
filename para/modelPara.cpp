#include "modelPara.h"
ModelPara::ModelPara()
{
	modelexist=false;
	localexist=false;
	trianglePara=NULL;
	LocalScene_range=2000;

}

ModelPara::~ModelPara()
{
	if (trianglePara!=NULL)
	{
		delete trianglePara;
	}

}

/************************************************************************/
/* 得到一个点对应的高度                                                                     */
/************************************************************************/
double ModelPara:: getPointAltitude(double x,double y)
{
	double xmax=xmin[0]+(colNum[0])*stdLen[0];
	double ymin=ymax[0]-(rowNum[0])*stdLen[0];
	if(x<xmin[0]||y>ymax[0]||x>xmax||y<ymin)
	{
		cout<<"wrong coordinates from building vector"<<endl;
	}
	int pointRow=int((ymax[0]-y)/stdLen[0]);
	int pointCol=int((x-xmin[0])/stdLen[0]);
#ifdef DEBUG_MODE
	if(heightR[0].p[pointRow][pointCol]<0)
	{
		cout<<heightR[0].p[pointRow][pointCol]<<endl;
	}
#endif

	return heightR[0].p[pointRow][pointCol];

	//return 0;
}