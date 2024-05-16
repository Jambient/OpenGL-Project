#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"
#include "MeshLoader.h"

Cube::Cube(Texture2D* texture, glm::vec3 position, float rotation) : SceneObject(MeshLoader::Load("Models/cube.txt"), texture, position)
{
	m_rotation = glm::vec3(rotation, rotation, rotation);
}