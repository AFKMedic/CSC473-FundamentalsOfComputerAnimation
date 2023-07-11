#include "partSys.h"

ParticleSystem::ParticleSystem( const std::string& name ) :
BaseSystem(name)
{

}

void ParticleSystem::display(GLenum mode)
{
	// Draw the plane
	set_colour(100.0f, 100.0f, 100.0f);
	glBegin(GL_QUADS);
	glVertex3f(	 100,  0,  100);
	glVertex3f( -100,  0,  100);
	glVertex3f(	-100,  0, -100);
	glVertex3f(  100,  0, -100);
	glEnd();

	// Draw the particles as points
	glPointSize(5.0);
	set_colour(255.0f, 0.0f, 0.0f);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (auto& particle : particles) {
		glVertex3f(particle->pos[0], particle->pos[1], particle->pos[2]);
	}
	glEnd();
	
	// Draw the springs between particles
	set_colour(255.0f, 255.0f, 255.0f);
	glBegin(GL_LINES);
	for (auto& spring : connected_particles) {
		glVertex3f(	particles[get<0>(spring)]->pos[0], 
					particles[get<0>(spring)]->pos[1], 
					particles[get<0>(spring)]->pos[2]);
		glVertex3f(	particles[get<1>(spring)]->pos[0],
					particles[get<1>(spring)]->pos[1],
					particles[get<1>(spring)]->pos[2]);
	}
	glEnd();
}

int ParticleSystem::command(int argc, myCONST_SPEC char** argv)
{
	boolean callDraw = true;
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}

	// This command initializes the particle system to hold up to the given 
	// number of particles. Particles may be initialized to the origin of 
	// the world, or may not appear until added by the next command.
	// argv[1] = # of particles
	else if (strcmp(argv[0], "dim") == 0)
	{
		// Clear particles list if dim is changed from previous session
		particles.clear();

		max_particles = atoi(argv[1]);
		//particles.reserve(max_particles);
		for (int i = 0; i < max_particles; i++) {
			particles.push_back(new Particle());
		}
		callDraw = false;
	}

	// This command sets a position, mass, and velocity for a given particle.
	// argv[1] = index
	// argv[2] = mass
	// argv[3] = x position
	// argv[4] = y position
	// argv[5] = z position
	// argv[6] = x velocity
	// argv[7] = y velocity
	// argv[8] = z velocity
	else if (strcmp(argv[0], "particle") == 0)
	{
		double mass = atof(argv[2]);
		double x	= atof(argv[3]);
		double y	= atof(argv[4]);
		double z	= atof(argv[5]);
		double vx	= atof(argv[6]);
		double vy	= atof(argv[7]);
		double vz	= atof(argv[8]);
		particles[atoi(argv[1])]->createParticle(mass, x, y, z, vx, vy, vz);
	}

	// This command sets the velocity of all particles.
	// argv[1] = x velocity
	// argv[2] = y velocity
	// argv[3] = z velocity
	else if (strcmp(argv[0], "all_velocities") == 0)
	{
		double vx = atof(argv[1]);
		double vy = atof(argv[2]);
		double vz = atof(argv[3]);
		for (auto particle : particles)
		{
			if (particle->created)
			{
				particle->setVelocity(vx, vz, vy);
			}
		}
	}
	if(callDraw) glutPostRedisplay();
	return TCL_OK;
}

void ParticleSystem::getParticleList(vector<Particle*>& p) 
{
	p = particles;
}

void ParticleSystem::addSpringConnection(int index_1, int index_2)
{
	connected_particles.push_back(tuple<int, int>(index_1, index_2));
	glutPostRedisplay();
}