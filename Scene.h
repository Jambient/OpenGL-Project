#pragma once
#ifndef _SCENE_H
#define _SCENE_H
#include "pugixml.hpp"
#include "SceneObject.h"
#include "MeshLoader.h"
#include "Texture2D.h"
#include "Cube.h"
#include <iostream>
#include <functional>

/**
 * @enum TraversalType
 * @brief The traversal type when iterating the tree.
 */
enum TraversalType {
	PRE_ORDER = 1,
	IN_ORDER = 2,
	POST_ORDER = 4
};

/**
 * @struct TreeNode
 * @brief Represents a node in the scene tree.
 */
struct TreeNode
{
	SceneObject* object = nullptr;
	TreeNode* child = nullptr;
	TreeNode* sibling = nullptr;
	std::string name;
};

/**
 * @class Scene
 * @brief A class representing a scene loaded from file.
 */
class Scene
{
public:
	/**
	 * @brief Constructs a Scene object.
	 * @param filePath The path to the scene XML file.
	 */
	Scene(const char* filePath);
	/**
	 * @brief Deconstructs the object.
	 */
	~Scene();

	/**
	 * @brief Iterates over the scene tree.
	 * @param node The node to start from.
	 * @param currentDepth The depth amount to start from when counting depth.
	 * @param onNodeCallback The callback function for when a node is reached in the traversal.
	                         Parameters are the current node, current depth, and traversal type used when the node was reached.
	 * @param traversalType The type of traversal to use when iterating. Can use multiple traversal types by ORing the enums.
	 */
	void IterateTree(TreeNode* node, int currentDepth, std::function<void(TreeNode*, int, TraversalType)> onNodeCallback, TraversalType traversalType);

	/**
	 * @brief Gets the root node for the scene tree.
	 * @return The root node of the scene tree.
	 */
	TreeNode* GetRoot() const { return m_treeRoot; }

	/**
	 * @brief Gets the initial camera position specified in the scene data.
	 * @return The intial camera position for the scene.
	 */
	glm::vec3 GetCameraPosition() const { return m_cameraPosition; }

	/**
	 * @brief Gets the initial camera rotation specified in the scene data.
	 * @return The initial camera rotation for the scene.
	 */
	glm::vec3 GetCameraRotation() const { return m_cameraRotation; }

private:
	/**
	 * @brief Recursively builds the scene tree from the data.
	 * @param parentNode The current parent node.
	 * @param xmlNode The current data node.
	 */
	void BuildSceneTreeRecursively(TreeNode* parentNode, pugi::xml_node xmlNode);

	/**
	 * @brief Gets vector data from an XML node represented by 3 attributes: x, y, and z.
	 * @param node The node to get the data form.
	 * @param defaultValue The default value if the xml node is not valid.
	 * @return The vector data if the node was valid, otherwise the default value parameter.
	 */
	glm::vec3 GetVectorFromNode(pugi::xml_node node, glm::vec3 defaultValue);

	TreeNode* m_treeRoot; /* The root of the scene tree. */
	glm::vec3 m_cameraPosition = glm::vec3(); /* The initial camera position for the scene. */
	glm::vec3 m_cameraRotation = glm::vec3(); /* The initial camera rotation for the scene. */
};
#endif //_SCENE_H