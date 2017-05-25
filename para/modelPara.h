#ifndef MODELPARA_H
#define  MODELPARA_H
#include "../Model/cityModel/cityLocalModel.h"

class ModelPara
{
public:
	ModelPara();
	~ModelPara();
	void setlocalmodel(cityLocalModel* a){ localModel = a; }
	cityLocalModel* getlocalmodel(){ return localModel; }
public:
	cityLocalModel * localModel;
private:

};





#endif