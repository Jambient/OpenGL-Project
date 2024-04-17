#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include "Structures.h"

class Camera
{
private:
	Vector3 position;
	Vector3 lookVector;
	Vector3 upVector;
	Vector3 rotation;

	Vector3 GetRotatedVector(Vector3 vector);
public:
	Camera(Vector3 _position, Vector3 _lookVector, Vector3 _up);

	Vector3 GetPosition() { return position; }
	Vector3 GetForwardVector() { return GetRotatedVector(lookVector); }
	Vector3 GetUpVector() { return GetRotatedVector(upVector); }
	Vector3 GetRightVector();

	void SetPosition(Vector3 _position) { position = _position; }
	void OffsetPosition(Vector3 _offset) { position += _offset; }

	void SetRotation(Vector3 _rotation) { rotation = _rotation; }
	void OffsetRotation(Vector3 _offset) { rotation += _offset; rotation.x = min(max(rotation.x, -89.0f), 89.0f); }

	void Update();
};
