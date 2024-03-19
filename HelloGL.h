#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>

#define vector2 std::pair<double, double>
#define REFRESHRATE 16

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Update();
	void Display();
	void DrawPolygon();
	void DrawRegularPolygon(vector2 center, float rotation, float radius, bool filled = true, float sides = 64.0f);
	void DrawTriangleFromAngles(float angle1, float angle2, float base, vector2 pos, float rotation);
	void Keyboard(unsigned char key, int x, int y);

private:
	float row1Rotation, row2Rotation, row3Rotation;
	float scale;
};