#pragma once
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include "Structures.h"
#include <algorithm>

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 lookVector;
	glm::vec3 upVector;
	glm::vec3 rotation;

	glm::vec3 GetRotatedVector(glm::vec3 vector);
public:
	Camera(glm::vec3 _position, glm::vec3 _lookVector, glm::vec3 _up);

	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetForwardVector() { return GetRotatedVector(lookVector); }
	glm::vec3 GetUpVector() { return GetRotatedVector(upVector); }
	glm::vec3 GetRightVector();

	void SetPosition(glm::vec3 _position) { position = _position; }
	void OffsetPosition(glm::vec3 _offset) { position += _offset; }

	void SetRotation(glm::vec3 _rotation) { rotation = _rotation; }
	void OffsetRotation(glm::vec3 _offset) { rotation += _offset; rotation.x = std::min(std::max(rotation.x, -89.0f), 89.0f); }

	void Update(glm::mat4& viewMatrix);
};
