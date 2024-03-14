#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>

#define mk_pair std::pair<double, double>

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Display();
	void DrawPolygon();
	void DrawRegularPolygon(mk_pair center, float radius, bool filled = true, float sides = 64.0f);
	void DrawTriangleFromAngles(float angle1, float angle2, float base, mk_pair pos);
};