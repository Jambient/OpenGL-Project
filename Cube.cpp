#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"

Cube::Cube(Mesh* mesh, Texture2D* texture, glm::vec3 position, float rotation) : SceneObject(mesh, texture, position)
{
	_rotation = glm::vec3(rotation, rotation, rotation);
}

void Cube::Update()
{
	_rotation.y += 1.0f;
	//_position.z = std::fmod(_position.z - 0.4f, -200.0f);
}