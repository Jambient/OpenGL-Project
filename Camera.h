#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include "Commons.h"
#include <algorithm>
#include <map>

/**
 * @enum ViewMode
 * @brief An enum for setting the view mode of the camera.
 */
enum class ViewMode
{
	FLY,
	LOCK,
	ORBIT
};

/**
 * @class Camera
 * @brief A class that wraps around the OpenGL camera and provides helpful functions.
 */
class Camera
{
public:
	/**
	 * @brief Constructs a Camera object
	 * @param position The intial position of the camera. Default is [0, 0, 0]
	 * @param rotation The intial rotation of the camera. Default is [0, 0, 0]
	 */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));

	/**
	 * @brief Gets the position of the camera.
	 * @return The current position of the camera.
	 */
	glm::vec3 GetPosition() const { return m_position; }

	/**
	 * @brief Gets the forward vector of the camera. Also referred to as the look vector.
	 * @return The current forward vector of the camera.
	 */
	glm::vec3 GetForwardVector() { return RotateVector(glm::vec3(0.0f, 0.0f, 1.0f), m_rotation); }

	/**
	 * @brief Gets the up vector of the camera.
	 * @return The current up vector of the camera.
	 */
	glm::vec3 GetUpVector() { return RotateVector(glm::vec3(0.0f, 1.0f, 0.0f), m_rotation); }

	/**
	 * @brief Gets the right vector of the camera.
	 * @return The current right vector of the camera.
	 */
	glm::vec3 GetRightVector();

	/**
	 * @brief Gets the view mode of the camera.
	 * @return The currently active view mode of the camera.
	 */
	ViewMode GetViewMode() const { return m_viewMode; }

	/**
	 * @brief Gets the view mode of the camera as a string representing the ViewMode enums identifier.
	 * @return The currently active view mode as a string.
	 */
	std::string GetViewModeAsString() const { return s_viewModeToString[m_viewMode]; }

	/**
	 * @brief Sets the view mode of the camera.
	 * @param mode The new view mode.
	 */
	void SetViewMode(ViewMode mode) { m_viewMode = mode; };

	/**
	 * @brief Sets the target position for orbit mode. This is the origin position that the camera will orbit around.
	 * @param target The new target position.
	 */
	void SetOrbitTargetPosition(glm::vec3 target) { m_orbitTargetPosition = target; }

	/**
	 * @brief Sets the orbit distance for orbit mode. This is the distance from the origin the camera will be.
	 * @param distance The new distance.
	 */
	void SetOrbitDistance(float distance) { m_orbitDistance = distance; }

	/**
	 * @brief Sets the position of the camera.
	 * @param position The new position.
	 */
	void SetPosition(glm::vec3 position) { m_position = position; }

	/**
	 * @brief Offsets the position of the camera.
	 * @param offset The position offset to apply.
	 */
	void OffsetPosition(glm::vec3 offset) { m_position += offset; }

	/**
	 * @brief Sets the rotation of the camera.
	 * @param rotation The new rotation.
	 */
	void SetRotation(glm::vec3 rotation) { m_rotation = rotation; }

	/**
	 * @brief Offsets the rotation of the camera.
	 * @param offset The rotation offset to apply.
	 */
	void OffsetRotation(glm::vec3 offset) { m_rotation += offset; m_rotation.x = std::min(std::max(m_rotation.x, -89.0f), 89.0f); }

	/**
	 * @brief Updates the camera using gluLookAt based on the current camera mode and updates a passed view matrix.
	 * @param viewMatrix The view matrix to update.
	 */
	void Update(glm::mat4& viewMatrix);

private:
	static std::map<ViewMode, std::string> s_viewModeToString; /* Maps view mode enums to their enum identifiers. */

	glm::vec3 m_position; /* The current camera position. */
	glm::vec3 m_rotation; /* The current camera rotation. */
	ViewMode m_viewMode = ViewMode::FLY; /* The current camera view mode. Default is FLY. */
	glm::vec3 m_orbitTargetPosition = glm::vec3(0.0f, 0.0f, 0.0f); /* The current orbit target for orbit mode. Default is world origin */
	float m_orbitDistance = 10.0f; /* The distance the camera should be from the orbit target. Default is 10.0f. */

};
#endif //_CAMERA_H