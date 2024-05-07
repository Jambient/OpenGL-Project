#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"

Cube::Cube(Mesh* mesh, Texture2D* texture, float x, float y, float z, float rotation) : SceneObject(mesh, texture, glm::vec3(x, y, z))
{
	_position.x = x; _position.y = y; _position.z = z;
	//_rotation = rotation;

	_material = new Material();
	_material->Ambient.x = 0.8f; _material->Ambient.y = 0.05f; _material->Ambient.z = 0.0f;
	_material->Ambient.z = 1.0f;
	_material->Diffuse.x = 0.8f; _material->Diffuse.y = 0.05f; _material->Diffuse.z = 0.0f;
	_material->Diffuse.z = 1.0f;
	_material->Specular.x = 1.0f; _material->Specular.y = 1.0f; _material->Specular.z = 1.0f;
	_material->Specular.z = 1.0f;
	_material->Shininess = 100.0f;
}

Cube::~Cube()
{

}

void Cube::Draw()
{
	if (_mesh->Vertices != nullptr && _mesh->Normals != nullptr && _mesh->Indices != nullptr && _mesh->TexCoords != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, _texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, _mesh->Vertices->data());
		glNormalPointer(GL_FLOAT, 0, _mesh->Normals->data());
		glTexCoordPointer(2, GL_FLOAT, 0, _mesh->TexCoords->data());

		glMaterialfv(GL_FRONT, GL_AMBIENT, &(_material->Ambient.x));
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &(_material->Diffuse.x));
		glMaterialfv(GL_FRONT, GL_SPECULAR, &(_material->Specular.x));
		glMaterialf(GL_FRONT, GL_SHININESS, _material->Shininess);

		glPushMatrix();
		glTranslatef(_position.x, _position.y, _position.z);
		glRotatef(_rotation, 1.0f, 1.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, _mesh->Indices->size(), GL_UNSIGNED_SHORT, _mesh->Indices->data());
		glPopMatrix();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void Cube::Update()
{
	_rotation += 1.0f;
	//_position.z = std::fmod(_position.z - 0.4f, -200.0f);
}