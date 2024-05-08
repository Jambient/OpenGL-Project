#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <array>

struct Color
{
	GLfloat r, g, b;
};

struct Vertex
{
	GLfloat x, y, z;
};

struct TexCoord
{
	GLfloat u, v;
};

struct MultiIndex
{
	GLshort verticesIndex;
	GLshort texCoordsIndex;
	GLshort normalsIndex;
};

struct Lighting
{
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
};
struct Material
{
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	GLfloat Shininess;
};

struct Mesh
{
	std::vector<Vertex>* Vertices;
	std::vector<TexCoord>* TexCoords;
	std::vector<Vertex>* Normals;
	std::vector<GLushort>* Indices;
	std::map<std::string, Material> Materials;
	std::map<std::array<GLuint, 2>, std::string> MaterialUsage;
	int VertexCount, TexCoordCount, NormalCount, IndexCount;
};