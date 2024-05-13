#pragma once
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include "Commons.h"
#include <algorithm>

enum class ViewMode
{
	FLY,
	ORBIT
};

class Camera
{
private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;

	ViewMode m_viewMode = ViewMode::FLY;
	glm::vec3 m_orbitTargetPosition = glm::vec3();
	float m_orbitDistance = 10.0f;
public:
	Camera(glm::vec3 position, glm::vec3 rotation);

	glm::vec3 GetPosition() const { return m_position; }
	glm::vec3 GetForwardVector() { return RotateVector(glm::vec3(0.0f, 0.0f, 1.0f), m_rotation); }
	glm::vec3 GetUpVector() { return RotateVector(glm::vec3(0.0f, 1.0f, 0.0f), m_rotation); }
	glm::vec3 GetRightVector();

	ViewMode GetViewMode() const { return m_viewMode; }
	void SetViewMode(ViewMode mode) { m_viewMode = mode; };
	void SetOrbitTargetPosition(glm::vec3 target) { m_orbitTargetPosition = target; }
	void SetOrbitDistance(float distance) { m_orbitDistance = distance; }

	void SetPosition(glm::vec3 position) { m_position = position; }
	void OffsetPosition(glm::vec3 offset) { m_position += offset; }

	void SetRotation(glm::vec3 rotation) { m_rotation = rotation; }
	void OffsetRotation(glm::vec3 offset) { m_rotation += offset; m_rotation.x = std::min(std::max(m_rotation.x, -89.0f), 89.0f); }

	void Update(glm::mat4& viewMatrix);
};
