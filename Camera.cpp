#include "Camera.h"
#include <iostream>

glm::vec3 Camera::GetRotatedVector(glm::vec3 vector)
{
	float pitch = glm::radians(m_rotation.x);
	float yaw = glm::radians(m_rotation.y);

	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	glm::mat3 rotationXMat = glm::mat3(
		1.0f, 0.0f, 0.0f,
		0.0f, cosPitch, -sinPitch,
		0.0f, sinPitch, cosPitch
	);

	glm::mat3 rotationYMat = glm::mat3(
		cosYaw, 0.0f, sinYaw,
		0.0f, 1.0f, 0.0f,
		-sinYaw, 0.0f, cosYaw
	);

	return rotationYMat * (rotationXMat * vector);
}

Camera::Camera(glm::vec3 position, glm::vec3 rotation)
{
	m_position = position;
	m_rotation = rotation;
}

glm::vec3 Camera::GetRightVector()
{
	return glm::cross(GetRotatedVector(glm::vec3(0.0f, 0.0f, 1.0f)), GetRotatedVector(glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::Update(glm::mat4& viewMatrix)
{
	glm::vec3 lookVector = GetRotatedVector(glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 upVector = GetRotatedVector(glm::vec3(0.0f, 1.0f, 0.0f));

	switch (m_viewMode)
	{
	case ViewMode::FLY:
		glm::vec3 center = m_position + lookVector;
		gluLookAt(m_position.x, m_position.y, m_position.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);
		viewMatrix = glm::lookAt(m_position, center, upVector);
		break;
	case ViewMode::ORBIT:
		m_position = m_orbitTargetPosition - lookVector * m_orbitDistance;
		gluLookAt(m_position.x, m_position.y, m_position.z, m_orbitTargetPosition.x, m_orbitTargetPosition.y, m_orbitTargetPosition.z, upVector.x, upVector.y, upVector.z);
		viewMatrix = glm::lookAt(m_position, m_orbitTargetPosition, upVector);
		break;
	}
}