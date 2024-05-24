#include "HelloGL.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Commons.h"
#include "Camera.h"
#include "InputManager.h"
#include "MeshLoader.h"

std::vector<const char*> HelloGL::sceneNames = {
	"Animation Scene",
	"House Scene",
	"Church Scene",
	"Forest Scene"
};

std::vector<const char*> HelloGL::scenePaths = {
	"Scenes/animationScene.xml",
	"Scenes/houseScene.xml",
	"Scenes/churchScene.xml",
	"Scenes/forestScene.xml",
};

glm::vec3 HelloGL::axisAlignedVectors[6] = {
	{0, 1, 0},
	{0, -1, 0},
	{1, 0, 0},
	{-1, 0, 0},
	{0, 0, 1},
	{0, 0, -1}
};

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

	for (unsigned int i = 0; i < scenes.size(); i++)
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
			RenderText(node->name.c_str(), glm::ivec2(20 + depth * 30, currentY), {0.0f, 0.0f}, node == selectedObject ? highlightedColor : textColor);
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
	RenderText(("FPS: " + std::to_string(fps)).c_str(), { VIEWPORT_WIDTH - 20, 30 }, {1.0f, 0.0f});

	// display the current camera mode
	std::string cameraModeText = "Camera Mode: " + camera->GetViewModeAsString();
	RenderText(cameraModeText.c_str(), { VIEWPORT_WIDTH / 2, 30 }, {0.5f, 0.0f});

	// display the current camera position
	glm::vec3 cameraPos = camera->GetPosition();
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1);
	stream << "( " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << " )";
	RenderText(stream.str().c_str(), { VIEWPORT_WIDTH / 2, 60 }, {0.5f, 0.0f}, { 0.1f, 0.1f, 0.1f });

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
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentTime - previousElapsedTime) / 1000.0f;
	fps = (int)(1.0f / deltaTime);

	// update lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(lightData.ambient));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(lightData.diffuse));
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(lightData.specular));

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

	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(lightPosition));
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

void HelloGL::Raycast(int x, int y)
{
	// build ray from direction and origin
	glm::vec3 rayDirection = GetRayFromScreenPosition(x, y);
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
			if (bbox.Intersect(ray, intersectionDistance))
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
	InitMenu();
}

void HelloGL::RenderText(const char* text, const glm::ivec2& screenPosition, const glm::vec2& anchorPoint, const Color& color)
{
	int textWidth = 0;
	int textHeight = 18;

	// calculate width of text
	for (const char* p = text; *p; ++p) {
		textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);
	}

	// get world position from screen position
	glm::vec3 cameraPosition = camera->GetPosition();
	glm::vec3 rayDirection = GetRayFromScreenPosition(screenPosition.x - (int)(textWidth * anchorPoint.x), screenPosition.y - (int)(textHeight * anchorPoint.y));
	glm::vec3 worldPosition = cameraPosition + rayDirection;

	glDisable(GL_LIGHTING);
	glColor3f(color.r, color.g, color.b);
	glPushMatrix();

	// move string to correct render position
	glTranslatef(worldPosition.x, worldPosition.y, worldPosition.z);
	glRasterPos2f(0.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)text);

	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

void HelloGL::BaseMenu(int item){}

void HelloGL::SceneMenu(int item)
{
	// chosen scene has not been loaded before then load it
	if (scenes[item] == nullptr)
	{
		scenes[item] = new Scene(scenePaths[item]);
	}
	selectedObject = nullptr;
	currentScene = scenes[item];

	// reset and update camera
	camera->SetViewMode(ViewMode::FLY);
	camera->SetPosition(currentScene->GetCameraPosition());
	camera->SetRotation(currentScene->GetCameraRotation());
}

void HelloGL::TextureMenu(int item)
{
	if (originalObjectTexture.find(selectedObject) == originalObjectTexture.end())
	{
		originalObjectTexture[selectedObject] = selectedObject->object->GetTexture();
	}
	Texture2D* newTexture = new Texture2D();

	switch (item)
	{
	case 0:
		delete newTexture;
		newTexture = originalObjectTexture[selectedObject];
		break;
	case 1:
		newTexture->LoadRAW("Textures/penguins.raw");
		break;
	case 2:
		newTexture->LoadRAW("Textures/stars.raw");
		break;
	case 3:
		newTexture->LoadBMP("Textures/sand.bmp");
		break;
	case 4:
		newTexture->LoadPNG("Textures/cat.png");
		break;
	case 5:
		newTexture->LoadPNG("Textures/grass.png");
		break;
	}

	selectedObject->object->SetTexture(newTexture);
}

