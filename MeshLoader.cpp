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
	namespace
	{
		std::map<std::string, Mesh*> cache;

		void LoadVertices(ifstream& inFile, Mesh& mesh);
		void LoadNormals(ifstream& inFile, Mesh& mesh);
		void LoadIndices(ifstream& inFile, Mesh& mesh);

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
				Vertex vector;
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

			mesh->Vertices = new std::vector<Vertex>();
			mesh->Indices = new std::vector<GLushort>();
			mesh->Normals = new std::vector<Vertex>();
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
						//materials->insert(pair<string, Material>(currentMaterialName, *currentMaterialData));
						materials[currentMaterialName] = *currentMaterialData;
						delete currentMaterialData;
					}
					currentMaterialData = new Material();
					currentMaterialName = data[0];
				}
				else if (type == "Ns")
				{
					currentMaterialData->Shininess = stof(data[0]);
				}
				else if (type == "Ka")
				{
					currentMaterialData->Ambient = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
				}
				else if (type == "Kd")
				{
					currentMaterialData->Diffuse = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
				}
				else if (type == "Ks")
				{
					currentMaterialData->Specular = glm::vec4(stof(data[0]), stof(data[1]), stof(data[2]), 1.0f);
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

		struct ArrayHasher {
			std::size_t operator()(const std::array<GLshort, 3>& a) const {
				std::size_t h = 0;

				for (auto e : a) {
					h ^= std::hash<int>{}(e)+0x9e3779b9 + (h << 6) + (h >> 2);
				}
				return h;
			}
		};

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
							multiIndex.verticesIndex = stof(indices[0]) - 1.0f;
						}
						if (indices.size() >= 2 && indices[1].size() > 0) // vertex + texture coordinate indices
						{
							multiIndex.texCoordsIndex = stof(indices[1]) - 1.0f;
						}
						if (indices.size() == 3)
						{
							multiIndex.normalsIndex = stof(indices[2]) - 1.0f;
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

					mesh->Materials = loadMTL(mtlPath.c_str());
				}
				else if (type == "usemtl")
				{
					// add last material usage
					if (materialName.size() > 0)
					{
						array<GLuint, 2> usageIndexes = { (GLuint)materialUsageStart, (GLuint)lastFaceIndex };
						//mesh->MaterialUsage->insert(pair<array<GLuint, 2>, string>(usageIndexes, materialName));
						mesh->MaterialUsage[usageIndexes] = materialName;
					}

					materialUsageStart = lastFaceIndex + 1;
					materialName = data[0];
				}
			}
			if (materialName.size() > 0)
			{
				array<GLuint, 2> usageIndexes = { (GLuint)materialUsageStart, (GLuint)lastFaceIndex };
				mesh->MaterialUsage[usageIndexes] = materialName;
			}

			inFile.close();

			mesh->Vertices = new vector<Vertex>();
			mesh->Indices = new vector<GLushort>();
			mesh->Normals = new vector<Vertex>();
			mesh->TexCoords = new vector<TexCoord>();
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
					mesh->Indices->push_back(currentIndex);
					indexMap[key] = currentIndex;

					mesh->Vertices->push_back(tempVertices[multiIndex.verticesIndex]);

					if (multiIndex.normalsIndex >= 0)
						mesh->Normals->push_back(tempNormals[multiIndex.normalsIndex]);
					if (multiIndex.texCoordsIndex >= 0)
						mesh->TexCoords->push_back(tempTexCoords[multiIndex.texCoordsIndex]);

					currentIndex++;
				}
				else
				{
					mesh->Indices->push_back(it->second);
				}
			}

			return mesh;
		}

		string getFileExtension(const char* path)
		{
			string strPath(path);
			size_t lastDotPos = strPath.find_last_of('.');
			if (lastDotPos != string::npos) {
				return strPath.substr(lastDotPos + 1);
			}
			return "";
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
		std::string extension = getFileExtension(path);
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
