#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>

#define vector2 std::pair<double, double>

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Display();
	void DrawPolygon();
	void DrawRegularPolygon(vector2 center, float radius, bool filled = true, float sides = 64.0f);
	void DrawTriangleFromAngles(float angle1, float angle2, float base, vector2 pos);
};