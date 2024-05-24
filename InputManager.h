#pragma once
#ifndef _INPUTMANAGER_H
#define _INPUTMANAGER_H

/**
 * @brief Namespace for various input manager functions.
 */
namespace InputManager
{
	/**
	 * @brief Callback for when a keyboard key is pressed down.
	 * @param key The key that was pressed.
	 */
	void OnKeyboardDown(unsigned char key);

	/**
	 * @brief Callback for when a keyboard key was released.
	 * @param key The key that was released.
	 */
	void OnKeyboardUp(unsigned char key);

    /**
     * @brief Callback for when a special keyboard key is pressed down.
     * @param key 
     */
    void OnKeyboardSpecialDown(int key);

    /**
     * @brief Callback for when a special keyboard key is released.
     * @param key 
     */
    void OnKeyboardSpecialUp(int key);

	/**
	 * @brief Callback for when a mouse button is pressed or released.
	 * @param button The mouse button.
	 * @param state The state of the button. Either Up or Down.
	 */
	void OnMouseEvent(int button, int state);

	/**
	 * @brief Checks if a keyboard key is currently pressed.
	 * @param key The key to check.
	 * @return True if the key is pressed, false otherwise.
	 */
	bool IsKeyDown(int key);

	/**
	 * @brief Checks if a special keyboard key is currently pressed.
	 * @param key The key to check.
	 * @return True if the key is pressed, false otherwise.
	 */
	bool IsSpecialKeyDown(int key);

	/**
	 * @brief Checks if a mouse button is currently pressed.
	 * @param button The button to check.
	 * @return True if the button is pressed, false otherwise.
	 */
	bool IsMouseButtonDown(int button);
}
#endif //_INPUTMANAGER_H