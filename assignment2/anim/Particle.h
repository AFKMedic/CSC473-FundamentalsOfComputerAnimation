#ifndef MY_PARTICLE_H
#define MY_PARTICLE_H

#include <util/util.h>
#include <vector>
#include <tuple>

using namespace std;

struct Particle
{
	boolean created;
	boolean fixed = false;
	double mass;
	double vx;
	double vy;
	double vz;
	double vx_prev;
	double vy_prev;
	double vz_prev;
	Vector pos;
	Vector pos_prev;
	Vector pos_verlet;
	// List of tuples to other particles that are connected by springs
	// tuple<index, ks, kd, rest length> 
	vector<tuple<int, double, double, double>> connected_to;

	Particle(double _mass, double _x, double _y, double _z, double _vx, double _vy, double _vz)
	{
		mass = _mass;
		vx = _vx;
		vy = _vy;
		vz = _vz;
		vx_prev = _vx;
		vy_prev = _vy;
		vz_prev = _vz;
		setVector(pos, _x, _y, _z);
		setVector(pos_prev, _x, _y, _z);
		created = true;
	};

	Particle()
	{
		mass = 0;
		vx = 0;
		vy = 0;
		vz = 0;
		vx_prev = 0;
		vy_prev = 0;
		vz_prev = 0;
		setVector(pos, 0, 0, 0);
		setVector(pos_prev, 0, 0, 0);
		created = false;
	};

	void getState(double* p)
	{
		VecCopy(p, pos);
	}

	void setState(double* p)
	{
		VecCopy(pos, p);
	}

	void getPrevState(double* p)
	{
		VecCopy(p, pos_prev);
	}

	void setPrevState(double* p)
	{
		VecCopy(pos_prev, p);
	}

	void getVerletState(double* p)
	{
		VecCopy(p, pos_verlet);
	}

	void setVerletState(double* p)
	{
		VecCopy(pos_verlet, p);
	}

	void getVelocity(double* v)
	{
		setVector(v, vx, vy, vz);
	}

	void setVelocity(double _vx, double _vy, double _vz)
	{
		vx = _vx;
		vy = _vy;
		vz = _vz;
	};

	void getPrevVelocity(double* v)
	{
		setVector(v, vx_prev, vy_prev, vz_prev);
	}

	void setPrevVelocity(double _vx, double _vy, double _vz)
	{
		vx_prev = _vx;
		vy_prev = _vy;
		vz_prev = _vz;
	};

	void createParticle(double _mass, double _x, double _y, double _z, double _vx, double _vy, double _vz)
	{
		mass = _mass;
		vx = _vx;
		vy = _vy;
		vz = _vz;
		setVector(pos, _x, _y, _z);
		setVector(pos_prev, _x, _y, _z);
		created = true;
	}

	void fix()
	{
		fixed = true;
	}

	void addConnection(int index, double ks, double kd, double rest_length)
	{
		connected_to.push_back(tuple<int, double, double, double>(index, ks, kd, rest_length));
	}

	void calculateInitialVerlet(double time_step)
	{
		Vector v0;
		setVector(v0, vx, vy, vz);
		VecScale(v0, time_step);
		VecCopy(pos_verlet, pos);
		VecSubtract(pos_verlet, pos_verlet, v0);
	}

	boolean getFixed()
	{
		return fixed;
	}
};
#endif
