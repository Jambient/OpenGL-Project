#pragma once

#include "SceneObject.h"

class TreeNode
{
	SceneObject object;
	std::vector<TreeNode> children;
};