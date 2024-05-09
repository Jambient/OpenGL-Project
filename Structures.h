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

class Ray
{
public:
    Ray(const glm::vec3& orig, const glm::vec3& dir) : orig(orig), dir(dir)
    {
        invdir = glm::vec3(1 / dir.x, 1 / dir.y, 1 / dir.z);
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }
    glm::vec3 orig, dir; // ray orig and dir
    glm::vec3 invdir;
    int sign[3];
};

class AABBox
{
public:
    AABBox(const glm::vec3& b0 = glm::vec3(), const glm::vec3& b1 = glm::vec3()) { bounds[0] = b0, bounds[1] = b1; }
    bool intersect(const Ray& r, float& t) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
        tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
        tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
        tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
        tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        t = tmin;

        if (t < 0) {
            t = tmax;
            if (t < 0) return false;
        }

        return true;
    }
    glm::vec3 bounds[2];
};