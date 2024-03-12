#include "HelloGL.h"
#include <cmath>

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
	DrawCircleOutline(0.5f);
	glFlush();
}

void HelloGL::DrawPolygon()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.75, 0.5);
		glVertex2f(0.75, 0.5);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.75, -0.5);
		glVertex2f(-0.75, -0.5);
		glEnd();
	}
}
void HelloGL::DrawCircleOutline(float radius, float sides)
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < sides; i++) {
		float angle = 6.2832 * i / 64;  // 6.2832 represents 2*PI
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

int main(int argc, char* argv[]) 
{
	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}