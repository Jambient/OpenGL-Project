#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include "Commons.h"
#include "Cube.h"
#include "MovingCube.h"
#include "Camera.h"
#include "InputManager.h"
#include "MeshLoader.h"

#define VIEWPORT_WIDTH 800
#define VIEWPORT_HEIGHT 800

HelloGL::HelloGL(int argc, char* argv[]) 
{
	InitGL(argc, argv);
	InitObjects();
	InitLighting();
	//InitFont();

	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
	camera = nullptr;

	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	skybox->Draw();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	int currentY = 30;
	Color textColor = { 1.0f, 1.0f, 1.0f };
	Color highlightedColor = { 0.0f, 0.0f, 1.0f };
	glm::vec3 positionOffset = glm::vec3();
	glm::vec3 selectedObjectOffset = glm::vec3();
	currentScene->IterateTree(currentScene->GetRoot(), 0, [&](TreeNode* node, int depth, TraversalType type) {
		glm::vec3 objectPositionOffset = glm::vec3();
		if (node->object != nullptr)
		{
			objectPositionOffset = node->object->GetPosition();
		}

		if (type & PRE_ORDER)
		{
			RenderText(node->name.c_str(), glm::ivec2(20 + depth * 30, currentY), node == selectedObject ? highlightedColor : textColor);
			currentY += 30;

			if (node->object != nullptr)
			{
				if (node == selectedObject)
				{
					selectedObjectOffset = positionOffset;
				}
				node->object->Draw(node == selectedObject, positionOffset);
				positionOffset += objectPositionOffset;
			}
		}
		else if (type & IN_ORDER)
		{
			positionOffset -= objectPositionOffset;
		}
		}, TraversalType(PRE_ORDER | IN_ORDER));

	RenderText(("FPS: " + std::to_string(fps)).c_str(), { VIEWPORT_WIDTH - 80, 30 });
	std::string cameraModeText = "Camera Mode: ";
	cameraModeText += (camera->GetViewMode() == ViewMode::FLY) ? "FLY" : "ORBIT";
	RenderText(cameraModeText.c_str(), {VIEWPORT_WIDTH / 2 - 80, 30});

	if (selectedObject != nullptr)
	{
		AABBox bbox = selectedObject->object->GetBoundingBox();
		glm::vec3 center = (bbox.bounds[0] + bbox.bounds[1]) / 2.0f;
		camera->SetOrbitTargetPosition(center + selectedObjectOffset);
	}

	glFlush();
	glutSwapBuffers();
}

glm::vec3 HelloGL::GetClosestAxisAlignedVector(glm::vec3 vec)
{
	vec = glm::normalize(vec);
	glm::vec3 closestVector;
	float closestDotProduct = -1;

	for (glm::vec3& aaV : axisAlignedVectors)
	{
		float dotProduct = glm::dot(vec, aaV);
		if (dotProduct > closestDotProduct)
		{
			closestDotProduct = dotProduct;
			closestVector = aaV;
		}
	}

	return closestVector;
}

void HelloGL::Update()
{
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentTime - previousElapsedTime) / 1000.0f;
	UpdateFPS(1.0f / deltaTime);

	// update lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, &(lightData->Ambient.x));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &(lightData->Diffuse.x));
	glLightfv(GL_LIGHT0, GL_SPECULAR, &(lightData->Specular.x));
	//glLightfv(GL_LIGHT0, GL_POSITION, &(lightPosition->x));
	GLfloat testPos[] = { lightPosition->x, lightPosition->y, lightPosition->z, lightPosition->w };
	glLightfv(GL_LIGHT0, GL_POSITION, testPos);

	glLoadIdentity();

	//row1Rotation = fmod(row1Rotation + 0.5f, 360.0f);
	row2Rotation = fmod(row2Rotation + 5.0f, VIEWPORT_WIDTH);
	row3Rotation = fmod(row3Rotation - 0.5f, 360.0f);
	scale = abs(sin(currentTime / 500.0f)) / 1.5f + 0.2f;

	currentScene->IterateTree(currentScene->GetRoot(), 0, [&](TreeNode* node, int depth, TraversalType type) {
		if (node->object != nullptr)
		{
			Animation* anim = node->object->GetAnimation();
			if (anim != nullptr)
				anim->Update(deltaTime);
		}
	}, PRE_ORDER);

	glm::vec3 cameraForwardVector = camera->GetForwardVector();
	glm::vec3 cameraRightVector = camera->GetRightVector();
	glm::vec3 cameraUpVector = camera->GetUpVector();

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

	if (selectedObject != nullptr)
	{
		if (InputManager::IsSpecialKeyDown(GLUT_KEY_LEFT))
			selectedObject->object->OffsetPosition(GetClosestAxisAlignedVector(-cameraRightVector) * 0.1f);
		if (InputManager::IsSpecialKeyDown(GLUT_KEY_RIGHT))
			selectedObject->object->OffsetPosition(GetClosestAxisAlignedVector(cameraRightVector) * 0.1f);
		if (InputManager::IsSpecialKeyDown(GLUT_KEY_UP))
			selectedObject->object->OffsetPosition(GetClosestAxisAlignedVector(cameraUpVector) * 0.1f);
		if (InputManager::IsSpecialKeyDown(GLUT_KEY_DOWN))
			selectedObject->object->OffsetPosition(GetClosestAxisAlignedVector(-cameraUpVector) * 0.1f);
	}

	camera->Update(viewMatrix);
	skybox->SetPosition(camera->GetPosition());
	previousElapsedTime = currentTime;
	glutPostRedisplay();
}

