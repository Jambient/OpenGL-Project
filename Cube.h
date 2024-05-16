#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include "GL/freeglut.h"
#include "Commons.h"
#include "SceneObject.h"

class Cube : public SceneObject
{
public:
	Cube(Texture2D* texture, glm::vec3 position, float rotation = 0.0f);
};