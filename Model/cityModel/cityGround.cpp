#include "cityGround.h"
#include <fstream>
#include <iostream>
using namespace std;
cityGround::cityGround(const string path, double _xmin, double _xmax, double _ymin, double _ymax, double _base)
{
	xmin = _xmin;
	xmax = _xmax;
	ymin = _ymin;
	ymax = _ymax;
	base = _base;

	ifstream FileH;
	FileH.open(path, ios::binary);
	//获取行列值
	row = int((ymax - ymin) / base);
	col  = int((xmax - xmin) / base);

	groundMatrix.resize(row, vector<int>(col));

	unsigned char c1, c2;
	char c3[2];
	const int Nc = 80;
	int no = 0, flag = 1;
	int num = 0;
	int colt = col;
	while (FileH.good() && !FileH.eof())
	{
		FileH.read(c3, 2);
		c1 = c3[0];
		c2 = c3[1];

		int tmp = int(c1) * 256 + int(c2);
		if (no <row*col)
		{
			groundMatrix[no / colt][no%colt] = tmp;
			no++;
		}
	}
	FileH.close();
}


double  cityGround::getPointAltitude(double x, double y)
{
	if (x<xmin || y>ymax || x > xmax || y < ymin)
	{
		cout << "error:  wrong coordinates from building vector" << endl;
		return 0;
	}
	int pointRow = int((ymax - y) / base);
	int pointCol = int((x - xmin) / base);
	return groundMatrix[pointRow][pointCol];
}

cityGround::~cityGround()
{
	groundMatrix.clear();
}