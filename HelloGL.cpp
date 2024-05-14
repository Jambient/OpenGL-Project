#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include "Commons.h"
#include "Cube.h"
#include "MovingCube.h"
#include "Camera.h"
#include "InputManager.h"
#include "MeshLoader.h"

HelloGL* g_game;
void CloseCallback() {
	delete g_game;
	exit(0);
}

HelloGL::HelloGL(int argc, char* argv[]) 
{
	InitGL(argc, argv);
	InitObjects();
	InitLighting();
}

HelloGL::~HelloGL(void)
{
	delete camera;
	delete skybox;

	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the skybox
	glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
	skybox->Draw();
	glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST); glEnable(GL_CULL_FACE);

	if (testObject != nullptr)
		testObject->Draw();

	// render every object in the scene
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

	// display FPS
	RenderText(("FPS: " + std::to_string(fps)).c_str(), { VIEWPORT_WIDTH - 80, 30 });

	// display the current camera mode
	std::string cameraModeText = "Camera Mode: ";
	cameraModeText += (camera->GetViewMode() == ViewMode::FLY) ? "FLY" : "ORBIT";
	RenderText(cameraModeText.c_str(), {VIEWPORT_WIDTH / 2 - 80, 30});

	// update orbit target position if an object is selected.
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
	glLoadIdentity();

	// calculate delta time and update fps
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentTime - previousElapsedTime) / 1000.0f;
	fps = 1.0f / deltaTime;

	// update lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(lightData.Ambient));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(lightData.Diffuse));
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(lightData.Specular));
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(lightPosition));

	// update all animations
	currentScene->IterateTree(currentScene->GetRoot(), 0, [&](TreeNode* node, int depth, TraversalType type) {
		if (node->object != nullptr)
		{
			Animation* anim = node->object->GetAnimation();
			if (anim != nullptr)
				anim->Update(deltaTime);
		}
	}, PRE_ORDER);

	// get camera vectors
	glm::vec3 cameraForwardVector = camera->GetForwardVector();
	glm::vec3 cameraRightVector = camera->GetRightVector();
	glm::vec3 cameraUpVector = camera->GetUpVector();

	// update camera position based on input
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

	// update object position based on input
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
	// build ray from direction and origin
	glm::vec3 rayDirection = GetRayFromScreenPosition(mouseX, mouseY);
	glm::vec3 rayOrigin = camera->GetPosition();
	Ray ray = Ray(rayOrigin, rayDirection);

	// initialise variables
	float closestIntersection = std::numeric_limits<float>::max();
	TreeNode* closestObject = nullptr;
	glm::vec3 positionOffset = glm::vec3();

	currentScene->IterateTree(currentScene->GetRoot(), 0, [&](TreeNode* node, int depth, TraversalType type) {
		SceneObject* obj = node->object;
		if (obj == nullptr) { return; }
		glm::vec3 objectPositionOffset = node->object->GetPosition();

		if (type & PRE_ORDER)
		{
			// get bounding box and offset it according to hierarchy
			AABBox bbox = obj->GetBoundingBox();
			bbox.bounds[0] += positionOffset;
			bbox.bounds[1] += positionOffset;

			// check if ray intersects with bounding box
			float intersectionDistance;
			if (bbox.intersect(ray, intersectionDistance))
			{
				if (intersectionDistance < closestIntersection)
				{
					closestIntersection = intersectionDistance;
					closestObject = node;
				}
			}

			// add current objects position to offset
			positionOffset += objectPositionOffset;
		}
		else if (type & IN_ORDER)
		{
			// remove objects position from offset so siblings are not affected.
			positionOffset -= objectPositionOffset;
		}

	}, TraversalType(PRE_ORDER | IN_ORDER));


	if (closestObject == nullptr) // if no object is selected, switch off orbit mode
		camera->SetViewMode(ViewMode::FLY);
	else
	{
		// if an object is selected then set the orbit distance to fit the whole object.
		AABBox bbox = closestObject->object->GetBoundingBox();
		glm::vec3 size = bbox.bounds[1] - bbox.bounds[0];
		float distance = glm::max(glm::max(size.x, size.y), size.z);

		camera->SetOrbitDistance(distance * 3.0f);
	}

	selectedObject = closestObject;
}

void HelloGL::RenderText(const char* text, const glm::ivec2& screenPosition, const Color& color)
{
	// get world position from screen position
	glm::vec3 cameraPosition = camera->GetPosition();
	glm::vec3 rayDirection = GetRayFromScreenPosition(screenPosition.x, screenPosition.y);
	glm::vec3 worldPosition = cameraPosition + rayDirection;

	glDisable(GL_LIGHTING);
	glColor3f(color.r, color.g, color.b);
	glPushMatrix();

	// move string to correct render position
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
		scenes[item] = new Scene(scenePaths[item]);
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
	InputManager::OnMouseEvent(button, state);

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{
		ShowCursor(FALSE);
		glutSetCursor(GLUT_CURSOR_NONE);
		lockedMousePosition.x = x + glutGet(GLUT_WINDOW_X);
		lockedMousePosition.y = y + glutGet(GLUT_WINDOW_Y);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) 
	{
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
	if (InputManager::IsMouseButtonDown(GLUT_RIGHT_BUTTON))
	{
		glm::vec3 currentMousePosition = glm::vec3(x + glutGet(GLUT_WINDOW_X), y + glutGet(GLUT_WINDOW_Y), 0);
		glm::vec3 mouseOffset = currentMousePosition - lockedMousePosition;

		camera->OffsetRotation(glm::vec3(mouseOffset.y, -mouseOffset.x, 0.0f) * 0.01f);

		SetCursorPos(lockedMousePosition.x, lockedMousePosition.y);
	}
}

void HelloGL::InitObjects()
{
	camera = new Camera();

	scenes.resize(sceneNames.size());
	SceneMenu(0);

	Mesh* skyboxMesh = MeshLoader::Load("Models/skybox.obj");
	Texture2D* skyboxTexture = new Texture2D();
	skyboxTexture->LoadBMP("Textures/blue-sky.bmp");
	skybox = new SceneObject(skyboxMesh, skyboxTexture, camera->GetPosition());

	/*Mesh* cubeMesh = MeshLoader::Load("Models/cube.txt");
	Texture2D* pngTexture = new Texture2D();
	pngTexture->LoadPNG("Textures/new-cat.png");
	testObject = new SceneObject(cubeMesh, pngTexture, glm::vec3(0, 10, 0));*/
}

void HelloGL::InitLighting()
{
	lightPosition = glm::vec4(5.0f, 1.0f, 5.0f, 0.0f);

	lightData.Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	lightData.Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightData.Specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
	glutCloseFunc(CloseCallback);

	// create scene menu
	glutCreateMenu(GLUTCallbacks::SceneMenu);
	for (int i = 0; i < sceneNames.size(); i++)
	{
		glutAddMenuEntry(sceneNames[i], i);
	}

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

	g_game = new HelloGL(argc, argv);
	glutMainLoop();

	return 0;
}