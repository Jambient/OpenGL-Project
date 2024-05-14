#pragma once
class HelloGL;

namespace GLUTCallbacks
{
	void Init(HelloGL* gl);

	void Display();
	void Timer(int preferredRefresh);
	void KeyboardDown(unsigned char key, int x, int y);
	void KeyboardUp(unsigned char key, int x, int y);
	void KeyboardSpecialDown(int key, int x, int y);
	void KeyboardSpecialUp(int key, int x, int y);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void BaseMenu(int item);
	void SceneMenu(int item);
}