#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"

Cube::Cube(Mesh* mesh, float x, float y, float z) : SceneObject(mesh)
{
	_position.x = x; _position.y = y; _position.z = z;
	_rotation = 0.0f;
}

Cube::~Cube()
{

}

void Cube::Draw()
{
	if (_mesh->Vertices != nullptr && _mesh->Colors != nullptr && _mesh->Indices != nullptr)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		float* pvertex = (float*)_mesh->Vertices->data();
		glVertexPointer(3, GL_FLOAT, 0, pvertex);
		glColorPointer(3, GL_FLOAT, 0, _mesh->Colors->data());

		glPushMatrix();
		glTranslatef(_position.x, _position.y, _position.z);
		glRotatef(_rotation, 1.0f, 1.0f, 1.0f);

		unsigned short* pindices = (unsigned short* )_mesh->Indices->data();
		glDrawElements(GL_TRIANGLES, _mesh->Indices->size(), GL_UNSIGNED_SHORT, pindices);
		glPopMatrix();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void Cube::Update()
{
	_rotation += 0.3f;
}