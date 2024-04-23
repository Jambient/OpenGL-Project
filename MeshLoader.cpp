#include <iostream>
#include <fstream>
#include "MeshLoader.h"

using namespace std;

namespace MeshLoader
{
	void LoadVertices(ifstream& inFile, Mesh& mesh);
	void LoadColours(ifstream& inFile, Mesh& mesh);
	void LoadIndices(ifstream& inFile, Mesh& mesh);

	void LoadVertices(ifstream& inFile, Mesh& mesh)
	{
		inFile >> mesh.VertexCount;

		for (int i = 0; i < mesh.VertexCount; i++)
		{
			Vertex vertex;
			inFile >> vertex.x >> vertex.y >> vertex.z;
			mesh.Vertices->push_back(vertex);
		}
	}

	void LoadColours(ifstream& inFile, Mesh& mesh)
	{
		inFile >> mesh.ColorCount;

		for (int i = 0; i < mesh.ColorCount; i++)
		{
			Color color;
			inFile >> color.r >> color.g >> color.b;
			mesh.Colors->push_back(color);
		}
	}

	void LoadIndices(ifstream& inFile, Mesh& mesh)
	{
		inFile >> mesh.IndexCount;

		for (int i = 0; i < mesh.IndexCount; i += 3)
		{
			GLushort i1, i2, i3;
			inFile >> i1 >> i2 >> i3;
			mesh.Indices->push_back(i1);
			mesh.Indices->push_back(i2);
			mesh.Indices->push_back(i3);
		}
	}

	Mesh* MeshLoader::LoadTXT(char* path)
	{
		Mesh* mesh = new Mesh();

		ifstream inFile;

		inFile.open(path);

		if (!inFile.good())  
		{
			cerr  << "Can't open texture file " << path << endl;
			return nullptr;
		}

		mesh->Vertices = new std::vector<Vertex>();
		mesh->Indices = new std::vector<GLushort>();
		mesh->Colors = new std::vector<Color>();

		LoadVertices(inFile, *mesh);
		LoadColours(inFile, *mesh);
		LoadIndices(inFile, *mesh);

		mesh->Vertices->shrink_to_fit();
		mesh->Colors->shrink_to_fit();
		mesh->Indices->shrink_to_fit();

		inFile.close();

		return mesh;
	}

	Mesh* MeshLoader::LoadOBJ(char* path)
	{
		Mesh* mesh = new Mesh();

		ifstream inFile;

		inFile.open(path);

		if (!inFile.good())
		{
			cerr << "Can't open object file " << path << endl;
			return nullptr;
		}

		mesh->Vertices = new std::vector<Vertex>();
		mesh->Indices = new std::vector<GLushort>();
		mesh->Colors = new std::vector<Color>();
		mesh->VertexCount, mesh->ColorCount, mesh->IndexCount = 1;

		char c;
		while (inFile >> c)
		{
			if (c == 'v')
			{
				Vertex vertex;
				inFile >> vertex.x >> vertex.y >> vertex.z;
				mesh->Vertices->push_back(vertex);
			}
			else if (c == 'f')
			{
				GLushort i1, i2, i3;
				inFile >> i1 >> i2 >> i3;
				mesh->Indices->push_back(i1 - 1);
				mesh->Indices->push_back(i2 - 1);
				mesh->Indices->push_back(i3 - 1);
			}
		}

		for (int i = 0; i < mesh->Vertices->size(); i++)
		{
			Color color; color.r = ((double)rand()) / RAND_MAX; color.g = ((double)rand()) / RAND_MAX; color.b = ((double)rand()) / RAND_MAX;
			mesh->Colors->push_back(color);
		}

		mesh->Vertices->shrink_to_fit();
		mesh->Colors->shrink_to_fit();
		mesh->Indices->shrink_to_fit();

		return mesh;
	}
}