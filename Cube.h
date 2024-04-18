#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "Structures.h"

class Cube
{
private:
	static std::vector<Vertex>* indexedVertices;
	static std::vector<Color>* indexedColors;
	static std::vector<GLuint>* indices;
	static int numVertices, numColors, numIndices;

	GLfloat _rotation;
	Vector3 _position;

public:
	Cube(float x, float y, float z);
	~Cube();

	static bool LoadTXT(char* path);
	static bool LoadOBJ(char* path);
	void Draw();
	void Update();
};