#include "SceneObject.h"
#include "Commons.h"
#include <glm/glm.hpp>

glm::vec3 vertexToVector(Vertex vertex)
{
	return glm::vec3(vertex.x, vertex.y, vertex.z);
}

void SceneObject::UpdateBoundingBox()
{
	m_bbox.bounds[0] = m_bbox.bounds[1] = RotateVector(vertexToVector((*m_mesh->Vertices)[0]) * m_scale, m_rotation);

	for (const Vertex& vertex : *m_mesh->Vertices)
	{
		glm::vec3 scaledVertex = RotateVector(vertexToVector(vertex) * m_scale, m_rotation);
		m_bbox.bounds[0] = glm::min(m_bbox.bounds[0], scaledVertex);
		m_bbox.bounds[1] = glm::max(m_bbox.bounds[1], scaledVertex);
	}

	m_bbox.bounds[0] += m_position;
	m_bbox.bounds[1] += m_position;
}

SceneObject::SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position)
{
	m_mesh = mesh;
	m_texture = texture;
	m_position = position;
	m_rotation = glm::vec3();
	m_scale = glm::vec3(1, 1, 1);

	m_material = new Material();
	m_material->Ambient.x = 1.0f; m_material->Ambient.y = 1.0f; m_material->Ambient.z = 1.0f;
	m_material->Ambient.w = 1.0f;
	m_material->Diffuse.x = 1.0f; m_material->Diffuse.y = 1.0f; m_material->Diffuse.z = 1.0f;
	m_material->Diffuse.w = 1.0f;
	m_material->Specular.x = 0.0f; m_material->Specular.y = 0.0f; m_material->Specular.z = 0.0f;
	m_material->Specular.w = 1.0f;
	m_material->Shininess = 250.0f;

	UpdateBoundingBox();
}

SceneObject::~SceneObject(){}
void SceneObject::Update(){}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, std::vector<GLushort>* indices) {
	GLsizei range_count = (end - start + 1);
	glDrawElements(mode, range_count, type, &((*indices)[start]));
}

void SceneObject::Draw(bool drawBoundingBox, glm::vec3 positionOffset)
{
	if (m_mesh->Vertices != nullptr && m_mesh->Normals != nullptr && m_mesh->Indices != nullptr && m_mesh->TexCoords != nullptr)
	{
		bool hasTexCoords = m_mesh->TexCoords->size() > 0;

		if (hasTexCoords)
			glBindTexture(GL_TEXTURE_2D, m_texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		if (hasTexCoords)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, m_mesh->Vertices->data());
		glNormalPointer(GL_FLOAT, 0, m_mesh->Normals->data());
		if (hasTexCoords)
			glTexCoordPointer(2, GL_FLOAT, 0, m_mesh->TexCoords->data());

		glPushMatrix();
		glTranslatef(m_position.x + positionOffset.x, m_position.y + positionOffset.y, m_position.z + positionOffset.z);
		glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);
		glScalef(m_scale.x, m_scale.y, m_scale.z);

		if (m_mesh->MaterialUsage.size() > 0)
		{
			for (const auto& pair : m_mesh->MaterialUsage)
			{
				Material currentMat = m_mesh->Materials[pair.second];
				// NEED TO UNDERSTAND STAND WHY SWAPPING THESE AROUND WORKED
				glMaterialfv(GL_FRONT, GL_AMBIENT, &(currentMat.Diffuse.x));
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &(currentMat.Ambient.x));
				glMaterialfv(GL_FRONT, GL_SPECULAR, &(currentMat.Specular.x));
				glMaterialf(GL_FRONT, GL_SHININESS, currentMat.Shininess);

				glDrawRangeElements(GL_TRIANGLES, pair.first[0], pair.first[1], m_mesh->Indices->size(), GL_UNSIGNED_SHORT, m_mesh->Indices);
			}
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, &(m_material->Ambient.x));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &(m_material->Diffuse.x));
			glMaterialfv(GL_FRONT, GL_SPECULAR, &(m_material->Specular.x));
			glMaterialf(GL_FRONT, GL_SHININESS, m_material->Shininess);

			glDrawElements(GL_TRIANGLES, m_mesh->Indices->size(), GL_UNSIGNED_SHORT, m_mesh->Indices->data());
		}

		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (drawBoundingBox)
	{
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glm::vec3 size = (m_bbox.bounds[1] - m_bbox.bounds[0]);
		glm::vec3 center = (m_bbox.bounds[0] + m_bbox.bounds[1]) / 2.0f + positionOffset;

		glColor3f(0.0f, 0.0f, 1.0f);
		glTranslatef(center.x, center.y, center.z);
		glScalef(size.x, size.y, size.z);
		glutWireCube(1);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}
