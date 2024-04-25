#pragma once
#include "Structures.h"

namespace MeshLoader
{
	Mesh* LoadTXT(char* path);
	Mesh* LoadOBJ(char* path);
};