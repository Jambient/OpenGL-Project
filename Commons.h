#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <Windows.h>
#include <gl/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <array>

// definitions
#define M_PI 3.14159265358979323846
#define VIEWPORT_WIDTH 800
#define VIEWPORT_HEIGHT 800

// structures
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
};

// classes
class Ray
{
public:
	Ray(const glm::vec3& orig, const glm::vec3& dir);

    glm::vec3 orig, dir, invdir;
};

class AABBox
{
public:
    AABBox(const glm::vec3& b0 = glm::vec3(), const glm::vec3& b1 = glm::vec3()) { bounds[0] = b0, bounds[1] = b1; }
	bool intersect(const Ray& r, float& t);

    glm::vec3 bounds[2];
};

// functions
glm::vec3 RotateVector(const glm::vec3& vector, const glm::vec3& rotation);