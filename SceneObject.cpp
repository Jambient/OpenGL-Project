#include "SceneObject.h"
#include "Commons.h"
#include <glm/glm.hpp>

Material* SceneObject::s_defaultMaterial = nullptr;

glm::vec3 vertexToVector(Vertex vertex)
{
	return glm::vec3(vertex.x, vertex.y, vertex.z);
}

void SceneObject::UpdateBoundingBox()
{
	m_bbox.bounds[0] = m_bbox.bounds[1] = RotateVector(vertexToVector((m_mesh->vertices)[0]) * m_scale, m_rotation);

	for (const Vertex& vertex : m_mesh->vertices)
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

	if (s_defaultMaterial == nullptr)
	{
		s_defaultMaterial = new Material();
		s_defaultMaterial->ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		s_defaultMaterial->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		s_defaultMaterial->specular = glm::vec4(0.70f, 0.70f, 0.70f, 1.0f);
		s_defaultMaterial->shininess = 32.0f;
	}

	UpdateBoundingBox();
}

SceneObject::~SceneObject()
{
	delete m_mesh;
	delete m_texture;
}
void SceneObject::Update(){}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, std::vector<GLushort>* indices) {
	GLsizei range_count = (end - start + 1);
	glDrawElements(mode, range_count, type, &((*indices)[start]));
}

void SceneObject::Draw(bool drawBoundingBox, glm::vec3 positionOffset)
{
	if (m_mesh->vertices.size() > 0 && m_mesh->indices.size() > 0)
	{
		bool isTextured = m_mesh->texCoords.size() > 0 && m_texture != nullptr;
		bool hasNormals = m_mesh->normals.size() > 0;

		if (isTextured)
			glBindTexture(GL_TEXTURE_2D, m_texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		if (hasNormals)
			glEnableClientState(GL_NORMAL_ARRAY);
		if (isTextured)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, m_mesh->vertices.data());
		if (hasNormals)
			glNormalPointer(GL_FLOAT, 0, m_mesh->normals.data());
		if (isTextured)
			glTexCoordPointer(2, GL_FLOAT, 0, m_mesh->texCoords.data());

		glPushMatrix();
		glTranslatef(m_position.x + positionOffset.x, m_position.y + positionOffset.y, m_position.z + positionOffset.z);
		glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);
		glScalef(m_scale.x, m_scale.y, m_scale.z);

		if (m_mesh->materialUsage.size() > 0)
		{
			for (const auto& pair : m_mesh->materialUsage)
			{
				Material currentMat = m_mesh->materials[pair.second];
				glMaterialfv(GL_FRONT, GL_AMBIENT, &(currentMat.ambient.x));
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &(currentMat.diffuse.x));
				glMaterialfv(GL_FRONT, GL_SPECULAR, &(currentMat.specular.x));
				glMaterialf(GL_FRONT, GL_SHININESS, currentMat.shininess);

				glDrawRangeElements(GL_TRIANGLES, pair.first[0], pair.first[1], m_mesh->indices.size(), GL_UNSIGNED_SHORT, &m_mesh->indices);
			}
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, &(s_defaultMaterial->ambient.x));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &(s_defaultMaterial->diffuse.x));
			glMaterialfv(GL_FRONT, GL_SPECULAR, &(s_defaultMaterial->specular.x));
			glMaterialf(GL_FRONT, GL_SHININESS, s_defaultMaterial->shininess);

			glDrawElements(GL_TRIANGLES, m_mesh->indices.size(), GL_UNSIGNED_SHORT, m_mesh->indices.data());
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
