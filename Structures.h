#pragma once
#include <gl/GL.h>
#include "Vector3.h"

//class Vector3
//{
//public:
//	float x, y, z;
//
//	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
//
//	Vector3 operator+(const Vector3& v) const {
//		return Vector3(x + v.x, y + v.y, z + v.z);
//	}
//};

struct Color
{
	GLfloat r, g, b;
};

struct Vertex
{
	GLfloat x, y, z;
};