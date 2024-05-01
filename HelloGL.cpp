#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include "Structures.h"
#include "Cube.h"
#include "MovingCube.h"
#include "Camera.h"
#include "InputManager.h"
#include "MeshLoader.h"

float DegreesToRadians(float degrees) { return degrees * (3.1415926 / 180.0f); }
#define VIEWPORT_WIDTH 800
#define VIEWPORT_HEIGHT 800

HelloGL::HelloGL(int argc, char* argv[]) 
{
	InitGL(argc, argv);
	InitObjects();

	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
	camera = nullptr;

	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Draw();
	}

	glFlush();
	glutSwapBuffers();
}

void HelloGL::Update()
{
	glLoadIdentity();
	camera->Update(viewMatrix);

	//row1Rotation = fmod(row1Rotation + 0.5f, 360.0f);
	row2Rotation = fmod(row2Rotation + 1.5f, 360.0f);
	row3Rotation = fmod(row3Rotation - 0.5f, 360.0f);
	scale = abs(sin(glutGet(GLUT_ELAPSED_TIME) / 500.0f)) / 1.5f + 0.2f;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update();
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

void HelloGL::Raycast(int mouseX, int mouseY)
{
	// convert to 3d normalised device coordinates
	float viewportX = (2.0f * mouseX) / VIEWPORT_WIDTH - 1.0f;
	float viewportY = 1.0f - (2.0f * mouseY) / VIEWPORT_HEIGHT;
	glm::vec3 rayNDS(viewportX, viewportY, 1.0f);

	// convert to 4d homogeneous clip coordinates
	glm::vec4 rayClip = glm::vec4(rayNDS.x, rayNDS.y, -1.0, 1.0);

	// convert to 4d eye coordinates
	glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

	// convert to 4d world coordinates
	glm::vec3 rayDirection = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * rayEye));
	Vector3 rayOrigin = camera->GetPosition();

	Mesh* cubeMesh = MeshLoader::LoadTXT((char*)"cube.txt");
	Texture2D* texture3 = new Texture2D();
	texture3->LoadBMP((char*)"transparent-cat.bmp");

	float closestIntersection = std::numeric_limits<float>::max();
	SceneObject* closestObject = nullptr;

	for (SceneObject* obj : objects) {
		// Perform ray-object intersection test
		/*float intersectionDistance = obj->RayIntersection(rayOrigin, rayDirection);*/
		Vector3 offset = obj->GetPosition() - rayOrigin;
		float distanceAlongRay = glm::dot(glm::vec3(offset.x, offset.y, offset.z), rayDirection);

		if (distanceAlongRay < 0) continue;

		glm::vec3 intersectionPoint = glm::vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z) + rayDirection * distanceAlongRay;

		float intersectionDistance = obj->SignedDistanceField(intersectionPoint);

		std::cout << "DISTANCE: " << intersectionDistance << std::endl;

		if (intersectionDistance > 0 && intersectionDistance < closestIntersection) {
			closestIntersection = intersectionDistance;
			closestObject = obj;
		}
	}

	std::cout << (closestObject != nullptr) << std::endl;
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
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{
		isRightClickDown = true;
		ShowCursor(FALSE);
		glutSetCursor(GLUT_CURSOR_NONE);
		lockedMousePosition.x = x + glutGet(GLUT_WINDOW_X);
		lockedMousePosition.y = y + glutGet(GLUT_WINDOW_Y);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) 
	{
		isRightClickDown = false;
		ShowCursor(TRUE);
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Raycast(x, y);
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

void HelloGL::InitObjects()
{
	camera = new Camera(Vector3(5.0f, 5.0f, -170.f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));

	Texture2D* texture = new Texture2D();
	texture->LoadRAW((char*)"penguins.raw", 512, 512);
	//texture->LoadTGA((char*)"cat.tga");

	Texture2D* texture2 = new Texture2D();
	texture2->LoadBMP((char*)"funnycat.bmp");

	Texture2D* texture3 = new Texture2D();
	texture3->LoadBMP((char*)"transparent-cat.bmp");

	Texture2D* texture4 = new Texture2D();
	texture4->LoadPNG((char*)"new-cat.png");

	Mesh* cubeMesh = MeshLoader::LoadTXT((char*)"cube.txt");
	Mesh* pyramidMesh = MeshLoader::LoadTXT((char*)"pyramid.txt");
	/*Mesh* teapotMesh = MeshLoader::LoadOBJ((char*)"teapot.obj");
	Mesh* cowMesh = MeshLoader::LoadOBJ((char*)"cow.obj");*/

	objects = std::vector<SceneObject*>();
	for (int i = 0; i < 1; i++)
	{
		objects.push_back(new Cube(cubeMesh, texture2, ((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 5.0f, rand() % 360));
	}
	/*for (int i = 0; i < objectCount / 4; i++)
	{
		objects[i] = new Cube(cubeMesh, texture2, ((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 5.0f, rand() % 360);
	}
	for (int i = objectCount / 4; i < objectCount / 2; i++)
	{
		objects[i] = new Cube(cubeMesh, texture2, ((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 5.0f, rand() % 360);
	}
	for (int i = objectCount / 2; i < objectCount; i++)
	{
		objects[i] = new Cube(cubeMesh, texture2, ((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 5.0f, rand() % 360);
	}*/

	row1Rotation = 0.0f;
	row2Rotation = 0.0f;
	row3Rotation = 0.0f;
	scale = 0.0f;
}

void HelloGL::InitGL(int argc, char* argv[])
{

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
	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	// Set the correct perspective
	gluPerspective(45, 1, 0.1f, 200);
	projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 200.0f);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

int main(int argc, char* argv[]) 
{
	srand(time(NULL));

	HelloGL* game = new HelloGL(argc, argv);

	return 0;
}