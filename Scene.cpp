#include "Scene.h"

Scene::Scene(const char* filePath)
{
	pugi::xml_document doc;
	if (!doc.load_file(filePath))
		std::cerr << "Failed to load scene xml: " << filePath << std::endl;

	pugi::xml_node scene = doc.child("scene");
	pugi::xml_node camera = scene.child("camera");

	if (camera)
	{
		m_cameraPosition = GetVectorFromNode(camera.child("position"), glm::vec3());
		m_cameraRotation = GetVectorFromNode(camera.child("rotation"), glm::vec3());
	}

	pugi::xml_node objectsRoot = scene.child("objects");
	
	m_treeRoot = new TreeNode();
	m_treeRoot->name = "Scene";
	BuildSceneTreeRecursively(m_treeRoot, objectsRoot);
}

Scene::~Scene()
{
	IterateTree(m_treeRoot, 0, [](TreeNode* node, int depth, TraversalType type) {
		delete node;
		node = nullptr;
	}, POST_ORDER);
}

void Scene::IterateTree(TreeNode* node, int currentDepth, std::function<void(TreeNode*, int, TraversalType)> onNodeCallback, TraversalType traversalType)
{
	if (node == nullptr)
		return;

	if (traversalType & PRE_ORDER)
		onNodeCallback(node, currentDepth, PRE_ORDER);

	// Traverse the left child
	IterateTree(node->child, currentDepth + 1, onNodeCallback, traversalType);

	if (traversalType & IN_ORDER)
		onNodeCallback(node, currentDepth, IN_ORDER);

	// Traverse the right sibling
	IterateTree(node->sibling, currentDepth, onNodeCallback, traversalType);

	if (traversalType & POST_ORDER)
		onNodeCallback(node, currentDepth, POST_ORDER);
}

glm::vec3 GetVectorFromNode(pugi::xml_node node, glm::vec3 defaultValue)
{
	if (!node) { return defaultValue; }

	return glm::vec3(node.attribute("x").as_float(), node.attribute("y").as_float(), node.attribute("z").as_float());
}

void Scene::BuildSceneTreeRecursively(TreeNode* parentNode, pugi::xml_node xmlNode)
{
	TreeNode* previousNode = nullptr;
	for (pugi::xml_node object : xmlNode.children())
	{
		std::string objectType = std::string(object.name());
		if (objectType == "properties") { continue; }

		TreeNode* childNode = new TreeNode();
		if (pugi::xml_attribute nameAttribute = object.attribute("name"))
			childNode->name = nameAttribute.value();
		else
			childNode->name = objectType;

		// create scene object
		pugi::xml_node properties = object.child("properties");
		pugi::xml_node transform = properties.child("transform");
		pugi::xml_node textureProperty = properties.child("texture");

		if (objectType == "obj")
		{
			Mesh* objMesh = MeshLoader::Load(object.attribute("path").value());
			Texture2D* texture = new Texture2D();
			if (textureProperty)
				texture->LoadBMP(textureProperty.attribute("path").value());
			else
				texture->LoadRAW("Textures/penguins.raw", 512, 512);

			if (properties && transform)
			{
				glm::vec3 position = GetVectorFromNode(transform.child("position"), glm::vec3(0.0f, 0.0f, 0.0f));
				glm::vec3 rotation = GetVectorFromNode(transform.child("rotation"), glm::vec3(0.0f, 0.0f, 0.0f));
				glm::vec3 scale = GetVectorFromNode(transform.child("scale"), glm::vec3(1.0f, 1.0f, 1.0f));
				childNode->object = new SceneObject(objMesh, texture, position);
				childNode->object->SetRotation(rotation);
				childNode->object->SetScale(scale);
			}
		}
		else if (objectType == "cube")
		{
			Mesh* cubeMesh = MeshLoader::Load("Models/cube.txt");
			Texture2D* texture = new Texture2D();
			if (textureProperty)
				texture->LoadBMP(textureProperty.attribute("path").value());
			else
				texture->LoadRAW("Textures/penguins.raw", 512, 512);

			if (properties && transform)
			{
				glm::vec3 position = GetVectorFromNode(transform.child("position"), glm::vec3(0.0f, 0.0f, 0.0f));
				glm::vec3 rotation = GetVectorFromNode(transform.child("rotation"), glm::vec3(0.0f, 0.0f, 0.0f));
				glm::vec3 scale = GetVectorFromNode(transform.child("scale"), glm::vec3(1.0f, 1.0f, 1.0f));
				childNode->object = new Cube(cubeMesh, texture, position, 0.0f);
				childNode->object->SetRotation(rotation);
				childNode->object->SetScale(scale);
			}
		}

		BuildSceneTreeRecursively(childNode, object);

		if (previousNode == nullptr)
			parentNode->child = childNode;
		else
			previousNode->sibling = childNode;

		previousNode = childNode;
	}
}

glm::vec3 Scene::GetVectorFromNode(pugi::xml_node node, glm::vec3 defaultValue)
{
	if (!node) { return defaultValue; }
	return glm::vec3(node.attribute("x").as_float(), node.attribute("y").as_float(), node.attribute("z").as_float());
}
