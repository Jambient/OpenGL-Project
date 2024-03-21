#include "HelloGL.h"
#include <cmath>
#include <iostream>

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180.0f); }

HelloGL::HelloGL(int argc, char* argv[]) 
{
	row1Rotation = 0.0f;
	row2Rotation = 0.0f;
	row3Rotation = 0.0f;

	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glutKeyboardFunc(GLUTCallbacks::Keyboard);
	glutMainLoop();
}

HelloGL::~HelloGL(void)
{

}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTriangleFromAngles(30, 20, 0.3f, std::make_pair(-0.7f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(70, 70, 0.3f, std::make_pair(-0.15f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(60, 60, 0.3f, std::make_pair(0.4f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(70, 45, 0.3f, std::make_pair(-0.7f, -0.1f), row2Rotation);
	DrawTriangleFromAngles(90, 45, 0.3f, std::make_pair(-0.15f, -0.1f), row2Rotation);
	DrawTriangleFromAngles(110, 30, 0.3f, std::make_pair(0.4f, -0.1f), row2Rotation);

	DrawRegularPolygon(std::make_pair(-0.5f, -0.5f), row3Rotation, 0.2f, true, 5);
	DrawRegularPolygon(std::make_pair(0, -0.5f), row3Rotation, 0.2f, true, 6);
	DrawRegularPolygon(std::make_pair(0.5f, -0.5f), row3Rotation, 0.2f, true, 7);

	glFlush();
	glutSwapBuffers();
}

void HelloGL::Update()
{
	//row1Rotation = fmod(row1Rotation + 0.5f, 360.0f);
	row2Rotation = fmod(row2Rotation + 1.5f, 360.0f);
	row3Rotation = fmod(row3Rotation - 0.5f, 360.0f);
	scale = abs(sin(glutGet(GLUT_ELAPSED_TIME) / 500.0f)) / 1.5f + 0.2f;

	glutPostRedisplay();
}

void HelloGL::DrawPolygon()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glRotatef(0, 0, 0, -1.0f);
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
void HelloGL::DrawRegularPolygon(vector2 center, float rotation, float radius, bool filled, float sides)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(center.first, center.second, 0.0f);
	glRotatef(rotation, 0, 0, -1.0f);
	glScalef(scale, scale, 1);

	glBegin(filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
	for (int i = 0; i < sides; i++) {
		float angle = 6.2832 * i / sides;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glPopMatrix();
}

void HelloGL::DrawTriangleFromAngles(float angle1, float angle2, float base, vector2 pos, float rotation)
{
	if (angle1 + angle2 >= 180) {
		std::cout << "Triangle with angles " << angle1 << " and " << angle2 << " is not possible.";
		return;
	}

	float angle3 = 180 - angle1 - angle2;
	float side1 = (base * sin(DegreesToRadians(angle3))) / sin(DegreesToRadians(angle2));

	vector2 side1_direction = std::make_pair(cos(DegreesToRadians(angle1)), sin(DegreesToRadians(angle1)));

	vector2 secondVertex = std::make_pair(side1_direction.first * side1, side1_direction.second * side1);
	vector2 centroid = std::make_pair((secondVertex.first + base) / 3.0f, secondVertex.second / 3.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(pos.first + centroid.first, pos.second + centroid.second, 0.0f);
	glRotatef(rotation, 0, 0, -1.0f);
	glTranslatef(-centroid.first, -centroid.second, 0.0f);
	glBegin(GL_POLYGON);
	{
		glVertex2f(0.0f, 0.0f);
		glVertex2f(secondVertex.first, secondVertex.second);
		glVertex2f(base, 0.0f);
	}
	glEnd();
	glPopMatrix();
}

void HelloGL::Keyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
		row1Rotation -= 20.5f;
	if (key == 'd')
		row1Rotation += 20.5f;
}



int main(int argc, char* argv[]) 
{
	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}