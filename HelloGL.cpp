#include "HelloGL.h"
#include <cmath>
#include <iostream>

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180.0f); }

HelloGL::HelloGL(int argc, char* argv[]) 
{
	bodies[0] = Body({ 0, 0 }, { 0, 0 }, 10000.0f);
	bodies[1] = Body({ 0.1f, 0 }, { 0, 0.7f }, 400.0f);
	bodies[1] = Body({ 0.3f, 0 }, { 0, 1.5f }, 400.0f);

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

	for (size_t i = 0; i < body_count; i++)
	{
		DrawRegularPolygon(bodies[i].position, 0, 0.02f, true, 5);
	}

	glFlush();
	glutSwapBuffers();
}

void HelloGL::Update()
{
	for (size_t b1_idx = 0; b1_idx < body_count; b1_idx++)
	{
		Vector2 a_g = { 0, 0 };

		for (size_t b2_idx = 0; b2_idx < body_count; b2_idx++)
		{
			if (b2_idx != b1_idx)
			{
				Vector2 r_vector = {bodies[b2_idx].position.x - bodies[b1_idx].position.x, bodies[b2_idx].position.y - bodies[b1_idx].position.y};
				float r_mag = sqrt(r_vector.x * r_vector.x + r_vector.y * r_vector.y);
				float acceleration = (GRAVITY * (bodies[b2_idx].mass) / pow(r_mag, 2.0)) / bodies[b1_idx].mass;
				Vector2 r_unit_vector = { r_vector.x / r_mag, r_vector.y / r_mag };

				a_g.x = acceleration * r_unit_vector.x;
				a_g.y = acceleration * r_unit_vector.y;
			}
		}

		bodies[b1_idx].velocity.x += a_g.x * 1/60.0f * timeScale;
		bodies[b1_idx].velocity.y += a_g.y * 1/60.0f * timeScale;
	}

	for (size_t i = 0; i < body_count; i++)
	{
		bodies[i].position.x += bodies[i].velocity.x * 1/60.0f * timeScale;
		bodies[i].position.y += bodies[i].velocity.y * 1/60.0f * timeScale;
	}

	glutPostRedisplay();
}

void HelloGL::DrawRegularPolygon(Vector2 center, float rotation, float radius, bool filled, float sides)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(center.x, center.y, 0.0f);
	glRotatef(rotation, 0, 0, -1.0f);

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

//void HelloGL::DrawTriangleFromAngles(float angle1, float angle2, float base, Vector2 pos, float rotation)
//{
//	if (angle1 + angle2 >= 180) {
//		std::cout << "Triangle with angles " << angle1 << " and " << angle2 << " is not possible.";
//		return;
//	}
//
//	float angle3 = 180 - angle1 - angle2;
//	float side1 = (base * sin(DegreesToRadians(angle3))) / sin(DegreesToRadians(angle2));
//
//	vector2 side1_direction = std::make_pair(cos(DegreesToRadians(angle1)), sin(DegreesToRadians(angle1)));
//
//	vector2 secondVertex = std::make_pair(side1_direction.first * side1, side1_direction.second * side1);
//	vector2 centroid = std::make_pair((secondVertex.first + base) / 3.0f, secondVertex.second / 3.0f);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	glPushMatrix();
//	glTranslatef(pos.first + centroid.first, pos.second + centroid.second, 0.0f);
//	glRotatef(rotation, 0, 0, -1.0f);
//	glTranslatef(-centroid.first, -centroid.second, 0.0f);
//	glBegin(GL_POLYGON);
//	{
//		glVertex2f(0.0f, 0.0f);
//		glVertex2f(secondVertex.first, secondVertex.second);
//		glVertex2f(base, 0.0f);
//	}
//	glEnd();
//	glPopMatrix();
//}

void HelloGL::Keyboard(unsigned char key, int x, int y)
{
}



int main(int argc, char* argv[]) 
{
	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}