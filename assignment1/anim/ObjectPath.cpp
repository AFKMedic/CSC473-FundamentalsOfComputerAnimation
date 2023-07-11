#include "ObjectPath.h"

ObjectPath::ObjectPath(const std::string& name) :
	BaseSystem(name),
	m_sx(0.015f),
	m_sy(0.015f),
	m_sz(0.015f)
{
	m_model.ReadOBJ("data/porsche.obj");
}

void ObjectPath::display(GLenum mode)
{

	glPointSize(5.0);
	set_colour(0.0f, 255.0f, 0.0f);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (auto& element : points) {
		glVertex3f(element.x, element.y, element.z);
	}
	glEnd();

	set_colour(255.0f, 255.0f, 255.0f);
	glBegin(GL_LINE_STRIP);
	for (auto& element : draw_this) {
		glVertex3f(element.x, element.y, element.z);
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslated(m_pos[0], m_pos[1], m_pos[2]);
	// Quat rotation not working
	//glRotatef(w_rot, x_rot, y_rot, z_rot);
	glScalef(m_sx, m_sy, m_sz);
	if (m_model.numvertices > 0)
		glmDraw(&m_model, GLM_SMOOTH | GLM_MATERIAL);
	else
		glutSolidSphere(1.0, 20, 20);
	
	glPopMatrix();
	glPopAttrib();
}

void ObjectPath::getState(double* p)
{

	VecCopy(p, m_pos);

}

void ObjectPath::setState(double  *p, double t, double i)
{	
	map<double, Point>::iterator l;
	l = arcPoints.lower_bound(t);

	map<double, Point>::iterator n = l--;
	double t_prev = l->first;
	double t_next = n->first;
	double new_x = arcPoints[t_prev].x +
		((t - t_prev) * ((arcPoints[t_next].x - arcPoints[t_prev].x) / (t_next - t_prev)));
	double new_y = arcPoints[t_prev].y +
		((t - t_prev) * ((arcPoints[t_next].y - arcPoints[t_prev].y) / (t_next - t_prev)));
	double new_z = arcPoints[t_prev].z +
		((t - t_prev) * ((arcPoints[t_next].z - arcPoints[t_prev].z) / (t_next - t_prev)));

	// Quat rotation not working
	//Quaternion q_x,q_y,q_z;
	//map<double, Frame>::iterator l2;
	//l2 = frames.lower_bound(t);

	//map<double, Frame>::iterator n2 = l2--;
	//double from[3], next[3];
	//from[0] = frames[l2->first].u.x;
	//from[1] = frames[l2->first].u.y;
	//from[2] = frames[l2->first].u.z;
	//next[0] = frames[n2->first].u.x;
	//next[1] = frames[n2->first].u.y;
	//next[2] = frames[n2->first].u.z;
	//q_x.rotateAxis(from, next);
	////animTcl::OutputMessage("q1:%f q2:%f q3:%f q4:%f",	q_x[0], q_x[1], q_x[2], q_x[3]);
	//x_rot = q_x[0];
	//y_rot = q_x[1];
	//z_rot = q_x[2];
	//w_rot = q_x[3];


	setVector(tangent,
		arcPoints[t_next].x - arcPoints[t_prev].x,
		arcPoints[t_next].y - arcPoints[t_prev].y,
		arcPoints[t_next].z - arcPoints[t_prev].z);
	VecNormalize(tangent);
	setVector(m_pos, new_x, new_y, new_z);

}

void ObjectPath::reset(double time)
{
	setVector(m_pos, points[0].x, points[0].y, points[0].z);
}

double ObjectPath::getArclength()
{
	return total_length;
}

int ObjectPath::command(int argc, myCONST_SPEC char** argv)
{
	// No args given
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
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
			setVector(m_pos, points[0].x, points[0].y, points[0].z);
			calculateDraw();
		}
		else
		{
			animTcl::OutputMessage("Failed to open file\n");
			return TCL_ERROR;
		}
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
			animTcl::OutputMessage("%f\n", low->first);
		}
		else
		{
			// For some reason low->first gives first value 
			// less than lower bound instead of greater
			map<double, double>::iterator next = low--;
			double t_prev = low->first;
			double t_next = next->first;
			double length = arclengths[t_prev] + (((t - t_prev) / (t_next - t_prev)) * (arclengths[t_next] - arclengths[t_prev]));
			animTcl::OutputMessage("%f", length);
		}
	}
	else if (strcmp(argv[0], "printarc2") == 0)
	{
		int i = 0;
		for (auto& element : arcPoints) {
			animTcl::OutputMessage("Length:%f X:%f Y:%f Z:%f", element.first, element.second.x, element.second.y, element.second.z);
		}
	}
	else if (strcmp(argv[0], "printarc") == 0)
	{
		int i = 0;
		for (auto& element : arclengths) {
			animTcl::OutputMessage("Parametric Entry: %f Value: %f", element.first, element.second);
		}
	}
	glutPostRedisplay();
	return TCL_OK;
}

