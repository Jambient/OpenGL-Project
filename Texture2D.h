#pragma once
#include <iostream>
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

class Texture2D
{
private:
	GLuint m_ID = 0; // Texture ID
	int m_width, m_height = 0;

public:
	Texture2D();
	~Texture2D();

	bool LoadRAW(const char* path, int width, int height);
	bool LoadTGA(const char* path);
	bool LoadBMP(const char* path);
	bool LoadPNG(const char* path);

	GLuint GetID() const { return m_ID; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
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
	uint32_t length = 0;
	std::string type;
	std::vector<char> data;
};

enum class FilterType {
	None = 0,
	Sub = 1,
	Up = 2,
	Average = 3,
	Paeth = 4
};