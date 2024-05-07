#include <iostream>
#include <fstream>
#include "MeshLoader.h"

using namespace std;

namespace MeshLoader
{
	void LoadVertices(ifstream& inFile, Mesh& mesh);
	void LoadNormals(ifstream& inFile, Mesh& mesh);
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

	void LoadNormals(ifstream& inFile, Mesh& mesh)
	{
		inFile >> mesh.NormalCount;

		for (int i = 0; i < mesh.NormalCount; i++)
		{
			glm::vec3 vector;
			inFile >> vector.x >> vector.y >> vector.z;
			mesh.Normals->push_back(vector);
		}
	}

	void LoadTexCoords(ifstream& inFile, Mesh& mesh)
	{
		inFile >> mesh.TexCoordCount;

		for (int i = 0; i < mesh.TexCoordCount; i++)
		{
			TexCoord texCoord;
			inFile >> texCoord.u >> texCoord.v;
			mesh.TexCoords->push_back(texCoord);
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
		mesh->Normals = new std::vector<glm::vec3>();
		mesh->TexCoords = new std::vector<TexCoord>();

		LoadVertices(inFile, *mesh);
		LoadTexCoords(inFile, *mesh);
		LoadNormals(inFile, *mesh);
		LoadIndices(inFile, *mesh);

		mesh->Vertices->shrink_to_fit();
		mesh->Normals->shrink_to_fit();
		mesh->Indices->shrink_to_fit();
		mesh->TexCoords->shrink_to_fit();

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
		mesh->Normals = new std::vector<glm::vec3>();

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

		mesh->Vertices->shrink_to_fit();
		mesh->Normals->shrink_to_fit();
		mesh->Indices->shrink_to_fit();

		return mesh;
	}
}