#include <fstream>
#include <vector>
#include "Texture2D.h"
#include <stdio.h>
#include <algorithm>
#include "inflatecpp/decompressor.h"

using namespace std;

Texture2D::Texture2D(){}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &_ID);
}

bool Texture2D::LoadRAW(const char* path, int width, int height)
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
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, tempTextureData);

	delete[] tempTextureData;
	return true;
}

bool Texture2D::LoadTGA(const char* path)
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

bool Texture2D::LoadBMP(const char* path)
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
    GLint componentCount = infoHeaderData.biBitCount / 8;
    if (componentCount == 3)
        gluBuild2DMipmaps(GL_TEXTURE_2D, componentCount, infoHeaderData.biWidth, infoHeaderData.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, componentCount, infoHeaderData.biWidth, infoHeaderData.biHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tempTextureData);

    delete[] tempTextureData;

    return true;
}

void applyFilter(FilterType type, std::vector<char>& prevScanline, std::vector<char>& scanline, size_t bytesPerPixel) {
    switch (type) {
    case FilterType::None:
        // No filtering, do nothing
        break;
    case FilterType::Sub:
        // Sub filter: subtract each byte from the corresponding byte of the previous pixel
        for (size_t i = bytesPerPixel; i < scanline.size(); ++i) {
            scanline[i] -= scanline[i - bytesPerPixel];
        }
        break;
    case FilterType::Up:
        // Up filter: subtract each byte from the corresponding byte of the previous scanline
        for (size_t i = 0; i < scanline.size(); ++i) {
            scanline[i] -= prevScanline[i];
        }
        break;
    case FilterType::Average:
        // Average filter: subtract each byte from the average of the corresponding bytes of the previous pixel and scanline
        for (size_t i = 0; i < scanline.size(); ++i) {
            char prevByte = (i < bytesPerPixel) ? 0 : scanline[i - bytesPerPixel];
            char prevScanByte = prevScanline[i];
            char aboveByte = (prevByte + prevScanByte) / 2;
            scanline[i] -= aboveByte;
        }
        break;
    case FilterType::Paeth:
        // Paeth filter: predict byte based on previous pixel, previous scanline, and previous pixel of previous scanline
        for (size_t i = 0; i < scanline.size(); ++i) {
            char a = (i < bytesPerPixel) ? 0 : scanline[i - bytesPerPixel];
            char b = prevScanline[i];
            char c = (i < bytesPerPixel) ? 0 : prevScanline[i - bytesPerPixel];
            char p = a + b - c;
            char pa = std::abs(p - a);
            char pb = std::abs(p - b);
            char pc = std::abs(p - c);
            char predicted = (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
            scanline[i] -= predicted;
        }
        break;
    }
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

// Apply filtering to each scanline of the image data
void applyFilters(std::vector<char>& imageData, size_t width, size_t height, size_t bytesPerPixel) {
    std::vector<char> prevScanline(width * bytesPerPixel, 0); // Previous scanline initialized to zeros

    for (size_t y = 0; y < height; ++y) {
        size_t offset = y * width * bytesPerPixel;
        FilterType filterType = static_cast<FilterType>(imageData[offset]); // Filter type is the first byte of each scanline
        std::vector<char> scanline(imageData.begin() + offset + 1, imageData.begin() + offset + 1 + width * bytesPerPixel);
        applyFilter(filterType, prevScanline, scanline, bytesPerPixel);

        // Update previous scanline for next iteration
        prevScanline = scanline;

        // Copy filtered scanline back to image data
        std::copy(scanline.begin(), scanline.end(), imageData.begin() + offset + 1);
    }
}

bool Texture2D::LoadPNG(const char* path)
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

    std::vector<char> textureData = std::vector<char>();

    chunkData = readNextChunk(inFile);
    cout << chunkData->length << " : " << chunkData->type << endl;
    infoHeaderData = *reinterpret_cast<PNGINFOHEADER*>(chunkData->data.data());
    infoHeaderData.width = swapEndian(infoHeaderData.width);
    infoHeaderData.height = swapEndian(infoHeaderData.height);

    while (chunkData != nullptr) {
        cout << chunkData->length << " : " << chunkData->type << endl;
        if (chunkData->type == "IDAT")
        {

            //textureData->insert(textureData->end(), &decompressed_data[0], &decompressed_data[decompressed_data_size]);
            textureData.insert(textureData.end(), chunkData->data.begin(), chunkData->data.end());
        }

        chunkData = readNextChunk(inFile);
    }

    inFile.close();

    size_t bytesPerPixel = 1;
    switch (infoHeaderData.colorType) {
    case 0: // Grayscale
        bytesPerPixel = (infoHeaderData.bitDepth == 16) ? 2 : 1;
        break;
    case 2: // RGB
        bytesPerPixel = (infoHeaderData.bitDepth * 3 + 7) / 8;
        break;
    case 3: // Palette
        bytesPerPixel = 1;
        break;
    case 4: // Grayscale with alpha
        bytesPerPixel = (infoHeaderData.bitDepth * 2 + 7) / 8;
        break;
    case 6: // RGBA
        bytesPerPixel = (infoHeaderData.bitDepth * 4 + 7) / 8;
        break;
    default:
        // Handle unsupported color types
        break;
    }

    Decompressor data_decompressor = Decompressor();
    unsigned int compressed_data_size = textureData.size();
    unsigned int max_decompressed_data_size = 300000000;
    unsigned char* compressed_data = new unsigned char[compressed_data_size];
    unsigned char* decompressed_data = new unsigned char[max_decompressed_data_size];

    std::copy(textureData.begin(), textureData.end(), compressed_data);

    unsigned int decompressed_data_size = data_decompressor.Feed(compressed_data, compressed_data_size, decompressed_data, max_decompressed_data_size, true);

    if (decompressed_data_size == -1)
    {
        cout << "decompression error!" << endl;
        delete[] compressed_data;
        delete[] decompressed_data;

        return false;
    }

    cout << "decompressed " << decompressed_data_size << " bytes" << endl;

    std::vector<char>* uncompressedData = new std::vector<char>();
    uncompressedData->insert(uncompressedData->end(), &decompressed_data[0], &decompressed_data[decompressed_data_size]);

    applyFilters(*uncompressedData, infoHeaderData.width, infoHeaderData.height, bytesPerPixel);

    cout << infoHeaderData.width << " : " << infoHeaderData.height << endl;
    cout << (int)infoHeaderData.bitDepth << " : " << (int)infoHeaderData.colorType << endl;

    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoHeaderData.width, infoHeaderData.height, GL_RGBA, GL_UNSIGNED_BYTE, uncompressedData->data());

    delete[] compressed_data;
    delete[] decompressed_data;
    //delete[] uncompressedData;

    return true;
}
