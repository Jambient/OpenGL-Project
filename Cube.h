#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "Structures.h"
#include "SceneObject.h"

class Cube : public SceneObject
{
private:
	GLfloat _rotation;
	Vector3 _position;

public:
	Cube(Mesh* mesh, Texture2D* texture, float x, float y, float z, float rotation = 0.0f);
	~Cube();

	void Draw();
	void Update();
};