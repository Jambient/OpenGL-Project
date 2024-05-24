#include "Scene.h"

Scene::Scene(const char* filePath)
{
	// attempt to load the xml document
	pugi::xml_document doc;
	if (!doc.load_file(filePath))
		std::cerr << "Failed to load scene xml: " << filePath << std::endl;

	pugi::xml_node scene = doc.child("scene");
	pugi::xml_node camera = scene.child("camera");

	// parse intial camera data if it exists.
	if (camera)
	{
		m_cameraPosition = GetVectorFromNode(camera.child("position"), glm::vec3());
		m_cameraRotation = GetVectorFromNode(camera.child("rotation"), glm::vec3());
	}
	
	m_treeRoot = new TreeNode();
	m_treeRoot->name = "Scene";
	BuildSceneTreeRecursively(m_treeRoot, scene.child("objects"));
}

Scene::~Scene()
{
	// iterate the tree and delete the nodes from bottom to top.
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

void Scene::BuildSceneTreeRecursively(TreeNode* parentNode, pugi::xml_node xmlNode)
{
	TreeNode* previousNode = nullptr;
	for (pugi::xml_node object : xmlNode.children())
	{
		// get the node identifier
		std::string objectType = std::string(object.name());

		// make sure to not iterate through object properties
		if (objectType == "properties") { continue; }

		// get the name of the object
		TreeNode* childNode = new TreeNode();
		if (pugi::xml_attribute nameAttribute = object.attribute("name"))
			childNode->name = nameAttribute.value();
		else
			childNode->name = objectType;

		// get object properties
		pugi::xml_node properties = object.child("properties");
		pugi::xml_node transformData = properties.child("transform");
		pugi::xml_node textureData = properties.child("texture");
		pugi::xml_node animationData = properties.child("animation");

		// get the texture for the object
		Texture2D* texture = nullptr;
		if (textureData)
		{
			texture = new Texture2D();
			const char* path = textureData.attribute("path").as_string();
			std::string extension = GetFileExtension(path);
			if (extension == "raw")
				texture->LoadRAW(path);
			if (extension == "tga")
				texture->LoadTGA(path);
			if (extension == "bmp")
				texture->LoadBMP(path);
			if (extension == "png")
				texture->LoadPNG(path);
		}

		// get object transform properties
		glm::vec3 position = GetVectorFromNode(transformData.child("position"), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::vec3 rotation = GetVectorFromNode(transformData.child("rotation"), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::vec3 scale = GetVectorFromNode(transformData.child("scale"), glm::vec3(1.0f, 1.0f, 1.0f));

		// create object
		if (objectType == "obj")
		{
			Mesh* objMesh = MeshLoader::Load(object.attribute("path").value());
			childNode->object = new SceneObject(objMesh, texture, position);
			childNode->object->SetRotation(rotation);
			childNode->object->SetScale(scale);
		}
		else if (objectType == "cube")
		{
			childNode->object = new Cube(texture, position, 0.0f);
			childNode->object->SetRotation(rotation);
			childNode->object->SetScale(scale);
		}
		else
		{
			std::cout << "Unknown Object Type: " << objectType;
			delete texture;
		}

		// handle animation
		if (animationData)
		{
			// add all the defined keyframes
			std::vector<Keyframe> keyframes;
			keyframes.push_back({0, position, rotation, scale});
			for (pugi::xml_node& keyframeNode : animationData.children("keyframe"))
			{
				keyframes.push_back({
					keyframeNode.attribute("time").as_float(),
					GetVectorFromNode(keyframeNode.child("position"), position),
					GetVectorFromNode(keyframeNode.child("rotation"), rotation),
					GetVectorFromNode(keyframeNode.child("scale"), scale)
				});
			}

			// create the Animation object and set the defined properties
			Animation* anim = new Animation(childNode->object, keyframes);
			if (pugi::xml_attribute a = animationData.attribute("easingStyle"))
				anim->SetEasingStyle(a.as_string());
			if (pugi::xml_attribute a = animationData.attribute("easingDirection"))
				anim->SetEasingDirection(a.as_string());
			if (pugi::xml_attribute a = animationData.attribute("shouldReverse"))
				anim->ShouldReverse(a.as_bool());

			// bind the animation to the scene object
			childNode->object->SetAnimation(anim);
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