// Approximates the curve between two points
// For every pair of points the arclength is also calculated
void ObjectPath::calculateDraw() {
	draw_this.clear();
	arclengths.clear();

	if (max_index < 1) return;

	Vector point1;
	Vector point2;
	Vector p1p2;
	arclengths[0.0] = 0.0;
	arcPoints[0.0] = points[0];
	double arclength = 0.0;
	for (int i = 0; i < points.size() - 1; i++) {

		Point p1 = points[i];
		Point p2 = points[i + 1];

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

		// Create 20 points to approximate the curve
		for (int i = 0; i < 20; i++) {

			double t = i * 0.05;

			double h1 = ((2 * pow(t, 3)) - (3 * pow(t, 2)) + 1);
			double h2 = (-2 * pow(t, 3)) + (3 * pow(t, 2));
			double h3 = (pow(t, 3) - (2 * pow(t, 2)) + t);
			double h4 = (pow(t, 3) - pow(t, 2));

			/*
			double h_1_1 = (6 * pow(t, 2)) + (6 * t);
			double h_1_2 = (-6 * pow(t, 2)) + (6 * t);
			double h_1_3 = (3 * pow(t, 2) - (4 * t) + 1);
			double h_1_4 = (3 * pow(t, 2)) - (2 * t);

			double h_2_1 = 12 * t + 6;
			double h_2_2 = -12 * t + 6;
			double h_2_3 = 6 * t - 4;
			double h_2_4 = 6 * t - 2;
			*/

			// P(s)
			Point p((h1 * x0) + (h2 * x1) + (h3 * x_s0) + (h4 * x_s1),
					(h1 * y0) + (h2 * y1) + (h3 * y_s0) + (h4 * y_s1),
					(h1 * z0) + (h2 * z1) + (h3 * z_s0) + (h4 * z_s1));
			/*
			// P`(s)
			Point p_1(	(h_1_1 * x0) + (h_1_2 * x1) + (h_1_3 * x_s0) + (h_1_4 * x_s1),
						(h_1_1 * y0) + (h_1_2 * y1) + (h_1_3 * y_s0) + (h_1_4 * y_s1),
						(h_1_1 * z0) + (h_1_2 * z1) + (h_1_3 * z_s0) + (h_1_4 * z_s1));
			// P``(s)		  
			Point p_2(	(h_2_1 * x0) + (h_2_2 * x1) + (h_2_3 * x_s0) + (h_2_4 * x_s1),
						(h_2_1 * y0) + (h_2_2 * y1) + (h_2_3 * y_s0) + (h_2_4 * y_s1),
						(h_2_1 * z0) + (h_2_2 * z1) + (h_2_3 * z_s0) + (h_2_4 * z_s1));
			*/

			draw_this.push_back(p);
			if (draw_this.size() > 1) {

				setVector(point1, draw_this[draw_this.size() - 1].x, draw_this[draw_this.size() - 1].y, draw_this[draw_this.size() - 1].z);
				setVector(point2, draw_this[draw_this.size() - 2].x, draw_this[draw_this.size() - 2].y, draw_this[draw_this.size() - 2].z);
				VecSubtract(p1p2, point1, point2);
				arclength += VecLength(p1p2);
			}
			arcPoints[arclength] = p;

			// Quat rotation not working
			/*
			Vector v_w;
			Vector v_p2;
			Vector v_u;
			Vector v_v;
			setVector(v_w, p_1.x, p_1.y, p_1.z);
			VecNormalize(v_w);
			setVector(v_p2, p_2.x, p_2.y, p_2.z);
			VecNormalize(v_p2);
			VecCrossProd(v_u, v_w, v_p2);
			VecNormalize(v_u);
			VecCrossProd(v_v, v_w, v_u);
			VecNormalize(v_v);
			Point w(v_w[0], v_w[1], v_w[2]);
			Point u(v_u[0], v_u[1], v_u[2]);
			Point v(v_v[0], v_v[1], v_v[2]);
			animTcl::OutputMessage("-----------------------------");
			animTcl::OutputMessage("W_x:%f W_y:%f W_z:%f", w.x, w.y, w.z);
			animTcl::OutputMessage("U_x:%f U_y:%f Uz:%f", u.x, u.y, u.z);
			animTcl::OutputMessage("V_x:%f V_y:%f V_z:%f", v.x, v.y, v.z);
			animTcl::OutputMessage("-----------------------------");
			Frame f(w, u, v);
			frames[arclength] = f;
			*/
		}
		double para_entry = static_cast<float>(i + 1) / static_cast<float>(max_index);
		arclengths[para_entry] = arclength;
	}
	// Add the final point
	setVector(point1, points[points.size() - 1].x, points[points.size() - 1].y, points[points.size() - 1].z);
	setVector(point2, draw_this[draw_this.size() - 1].x, draw_this[draw_this.size() - 1].y, draw_this[draw_this.size() - 1].z);
	VecSubtract(p1p2, point1, point2);
	arclength += VecLength(p1p2);
	arclengths[1.0] = arclength;
	arcPoints[arclength] = points[points.size() - 1];
	total_length = arclength;
	draw_this.push_back(points[points.size() - 1]);
}

void ObjectPath::test(double i) {



}