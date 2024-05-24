#pragma once
#ifndef _CUBE_H
#define _CUBE_H
#include <Windows.h>
#include <vector>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include "GL/freeglut.h"
#include "Commons.h"
#include "SceneObject.h"

/**
 * @class Cube
 * @brief A scene object class that loads a cube.
 */
class Cube : public SceneObject
{
public:
	/**
	 * @brief Construct a Cube object.
	 * @param texture The texture for the cube. Pass nullptr for no texture.
	 * @param position The initial position for the cube.
	 * @param rotation The intial rotation for the cube. All three axes will become the same rotation. Default is 0.0f.
	 */
	Cube(Texture2D* texture, glm::vec3 position, float rotation = 0.0f);
};
#endif //_CUBE_H