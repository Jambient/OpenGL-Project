#include "MovingCube.h"

MovingCube::MovingCube(Mesh* mesh, Texture2D* texture, Vector3 initialPosition, Vector3 velocity): SceneObject(mesh, texture)
{
	_position = initialPosition;
	_velocity = velocity;
}

MovingCube::~MovingCube()
{
}

void MovingCube::Draw()
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

		glDrawElements(GL_TRIANGLES, _mesh->Indices->size(), GL_UNSIGNED_SHORT, _mesh->Indices->data());
		glPopMatrix();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void MovingCube::Update()
{
	_position = _position + _velocity * 0.06;
}
