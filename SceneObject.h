#pragma once
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include "Structures.h"
#include "Texture2D.h"

class SceneObject
{
protected:
	Mesh* _mesh;
	Texture2D* _texture;
	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;
	Material* _material;
	BoundingBox _bbox;
public:
	SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position);
	virtual ~SceneObject();

	virtual void Update();
	virtual void Draw();
	virtual float SignedDistanceField(const glm::vec3& point);

	glm::vec3 GetPosition() const { return _position; }
	void SetPosition(glm::vec3 position) { _position = position; }
	void OffsetPosition(glm::vec3 offset) { _position += offset; }

	glm::vec3 GetRotation() const { return _rotation; }
	void SetRotation(glm::vec3 rotation) { _rotation = rotation; }
	void OffsetRotation(glm::vec3 offset) { _rotation += offset; }

	glm::vec3 GetScale() const { return _scale; }
	void SetScale(glm::vec3 scale) { _scale = scale; }
};