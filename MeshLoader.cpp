#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <tuple>
#include <array>
#include "MeshLoader.h"

using namespace std;

namespace MeshLoader
{
	// namespace for local variables and methods
	namespace
	{
		// variables
		std::map<std::string, Mesh*> cache;

		// structures
		struct ArrayHasher {
			std::size_t operator()(const std::array<GLshort, 3>& a) const {
				std::size_t h = 0;

				for (auto e : a) {
					h ^= std::hash<int>{}(e)+0x9e3779b9 + (h << 6) + (h >> 2);
				}
				return h;
			}
		};

		struct MultiIndex
		{
			GLshort verticesIndex;
			GLshort texCoordsIndex;
			GLshort normalsIndex;
		};

		// helper functions
		vector<string> splitString(string str, char delim)
		{
			string s;
			stringstream ss(str);
			vector<string> v;

			while (getline(ss, s, delim))
			{
				v.push_back(s);
			}

			return v;
		}

		// loading functions
		void LoadVertices(ifstream& inFile, Mesh& mesh)
		{
			int vertexCount;
			inFile >> vertexCount;

			for (int i = 0; i < vertexCount; i++)
			{
				Vertex vertex;
				inFile >> vertex.x >> vertex.y >> vertex.z;
				mesh.vertices.push_back(vertex);
			}
		}

		void LoadNormals(ifstream& inFile, Mesh& mesh)
		{
			int normalCount;
			inFile >> normalCount;

			for (int i = 0; i < normalCount; i++)
			{
				Vertex vector;
				inFile >> vector.x >> vector.y >> vector.z;
				mesh.normals.push_back(vector);
			}
		}

		void LoadTexCoords(ifstream& inFile, Mesh& mesh)
		{
			int texCoordCount;
			inFile >> texCoordCount;

			for (int i = 0; i < texCoordCount; i++)
			{
				TexCoord texCoord;
				inFile >> texCoord.u >> texCoord.v;
				mesh.texCoords.push_back(texCoord);
			}
		}

		void LoadIndices(ifstream& inFile, Mesh& mesh)
		{
			int indexCount;
			inFile >> indexCount;

			for (int i = 0; i < indexCount; i += 3)
			{
				GLushort i1, i2, i3;
				inFile >> i1 >> i2 >> i3;
				mesh.indices.push_back(i1);
				mesh.indices.push_back(i2);
				mesh.indices.push_back(i3);
			}
		}

		Mesh* loadTXT(const char* path)
		{
			Mesh* mesh = new Mesh();
			ifstream inFile;

			inFile.open(path);
			if (!inFile.good())
			{
				cerr << "Can't open texture file " << path << endl;
				return nullptr;
			}

			LoadVertices(inFile, *mesh);
			LoadTexCoords(inFile, *mesh);
			LoadNormals(inFile, *mesh);
			LoadIndices(inFile, *mesh);

			inFile.close();

			return mesh;
		}

		map<string, Material> loadMTL(const char* path)
		{
			map<string, Material> materials;
			ifstream inFile;

			inFile.open(path);

			if (!inFile.good())
			{
				cerr << "Can't open material file " << path << endl;
				return materials;
			}

			string currentMaterialName;
			Material* currentMaterialData = nullptr;
			string line;
			while (getline(inFile, line))
			{
				size_t spacePos = line.find(' ');
				string type = line.substr(0, spacePos);
				vector<string> data = splitString(line.substr(spacePos + 1), ' ');

				if (type == "newmtl")
				{
					if (currentMaterialData != nullptr)
					{
						materials[currentMaterialName] = *currentMaterialData;
						delete currentMaterialData;
					}
					currentMaterialData = new Material();
					currentMaterialName = data[0];
				}
				if (currentMaterialData != nullptr)
				{
					if (type == "Ns")
					{
						currentMaterialData->shininess = stof(data[0]);
					}
					else if (type == "Ka")
					{
						currentMaterialData->ambient = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
					}
					else if (type == "Kd")
					{
						currentMaterialData->diffuse = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
					}
					else if (type == "Ks")
					{
						currentMaterialData->specular = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
					}
				}
			}

			// add final material
			if (currentMaterialData != nullptr)
			{
				materials[currentMaterialName] = *currentMaterialData;
				delete currentMaterialData;
			}

			inFile.close();

			return materials;
		}

