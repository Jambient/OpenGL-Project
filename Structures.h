#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <vector>
#include "Vector3.h"

struct Color
{
	GLfloat r, g, b;
};

struct Vertex
{
	GLfloat x, y, z;
};

struct Mesh
{
	std::vector<Vertex>* Vertices;
	std::vector<Color>* Colors;
	std::vector<GLushort>* Indices;
	int VertexCount, ColorCount, IndexCount;
};