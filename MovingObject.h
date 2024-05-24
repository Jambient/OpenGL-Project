#pragma once
#ifndef _MOVINGCUBE_H
#define _MOVINGCUBE_H
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "Commons.h"
#include "SceneObject.h"

/**
 * @class MovingObject
 * @brief A class for an object with velocity.
 */
class MovingObject : public SceneObject
{
public:
	/**
	 * @brief Constructs a MovingObject object.
	 * @param mesh A pointer to the object mesh.
	 * @param texture A pointer to the object texture. Pass nullptr for no texture.
	 * @param initialPosition The initial position of the object.
	 * @param velocity The initial velocity of the object.
	 */
	MovingObject(Mesh* mesh, Texture2D* texture, glm::vec3 initialPosition, glm::vec3 velocity);
	/**
	 * @brief Deconstructs the object.
	 */
	~MovingObject();

	/**
	 * @brief Updates the object by applying the velocity.
	 *        Overrides from the Update function in SceneObject.
	 */
	void Update();

	/**
	 * @brief Sets the velocity of the object.
	 * @param velocity The new velocity.
	 */
	void SetVelocity(glm::vec3 velocity) { m_velocity = velocity; }

	/**
	 * @brief Gets the velocity of the object.
	 * @return The current velocity of the object.
	 */
	glm::vec3 GetVelocity() const { return m_velocity; }

private:
	glm::vec3 m_velocity; /* The velocity of the object. */
};
#endif //_MOVINGCUBE_H