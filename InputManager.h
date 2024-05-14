#pragma once

namespace InputManager
{
	void OnKeyboardDown(unsigned char key);
	void OnKeyboardUp(unsigned char key);
    void OnKeyboardSpecialDown(int key);
    void OnKeyboardSpecialUp(int key);
	void OnMouseEvent(int button, int state);

	bool IsKeyDown(int key);
	bool IsSpecialKeyDown(int key);
	bool IsMouseButtonDown(int button);
}