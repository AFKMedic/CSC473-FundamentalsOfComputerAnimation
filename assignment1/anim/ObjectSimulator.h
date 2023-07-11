#ifndef MY_OBJECT_SIMULATOR_H
#define MY_OBJECT_SIMULATOR_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include "ObjectPath.h"

#include <string>

class ObjectSimulator : public BaseSimulator
{
public:

	ObjectSimulator(const std::string& name, ObjectPath* target);
	~ObjectSimulator();

	int step(double time);
	int init(double time)
	{
		distance = 0.0;
		vel = 0.0;
		t_max = 10.0;
		next_second = 0;
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv) { return TCL_OK; }

protected:
	Vector m_pos;
	double length;
	double distance;
	double vel;
	double t_max;
	ObjectPath* m_object;;
	int next_second;
};


#endif