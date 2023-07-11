#ifndef MY_OBJECT_PATH_H
#define MY_OBJECT_PATH_H

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
#include "Frame.h"

using namespace std;

class ObjectPath : public BaseSystem
{
public:

	ObjectPath(const std::string& name);
	int init(double time)
	{
		max_index = -1;
		return 0;
		x_rot = 0;
		y_rot = 0;
		z_rot = 0;
	};

	void reset(double time);
	void display(GLenum mode = GL_RENDER);
	void calculateDraw();
	int command(int argc, myCONST_SPEC char** argv);
	void getState( double *p);
	void setState( double  *p, double t, double i);
	double getArclength();
	void test(double d);

protected:
	int max_index;
	vector<Point> draw_this;
	vector<Point> points;
	//map<double, Frame> frames;
	map<double, double> arclengths;
	map<double, Point> arcPoints;
	double total_length;
	double w_rot;
	double x_rot;
	double y_rot;
	double z_rot;
	float m_sx;
	float m_sy;
	float m_sz;
	Vector m_pos;
	Vector tangent;
	GLMmodel m_model;
};


#endif