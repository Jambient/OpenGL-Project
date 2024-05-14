#include "Camera.h"
#include "Commons.h"
#include <iostream>

std::map<ViewMode, std::string> Camera::m_viewModeToString = {
	{ViewMode::FLY, "FLY"},
	{ViewMode::LOCK, "LOCK"},
	{ViewMode::ORBIT, "ORBIT"}
};

Camera::Camera(glm::vec3 position, glm::vec3 rotation)
{
	m_position = position;
	m_rotation = rotation;
}

glm::vec3 Camera::GetRightVector()
{
	return glm::cross(RotateVector(glm::vec3(0.0f, 0.0f, 1.0f), m_rotation), RotateVector(glm::vec3(0.0f, 1.0f, 0.0f), m_rotation));
}

void Camera::Update(glm::mat4& viewMatrix)
{
	// calculate vectors based on rotation
	glm::vec3 lookVector = RotateVector(glm::vec3(0.0f, 0.0f, 1.0f), m_rotation);
	glm::vec3 upVector = RotateVector(glm::vec3(0.0f, 1.0f, 0.0f), m_rotation);

	switch (m_viewMode)
	{
	case ViewMode::FLY:
		glm::vec3 center = m_position + lookVector;
		gluLookAt(m_position.x, m_position.y, m_position.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);
		viewMatrix = glm::lookAt(m_position, center, upVector);
		break;
	case ViewMode::LOCK:
		glm::vec3 newLook = m_position + glm::normalize((m_orbitTargetPosition - m_position));
		gluLookAt(m_position.x, m_position.y, m_position.z, newLook.x, newLook.y, newLook.z, upVector.x, upVector.y, upVector.z);
		viewMatrix = glm::lookAt(m_position, newLook, upVector);
		break;
	case ViewMode::ORBIT:
		m_position = m_orbitTargetPosition - lookVector * m_orbitDistance;
		gluLookAt(m_position.x, m_position.y, m_position.z, m_orbitTargetPosition.x, m_orbitTargetPosition.y, m_orbitTargetPosition.z, upVector.x, upVector.y, upVector.z);
		viewMatrix = glm::lookAt(m_position, m_orbitTargetPosition, upVector);
		break;
	}
}