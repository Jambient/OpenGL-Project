#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>
#include <vector>
#include <string>
#include "Commons.h"
#include "Cube.h"
#include "Camera.h"
#include "Scene.h"

#define REFRESHRATE 16

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Update();
	void Display();
	void KeyboardDown(unsigned char key, int x, int y);
	void KeyboardUp(unsigned char key, int x, int y);
	void KeyboardSpecialDown(int key, int x, int y);
	void KeyboardSpecialUp(int key, int x, int y);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);

	void InitObjects();
	void InitLighting();
	void InitGL(int argc, char* argv[]);

	glm::vec3 GetRayFromScreenPosition(int x, int y);
	void Raycast(int mouseX, int mouseY);
	void RenderText(const char* text, const glm::ivec2& screenPosition, const Color& color = {1.0f, 1.0f, 1.0f});

	glm::vec3 GetClosestAxisAlignedVector(glm::vec3 vec);

	void BaseMenu(int item);
	void SceneMenu(int item);

private:
	Camera* camera;
	SceneObject* skybox;
	TreeNode* selectedObject;
	SceneObject* testObject;

	glm::vec3 lastMousePosition;
	glm::vec3 lockedMousePosition;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	glm::vec4 lightPosition;
	Lighting lightData;

	Scene* currentScene;
	std::vector<Scene*> scenes;
	std::vector<const char*> sceneNames = {
		"Test Scene",
		"House Scene",
		"Church Scene"
	};
	std::vector<const char*> scenePaths = {
		"Scenes/scene1.xml",
		"Scenes/scene2.xml",
		"Scenes/scene3.xml",
	};

	glm::vec3 axisAlignedVectors[6] = {
		{0, 1, 0},
		{0, -1, 0},
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, 1},
		{0, 0, -1}
	};

	int previousElapsedTime = 0;
	int fps = 0;
};