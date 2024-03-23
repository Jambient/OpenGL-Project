#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>

#define GRAVITY 0.00961f
#define REFRESHRATE 16

struct Vector2
{
	float x, y;

	inline Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	inline Vector2()
	{
		x = 0;
		y = 0;
	}
};

struct Body
{
	Vector2 position;
	Vector2 velocity;
	float mass;

	inline Body(Vector2 position, Vector2 velocity, float mass)
	{
		this->position = position;
		this->velocity = velocity;
		this->mass = mass;
	}
	inline Body()
	{
		this->position = Vector2();
		this->velocity = Vector2();
		this->mass = 1;
	}
};

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Update();
	void Display();
	void DrawRegularPolygon(Vector2 center, float rotation, float radius, bool filled = true, float sides = 64.0f);
	//void DrawTriangleFromAngles(float angle1, float angle2, float base, Vector2 pos, float rotation);
	void Keyboard(unsigned char key, int x, int y);

private:
	Body bodies[3];
	unsigned int body_count = 3;

	float timeScale = 0.1f;
};