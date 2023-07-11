#ifndef MY_PARTICLE_SIMULATOR_H
#define MY_PARTICLE_SIMULATOR_H

#include <GLModel/GLModel.h>
#include <map>
#include <shared/defs.h>
#include <tuple>
#include <util/util.h>
#include <utility>
#include <vector>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include "GlobalResourceManager.h"
#include "Particle.h"
#include "partSys.h"

#include <string>

using namespace std;

enum integrator { euler, symplectic, verlet };

class ParticleSimulator : public BaseSimulator
{
public:

	ParticleSimulator(const std::string& name, ParticleSystem* target);
	~ParticleSimulator();

	int step(double time);
	int init(double time)
	{
		k_drag = 0;
		setVector(N, 0, 1, 0);
		return 0;
	};
	int command(int argc, myCONST_SPEC char** argv);

protected:

	integrator technique;
	double ground_ks;
	double ground_kd;
	double gravity;
	double k_drag;
	double time_step;
	//vector<Spring *> springs;
	vector<Particle*> particle_list;
	Vector N;
	ParticleSystem* particle_system;
	map<pair<int, int>, tuple<double, double, double>> forces;
};


#endif