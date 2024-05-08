#pragma once
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include "GL/freeglut.h"
#include "Structures.h"
#include "SceneObject.h"

class Cube : public SceneObject
{
public:
	Cube(Mesh* mesh, Texture2D* texture, glm::vec3 position, float rotation = 0.0f);

	void Update();

	float SignedDistanceField(const glm::vec3& point) {
		/*float _size = 2.0f;
		glm::vec3 p = glm::abs(point - _position) - glm::vec3(_size / 2.0f);
		float maxDistance = glm::max(glm::max(p.x, p.y), p.z);
		return glm::min(maxDistance, 0.0f) + glm::length(glm::max(p, glm::vec3(0.0f)));*/
		return 1.0f;
	}
};