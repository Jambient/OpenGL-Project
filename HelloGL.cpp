#include "HelloGL.h"
#include <cmath>
#include <iostream>

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180); }

HelloGL::HelloGL(int argc, char* argv[]) 
{
	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutMainLoop();
}

HelloGL::~HelloGL(void)
{

}

void HelloGL::Display()
{ 

	glClear(GL_COLOR_BUFFER_BIT);
	//DrawPolygon();
	//DrawRegularPolygon(0.5f, true, 3);
	DrawTriangleFromAngles(30, 20, 0.3f, std::make_pair(-0.7f, 0.5f));
	DrawTriangleFromAngles(70, 70, 0.3f, std::make_pair(-0.15f, 0.5f));
	DrawTriangleFromAngles(60, 60, 0.3f, std::make_pair(0.4f, 0.5f));
	DrawTriangleFromAngles(70, 45, 0.3f, std::make_pair(-0.7f, -0.1f));
	DrawTriangleFromAngles(90, 45, 0.3f, std::make_pair(-0.15f, -0.1f));
	DrawTriangleFromAngles(110, 30, 0.3f, std::make_pair(0.4f, -0.1f));

	DrawRegularPolygon(std::make_pair(-0.5f, -0.5f), 0.2f, true, 5);
	DrawRegularPolygon(std::make_pair(0, -0.5f), 0.2f, true, 6);
	DrawRegularPolygon(std::make_pair(0.5f, -0.5f), 0.2f, true, 7);

	glFlush();
}

void HelloGL::DrawPolygon()
{
	glMatrixMode(GL_MODELVIEW); // Make sure the modelview matrix is active
	glLoadIdentity(); // Reset the modelview matrix

	glPushMatrix();
	glRotatef(-30.0f, 0, 0, 1.0f); // Rotate the modelview matrix
	glBegin(GL_POLYGON);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.75, 0.5);
		glVertex2f(0.75, 0.5);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.75, -0.5);
		glVertex2f(-0.75, -0.5);
	}
	glEnd();
	glPopMatrix();
}
void HelloGL::DrawRegularPolygon(vector2 center, float radius, bool filled, float sides)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(center.first, center.second, 0.0f);

	glBegin(filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
	for (int i = 0; i < sides; i++) {
		float angle = 6.2832 * i / sides;  // 6.2832 represents 2*PI
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glPopMatrix();
}

vector2 intersection(vector2 A, vector2 B, vector2 C, vector2 D) {
	// Line AB represented as a1x + b1y = c1
	double a = B.second - A.second;
	double b = A.first - B.first;
	double c = a * (A.first) + b * (A.second);

	// Line CD represented as a2x + b2y = c2
	double a1 = D.second - C.second;
	double b1 = C.first - D.first;
	double c1 = a1 * (C.first) + b1 * (C.second);

	double det = a * b1 - a1 * b;
	if (det == 0) {
		return std::make_pair(FLT_MAX, FLT_MAX);
	}
	else {
		double x = (b1 * c - b * c1) / det;
		double y = (a * c1 - a1 * c) / det;
		return std::make_pair(x, y);
	}
}

void HelloGL::DrawTriangleFromAngles(float angle1, float angle2, float base, vector2 pos)
{
	if (angle1 + angle2 >= 180) {
		std::cout << "Triangle with angles " << angle1 << " and " << angle2 << " is not possible.";
		return;
	}

	/*vector2 a = std::make_pair(0, 0);
	vector2 b = std::make_pair(cos(angle1), sin(angle1));
	vector2 c = std::make_pair(base, 0);
	vector2 d = std::make_pair(base + cos(6.2831852 - angle2), sin(6.2831852 - angle2));

	vector2 inter = intersection(a, b, c, d);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(pos.first, pos.second, 0.0f);
	glBegin(GL_POLYGON);
	{
		glVertex2f(0.0f, 0.0f);
		glVertex2f(inter.first, inter.second);
		glVertex2f(base, 0.0f);
	}
	glEnd();
	glPopMatrix();*/

	float angle3 = 180 - angle1 - angle2;

	float side1 = (base * sin(DegreesToRadians(angle3))) / sin(DegreesToRadians(angle2));
	/*float side2 = (base * sin(DegreesToRadians(angle1))) / sin(DegreesToRadians(angle3));
	std::cout << side1 << " : " << side2 << std::endl;*/

	vector2 side1_direction = std::make_pair(cos(DegreesToRadians(angle1)), sin(DegreesToRadians(angle1)));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(pos.first, pos.second, 0.0f);
	glBegin(GL_POLYGON);
	{
		glVertex2f(0.0f, 0.0f);
		glVertex2f(side1_direction.first * side1, side1_direction.second * side1);
		glVertex2f(base, 0.0f);
	}
	glEnd();
	glPopMatrix();
}



int main(int argc, char* argv[]) 
{
	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}