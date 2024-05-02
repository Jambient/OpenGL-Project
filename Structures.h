#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vector3.h"

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

struct Mesh
{
	std::vector<Vertex>* Vertices;
	std::vector<TexCoord>* TexCoords;
	std::vector<Vector3>* Normals;
	std::vector<GLushort>* Indices;
	int VertexCount, TexCoordCount, NormalCount, IndexCount;
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