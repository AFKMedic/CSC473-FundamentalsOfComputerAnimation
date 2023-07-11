#ifndef MY_HERMITE_SPLINE_H
#define MY_HERMITE_SPLINE_H

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "shared/opengl.h"
#include "Point.h"

using namespace std;

class HermiteSpline : public BaseSystem
{
public:

	HermiteSpline( const std::string& name );
	int init(double time)
	{
		max_index = -1;
		return 0;
	};

	void display(GLenum mode = GL_RENDER);
	void calculateDraw();
	void calculateCR();
	int command(int argc, myCONST_SPEC char** argv);

protected:
	int max_index;
	vector<Point> draw_this;
	vector<Point> points;
	vector<Point> cr_points;
	map<double, double> arclengths;
	boolean catmull_rom;
};
#endif