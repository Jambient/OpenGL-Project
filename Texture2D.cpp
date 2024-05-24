#include <fstream>
#include <vector>
#include "Texture2D.h"
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <zlib.h>

using namespace std;

Texture2D::Texture2D()
{
    m_ID = 0;
    m_width = 0;
    m_height = 0;
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_ID);
}

bool Texture2D::LoadRAW(const char* path)
{
    char* tempTextureData;
    int fileSize;
    std::ifstream inFile;

    inFile.open(path, std::ios::binary);

    if (!inFile.good())
    {
        std::cerr << "Can't open texture file " << path << std::endl;
        return false;
    }

    // get the files size
    inFile.seekg(0, std::ios::end);
    fileSize = static_cast<int>(inFile.tellg());
    tempTextureData = new char[fileSize];

    // read in all the file data
    inFile.seekg(0, std::ios::beg);
    inFile.read(tempTextureData, fileSize);
    inFile.close();

    std::cout << path << " loaded." << std::endl;

    // calculate the total amount of pixels in the image.
    const int bytesPerPixel = 3 * sizeof(unsigned char);
    const int totalPixels = fileSize / bytesPerPixel;

    // use that to work out the width and height assuming the image is square.
    m_width = static_cast<int>(std::sqrt(totalPixels));
    m_height = totalPixels / m_width;

    if (m_width != m_height)
    {
        std::cerr << "Texture file is not square " << path << std::endl;
        delete[] tempTextureData;
        return false;
    }

    glGenTextures(1, &m_ID); // get next texture id
    glBindTexture(GL_TEXTURE_2D, m_ID); // bind the texture to the ID
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, tempTextureData);

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
    m_width = ((unsigned char)tempHeaderData[13] << 8u) + (unsigned char)tempHeaderData[12]; // Find the width (Combines two bytes into a short)
    m_height = ((unsigned char)tempHeaderData[15] << 8u) + (unsigned char)tempHeaderData[14]; //Find the height
    pixelDepth = tempHeaderData[16]; // Find the pixel depth (24/32bpp)

    bool flipped = false;
    if ((int)((tempHeaderData[11] << 8) + tempHeaderData[10]) == 0)
        flipped = true;

    //We only support RGB type
    if (type == 2)
    {
        cout << path << " loaded." << endl;

        glGenTextures(1, &m_ID); //Get next Texture ID
        glBindTexture(GL_TEXTURE_2D, m_ID); //Bind the texture to the ID

        mode = pixelDepth / 8;

        //Note that TGA files are stored as BGR(A) - So we need to specify the format as GL_BGR(A)_EXT
        if (mode == 4)
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_width, m_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tempTextureData);
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_width, m_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);

    }

    delete[] tempHeaderData; //We don't need the header memory anymore
    delete[] tempTextureData; //Clear up the data - We don't need this any more

    return true;
}

bool Texture2D::LoadBMP(const char* path)
{
    BITMAPFILEHEADER fileHeaderData = {0};
    BITMAPINFOHEADER infoHeaderData = {0};
    char* tempTextureData;

    ifstream inFile;
    inFile.open(path, ios::binary);
    if (!inFile.good())
    {
        cerr << "Can't open texture file " << path << endl;
        return false;
    }

    // read in the file header and info header
    inFile.seekg(0, ios::beg);
    inFile.read(reinterpret_cast<char*>(&fileHeaderData), sizeof(BITMAPFILEHEADER));
    inFile.read(reinterpret_cast<char*>(&infoHeaderData), sizeof(BITMAPINFOHEADER));

    if (infoHeaderData.biWidth != infoHeaderData.biHeight)
    {
        cerr << "Texture file is not square: " << path << endl;
        return false;
    }

    // read in the image data
    inFile.seekg(fileHeaderData.bfOffBits, ios::beg);
    tempTextureData = new char[infoHeaderData.biSizeImage];
    inFile.read(tempTextureData, infoHeaderData.biSizeImage);
    inFile.close();

    glGenTextures(1, &m_ID); //Get next Texture ID
    glBindTexture(GL_TEXTURE_2D, m_ID); //Bind the texture to the ID
    GLint componentCount = infoHeaderData.biBitCount / 8;
    if (componentCount == 3)
        gluBuild2DMipmaps(GL_TEXTURE_2D, componentCount, infoHeaderData.biWidth, infoHeaderData.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempTextureData);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, componentCount, infoHeaderData.biWidth, infoHeaderData.biHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tempTextureData);

    delete[] tempTextureData;
    return true;
}

char PaethPredictor(char a, char b, char c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);

    if (pa <= pb && pa <= pc) {
        return a;
    }
    else if (pb <= pc) {
        return b;
    }
    else {
        return c;
    }
}

