#include "SceneObject.h"

glm::vec3 vertexToVector(Vertex vertex)
{
	return glm::vec3(vertex.x, vertex.y, vertex.z);
}

void SceneObject::UpdateBoundingBox()
{
	_bbox.bounds[0], _bbox.bounds[1] = vertexToVector((*_mesh->Vertices)[0]);

	for (const Vertex& vertex : *_mesh->Vertices)
	{
		_bbox.bounds[0] = glm::min(_bbox.bounds[0], vertexToVector(vertex));
		_bbox.bounds[1] = glm::max(_bbox.bounds[1], vertexToVector(vertex));
	}

	//_bbox.bounds[0] = (_bbox.bounds[0] + _position) * _scale;
	//_bbox.bounds[1] = (_bbox.bounds[1] + _position) * _scale;
	_bbox.bounds[0] = (_bbox.bounds[0] * _scale) + _position;
	_bbox.bounds[1] = (_bbox.bounds[1] * _scale) + _position;
}

SceneObject::SceneObject(Mesh* mesh, Texture2D* texture, glm::vec3 position)
{
	_mesh = mesh;
	_texture = texture;
	_position = position;
	_rotation = glm::vec3();
	_scale = glm::vec3(1, 1, 1);

	_material = new Material();
	_material->Ambient.x = 1.0f; _material->Ambient.y = 1.0f; _material->Ambient.z = 1.0f;
	_material->Ambient.w = 1.0f;
	_material->Diffuse.x = 1.0f; _material->Diffuse.y = 1.0f; _material->Diffuse.z = 1.0f;
	_material->Diffuse.w = 1.0f;
	_material->Specular.x = 0.0f; _material->Specular.y = 0.0f; _material->Specular.z = 0.0f;
	_material->Specular.w = 1.0f;
	_material->Shininess = 250.0f;

	UpdateBoundingBox();
}

SceneObject::~SceneObject(){}
void SceneObject::Update(){}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, std::vector<GLushort>* indices) {
	GLsizei range_count = (end - start + 1);
	glDrawElements(mode, range_count, type, &((*indices)[start]));
}

void SceneObject::Draw(bool drawBoundingBox)
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
				// NEED TO UNDERSTAND STAND WHY SWAPPING THESE AROUND WORKED
				glMaterialfv(GL_FRONT, GL_AMBIENT, &(currentMat.Ambient.x));
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &(currentMat.Diffuse.x));
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

	if (drawBoundingBox)
	{
		glDisable(GL_LIGHTING);
		glPushMatrix();
		double sizeX = _bbox.bounds[1].x - _bbox.bounds[0].x;
		double sizeY = _bbox.bounds[1].y - _bbox.bounds[0].y;
		double sizeZ = _bbox.bounds[1].z - _bbox.bounds[0].z;

		// Translate to the center of the bounding box
		double centerX = (_bbox.bounds[0].x + _bbox.bounds[1].x) / 2.0;
		double centerY = (_bbox.bounds[0].y + _bbox.bounds[1].y) / 2.0;
		double centerZ = (_bbox.bounds[0].z + _bbox.bounds[1].z) / 2.0;

		glColor3f(0.0f, 0.0f, 1.0f);
		glTranslatef(centerX, centerY, centerZ);
		glScalef(sizeX, sizeY, sizeZ);
		glutWireCube(1);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}

float SceneObject::SignedDistanceField(const glm::vec3& p) {
	return 1.0f;
};
