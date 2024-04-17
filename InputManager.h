#pragma once

namespace InputManager
{
	extern bool keys[256];
	extern bool mouse[3];

	void OnKeyboardDown(unsigned char key);
	void OnKeyboardUp(unsigned char key);

	bool IsKeyDown(int key);
}