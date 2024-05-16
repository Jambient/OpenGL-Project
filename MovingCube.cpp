#include "MovingCube.h"

MovingCube::MovingCube(Mesh* mesh, Texture2D* texture, glm::vec3 initialPosition, glm::vec3 velocity): SceneObject(mesh, texture, initialPosition)
{
	m_position = initialPosition;
	m_velocity = velocity;
}

MovingCube::~MovingCube()
{
}

void MovingCube::Draw()
{
	if (m_mesh->vertices.size() > 0 && m_mesh->indices.size() > 0)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, m_mesh->vertices.data());
		glNormalPointer(GL_FLOAT, 0, m_mesh->normals.data());
		glTexCoordPointer(2, GL_FLOAT, 0, m_mesh->texCoords.data());

		glPushMatrix();
		glTranslatef(m_position.x, m_position.y, m_position.z);

		glDrawElements(GL_TRIANGLES, m_mesh->indices.size(), GL_UNSIGNED_SHORT, m_mesh->indices.data());
		glPopMatrix();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void MovingCube::Update()
{
	m_position = m_position + m_velocity * 0.06f;
}
