#include "ObjectSimulator.h"

ObjectSimulator::ObjectSimulator(const std::string& name, ObjectPath* target) :
	BaseSimulator(name),
	m_object(target)
{
}

ObjectSimulator::~ObjectSimulator()
{
}

int ObjectSimulator::step(double time)
{	
	if (time > t_max)
	{
		return 0;
	}
	length = m_object->getArclength();

	double t_norm = time / t_max;
	double t_1 = 0.1;
	double t_2 = 0.9;
	double accel = length / ((t_1 * t_1 / 2) + (t_1 * (t_2 - t_1)) + ((1 - t_2) * t_1 / 2));
	double v_m = accel * t_1;
	vel = v_m;
	if (t_norm <= t_1 && t_norm >= 0) {
		vel = v_m * (t_norm / t_1);
		distance = (v_m * t_norm * t_norm) / (2 * t_1);
	}
	else if (t_norm <= 1 && t_norm >= t_2)
	{
		vel = v_m * (1 - ((t_norm - t_2) / (1-t_2)));
		distance =	(v_m * t_1 / 2) + 
					(v_m * (t_2 - t_1)) + 
					(v_m * (t_norm - t_2) * (1 - ((t_norm - t_2)/(2 * (1 - t_2)))));
	}
	else if (t_norm <= t_2 && t_norm >= t_1)
	{
		distance = (v_m * t_1 / 2) + (v_m * (t_norm - t_1));
	}
	
	m_object->test(t_norm);

	if (floorf(time) == next_second) 
	{
		animTcl::OutputMessage("Velocity: %f", vel);
		next_second++;
	}
	Vector pos;
	m_object->getState(pos);

	m_object->setState(pos, distance, t_norm);

	return 0;

}