glm::vec3 HelloGL::GetRayFromScreenPosition(int x, int y)
{
	// convert to 3d normalised device coordinates
	float viewportX = (2.0f * x) / VIEWPORT_WIDTH - 1.0f;
	float viewportY = 1.0f - (2.0f * y) / VIEWPORT_HEIGHT;
	glm::vec3 rayNDS(viewportX, viewportY, 1.0f);

	// convert to 4d homogeneous clip coordinates
	glm::vec4 rayClip = glm::vec4(rayNDS.x, rayNDS.y, -1.0, 1.0);

	// convert to 4d eye coordinates
	glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

	// convert to 4d world coordinates
	glm::vec3 rayDirection = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * rayEye));

	return rayDirection;
}

void HelloGL::Raycast(int mouseX, int mouseY)
{
	glm::vec3 rayDirection = GetRayFromScreenPosition(mouseX, mouseY);
	glm::vec3 rayOrigin = camera->GetPosition();
	Ray ray = Ray(rayOrigin, rayDirection);

	float closestIntersection = std::numeric_limits<float>::max();
	TreeNode* closestObject = nullptr;
	glm::vec3 positionOffset = glm::vec3();

	currentScene->IterateTree(currentScene->GetRoot(), 0, [&](TreeNode* node, int depth, TraversalType type) {
		SceneObject* obj = node->object;
		if (obj == nullptr) { return; }
		glm::vec3 objectPositionOffset = node->object->GetPosition();

		if (type & PRE_ORDER)
		{
			AABBox bbox = obj->GetBoundingBox();
			bbox.bounds[0] += positionOffset;
			bbox.bounds[1] += positionOffset;

			float intersectionDistance;
			if (bbox.intersect(ray, intersectionDistance))
			{
				if (intersectionDistance < closestIntersection)
				{
					closestIntersection = intersectionDistance;
					closestObject = node;
				}
			}
			positionOffset += objectPositionOffset;
		}
		else if (type & IN_ORDER)
		{
			positionOffset -= objectPositionOffset;
		}

	}, TraversalType(PRE_ORDER | IN_ORDER));

	if (closestObject == nullptr)
		camera->SetViewMode(ViewMode::FLY);
	else
	{
		AABBox bbox = closestObject->object->GetBoundingBox();
		glm::vec3 size = (bbox.bounds[1] - bbox.bounds[0]);
		float distance = glm::max(glm::max(size.x, size.y), size.z);
		camera->SetOrbitDistance(distance * 3.0f);
	}

	selectedObject = closestObject;
}

void HelloGL::RenderText(const char* text, const glm::ivec2& screenPosition, const Color& color)
{
	glm::vec3 cameraPosition = camera->GetPosition();
	glm::vec3 rayDirection = GetRayFromScreenPosition(screenPosition.x, screenPosition.y);
	glm::vec3 worldPosition = cameraPosition + rayDirection;

	glDisable(GL_LIGHTING); // Disable lighting for text rendering
	glColor3f(color.r, color.g, color.b);

	glPushMatrix();
	glTranslatef(worldPosition.x, worldPosition.y, worldPosition.z);
	glRasterPos2f(0.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)text);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void HelloGL::SceneMenu(int item)
{
	if (scenes[item] == nullptr)
	{
		std::string pathStr = ("Scenes/scene" + std::to_string((item + 1)) + ".xml");
		scenes[item] = new Scene(pathStr.c_str());
	}
	selectedObject = nullptr;
	currentScene = scenes[item];

	camera->SetViewMode(ViewMode::FLY);
	camera->SetPosition(currentScene->GetCameraPosition());
	camera->SetRotation(currentScene->GetCameraRotation());
}

