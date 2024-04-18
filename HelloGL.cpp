#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include "Structures.h"
#include "Cube.h"
#include "Camera.h"
#include "InputManager.h"

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180.0f); }

HelloGL::HelloGL(int argc, char* argv[]) 
{
	camera = new Camera(Vector3(5.0f, 5.0f, -170.f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));

	//Cube::LoadTXT((char*)"cube.txt");
	Cube::LoadOBJ((char*)"teapot.obj");
	for (int i = 0; i < cubeCount; i++)
	{
		cube[i] = new Cube(((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 10.0f);
		//cube[i] = new Cube(1.0f, 0.0f, 0.0f);
	}

	//camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = 1.0f;
	/*camera->eye.x = 5.0f; camera->eye.y = 5.0f; camera->eye.z = -170.0f;
	camera->center.x = 0.0f; camera->center.y = 0.0f; camera->center.z = 0.0f;
	camera->up.x = 0.0f; camera->up.y = 1.0f; camera->up.z = 0.0f;*/

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
	glutKeyboardFunc(GLUTCallbacks::KeyboardDown);
	glutKeyboardUpFunc(GLUTCallbacks::KeyboardUp);
	glutMouseFunc(GLUTCallbacks::Mouse);
	glutMotionFunc(GLUTCallbacks::Motion);

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
	camera->Update();

	//row1Rotation = fmod(row1Rotation + 0.5f, 360.0f);
	row2Rotation = fmod(row2Rotation + 1.5f, 360.0f);
	row3Rotation = fmod(row3Rotation - 0.5f, 360.0f);
	scale = abs(sin(glutGet(GLUT_ELAPSED_TIME) / 500.0f)) / 1.5f + 0.2f;

	for (int i = 0; i < cubeCount; i++)
	{
		cube[i]->Update();
	}

	Vector3 cameraForwardVector = camera->GetForwardVector();
	Vector3 cameraRightVector = camera->GetRightVector();
	Vector3 cameraUpVector = camera->GetUpVector();

	if (InputManager::IsKeyDown('w'))
		camera->OffsetPosition(cameraForwardVector);
	if (InputManager::IsKeyDown('s'))
		camera->OffsetPosition(-cameraForwardVector);
	if (InputManager::IsKeyDown('a'))
		camera->OffsetPosition(-cameraRightVector);
	if (InputManager::IsKeyDown('d'))
		camera->OffsetPosition(cameraRightVector);
	if (InputManager::IsKeyDown('e'))
		camera->OffsetPosition(cameraUpVector);
	if (InputManager::IsKeyDown('q'))
		camera->OffsetPosition(-cameraUpVector);

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

void HelloGL::KeyboardDown(unsigned char key, int x, int y)
{
	InputManager::OnKeyboardDown(key);
}

void HelloGL::KeyboardUp(unsigned char key, int x, int y)
{
	InputManager::OnKeyboardUp(key);
}

void HelloGL::Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		isRightClickDown = true;
		ShowCursor(FALSE);
		glutSetCursor(GLUT_CURSOR_NONE);
		lockedMousePosition.x = x + glutGet(GLUT_WINDOW_X);
		lockedMousePosition.y = y + glutGet(GLUT_WINDOW_Y);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		isRightClickDown = false;
		ShowCursor(TRUE);
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
}

void HelloGL::Motion(int x, int y)
{
	if (isRightClickDown)
	{
		Vector3 currentMousePosition = Vector3(x + glutGet(GLUT_WINDOW_X), y + glutGet(GLUT_WINDOW_Y), 0);
		Vector3 mouseOffset = currentMousePosition - lockedMousePosition;

		camera->OffsetRotation(Vector3(-mouseOffset.y, mouseOffset.x) * 0.01f);

		SetCursorPos(lockedMousePosition.x, lockedMousePosition.y);
	}
}

int main(int argc, char* argv[]) 
{
	srand(time(NULL));

	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}