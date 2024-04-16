#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include "Structures.h"
#include "Cube.h"

//Vertex HelloGL::vertices[] = { 
//	1, 1, 1, -1, 1, 1, -1,-1, 1, // v0-v1-v2 (front)
//	-1,-1, 1, 1,-1, 1, 1, 1, 1, // v2-v3-v0
//
//	1, 1, 1, 1,-1, 1, 1,-1,-1, // v0-v3-v4 (right)
//	1,-1,-1, 1, 1,-1, 1, 1, 1, // v4-v5-v0
//
//	1, 1, 1, 1, 1,-1, -1, 1,-1, // v0-v5-v6 (top)
//	-1, 1,-1, -1, 1, 1, 1, 1, 1, // v6-v1-v0
//
//	-1, 1, 1, -1, 1,-1, -1,-1,-1, // v1-v6-v7 (left)
//	-1,-1,-1, -1,-1, 1, -1, 1, 1, // v7-v2-v1
//
//	-1,-1,-1, 1,-1,-1, 1,-1, 1, // v7-v4-v3 (bottom)
//	1,-1, 1, -1,-1, 1, -1,-1,-1, // v3-v2-v7
//
//	1,-1,-1, -1,-1,-1, -1, 1,-1, // v4-v7-v6 (back)
//	-1, 1,-1, 1, 1,-1, 1,-1,-1 // v6-v5-v4
//};
//
//Color HelloGL::colors[] = { 
//	1, 1, 1, 1, 1, 0, 1, 0, 0, // v0-v1-v2 (front)
//	1, 0, 0, 1, 0, 1, 1, 1, 1, // v2-v3-v0
//
//	1, 1, 1, 1, 0, 1, 0, 0, 1, // v0-v3-v4 (right)
//	0, 0, 1, 0, 1, 1, 1, 1, 1, // v4-v5-v0
//
//	1, 1, 1, 0, 1, 1, 0, 1, 0, // v0-v5-v6 (top)
//	0, 1, 0, 1, 1, 0, 1, 1, 1, // v6-v1-v0
//
//	1, 1, 0, 0, 1, 0, 0, 0, 0, // v1-v6-v7 (left)
//	0, 0, 0, 1, 0, 0, 1, 1, 0, // v7-v2-v1
//
//	0, 0, 0, 0, 0, 1, 1, 0, 1, // v7-v4-v3 (bottom)
//	1, 0, 1, 1, 0, 0, 0, 0, 0, // v3-v2-v7
//
//	0, 0, 1, 0, 0, 0, 0, 1, 0, // v4-v7-v6 (back)
//	0, 1, 0, 0, 1, 1, 0, 0, 1  // v6-v5-v4
//};
//
//Vertex HelloGL::indexedVertices[] = { 
//	1, 1, 1, -1, 1, 1, // v0,v1,
//	-1,-1, 1, 1,-1, 1, // v2,v3
//	1,-1,-1, 1, 1,-1, // v4,v5
//	-1, 1,-1, -1,-1,-1 // v6,v7
//};
//
//Color HelloGL::indexedColors[] = { 
//	1, 1, 1, 1, 1, 0, // v0,v1,
//	1, 0, 0, 1, 0, 1, // v2,v3
//	0, 0, 1, 0, 1, 1, // v4,v5
//	0, 1, 0, 0, 0, 0 //v6,v7
//};
//
//GLushort HelloGL::indices[] = { 
//	0, 1, 2, 2, 3, 0, // front
//	0, 3, 4, 4, 5, 0, // right
//	0, 5, 6, 6, 1, 0, // top
//	1, 6, 7, 7, 2, 1, // left
//	7, 4, 3, 3, 2, 7, // bottom
//	4, 7, 6, 6, 5, 4 // back 
//};

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180.0f); }

HelloGL::HelloGL(int argc, char* argv[]) 
{
	camera = new Camera();
	//cubeCount = 2;

	for (int i = 0; i < cubeCount; i++)
	{
		cube[i] = new Cube(((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 10.0f);
		//cube[i] = new Cube(1.0f, 0.0f, 0.0f);
	}

	//camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = 1.0f;
	camera->eye.x = 5.0f; camera->eye.y = 5.0f; camera->eye.z = -170.0f;
	camera->center.x = 0.0f; camera->center.y = 0.0f; camera->center.z = 0.0f;
	camera->up.x = 0.0f; camera->up.y = 1.0f; camera->up.z = 0.0f;

	row1Rotation = 0.0f;
	row2Rotation = 0.0f;
	row3Rotation = 0.0f;
	scale = 0.0f;

	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glutKeyboardFunc(GLUTCallbacks::Keyboard);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, 800, 800);

	// Set the correct perspective
	gluPerspective(45, 1, 0.1f, 200);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
	camera = nullptr;

	for (int i = 0; i < cubeCount; i++)
	{
		delete cube[i];
	}
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	/*DrawTriangleFromAngles(30, 20, 0.3f, std::make_pair(-0.7f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(70, 70, 0.3f, std::make_pair(-0.15f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(60, 60, 0.3f, std::make_pair(0.4f, 0.5f), row1Rotation);
	DrawTriangleFromAngles(70, 45, 0.3f, std::make_pair(-0.7f, -0.1f), row2Rotation);
	DrawTriangleFromAngles(90, 45, 0.3f, std::make_pair(-0.15f, -0.1f), row2Rotation);
	DrawTriangleFromAngles(110, 30, 0.3f, std::make_pair(0.4f, -0.1f), row2Rotation);

	DrawRegularPolygon(std::make_pair(-0.5f, -0.5f), row3Rotation, 0.2f, true, 5);
	DrawRegularPolygon(std::make_pair(0, -0.5f), row3Rotation, 0.2f, true, 6);
	DrawRegularPolygon(std::make_pair(0.5f, -0.5f), row3Rotation, 0.2f, true, 7);*/

	/*glPushMatrix();
	glRotatef(row2Rotation, 1.0f, 1.0f, 1.0f);
	glPopMatrix();*/

	for (int i = 0; i < cubeCount; i++)
	{
		cube[i]->Draw();
	}

	glFlush();
	glutSwapBuffers();
}

void HelloGL::Update()
{
	glLoadIdentity();
	gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z, camera->center.x, camera->center.y, camera->center.z, camera->up.x, camera->up.y, camera->up.z);

	//row1Rotation = fmod(row1Rotation + 0.5f, 360.0f);
	row2Rotation = fmod(row2Rotation + 1.5f, 360.0f);
	row3Rotation = fmod(row3Rotation - 0.5f, 360.0f);
	scale = abs(sin(glutGet(GLUT_ELAPSED_TIME) / 500.0f)) / 1.5f + 0.2f;

	for (int i = 0; i < cubeCount; i++)
	{
		cube[i]->Update();
	}

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
	/*if (key == 'a')
		row1Rotation -= 20.5f;
	if (key == 'd')
		row1Rotation += 20.5f;*/

	if (key == 'w')
		camera->center.z -= 0.1f;
	if (key == 's')
		camera->center.z += 0.1f;
	if (key == 'a')
		camera->center.x -= 0.1f;
	if (key == 'd')
		camera->center.x += 0.1f;
}

int main(int argc, char* argv[]) 
{
	srand(time(NULL));

	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}