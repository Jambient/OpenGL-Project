#pragma once
#include <glm/glm.hpp>
#include "Structures.h"
#include "Texture2D.h"

class SceneObject
{
protected:
	Mesh* _mesh;
	Texture2D* _texture;
	glm::vec3 _position;
public:
	SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position);
	virtual ~SceneObject();

	virtual void Update();
	virtual void Draw();
	virtual float SignedDistanceField(const glm::vec3& point);

	glm::vec3 GetPosition() const { return _position; }
};