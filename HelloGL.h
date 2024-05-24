#pragma once
#ifndef _HELLOGL_H
#define _HELLOGL_H
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/freeglut.h"
#include "GLUTCallbacks.h"
#include <utility>
#include <vector>
#include <string>
#include "Commons.h"
#include "Cube.h"
#include "Camera.h"
#include "Scene.h"

/**
 * @class HelloGL
 * @brief The main class containing implementation for glut callbacks and other general functions.
 */
class HelloGL
{
public:
	/**
	 * @brief Constructs a HelloGL object.
	 * @param argc The amount of arguments passed in main()
	 * @param argv The argument values passed main()
	 */
	HelloGL(int argc, char* argv[]);
	/**
	 * @brief Desconstructs the object.
	 */
	~HelloGL(void);

	/**
	 * @brief Updates the scene
	 */
	void Update();

	/**
	 * @brief Renders the scene
	 */
	void Display();

	/**
	 * @brief Callback function for KeyboardDown event. Triggers when a normal keyboard key is pressed.
	 * @param key The key that was pressed down.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void KeyboardDown(unsigned char key, int x, int y);

	/**
	 * @brief Callback function for KeyboardUp event. Triggers when a normal keyboard key is released.
	 * @param key The key that was released.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void KeyboardUp(unsigned char key, int x, int y);

	/**
	 * @brief Callback function for KeyboardSpecialDown event. Triggers when a special keyboard key is pressed.
	 * @param key The special key that was pressed down.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void KeyboardSpecialDown(int key, int x, int y);

	/**
	 * @brief Callback function for KeyboardSpecialUp event. Triggers when a special keyboard key is released.
	 * @param key The special key that was released.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void KeyboardSpecialUp(int key, int x, int y);

	/**
	 * @brief Callback function for the Mouse event. Triggers when a button on the mouse is pressed or released.
	 * @param button The button on the mouse.
	 * @param state The state of the button. Either up or down.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void Mouse(int button, int state, int x, int y);

	/**
	 * @brief Callback function for the Motion event. Triggers when the mouse is moved while at least one button is pressed.
	 * @param x The x position of the mouse.
	 * @param y The y position of the mouse.
	 */
	void Motion(int x, int y);

	/**
	 * @brief Initialise the base objects. This is the camera, the first scene, and the skybox.
	 */
	void InitObjects();

	/**
	 * @brief Initialise the default lighting.
	 */
	void InitLighting();

	/**
	 * @brief Initialise OpenGL
	 * @param argc The amount of arguments passed in main()
	 * @param argv The argument values passed in main()
	 */
	void InitGL(int argc, char* argv[]);

	/**
	 * @brief Initialise a glut menu.
	 */
	void InitMenu();

	/**
	 * @brief Gets the ray direction at the specified screen position.
	 * @param x The x position on the screen. Can range from 0 to VIEWPORT_WIDTH.
	 * @param y The y position on the screen. Can range from 0 to VIEWPORT_HEIGHT.
	 * @return 
	 */
	glm::vec3 GetRayFromScreenPosition(int x, int y);

	/**
	 * @brief Raycast into the scene from the specified screen position.
	 * @param x The x position on the screen. Can range from 0 to VIEWPORT_WIDTH.
	 * @param y The y position on the screen. Can range from 0 to VIEWPORT_HEIGHT.
	 */
	void Raycast(int x, int y);

	/**
	 * @brief Renders the specified text onto the screen.
	 * @param text The text to render.
	 * @param screenPosition The screen position to render at.
	 * @param color The color of the text.
	 */
	void RenderText(const char* text, const glm::ivec2& screenPosition, const Color& color = {1.0f, 1.0f, 1.0f});

	/**
	 * @brief Gets the closest axis aligned vector to the specified vector via dot product.
	 * @param vec A direction vector.
	 * @return The closest axis aligned vector.
	 */
	glm::vec3 GetClosestAxisAlignedVector(glm::vec3 vec);

	/**
	 * @brief The callback for the base menu.
	 * @param item The selected item.
	 */
	void BaseMenu(int item);
	/**
	 * @brief The callback for the scene menu.
	 * @param item 
	 */
	void SceneMenu(int item);
	/**
	 * @brief The callback for the texture menu.
	 * @param item 
	 */
	void TextureMenu(int item);

private:
	Camera* camera; /* A pointer to the currently used camera. */
	SceneObject* skybox; /* A pointer to the skybox object. */
	TreeNode* selectedObject; /* A pointer to the currently selected object. This is nullptr if no object is selected. */
	std::map<TreeNode*, Texture2D*> originalObjectTexture; /* Maps an object to its original texture. This is used when its texture is changed. */

	glm::ivec2 lockedMousePosition; /* The mouse position on the frame the user started pressing the right mouse button. */

	glm::mat4 projectionMatrix; /* The current projection matrix. */
	glm::mat4 viewMatrix; /* The current view matrix. */

	glm::vec4 lightPosition; /* The position of the light in the scene. */
	Lighting lightData; /* The properties of the light in the scene. */

	Scene* currentScene; /* A pointer to the currently active scene. */
	std::vector<Scene*> scenes; /* A vector of all the loaded scenes. */

	int previousElapsedTime = 0; /* The recorded elapsed time on the previous frame. */
	int fps = 0; /* The current FPS. */

	static std::vector<const char*> sceneNames; /* The name of each scene in the scene paths vector. */
	static std::vector<const char*> scenePaths; /* The file path of each scene. */
	static glm::vec3 axisAlignedVectors[6]; /* The 6 normalised axis aligned direction vectors. */
};
#endif //_HELLOGL_H