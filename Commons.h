#pragma once
#ifndef _COMMONS_H
#define _COMMONS_H
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
#define REFRESHRATE 16

// structures
struct Color
{
	GLfloat r = 0.0f;
	GLfloat g = 0.0f;
	GLfloat b = 0.0f;
};

struct Vertex
{
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat z = 0.0f;
};

struct TexCoord
{
	GLfloat u = 0.0f;
	GLfloat v = 0.0f;
};

struct Lighting
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	GLfloat shininess;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<TexCoord> texCoords;
	std::vector<Vertex> normals;
	std::vector<GLushort> indices;
	std::map<std::string, Material> materials;
	std::map<std::array<GLuint, 2>, std::string> materialUsage;
};

// classes

/**
 * @class Ray
 * @brief A class for rays.
 */
class Ray
{
public:
	/**
	 * @brief Constructs a Ray object.
	 * @param orig The origin of the ray.
	 * @param dir The direction of the ray.
	 */
	Ray(const glm::vec3& orig, const glm::vec3& dir);

    glm::vec3 orig; /* The origin of the ray. */
	glm::vec3 dir; /* The direction of the ray. */
	glm::vec3 invdir; /* The inverse direction of the ray. Used in intersection calculations. */
};

/**
 * @class AABBox
 * @brief A class for axis aligned bounding boxes.
 */
class AABBox
{
public:
    /**
     * @brief Constructs an AABBox object.
     * @param b0 The minimum point of the bounding box.
     * @param b1 The maximum point of the bounding box.
     */
    AABBox(const glm::vec3& b0 = glm::vec3(), const glm::vec3& b1 = glm::vec3()) { bounds[0] = b0, bounds[1] = b1; }

	/**
	 * @brief Checks if a specified Ray intersects with the bounding box.
	 * @param r The ray to test intersection.
	 * @param t The distance along the ray the intersection happened at. Not set if no intersection occured.
	 * @return True if the ray intersects, false otherwise.
	 */
	bool Intersect(const Ray& r, float& t);

    glm::vec3 bounds[2]; /* The bounds of the bounding box. */
};

// functions

/**
 * @brief Rotates a vector by a specified rotation.
 * @param vector The vector to rotate.
 * @param rotation The rotation to apply.
 * @return The vector with the rotation applied.
 */
glm::vec3 RotateVector(const glm::vec3& vector, const glm::vec3& rotation);

/**
 * @brief Gets the file extension of the file given a path.
 * @param path The path to the file.
 * @return The file extension without the '.'. For example file.txt would return "txt".
 */
std::string GetFileExtension(const char* path);

#endif //_COMMONS_H