#include <iostream>
#include <fstream>
#include <vector>
#include "Cube.h"

std::vector<Vertex>* Cube::indexedVertices = nullptr;
std::vector<Color>* Cube::indexedColors = nullptr;
std::vector<GLuint>* Cube::indices = nullptr;
int Cube::numVertices = 0;
int Cube::numColors = 0;
int Cube::numIndices = 0;

Cube::Cube(float x, float y, float z)
{
	_position.x = x; _position.y = y; _position.z = z;
	_rotation = 0.0f;
}

Cube::~Cube()
{
	delete indexedVertices, indexedColors, indices;
}

bool Cube::LoadTXT(char* path)
{
	//std::ifstream inFile;
	//inFile.open(path);
	//if (!inFile.good())
	//{
	//	std::cerr << "Can't open text file " << path << std::endl;
	//	return false;
	//}

	//// load vertices
	//inFile >> numVertices;
	//indexedVertices = new Vertex[numVertices];
	//for (int i = 0; i < numVertices; i++)
	//{
	//	Vertex vertex;
	//	inFile >> vertex.x >> vertex.y >> vertex.z;
	//	indexedVertices[i] = vertex;
	//}

	//// load colors
	//inFile >> numColors;
	//indexedColors = new Color[numColors];
	//for (int i = 0; i < numColors; i++)
	//{
	//	Color color;
	//	inFile >> color.r >> color.g >> color.b;
	//	indexedColors[i] = color;
	//}

	//// load indices
	//inFile >> numIndices;
	//indices = new GLushort[numIndices];
	//for (int i = 0; i < numIndices; i+=3)
	//{
	//	inFile >> indices[i] >> indices[i+1] >> indices[i+2];
	//}

	//inFile.close();

	//return true;
	return true;
}

bool Cube::LoadOBJ(char* path)
{
	std::ifstream inFile;
	inFile.open(path);
	if (!inFile.good())
	{
		std::cerr << "Can't open object file " << path << std::endl;
		return false;
	}

	indexedVertices = new std::vector<Vertex>();
	indices = new std::vector<GLushort>();

	char c;
	while (inFile >> c)
	{
		if (c == 'v')
		{
			Vertex vertex;
			inFile >> vertex.x >> vertex.y >> vertex.z;
			indexedVertices->push_back(vertex);
			//std::cout << "{ " << vertex.x << ", " << vertex.y << ", " << vertex.z << " }" << std::endl;
		}
		else if (c == 'f')
		{
			GLushort i1, i2, i3;
			inFile >> i1 >> i2 >> i3;
			indices->push_back(i1);
			indices->push_back(i2);
			indices->push_back(i3);
			//std::cout << "{ " << i1 << ", " << i2 << ", " << i3 << " }" << std::endl;
		}
	}

	indexedVertices->shrink_to_fit();
	indices->shrink_to_fit();

	std::cout << indexedVertices->size() << std::endl;
	std::cout << indices->size() << std::endl;

	indexedColors = new std::vector<Color>();
	for (int i = 0; i < indexedVertices->size(); i++)
	{
		Color color; //color.r = ((double)rand()) / RAND_MAX; color.g = ((double)rand()) / RAND_MAX; color.b = ((double)rand()) / RAND_MAX;
		color.r = 1; color.g = 0; color.b = 0;
		indexedColors->push_back(color);
	}

	return true;
}

void Cube::Draw()
{
	if (indexedVertices != nullptr && indexedColors != nullptr && indices != nullptr)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		float* pvertex = (float*)indexedVertices->data();
		glVertexPointer(3, GL_FLOAT, 0, pvertex);
		glColorPointer(3, GL_FLOAT, 0, indexedColors->data());

		glPushMatrix();
		glTranslatef(_position.x, _position.y, _position.z);
		glRotatef(_rotation, 1.0f, 1.0f, 1.0f);

		unsigned short* pindices = (unsigned short* )indices->data();
		//glDrawElements(GL_TRIANGLES, indices->size() /*indexedVertices->size()*/, GL_UNSIGNED_SHORT, pindices);
		glDrawElements(GL_TRIANGLES, 11050 /*indexedVertices->size()*/, GL_UNSIGNED_SHORT, pindices);
		glPopMatrix();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void Cube::Update()
{
	_rotation += 0.3f;
}