#include "MovingObject.h"

MovingObject::MovingObject(Mesh* mesh, Texture2D* texture, glm::vec3 initialPosition, glm::vec3 velocity): SceneObject(mesh, texture, initialPosition)
{
	m_position = initialPosition;
	m_velocity = velocity;
}

MovingObject::~MovingObject(){}

void MovingObject::Update()
{
	m_position = m_position + m_velocity * 0.06f;
}
