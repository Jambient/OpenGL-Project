#pragma once
#ifndef _MESHLOADER_H
#define _MESHLOADER_H
#include "Commons.h"

/**
 * @brief The namespace for mesh loading functions.
 */
namespace MeshLoader
{
	/**
	 * @brief Loads a mesh from the specified path.
	 * @param path The file path to the mesh data.
	 * @return A pointer to the loaded mesh if successful, otherwise nullptr if the file could not load.
	 */
	Mesh* Load(const char* path);
};
#endif //_MESHLOADER_H