#include "Camera.h"
#include <iostream>

glm::vec3 Camera::GetRotatedVector(glm::vec3 vector)
{
	float yaw = glm::radians(rotation.y);
	float pitch = glm::radians(rotation.x);

	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);

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

Camera::Camera(glm::vec3 _position, glm::vec3 _lookVector, glm::vec3 _upVector)
{
	position = _position;
	lookVector = _lookVector;
	upVector = _upVector;
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 Camera::GetRightVector()
{
	return glm::cross(GetRotatedVector(lookVector), GetRotatedVector(upVector));
}

void Camera::Update(glm::mat4& viewMatrix)
{
	glm::vec3 newLookVector = glm::vec3(lookVector.x, lookVector.y, lookVector.z);
	glm::vec3 newUpVector = GetRotatedVector(upVector);

	glm::vec3 center = position + GetRotatedVector(newLookVector);
	gluLookAt(position.x, position.y, position.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);
	viewMatrix = glm::lookAt(glm::vec3(position.x, position.y, position.z), glm::vec3(center.x, center.y, center.z), glm::vec3(upVector.x, upVector.y, upVector.z));
}