#include "HermiteSpline.h"

HermiteSpline::HermiteSpline( const std::string& name ):
BaseSystem(name)
{
	catmull_rom = false;
}

void HermiteSpline::display(GLenum mode)
{
	// Draw the points provided
	glPointSize(5.0);
	set_colour(0.0f, 255.0f, 0.0f);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (auto& element : points) {
		glVertex3f(element.x, element.y, element.z);
	}
	glEnd();

	// Draw the curves
	set_colour(255.0f, 255.0f, 255.0f);
	glBegin(GL_LINE_STRIP);
	for (auto& element : draw_this) {
		glVertex3f(element.x,element.y,element.z);
	}
	glEnd();
}

int HermiteSpline::command(int argc, myCONST_SPEC char** argv)
{
	// No args given
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	// Catmull-Rom Enabled
	else if (strcmp(argv[0], "cr") == 0)
	{
		catmull_rom = !catmull_rom;
		calculateDraw();
	}
	// Add a new point-tangent pair
	else if (strcmp(argv[0], "add") == 0)
	{
		max_index++;
		Point point = Point((float)atof(argv[2]), (float)atof(argv[3]), (float)atof(argv[4]), 
							(float)atof(argv[5]), (float)atof(argv[6]), (float)atof(argv[7]));
		points.push_back(point);
		calculateDraw();
	}
	// Update a point/tangent vector
	else if (strcmp(argv[0], "set") == 0)
	{
		// Check to see if within added points
		if ((int)atof(argv[2]) > max_index)
		{
			animTcl::OutputMessage("Index Out of Range");
			return TCL_ERROR;
		}
		if (strcmp(argv[1], "point") == 0)
		{
			points[(int)atof(argv[2])].x = (float)atof(argv[3]);
			points[(int)atof(argv[2])].y = (float)atof(argv[4]);
			points[(int)atof(argv[2])].z = (float)atof(argv[5]);
		}
		else if (strcmp(argv[1], "tangent") == 0)
		{
			points[(int)atof(argv[2])].t_x = (float)atof(argv[3]);
			points[(int)atof(argv[2])].t_y = (float)atof(argv[4]);
			points[(int)atof(argv[2])].t_z = (float)atof(argv[5]);
		}
		calculateDraw();
	}
	// Load data from file
	else if (strcmp(argv[0], "load") == 0)
	{
		points.clear();
		draw_this.clear();
		ifstream file;
		string delim = " ";
		string s;
		char* c = new char[s.length() + 1];
		// Get first line
		file.open(argv[1]);
		if (file.is_open()) {
			getline(file, s);
			s.erase(0, s.find(delim) + delim.length());
			// Set new array max index
			max_index = stoi(s) - 1;

			// Fill Vector
			while (getline(file, s)) {
				string token;
				size_t pos = 0;
				double temp[6];
				int i = 0;
				while ((pos = s.find(delim)) != string::npos) {
					token = s.substr(0, pos);
					strcpy(c, token.c_str());
					if (i < 3)
					{
						temp[i] = stod(token);
					}
					else
					{
						temp[i] = stod(token);
					}
					s.erase(0, pos + delim.length());
					i++;
				}
				temp[5] = stod(s);
				Point point = Point(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
				points.push_back(point);
			}
			calculateDraw();
		}
		else
		{
			animTcl::OutputMessage("Failed to open file\n");
			return TCL_ERROR;
		}
	}
	// Export data to file
	else if (strcmp(argv[0], "export") == 0)
	{
		ofstream output(argv[1]);
		string hermite = "hermite " + to_string(max_index + 1);
		output << hermite << endl;
		for (auto& element : points) {
			output << to_string(element.x) << " " << to_string(element.y) << " " << to_string(element.z) << " "
					<< to_string(element.t_x) << " " << to_string(element.t_y) << " " << to_string(element.t_z) << endl;
		}
		output.close();
	}
	// Get Arclength
	else if (strcmp(argv[0], "getArcLength") == 0)
	{
		double t = (double)atof(argv[1]);
		if (t < 0 || t > 1) 
		{
			animTcl::OutputMessage("Please enter a value bewteen 0 and 1\n");
			return TCL_ERROR;
		}

		// Take t as the lower bound
		// If the lower bound is equal to a value, print it
		// Otherwise take the first value greater than the
		// lower bound and the previous value then interpolate
		map<double, double>::iterator low;
		low = arclengths.lower_bound(t);
		if (low->first == t)
		{
			animTcl::OutputMessage("%f\n", arclengths[low->first]);
		}
		else
		{
			// For some reason low->first gives first value 
			// less than lower bound instead of greater
			map<double, double>::iterator next = low--;
			double t_prev = low->first;
			double t_next = next->first;
			//animTcl::OutputMessage("%f", t_prev);
			//animTcl::OutputMessage("%f", t_next);
			double length = arclengths[t_prev] + (((t - t_prev) / (t_next - t_prev)) * (arclengths[t_next] - arclengths[t_prev]));
			//animTcl::OutputMessage("%f + ((%f - %f / %f - %f) * (%f - %f))", arclengths[t_prev], t, t_prev, t_next, t_prev, arclengths[t_next], arclengths[t_prev]);
			animTcl::OutputMessage("%f", length);
		}
	}
	// Test print
	else if (strcmp(argv[0], "printpoints") == 0)
	{
		int i = 0;
		for (auto& element : points) {
			animTcl::OutputMessage("Index %d Point: <%f, %f, %f> Tangent: <%f, %f, %f>\n", i,
				element.x, element.y, element.z,
				element.t_x, element.t_y, element.t_z);
			i++;
		}
	}
	else if (strcmp(argv[0], "printdraw") == 0)
	{
	int i = 0;
		for (auto& element : draw_this) {
			animTcl::OutputMessage("Index %d Point: <%f, %f, %f> Tangent: <%f, %f, %f>\n", i,
				element.x, element.y, element.z,
				element.t_x, element.t_y, element.t_z);
			i++;
		}
	}
	else if (strcmp(argv[0], "printarc") == 0)
	{
		int i = 0;
		for (auto& element : arclengths) {
			animTcl::OutputMessage("Parametric Entry: %f Value: %f", element.first, element.second);
		}
	}
	else if (strcmp(argv[0], "printcr") == 0)
	{
	int i = 0;
		for (auto& element : cr_points) {
			animTcl::OutputMessage("Index %d Point: <%f, %f, %f> Tangent: <%f, %f, %f>\n", i,
				element.x, element.y, element.z,
				element.t_x, element.t_y, element.t_z);
			i++;
		}
	}
	glutPostRedisplay();
	return TCL_OK;
}

// Approximates the curve between two points
// For every pair of points the arclength is also calculated
void HermiteSpline::calculateDraw() {
	draw_this.clear();
	arclengths.clear();

	if (max_index < 1) return;

	// Calculate Catmull-Rom if needed
	if (catmull_rom) calculateCR();

	Vector point1;
	Vector point2;
	Vector p1p2;
	arclengths[0.0] = 0.0;
	double arclength = 0.0;
	for (int i = 0; i < points.size() - 1; i++) {

		Point p1 = (catmull_rom) ? cr_points[i] : points[i];
		Point p2 = (catmull_rom) ? cr_points[i + 1] : points[i + 1];

		double x0 = p1.x;
		double y0 = p1.y;
		double z0 = p1.z;
		double x1 = p2.x;
		double y1 = p2.y;
		double z1 = p2.z;

		double x_s0 = p1.t_x;
		double y_s0 = p1.t_y;
		double z_s0 = p1.t_z;
		double x_s1 = p2.t_x;
		double y_s1 = p2.t_y;
		double z_s1 = p2.t_z;

		for (int i = 0; i < 20; i++) {
			double t = i * 0.05;
			double a = ((2 * pow(t, 3)) - (3 * pow(t, 2)) + 1);
			double b = (-2 * pow(t, 3)) + (3 * pow(t, 2));
			double c = (pow(t, 3) - (2 * pow(t, 2)) + t);
			double d = (pow(t, 3) - pow(t, 2));

			Point p((a * x0) + (b * x1) + (c * x_s0) + (d * x_s1),
				(a * y0) + (b * y1) + (c * y_s0) + (d * y_s1),
				(a * z0) + (b * z1) + (c * z_s0) + (d * z_s1));

			draw_this.push_back(p);
			if (draw_this.size() > 1) {
				setVector(point1, draw_this[draw_this.size() - 1].x, draw_this[draw_this.size() - 1].y, draw_this[draw_this.size() - 1].z);
				setVector(point2, draw_this[draw_this.size() - 2].x, draw_this[draw_this.size() - 2].y, draw_this[draw_this.size() - 2].z);
				VecSubtract(p1p2, point1, point2);
				arclength += VecLength(p1p2);
			}
		}
		double para_entry = static_cast<float>(i + 1) / static_cast<float>(max_index + 1);
		arclengths[para_entry] = arclength;
	}
	setVector(point1, points[points.size() - 1].x, points[points.size() - 1].y, points[points.size() - 1].z);
	setVector(point2, draw_this[draw_this.size() - 1].x, draw_this[draw_this.size() - 1].y, draw_this[draw_this.size() - 1].z);
	VecSubtract(p1p2, point1, point2);
	arclength += VecLength(p1p2);
	arclengths[1.0] = arclength;
	draw_this.push_back(points[points.size() - 1]);
}

void HermiteSpline::calculateCR()
{
	// First point remains unchanged
	cr_points.push_back(points[0]);

	// Delta t is the same for all points
	// so calculate delta t at first two points
	//double delta_t = ((double)2 / (double)max_index + 1) - ((double)1 / (double)max_index + 1);
	double delta_t = 1;

	for (int i = 1; i < max_index; i++) 
	{
		double s_x = (points[i + 1].x - points[i - 1].x) / (2 * delta_t);
		double s_y = (points[i + 1].y - points[i - 1].y) / (2 * delta_t);
		double s_z = (points[i + 1].z - points[i - 1].z) / (2 * delta_t);

		Point point(points[i].x, points[i].y, points[i].z, s_x, s_y, s_z);
		cr_points.push_back(point);
	}

	// Last point remains unchanged
	cr_points.push_back(points.back());
}