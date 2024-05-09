#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include "ft2build.h"
//#include FT_FREETYPE_H
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>
#include <vector>
#include "Structures.h"
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
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);

	void InitObjects();
	void InitLighting();
	//void InitFont();
	void InitGL(int argc, char* argv[]);

	glm::vec3 GetRayFromScreenPosition(int x, int y);
	void Raycast(int mouseX, int mouseY);
	void RenderText(const char* text, const glm::ivec2& screenPosition, const Color& color = {1.0f, 1.0f, 1.0f});

private:
	float row1Rotation, row2Rotation, row3Rotation;
	float scale;

	Camera* camera;
	std::vector<SceneObject*> objects;

	bool isRightClickDown;
	glm::vec3 lastMousePosition;
	glm::vec3 lockedMousePosition;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	SceneObject* selectedObject;

	glm::vec4* lightPosition;
	Lighting* lightData;

	Scene* currentScene;

	//FT_Library library;
	//FT_Face face;
};