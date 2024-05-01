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
	bool LoadPNG(char* path);

	GLuint GetID() const { return _ID; }
	int GetWidth() const { return _width; }
	int GetHeight() const { return _height; }
};

struct PNGINFOHEADER
{
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compression;
	uint8_t filter;
	uint8_t interlace;
};

struct PNGFILEHEADER {
	CHAR signature[8]; // PNG signature
};

struct ChunkData
{
	uint32_t length;
	std::string type;
	std::vector<char> data;
};