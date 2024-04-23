#include <iostream>
#include <fstream>
#include "Texture2D.h"

using namespace std;

Texture2D::Texture2D(){}

Texture2D::~Texture2D()
{
	glDeleteTextures();
}

bool Texture2D::Load(char* path, int width, int height)
{
	char* tempTextureData; int fileSize; ifstream inFile;
	_width = width; _height = height;
	inFile.open(path, ios::binary);

	if (!inFile.good())
	{
		cerr << "Can't open texture file " << path << endl;
		return false;
	}

	inFile.seekg(0, ios::end);
	fileSize = (int)inFile.tellg();
	tempTextureData = new char[fileSize];
	inFile.seekg(0, ios::beg);
	inFile.read(tempTextureData, fileSize);
	inFile.close();

	cout << path << " loaded." << endl;

	glGenTextures(1, &_ID); // get next texture id
	glBindTexture(GL_TEXTURE_2D, _ID); // bind the texture to the ID
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tempTextureData); // specify details of our texture image

	delete[] tempTextureData;
	return true;
}
