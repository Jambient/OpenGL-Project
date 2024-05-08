#pragma once
#include "pugixml.hpp"
#include "SceneObject.h"
#include "MeshLoader.h"
#include "Texture2D.h"
#include "Cube.h"
#include <iostream>
#include <functional>

enum TraversalType {
	PRE_ORDER,
	IN_ORDER,
	POST_ORDER
};

struct TreeNode
{
	SceneObject* object = nullptr;
	TreeNode* child = nullptr;
	TreeNode* sibling = nullptr;
	std::string name;
};

class Scene
{
public:
	Scene(const char* filePath);
	~Scene();

	void IterateTree(TreeNode* node, int currentDepth, std::function<void(TreeNode*, int)> onNodeCallback, TraversalType traversalType);

	TreeNode* GetRoot() const { return treeRoot; }
private:
	TreeNode* treeRoot;

	void BuildSceneTreeRecursively(TreeNode* parentNode, pugi::xml_node xmlNode);

};

