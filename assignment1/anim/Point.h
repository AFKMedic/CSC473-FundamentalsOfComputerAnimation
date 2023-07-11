#ifndef MY_POINT_H
#define MY_POINT_H
struct Point {
	double x;
	double y;
	double z;
	double t_x;
	double t_y;
	double t_z;

	Point(double x, double y, double z, double t_x, double t_y, double t_z) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->t_x = t_x;
		this->t_y = t_y;
		this->t_z = t_z;
	}
	Point(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point() {
		double x = 1;
		double y = 1;
		double z = 1;
		double t_x = 1;
		double t_y = 1;
		double t_z = 1;
	}
};

#endif