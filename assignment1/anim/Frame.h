#ifndef MY_FRAME_H
#define MY_FRAME_H
#include "Point.h"

struct Frame 
{
	Point w;
	Point u;
	Point v;

	Frame(Point w, Point u, Point v) 
	{
		this->w = w;
		this->u = u;
		this->v = v;
	}
	Frame() 
	{
	
	};
};

#endif