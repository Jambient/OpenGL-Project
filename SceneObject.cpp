#include "SceneObject.h"

SceneObject::SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position)
{
	_mesh = mesh;
	_texture = texture;
	_position = position;
	_rotation = glm::vec3();
	_scale = glm::vec3();

	_material = new Material();
	_material->Ambient.x = 1.0f; _material->Ambient.y = 1.0f; _material->Ambient.z = 1.0f;
	_material->Ambient.w = 1.0f;
	_material->Diffuse.x = 1.0f; _material->Diffuse.y = 1.0f; _material->Diffuse.z = 1.0f;
	_material->Diffuse.w = 1.0f;
	_material->Specular.x = 0.0f; _material->Specular.y = 0.0f; _material->Specular.z = 0.0f;
	_material->Specular.w = 1.0f;
	_material->Shininess = 250.0f;
}

SceneObject::~SceneObject(){}
void SceneObject::Update(){}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, std::vector<GLushort>* indices) {
	GLsizei range_count = (end - start + 1);
	glDrawElements(mode, range_count, type, &((*indices)[start]));
}

void SceneObject::Draw()
{
	if (_mesh->Vertices != nullptr && _mesh->Normals != nullptr && _mesh->Indices != nullptr && _mesh->TexCoords != nullptr)
	{
		bool hasTexCoords = _mesh->TexCoords->size() > 0;

		if (hasTexCoords)
			glBindTexture(GL_TEXTURE_2D, _texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		if (hasTexCoords)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, _mesh->Vertices->data());
		glNormalPointer(GL_FLOAT, 0, _mesh->Normals->data());
		if (hasTexCoords)
			glTexCoordPointer(2, GL_FLOAT, 0, _mesh->TexCoords->data());

		glPushMatrix();
		glTranslatef(_position.x, _position.y, _position.z);
		glRotatef(_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(_rotation.z, 0.0f, 0.0f, 1.0f);
		glScalef(_scale.x, _scale.y, _scale.z);

		if (_mesh->MaterialUsage.size() > 0)
		{
			for (const auto& pair : _mesh->MaterialUsage)
			{
				Material currentMat = _mesh->Materials[pair.second];
				glMaterialfv(GL_FRONT, GL_AMBIENT, &(currentMat.Diffuse.x));
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &(currentMat.Ambient.x));
				glMaterialfv(GL_FRONT, GL_SPECULAR, &(currentMat.Specular.x));
				glMaterialf(GL_FRONT, GL_SHININESS, currentMat.Shininess);
				glDrawRangeElements(GL_TRIANGLES, pair.first[0], pair.first[1], _mesh->Indices->size(), GL_UNSIGNED_SHORT, _mesh->Indices);
			}
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, &(_material->Ambient.x));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &(_material->Diffuse.x));
			glMaterialfv(GL_FRONT, GL_SPECULAR, &(_material->Specular.x));
			glMaterialf(GL_FRONT, GL_SHININESS, _material->Shininess);

			glDrawElements(GL_TRIANGLES, _mesh->Indices->size(), GL_UNSIGNED_SHORT, _mesh->Indices->data());
		}

		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

float SceneObject::SignedDistanceField(const glm::vec3& point) { return 1.0f; };
