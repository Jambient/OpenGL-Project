#include "SceneObject.h"

SceneObject::SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position)
{
	_mesh = mesh;
	_texture = texture;
	_position = position;
}

SceneObject::~SceneObject(){}
void SceneObject::Update(){}
void SceneObject::Draw(){}

float SceneObject::SignedDistanceField(const glm::vec3& point) { return 0.0f; };
