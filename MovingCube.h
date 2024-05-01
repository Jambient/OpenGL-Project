#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "Structures.h"
#include "SceneObject.h"

class MovingCube : public SceneObject
{
private:
	Vector3 _position;
	Vector3 _velocity;

public:
	MovingCube(Mesh* mesh, Texture2D* texture, Vector3 initialPosition, Vector3 velocity);
	~MovingCube();

	void Draw();
	void Update();
};