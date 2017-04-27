#pragma once
#include <string>
#include <vector>
using namespace std;
class cityGround
{
private:
	//����դ�� 
	std::vector< std::vector<int>> groundMatrix;
	int row, col;//������к���
	double base;//����
	double xmin,xmax;
	double ymax,ymin;
public:
	cityGround(const string path, double _xmin, double _xmax, double _ymin, double _ymax, double _base);
	~cityGround();
	std::vector< std::vector<int>> getGroundMatrix(){ return this->groundMatrix; }
	double getXmin() { return this->xmin; }
	double getXmax(){ return this->xmax; }
	double getYmin(){  return this->ymin; }
	double getYmax(){ return this->ymax; }
	int getRow(){ return this->row; }
	int getCol() { return this->col; }
	double getPrecesion() { return this->base; }
	double getPointAltitude(double x, double y);
};