void reverseFilter(FilterType filterType, const std::vector<char>& prevScanline, std::vector<char>& scanline, size_t bytesPerPixel) {
    switch (filterType) {
    case FilterType::None:
        // No operation needed for None filter
        break;
    case FilterType::Sub:
        // Apply Sub filter
        for (size_t i = 0; i < scanline.size(); i++) {
            char left = (i >= bytesPerPixel) ? scanline[i - bytesPerPixel] : 0;
            scanline[i] = static_cast<char>((scanline[i] + left) & 0xff);
        }
        break;
    case FilterType::Up:
        // Apply Up filter
        for (size_t i = 0; i < scanline.size(); i++) {
            char above = prevScanline[i];
            scanline[i] = static_cast<char>((scanline[i] + above) & 0xff);
        }
        break;
    case FilterType::Average:
        // Apply Average filter
        for (size_t i = 0; i < scanline.size(); i++) {
            char left = (i >= bytesPerPixel) ? scanline[i - bytesPerPixel] : 0;
            char above = prevScanline[i];
            char val = static_cast<char>(floor((left + above) / 2));
            scanline[i] = static_cast<char>((scanline[i] + val) & 0xff);
        }
        break;
    case FilterType::Paeth:
        // Apply Paeth filter
        for (size_t i = 0; i < scanline.size(); i++) {
            char left = (i >= bytesPerPixel) ? scanline[i - bytesPerPixel] : 0;
            char above = prevScanline[i];
            char upperLeft = (i >= bytesPerPixel) ? prevScanline[i - bytesPerPixel] : 0;
            char paethPredictor = PaethPredictor(left, above, upperLeft);
            scanline[i] = static_cast<char>((scanline[i] + paethPredictor) & 0xff);
        }
        break;
    default:
        std::cerr << "Unknown filter type: " << static_cast<int>(filterType) << std::endl;
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
        delete chunkData;
        return nullptr;
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

// undo filtering to each scanline of the image data
void undoFilters(std::vector<char>& imageData, size_t width, size_t height, size_t bytesPerPixel) {
    std::vector<char> prevScanline(width * bytesPerPixel, 0); // previous scanline initialized to zeros

    for (size_t y = 0; y < height; ++y) {
        size_t offset = y * (width * bytesPerPixel + 1); // calculate offset considering the filter byte
        FilterType filterType = static_cast<FilterType>(imageData[offset]); // filter type is the first byte of each scanline
        std::vector<char> scanline(imageData.begin() + offset + 1, imageData.begin() + offset + 1 + width * bytesPerPixel);

        reverseFilter(filterType, prevScanline, scanline, bytesPerPixel);

        // update previous scanline for next iteration
        prevScanline = scanline;

        // copy filtered scanline back to image data
        std::copy(scanline.begin(), scanline.end(), imageData.begin() + offset + 1);
    }
}

bool Texture2D::LoadPNG(const char* path) {
    PNGFILEHEADER fileHeaderData = {0};
    PNGINFOHEADER infoHeaderData = {0};
    ChunkData* chunkData;

    std::ifstream inFile;
    inFile.open(path, std::ios::binary);
    if (!inFile.good()) {
        std::cerr << "Can't open texture file " << path << std::endl;
        return false;
    }

    // make sure the file has the correct signature
    inFile.read(reinterpret_cast<char*>(&fileHeaderData), sizeof(PNGFILEHEADER));
    if (!(fileHeaderData.signature[1] == 'P' && fileHeaderData.signature[2] == 'N' && fileHeaderData.signature[3] == 'G')) {
        std::cerr << "Not a valid PNG file: " << path << std::endl;
        return false;
    }

    std::vector<char> textureData;
    bool foundHeaderChunk = false;

    // loop through every chunk in the file and get data
    while ((chunkData = readNextChunk(inFile)) != nullptr) {
        if (chunkData->type == "IHDR") {
            infoHeaderData = *reinterpret_cast<PNGINFOHEADER*>(chunkData->data.data());
            infoHeaderData.width = swapEndian(infoHeaderData.width);
            infoHeaderData.height = swapEndian(infoHeaderData.height);
            foundHeaderChunk = true;
        }
        else if (chunkData->type == "IDAT") {
            textureData.insert(textureData.end(), chunkData->data.begin(), chunkData->data.end());
        }
        delete chunkData;
    }

    if (!foundHeaderChunk)
    {
        std::cerr << "Could not find IHDR chunk: " << path << std::endl;
        return false;
    }

    inFile.close();
    m_width = infoHeaderData.width;
    m_height = infoHeaderData.height;

    size_t bytesPerPixel;
    if (infoHeaderData.colorType == 6) { // Truecolor with alpha
        bytesPerPixel = 4;
    }
    else if (infoHeaderData.colorType == 2) { // Truecolor without alpha
        bytesPerPixel = 3;
    }
    else {
        std::cerr << "Unsupported PNG color type: " << (int)infoHeaderData.colorType << std::endl;
        return false;
    }

    std::vector<char> decompressedData;
    decompressedData.resize(infoHeaderData.width * infoHeaderData.height * bytesPerPixel + infoHeaderData.height);

    // uncompress the data using zlib
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = reinterpret_cast<Bytef*>(textureData.data());
    stream.avail_in = textureData.size();
    stream.next_out = reinterpret_cast<Bytef*>(decompressedData.data());
    stream.avail_out = decompressedData.size();

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Error initializing zlib" << std::endl;
        return false;
    }

    if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
        std::cerr << "Error decompressing PNG" << std::endl;
        inflateEnd(&stream);
        return false;
    }

    inflateEnd(&stream);

    // undo the filters applied to the data
    undoFilters(decompressedData, infoHeaderData.width, infoHeaderData.height, bytesPerPixel);

    // remove the filter type bytes at the start of every scanline
    std::vector<char> imageData;
    imageData.resize(infoHeaderData.width * infoHeaderData.height * bytesPerPixel);
    for (size_t y = 0; y < infoHeaderData.height; ++y) {
        size_t offset = y * (infoHeaderData.width * bytesPerPixel + 1);
        std::copy(decompressedData.begin() + offset + 1, decompressedData.begin() + offset + 1 + infoHeaderData.width * bytesPerPixel, imageData.begin() + y * infoHeaderData.width * bytesPerPixel);
    }

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    GLenum format = (bytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, infoHeaderData.width, infoHeaderData.height, format, GL_UNSIGNED_BYTE, imageData.data());

    return true;
}