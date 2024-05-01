#include "Camera.h"
#include "Matrix3.h"
#include <iostream>
#define M_PI 3.14159265358979323846f

Vector3 Camera::GetRotatedVector(Vector3 vector)
{
	float yaw = rotation.y * M_PI / 180.0f;
	float pitch = rotation.x * M_PI / 180.0f;

	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);

	Matrix3 rotationXMat = Matrix3({
		{1.0f, 0.0f, 0.0f},
		{0.0f, cosPitch, -sinPitch},
		{0.0f, sinPitch, cosPitch}
	});

	Matrix3 rotationYMat = Matrix3({
		{cosYaw, 0.0f, sinYaw},
		{0.0f, 1.0f, 0.0f},
		{-sinYaw, 0.0f, cosYaw}
	});

	return rotationYMat * (rotationXMat * vector);
}

Camera::Camera(Vector3 _position, Vector3 _lookVector, Vector3 _upVector)
{
	position = _position;
	lookVector = _lookVector;
	upVector = _upVector;
}

Vector3 Camera::GetRightVector()
{
	return GetRotatedVector(lookVector).cross(GetRotatedVector(upVector));
}

void Camera::Update(glm::mat4& viewMatrix)
{
	Vector3 newLookVector = Vector3(lookVector);
	Vector3 newUpVector = GetRotatedVector(upVector);

	Vector3 center = position + GetRotatedVector(newLookVector);
	gluLookAt(position.x, position.y, position.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);
	viewMatrix = glm::lookAt(glm::vec3(position.x, position.y, position.z), glm::vec3(center.x, center.y, center.z), glm::vec3(upVector.x, upVector.y, upVector.z));
}