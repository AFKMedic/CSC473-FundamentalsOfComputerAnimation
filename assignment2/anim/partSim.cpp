#include "partSim.h"
ParticleSimulator::ParticleSimulator(const std::string& name, ParticleSystem* target) :
	BaseSimulator(name),
	particle_system(target)
{
}	// SampleGravitySimulator

ParticleSimulator::~ParticleSimulator() 
{

}

int ParticleSimulator::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}

	// This links the simulator to a particular particle system
	// and initializes it to work with a given number of springs.
	// argv[1] = Particle system name
	// argv[2] = # of springs
	else if (strcmp(argv[0], "link") == 0)
	{
		particle_system = (ParticleSystem *)GlobalResourceManager::use()->getSystem(argv[1]);
		// Grab vector of particles
		particle_system->getParticleList(particle_list);
	}

	// This sets up a given spring. If the rest length is a negative number, 
	// the system should automatically set the rest length of the spring to 
	// the distance between the corresponding particles at the time the command is given.
	// argv[1] = Index 1
	// argv[2] = Index 2
	// argv[3] = ks
	// argv[4] = kd
	// argv[5] = rest length
	else if (strcmp(argv[0], "spring") == 0)
	{


		// Check if input rest length is positive
		double rest_length;
		if (atof(argv[5]) >= 0)
		{
			rest_length = atof(argv[5]);
		}
		else
		{
			Vector a;
			Vector b;
			Vector c;
			particle_list[atoi(argv[1])]->getState(a);
			particle_list[atoi(argv[2])]->getState(b);
			VecSubtract(c, a, b);
			rest_length = VecLength(c);
		}
		// For drawing the springs
		particle_system->addSpringConnection(atoi(argv[1]), atoi(argv[2]));

		// For keeping track of connections per particle
		particle_list[atoi(argv[1])]->addConnection(atoi(argv[2]), atof(argv[3]), atof(argv[4]), rest_length);
		particle_list[atoi(argv[2])]->addConnection(atoi(argv[1]), atof(argv[3]), atof(argv[4]), rest_length);
	}

	// This command nails particle <index> to its current position.
	// argv[1] = Index
	else if (strcmp(argv[0], "fix") == 0)
	{
		particle_list[atoi(argv[1])]->fix();
	}

	// This changes the integration technique used by the given simulator
	// and sets the time step of the integration. 
	// argv[1] = Integrator technique
	// argv[2] = Time Step
	else if (strcmp(argv[0], "integration") == 0)
	{
		if(strcmp(argv[1], "euler") == 0)
		{
			technique = euler;
		}
		else if (strcmp(argv[1], "symplectic") == 0)
		{
			technique = symplectic;
		}
		else if (strcmp(argv[1], "verlet") == 0)
		{
			technique = verlet;
			
			// Calculate initial verlet positions
			for (auto& particle : particle_list)
			{
				particle->calculateInitialVerlet(atof(argv[2]));
			}
		}

		time_step = atof(argv[2]);
	}

	// Sets the parameters of the penalty forces applied to particles 
	// that try to go underground.
	// argv[1] = ks
	// argv[2] = kd
	else if (strcmp(argv[0], "ground") == 0)
	{
		ground_ks = atof(argv[1]); 
		ground_kd = atof(argv[2]);
	}

	// Sets the acceleration due to gravity, in unit length per unit time squared.
	// argv[1] = gravity
	else if (strcmp(argv[0], "gravity") == 0)
	{
		gravity = atof(argv[1]);
	}

	// Sets the global drag (friction) coefficient (Fi = -kdrag vi).  The command expects a positive number 
	// argv[1] = kdrag
	else if (strcmp(argv[0], "drag") == 0)
	{
		// Turn friction to 0 if a negative is given
		k_drag = (atof(argv[1]) >= 0) ? atof(argv[1]) : 0;
	}

}

