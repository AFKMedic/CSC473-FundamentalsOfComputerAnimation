#ifndef MY_PARTICLE_SYSTEM_H
#define MY_PARTICLE_SYSTEM_H

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>
#include "Particle.h"
#include "shared/opengl.h"
#include <tuple>
#include <vector>

using namespace std;

class ParticleSystem : public BaseSystem
{
public:
	ParticleSystem(const std::string& name);
	//virtual void getState(double* p);
	//virtual void setState(double* p);
	//void reset(double time);

	void display(GLenum mode = GL_RENDER);

	int command(int argc, myCONST_SPEC char** argv);
	void getParticleList(vector<Particle*>& p);
	void addSpringConnection(int index_1, int index_2);
	
protected:

	int max_particles;
	vector<Particle *> particles;
	vector<tuple<int, int>> connected_particles;
};
#endif
