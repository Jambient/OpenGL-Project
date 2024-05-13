#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "Commons.h"
#include "SceneObject.h"

class MovingCube : public SceneObject
{
private:
	glm::vec3 _position;
	glm::vec3 _velocity;

public:
	MovingCube(Mesh* mesh, Texture2D* texture, glm::vec3 initialPosition, glm::vec3 velocity);
	~MovingCube();

	void Draw();
	void Update();
};