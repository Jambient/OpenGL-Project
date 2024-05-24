#pragma once
#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include <iostream>
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

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

/**
 * @class Texture2D
 * @brief A class that represents a texture object. Supports RAW, TGA, BMP, and PNG files.
 */
class Texture2D
{
public:
	/**
	 * @brief Constructs a Texture2D object.
	 */
	Texture2D();
	/**
	 * @brief Deconstructs the object.
	 */
	~Texture2D();

	/**
	 * @brief Loads a RAW texture file. The texture file is required to be square.
	 * @param path The path to the file.
	 * @return True if the texture loaded succesfully, false otherwise.
	 */
	bool LoadRAW(const char* path);

	/**
	 * @brief Loads a TGA texture file.
	 * @param path The path to the file.
	 * @return True if the texture loaded succesfully, false otherwise.
	 */
	bool LoadTGA(const char* path);

	/**
	 * @brief Loads a BMP texture file.
	 * @param path The path to the file.
	 * @return True if the texture loaded succesfully, false otherwise.
	 */
	bool LoadBMP(const char* path);

	/**
	 * @brief Loads a PNG texture file. The texture file is required to be square.
	 * @param path The path to the file.
	 * @return True if the texture loaded succesfully, false otherwise.
	 */
	bool LoadPNG(const char* path);

	/**
	 * @brief Gets the unique texture ID for the texture.
	 * @return The textures unique ID.
	 */
	GLuint GetID() const { return m_ID; }

	/**
	 * @brief Gets the width of the texture.
	 * @return The texture width.
	 */
	int GetWidth() const { return m_width; }

	/**
	 * @brief Gets the height of the texture.
	 * @return The texture height.
	 */
	int GetHeight() const { return m_height; }

private:
	GLuint m_ID = 0; /* The texture ID. */
	int m_width = 0; /* The width of the texture. */
	int m_height = 0; /* The height of the texture. */
};
#endif //_TEXTURE2D_H