#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"

Cube::Cube(Mesh* mesh, Texture2D* texture, float x, float y, float z, float rotation) : SceneObject(mesh, texture, Vector3(x, y, z))
{
	_position.x = x; _position.y = y; _position.z = z;
	//_rotation = rotation;
}

Cube::~Cube()
{

}

void Cube::Draw()
{
	if (_mesh->Vertices != nullptr && _mesh->Colors != nullptr && _mesh->Indices != nullptr && _mesh->TexCoords != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, _texture->GetID());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, _mesh->Vertices->data());
		glColorPointer(3, GL_FLOAT, 0, _mesh->Colors->data());
		glTexCoordPointer(2, GL_FLOAT, 0, _mesh->TexCoords->data());

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
	//_rotation += 1.0f;
	//_position.z = std::fmod(_position.z - 0.4f, -200.0f);
}