void HelloGL::KeyboardDown(unsigned char key, int x, int y)
{
	InputManager::OnKeyboardDown(key);

	if (key == 'f' && selectedObject != nullptr)
		camera->SetViewMode(camera->GetViewMode() == ViewMode::FLY ? ViewMode::ORBIT : ViewMode::FLY);
}

void HelloGL::KeyboardUp(unsigned char key, int x, int y)
{
	InputManager::OnKeyboardUp(key);
}

void HelloGL::KeyboardSpecialDown(int key, int x, int y)
{
	InputManager::OnKeyboardSpecialDown(key);
}

void HelloGL::KeyboardSpecialUp(int key, int x, int y)
{
	InputManager::OnKeyboardSpecialUp(key);
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
		glm::vec3 currentMousePosition = glm::vec3(x + glutGet(GLUT_WINDOW_X), y + glutGet(GLUT_WINDOW_Y), 0);
		glm::vec3 mouseOffset = currentMousePosition - lockedMousePosition;

		camera->OffsetRotation(glm::vec3(mouseOffset.y, -mouseOffset.x, 0.0f) * 0.01f);

		SetCursorPos(lockedMousePosition.x, lockedMousePosition.y);
	}
}

void HelloGL::InitObjects()
{
	scenes.resize(3);
	scenes[0] = new Scene("Scenes/scene1.xml");
	currentScene = scenes[0];

	camera = new Camera(currentScene->GetCameraPosition(), currentScene->GetCameraRotation());
	glClearColor(0.25098f, 0.67058f, 0.93725f, 1.0);

	Mesh* skyboxMesh = MeshLoader::Load("Models/skybox.obj");
	Texture2D* skyboxTexture = new Texture2D();
	skyboxTexture->LoadBMP("Textures/blue-sky.bmp");
	skybox = new SceneObject(skyboxMesh, skyboxTexture, camera->GetPosition());
	skybox->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

	//camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	/*Texture2D* texture = new Texture2D();
	texture->LoadRAW("penguins.raw", 512, 512);*/
	//texture->LoadTGA("cat.tga");

	/*Texture2D* texture2 = new Texture2D();
	texture2->LoadBMP("funnycat.bmp");

	Texture2D* texture3 = new Texture2D();
	texture3->LoadBMP("transparent-cat.bmp");*/

	/*Texture2D* texture4 = new Texture2D();
	texture4->LoadPNG("small-test.png");*/

	//Mesh* cubeMesh = MeshLoader::Load("cube.txt");
	//Mesh* pyramidMesh = MeshLoader::LoadTXT("pyramid.txt");
	/*Mesh* teapotMesh = MeshLoader::LoadOBJ("teapot.obj");*/
	//Mesh* cowMesh = MeshLoader::Load("cow.obj");

	objects = std::vector<SceneObject*>();
	/*for (int i = 0; i < 20; i++)
	{
		objects.push_back(new Cube(cubeMesh, texture, ((rand() % 400) / 10.0f) - 20.0f, ((rand() % 200) / 10.0f) - 10.0f, -(rand() % 1000) / 5.0f));
	}*/
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

void HelloGL::InitLighting()
{
	lightPosition = new glm::vec4();
	lightPosition->x = 5.0f;
	lightPosition->y = 1.0f;
	lightPosition->z = 5.0f;
	lightPosition->w = 0.0f;

	lightData = new Lighting();
	lightData->Ambient.x = 0.2f;
	lightData->Ambient.y = 0.2f;
	lightData->Ambient.z = 0.2f;
	lightData->Ambient.w = 1.0f;

	lightData->Diffuse.x = 1.0f;
	lightData->Diffuse.y = 1.0f;
	lightData->Diffuse.z = 1.0f;
	lightData->Diffuse.w = 1.0f;

	lightData->Specular.x = 0.0f;
	lightData->Specular.y = 0.0f;
	lightData->Specular.z = 0.0f;
	lightData->Specular.w = 1.0f;
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
	glutSpecialFunc(GLUTCallbacks::KeyboardSpecialDown);
	glutSpecialUpFunc(GLUTCallbacks::KeyboardSpecialUp);
	glutMouseFunc(GLUTCallbacks::Mouse);
	glutMotionFunc(GLUTCallbacks::Motion);

	glutCreateMenu(GLUTCallbacks::SceneMenu);

	// Add menu items
	glutAddMenuEntry("Scene 1", 0);
	glutAddMenuEntry("Scene 2", 1);
	glutAddMenuEntry("Scene 3", 2);

	// Associate a mouse button with menu
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	// Set the correct perspective
	gluPerspective(45, 1, 0.1f, 200);
	projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 200.0f);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
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