int ParticleSimulator::step(double time) 
{
	int particle_index = 0;
	for (auto& particle : particle_list) 
	{
		if (!particle->getFixed()) {
			Vector pos;
			Vector vel;
			Vector net_force;
			setVector(net_force, 0, 0, 0);
			particle->getState(pos);
			particle->getVelocity(vel);
			particle->setPrevState(pos);
			particle->setPrevVelocity(vel[0], vel[1], vel[2]);
			
			// Calculate net forces
			// Drag Force
			Vector drag_component;
			VecCopy(drag_component, vel);
			VecScale(drag_component, -k_drag);

			// Gravity Force
			Vector gravity_component;
			setVector(gravity_component, 0, gravity, 0);

			//Sprint Force
			Vector spring_component;
			setVector(spring_component, 0, 0, 0);
			for (auto& neighbour : particle->connected_to) 
			{
				// Check if spring force calculation already done
				if (forces.find(make_pair(particle_index, get<0>(neighbour))) != forces.end())
				{
					Vector temp;
					setVector(temp, get<0>(forces[make_pair(particle_index, get<0>(neighbour))]), 
									get<1>(forces[make_pair(particle_index, get<0>(neighbour))]), 
									get<2>(forces[make_pair(particle_index, get<0>(neighbour))]));
					VecAdd(spring_component, spring_component, temp);
				}
				else
				{
					// Get linked particle position
					Vector x_j;
					particle_list[get<0>(neighbour)]->getPrevState(x_j);

					Vector x_ij;
					Vector x_ij_normalzied;
					Vector v_j;
					Vector v_ij;
					VecSubtract(x_ij, pos, x_j);
					VecCopy(x_ij_normalzied, x_ij);
					VecNormalize(x_ij_normalzied);
					double x_ij_magnitude = VecLength(x_ij);

					// Get linked particle velocity
					particle_list[get<0>(neighbour)]->getPrevVelocity(v_j);
					VecSubtract(v_ij, vel, v_j);

					// Spring force
					Vector pure_spring_force;
					VecCopy(pure_spring_force, x_ij);
					VecScale(pure_spring_force, 1 / x_ij_magnitude);
					VecScale(pure_spring_force, get<1>(neighbour));
					VecScale(pure_spring_force, get<3>(neighbour) - x_ij_magnitude);

					// Dampening force
					Vector dampening_force;
					VecCopy(dampening_force, x_ij);
					VecScale(dampening_force, VecDotProd(v_ij, x_ij_normalzied));
					VecScale(dampening_force, -(get<2>(neighbour)));

					VecAdd(spring_component, spring_component, pure_spring_force);
					VecAdd(spring_component, spring_component, dampening_force);
					forces.insert({ {make_pair(particle_index, get<0>(neighbour))}, tuple<double, double, double>(spring_component[0],spring_component[1], spring_component[2]) });
					forces.insert({ {make_pair(get<0>(neighbour), particle_index)}, tuple<double, double, double>(-spring_component[0],-spring_component[1], -spring_component[2]) });
				}
			}

			VecAdd(net_force, net_force, drag_component);
			VecAdd(net_force, net_force, gravity_component);
			VecAdd(net_force, net_force, spring_component);

			// Check if contacts ground
			if (abs(pos[1]) < 0.0000001 && abs(VecDotProd(vel, N)) < 0.0000001 || pos[1] < 0) {
				pos[1] = 0;
				Vector ground_force;
				Vector N_1;
				Vector N_2;
				setVector(ground_force, 0, 0, 0);
				// point on plane is [0, 0, 0]
				double ks_scaling = -ground_ks * VecDotProd(pos, N);
				double kd_scaling = ground_kd * VecDotProd(vel, N);
				VecCopy(N_1, N);
				VecCopy(N_2, N);
				VecScale(N_1, ks_scaling);
				VecScale(N_2, kd_scaling);
				VecSubtract(ground_force, N_1, N_2);
				VecAdd(net_force, net_force, ground_force);
			}

			switch (technique) {
			case euler:
				pos[0] = pos[0] + vel[0] * time_step;
				pos[1] = pos[1] + vel[1] * time_step;
				pos[2] = pos[2] + vel[2] * time_step;
				VecScale(net_force, time_step);
				VecScale(net_force, 1 / particle->mass);
				VecAdd(vel, vel, net_force);
				break;
			case symplectic:
				VecScale(net_force, time_step);
				VecScale(net_force, 1 / particle->mass);
				VecAdd(vel, vel, net_force);
				pos[0] = pos[0] + vel[0] * time_step;
				pos[1] = pos[1] + vel[1] * time_step;
				pos[2] = pos[2] + vel[2] * time_step;
				break;
			case verlet:
				Vector pos_verlet;
				particle->getVerletState(pos_verlet);
				pos_verlet[0] = pos_verlet[0] - vel[0] * time_step;
				pos_verlet[1] = pos_verlet[1] - vel[1] * time_step;
				pos_verlet[2] = pos_verlet[2] - vel[2] * time_step;

				pos[0] = (2 * pos[0]) - pos_verlet[0] + (net_force[0] * time_step * time_step);
				pos[1] = (2 * pos[1]) - pos_verlet[1] + (net_force[1] * time_step * time_step);
				pos[2] = (2  *pos[2]) - pos_verlet[2] + (net_force[2] * time_step * time_step);
				
				vel[0] = (pos[0] - pos_verlet[0]) / 2 * time_step;
				vel[1] = (pos[1] - pos_verlet[1]) / 2 * time_step;
				vel[2] = (pos[2] - pos_verlet[2]) / 2 * time_step;
				particle->setVerletState(pos_verlet);
				break;
			}
			particle->setState(pos);
			particle->setVelocity(vel[0], vel[1], vel[2]);
		}
		particle_index++;
	}
	forces.clear();
	return TCL_OK;
}