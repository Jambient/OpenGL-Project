#pragma once
#ifndef _SCENEOBJECT_H
#define _SCENEOBJECT_H
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include "Commons.h"
#include "Texture2D.h"
#include "Animation.h"

class Animation;

/**
 * @class SceneObject
 * @brief A base class for an object in the scene.
 */
class SceneObject
{
public:
	/**
	 * @brief Constructs a SceneObject object.
	 * @param mesh The mesh for the object.
	 * @param texture The texture for the object. Pass nullptr for no texture.
	 * @param position The initial position for the object.
	 */
	SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position);
	/**
	 * @brief Deconstructs the object.
	 */
	virtual ~SceneObject();

	/**
	 * @brief Updates the object. Default implementation does nothing.
	 */
	virtual void Update();

	/**
	 * @brief Draws the object
	 * @param drawBoundingBox Whether or not the bounding box should be drawn.
	 * @param positionOffset An offset to apply when drawing the object.
	 */
	virtual void Draw(bool drawBoundingBox = false, glm::vec3 positionOffset = glm::vec3());

	/**
	 * @brief Gets the position of the object.
	 * @return The current position of the object.
	 */
	glm::vec3 GetPosition() const { return m_position; }
	/**
	 * @brief Sets the position of the object. This triggers a bounding box update.
	 * @param position The new position.
	 */
	void SetPosition(glm::vec3 position) { m_position = position; UpdateBoundingBox(); }
	/**
	 * @brief Offset the position of the object. This triggers a bounding box update.
	 * @param offset The offset to apply to the position.
	 */
	void OffsetPosition(glm::vec3 offset) { m_position += offset; UpdateBoundingBox(); }

	/**
	 * @brief Gets the rotation of the object.
	 * @return The current rotation of the object.
	 */
	glm::vec3 GetRotation() const { return m_rotation; }
	/**
	 * @brief Sets the rotation of the object. This triggers a bounding box update.
	 * @param rotation The new rotation.
	 */
	void SetRotation(glm::vec3 rotation) { m_rotation = rotation; UpdateBoundingBox(); }
	/**
	 * @brief Offset the rotation of the object. This triggers a bounding box update.
	 * @param offset The offset to apply to the rotation.
	 */
	void OffsetRotation(glm::vec3 offset) { m_rotation += offset; UpdateBoundingBox(); }

	/**
	 * @brief Gets the scale of the object.
	 * @return The current scale of the object.
	 */
	glm::vec3 GetScale() const { return m_scale; }
	/**
	 * @brief Sets the scale of the object. This triggers a bounding box update.
	 * @param scale The new scale.
	 */
	void SetScale(glm::vec3 scale) { m_scale = scale; UpdateBoundingBox(); }

	/**
	 * @brief Gets the animation applied to the object.
	 * @return The current animation of the object.
	 */
	Animation* GetAnimation() const { return m_animation; }
	/**
	 * @brief Binds a new animation to the object.
	 * @param animation The new animation.
	 */
	void SetAnimation(Animation* animation) { m_animation = animation; }

	/**
	 * @brief Gets the texture applied to the object.
	 * @return The current texture of the object.
	 */
	Texture2D* GetTexture() const { return m_texture; }
	/**
	 * @brief Sets the texture applied to the object.
	 * @param texture The new texture.
	 */
	void SetTexture(Texture2D* texture) { m_texture = texture; }

	/**
	 * @brief Gets the axis aligned bounding box for the object.
	 * @return An axis aligned bounding box.
	 */
	AABBox GetBoundingBox() const { return m_bbox; }

protected:
	static Material* s_defaultMaterial; /* The default material applied to the object when no other material is specified. */

	Mesh* m_mesh; /* The objects mesh. */
	Texture2D* m_texture; /* The objects texture. */
	glm::vec3 m_position; /* The current position of the object. */
	glm::vec3 m_rotation; /* The current rotation of the object. */
	glm::vec3 m_scale; /* The current scale of the object. */
	AABBox m_bbox; /* The axis aligned bounding box of the object. */
	Animation* m_animation; /* The animation binded to the object. */

private:
	/**
	 * @brief Recalculates the bounding box for the object.
	 */
	void UpdateBoundingBox();
};
#endif _SCENEOBJECT_H