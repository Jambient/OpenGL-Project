#pragma once
#include "pugixml.hpp"
#include "SceneObject.h"
#include "MeshLoader.h"
#include "Texture2D.h"
#include "Cube.h"
#include <iostream>
#include <functional>

enum TraversalType {
	PRE_ORDER = 1,
	IN_ORDER = 2,
	POST_ORDER = 4
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

	void IterateTree(TreeNode* node, int currentDepth, std::function<void(TreeNode*, int, TraversalType)> onNodeCallback, TraversalType traversalType);

	TreeNode* GetRoot() const { return m_treeRoot; }
	glm::vec3 GetCameraPosition() const { return m_cameraPosition; }
	glm::vec3 GetCameraRotation() const { return m_cameraRotation; }

private:
	TreeNode* m_treeRoot;
	glm::vec3 m_cameraPosition = glm::vec3();
	glm::vec3 m_cameraRotation = glm::vec3();

	void BuildSceneTreeRecursively(TreeNode* parentNode, pugi::xml_node xmlNode);
	glm::vec3 GetVectorFromNode(pugi::xml_node node, glm::vec3 defaultValue);
};

