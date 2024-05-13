#pragma once
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

class SceneObject
{
private:
	void UpdateBoundingBox();

protected:
	Mesh* m_mesh;
	Texture2D* m_texture;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
	Material* m_material;
	AABBox m_bbox;
	Animation* m_animation;

public:
	SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position);
	virtual ~SceneObject();

	virtual void Update();
	virtual void Draw(bool drawBoundingBox = false, glm::vec3 positionOffset = glm::vec3());

	glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 position) { m_position = position; UpdateBoundingBox(); }
	void OffsetPosition(glm::vec3 offset) { m_position += offset; UpdateBoundingBox(); }

	glm::vec3 GetRotation() const { return m_rotation; }
	void SetRotation(glm::vec3 rotation) { m_rotation = rotation; UpdateBoundingBox(); }
	void OffsetRotation(glm::vec3 offset) { m_rotation += offset; UpdateBoundingBox(); }

	glm::vec3 GetScale() const { return m_scale; }
	void SetScale(glm::vec3 scale) { m_scale = scale; UpdateBoundingBox(); }

	Animation* GetAnimation() const { return m_animation; }
	void SetAnimation(Animation* animation) { m_animation = animation; }

	AABBox GetBoundingBox() const { return m_bbox; }
};