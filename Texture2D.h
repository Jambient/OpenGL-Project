#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

class Texture2D
{
private:
	GLuint _ID; // Texture ID
	int _width, _height;

public:
	Texture2D();
	~Texture2D();

	bool LoadRAW(char* path, int width, int height);
	bool LoadTGA(char* path);
	bool LoadBMP(char* path);

	GLuint GetID() const { return _ID; }
	int GetWidth() const { return _width; }
	int GetHeight() const { return _height; }
};