		Mesh* loadOBJ(const char* path)
		{
			Mesh* mesh = new Mesh();

			ifstream inFile;

			inFile.open(path);

			if (!inFile.good())
			{
				cerr << "Can't open object file " << path << endl;
				return nullptr;
			}

			vector<Vertex> tempVertices = vector<Vertex>();
			vector<MultiIndex> tempIndices = vector<MultiIndex>();
			vector<Vertex> tempNormals = vector<Vertex>();
			vector<TexCoord> tempTexCoords = vector<TexCoord>();
			GLint lastFaceIndex = -1;
			GLint materialUsageStart = -1;
			string materialName;

			string line;
			while (getline(inFile, line))
			{
				size_t spacePos = line.find(' ');
				string type = line.substr(0, spacePos);
				vector<string> data = splitString(line.substr(spacePos + 1), ' ');

				if (type == "v")
				{
					tempVertices.push_back({ stof(data[0]), stof(data[1]), stof(data[2]) });
				}
				else if (type == "vn")
				{
					tempNormals.push_back({ stof(data[0]), stof(data[1]), stof(data[2]) });
				}
				else if (type == "vt")
				{
					tempTexCoords.push_back({ stof(data[0]), stof(data[1]) });
				}
				else if (type == "f")
				{
					for (string& str : data)
					{
						vector<string> indices = splitString(str, '/');
						MultiIndex multiIndex = {-1, -1, -1};
						if (indices.size() >= 1) // only vertex indices
						{
							multiIndex.verticesIndex = (GLshort)stof(indices[0]) - 1;
						}
						if (indices.size() >= 2 && indices[1].size() > 0) // vertex + texture coordinate indices
						{
							multiIndex.texCoordsIndex = (GLshort)stof(indices[1]) - 1;
						}
						if (indices.size() == 3) // vertex, texture coordinate (optional), normal indices
						{
							multiIndex.normalsIndex = (GLshort)stof(indices[2]) - 1;
						}

						tempIndices.push_back(multiIndex);
						lastFaceIndex += 1;
					}
				}
				else if (type == "mtllib")
				{
					string pathStr = string(path);
					size_t directoryPos = pathStr.find_last_of('/');
					string mtlPath = data[0];
					if (directoryPos != string::npos)
						mtlPath = pathStr.substr(0, directoryPos + 1) + mtlPath;

					mesh->materials = loadMTL(mtlPath.c_str());
				}
				else if (type == "usemtl")
				{
					// add last material usage
					if (materialName.size() > 0)
					{
						array<GLuint, 2> usageIndexes = { (GLuint)materialUsageStart, (GLuint)lastFaceIndex };
						mesh->materialUsage[usageIndexes] = materialName;
					}

					materialUsageStart = lastFaceIndex + 1;
					materialName = data[0];
				}
			}
			if (materialName.size() > 0)
			{
				array<GLuint, 2> usageIndexes = { (GLuint)materialUsageStart, (GLuint)lastFaceIndex };
				mesh->materialUsage[usageIndexes] = materialName;
			}

			inFile.close();

			Vertex defaultNormal = { 0.0f, 1.0f, 0.0f };
			TexCoord defaultTexCoord = { 0.0f, 0.0f };

			using KeyType = std::array<GLshort, 3>;
			std::unordered_map<KeyType, GLushort, ArrayHasher> indexMap;
			GLushort currentIndex = 0;

			for (unsigned int i = 0; i < tempIndices.size(); i++)
			{
				MultiIndex multiIndex = tempIndices[i];
				KeyType key = { multiIndex.verticesIndex, multiIndex.texCoordsIndex, multiIndex.normalsIndex };

				auto it = indexMap.find(key);
				if (it == indexMap.end()) // index not found
				{
					mesh->indices.push_back(currentIndex);
					indexMap[key] = currentIndex;

					mesh->vertices.push_back(tempVertices[multiIndex.verticesIndex]);

					if (multiIndex.normalsIndex >= 0)
						mesh->normals.push_back(tempNormals[multiIndex.normalsIndex]);
					if (multiIndex.texCoordsIndex >= 0)
						mesh->texCoords.push_back(tempTexCoords[multiIndex.texCoordsIndex]);

					currentIndex++;
				}
				else
				{
					mesh->indices.push_back(it->second);
				}
			}

			return mesh;
		}
	}

	Mesh* MeshLoader::Load(const char* path)
	{
		std::string pathStr = path;
		
		// check if mesh has already been loaded
		if (cache.count(pathStr) > 0)
		{
			return cache[pathStr];
		}

		// load mesh
		std::string extension = GetFileExtension(path);
		Mesh* mesh = nullptr;
		if (extension == "txt")
			mesh = loadTXT(path);
		else if (extension == "obj")
			mesh = loadOBJ(path);

		// save mesh to cache
		if (mesh != nullptr)
			cache[pathStr] = mesh;
		else
			std::cerr << "Failed to load " << path << ", format not supported" << std::endl;

		return mesh;
	}
}
