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
#include "Structures.h"
#include "Cube.h"
#include "Camera.h"

#define vector2 std::pair<double, double>
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
	void InitGL(int argc, char* argv[]);

	void DrawPolygon();
	void DrawRegularPolygon(vector2 center, float rotation, float radius, bool filled = true, float sides = 64.0f);
	void DrawTriangleFromAngles(float angle1, float angle2, float base, vector2 pos, float rotation);
	void Raycast(int mouseX, int mouseY);

private:
	float row1Rotation, row2Rotation, row3Rotation;
	float scale;

	Camera* camera;
	std::vector<SceneObject*> objects;

	bool isRightClickDown;
	Vector3 lastMousePosition;
	Vector3 lockedMousePosition;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	SceneObject* selectedObject;

	glm::vec4* lightPosition;
	Lighting* lightData;
};