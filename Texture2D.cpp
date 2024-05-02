#include <iostream>
#include <fstream>
#include <vector>
#include "Texture2D.h"

using namespace std;

Texture2D::Texture2D(){}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &_ID);
}

bool Texture2D::LoadRAW(char* path, int width, int height)
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
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tempTextureData); // specify details of our texture image
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, tempTextureData);

	delete[] tempTextureData;
	return true;
}

bool Texture2D::LoadTGA(char* path)
{
    char* tempHeaderData = new char[18]; //18 Bytes is TGA Header Size
    char* tempTextureData;
    int fileSize;
    char type, pixelDepth, mode;

    ifstream inFile;

    inFile.open(path, ios::binary);
    if (!inFile.good())
    {
        cerr << "Can't open texture file " << path << endl;
        return false;
    }

    //18 Bytes is the size of a TGA Header
    inFile.seekg(0, ios::beg); //Seek back to beginning of file
    inFile.read(tempHeaderData, 18); //Read in all the data in one go

    inFile.seekg(0, ios::end); //Seek to end of file
    fileSize = (int)inFile.tellg() - 18; //Get current position in file - The End, this gives us total file size
    tempTextureData = new char[fileSize]; //Create an new aray to store data
    inFile.seekg(18, ios::beg); //Seek back to beginning of file + 18
    inFile.read(tempTextureData, fileSize); //Read in all the data in one go
    inFile.close(); //Close the file

    type = tempHeaderData[2]; //Get TGA Type out of Header - Must be RGB for this to work
    _width = ((unsigned char)tempHeaderData[13] << 8u) + (unsigned char)tempHeaderData[12]; // Find the width (Combines two bytes into a short)
    _height = ((unsigned char)tempHeaderData[15] << 8u) + (unsigned char)tempHeaderData[14]; //Find the height
    pixelDepth = tempHeaderData[16]; // Find the pixel depth (24/32bpp)

    bool flipped = false;
    if ((int)((tempHeaderData[11] << 8) + tempHeaderData[10]) == 0)
        flipped = true;

    //We only support RGB type
    if (type == 2)
    {
        cout << path << " loaded." << endl;

        glGenTextures(1, &_ID); //Get next Texture ID
        glBindTexture(GL_TEXTURE_2D, _ID); //Bind the texture to the ID

        mode = pixelDepth / 8;

        //Note that TGA files are stored as BGR(A) - So we need to specify the format as GL_BGR(A)_EXT
        if (mode == 4)
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tempTextureData);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _width, _height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tempTextureData);
        else
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _width, _height, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);

    }

    delete[] tempHeaderData; //We don't need the header memory anymore
    delete[] tempTextureData; //Clear up the data - We don't need this any more

    return true;
}

bool Texture2D::LoadBMP(char* path)
{
    BITMAPFILEHEADER fileHeaderData;
    BITMAPINFOHEADER infoHeaderData;
    char* tempTextureData;

    ifstream inFile;
    inFile.open(path, ios::binary);
    if (!inFile.good())
    {
        cerr << "Can't open texture file " << path << endl;
        return false;
    }

    inFile.seekg(0, ios::beg);
    inFile.read(reinterpret_cast<char*>(&fileHeaderData), sizeof(BITMAPFILEHEADER));
    inFile.read(reinterpret_cast<char*>(&infoHeaderData), sizeof(BITMAPINFOHEADER));

    if (infoHeaderData.biWidth != infoHeaderData.biHeight)
    {
        cerr << "Texture file is not square: " << path << endl;
        return false;
    }

    inFile.seekg(fileHeaderData.bfOffBits, ios::beg);
    tempTextureData = new char[infoHeaderData.biSizeImage];
    inFile.read(tempTextureData, infoHeaderData.biSizeImage);
    inFile.close();

    glGenTextures(1, &_ID); //Get next Texture ID
    glBindTexture(GL_TEXTURE_2D, _ID); //Bind the texture to the ID
    gluBuild2DMipmaps(GL_TEXTURE_2D, infoHeaderData.biBitCount / 8, infoHeaderData.biWidth, infoHeaderData.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);

    delete[] tempTextureData;

    return true;
}

uint32_t swapEndian(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x000000FF) << 24);
}

ChunkData* readNextChunk(std::ifstream& inFile) {
    ChunkData* chunkData = new ChunkData;

    // Read chunk length
    inFile.read(reinterpret_cast<char*>(&chunkData->length), sizeof(chunkData->length));
    if (inFile.eof()) { // Check if end of file is reached
        delete chunkData; // Clean up allocated memory
        return nullptr; // Return nullptr indicating end of file
    }
    chunkData->length = swapEndian(chunkData->length);

    // Read chunk type
    char buffer[5];
    inFile.read(buffer, 4);
    buffer[4] = '\0';
    chunkData->type = buffer;

    // Read chunk data
    chunkData->data.resize(chunkData->length);
    inFile.read(chunkData->data.data(), chunkData->length);

    // Seek past CRC
    inFile.seekg(4, ios::cur);

    return chunkData;
}

bool Texture2D::LoadPNG(char* path)
{
    PNGFILEHEADER fileHeaderData;
    PNGINFOHEADER infoHeaderData;
    ChunkData* chunkData;

    ifstream inFile;
    inFile.open(path, ios::binary);
    if (!inFile.good())
    {
        cerr << "Can't open texture file " << path << endl;
        return false;
    }

    inFile.read(reinterpret_cast<char*>(&fileHeaderData), sizeof(PNGFILEHEADER));
    if (!(fileHeaderData.signature[1] == 'P' && fileHeaderData.signature[2] == 'N' && fileHeaderData.signature[3] == 'G')) {
        cerr << "Not a valid PNG file: " << path << endl;
        return false;
    }

    std::vector<char>* textureData = new std::vector<char>();

    chunkData = readNextChunk(inFile);
    cout << chunkData->length << " : " << chunkData->type << endl;
    infoHeaderData = *reinterpret_cast<PNGINFOHEADER*>(chunkData->data.data());
    infoHeaderData.width = swapEndian(infoHeaderData.width);
    infoHeaderData.height = swapEndian(infoHeaderData.height);

    while (chunkData != nullptr) {
        cout << chunkData->length << " : " << chunkData->type << endl;
        if (chunkData->type == "IDAT")
        {
            //textureData->insert(textureData->end(), chunkData->data.begin(), chunkData->data.end());
            for (int i = 0; i < 20; i++) {
                cout << "Byte " << i << ": 0x" << hex << (int)(unsigned char)chunkData->data[i] << endl << dec;
            }
        }

        chunkData = readNextChunk(inFile);
    }

    inFile.close();

    cout << "DATA: " << textureData->size() << endl;
    cout << infoHeaderData.width << " : " << infoHeaderData.height << endl;
    cout << (int)infoHeaderData.bitDepth << " : " << (int)infoHeaderData.colorType << endl;

    /*glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoHeaderData.width, infoHeaderData.height, GL_RGB, GL_UNSIGNED_BYTE, textureData->data());*/

    return true;
}