void HelloGL::KeyboardDown(unsigned char key, int x, int y)
{
	InputManager::OnKeyboardDown(key);

	// key for switching camera modes
	if (key == 'v')
	{
		switch (camera->GetViewMode())
		{
		case ViewMode::FLY:
			if (selectedObject != nullptr) { camera->SetViewMode(ViewMode::LOCK); }
			break;
		case ViewMode::LOCK:
			if (selectedObject != nullptr) { camera->SetViewMode(ViewMode::ORBIT); }
			break;
		case ViewMode::ORBIT:
			camera->SetViewMode(ViewMode::FLY);
			break;
		default:
			break;
		}
	}
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

	// if right button is down then hide the cursor and save its position
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{
		ShowCursor(FALSE);
		glutSetCursor(GLUT_CURSOR_NONE);
		lockedMousePosition.x = x + glutGet(GLUT_WINDOW_X);
		lockedMousePosition.y = y + glutGet(GLUT_WINDOW_Y);
	}
	// if the right button is up then show the cursor again
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) 
	{
		ShowCursor(TRUE);
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	// if the left button is clicked, then raycast
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Raycast(x, y);
	}
}

void HelloGL::Motion(int x, int y)
{
	// when the right mouse button is down, rotate the camera based on mouse movement
	if (InputManager::IsMouseButtonDown(GLUT_RIGHT_BUTTON))
	{
		glm::ivec2 currentMousePosition = glm::vec2(x + glutGet(GLUT_WINDOW_X), y + glutGet(GLUT_WINDOW_Y));
		glm::ivec2 mouseOffset = currentMousePosition - lockedMousePosition;

		camera->OffsetRotation(glm::vec3(mouseOffset.y, -mouseOffset.x, 0.0f) * 0.01f);

		// relock the cursor
		SetCursorPos(lockedMousePosition.x, lockedMousePosition.y);
	}
}

void HelloGL::InitObjects()
{
	// initialise the camera
	camera = new Camera();

	// set up the first scene
	scenes.resize(sceneNames.size());
	SceneMenu(0);

	// load the skybox
	Mesh* skyboxMesh = MeshLoader::Load("Models/skybox.obj");
	Texture2D* skyboxTexture = new Texture2D();
	skyboxTexture->LoadBMP("Textures/blue-sky.bmp");
	skybox = new SceneObject(skyboxMesh, skyboxTexture, camera->GetPosition());
}

void HelloGL::InitLighting()
{
	lightPosition = glm::vec4(30.0f, 50.0f, 30.0f, 0.0f);

	lightData.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	lightData.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightData.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void HelloGL::InitGL(int argc, char* argv[])
{
	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL Program");

	glutDisplayFunc(GLUTCallbacks::Display);
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glutKeyboardFunc(GLUTCallbacks::KeyboardDown);
	glutKeyboardUpFunc(GLUTCallbacks::KeyboardUp);
	glutSpecialFunc(GLUTCallbacks::KeyboardSpecialDown);
	glutSpecialUpFunc(GLUTCallbacks::KeyboardSpecialUp);
	glutMouseFunc(GLUTCallbacks::Mouse);
	glutMotionFunc(GLUTCallbacks::Motion);
	glutCloseFunc(CloseCallback);

	InitMenu();

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

void HelloGL::InitMenu()
{
	glutDestroyMenu(glutGetMenu());

	// create base menu
	int baseMenu = glutCreateMenu(GLUTCallbacks::BaseMenu);

	// create scene menu
	int sceneMenu = glutCreateMenu(GLUTCallbacks::SceneMenu);
	for (unsigned int i = 0; i < sceneNames.size(); i++)
	{
		glutAddMenuEntry(sceneNames[i], i);
	}
	glutSetMenu(baseMenu);
	glutAddSubMenu("Change Scene", sceneMenu);

	// create texture menu
	if (selectedObject != nullptr)
	{
		int textureMenu = glutCreateMenu(GLUTCallbacks::TextureMenu);
		glutAddMenuEntry("Default", 0);
		glutAddMenuEntry("Penguins", 1);
		glutAddMenuEntry("Stars", 2);
		glutAddMenuEntry("Sand", 3);
		glutAddMenuEntry("Cat", 4);
		glutAddMenuEntry("Grass", 5);

		glutSetMenu(baseMenu);
		glutAddSubMenu("Set Object Texture", textureMenu);
	}

	// Associate a mouse button with menu
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

int main(int argc, char* argv[]) 
{
	srand((unsigned int)time(NULL));

	g_game = new HelloGL(argc, argv);
	glutMainLoop();

	return 0;
}