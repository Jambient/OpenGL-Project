#include "GLUTCallbacks.h"
#include "HelloGL.h"

namespace GLUTCallbacks
{
	namespace
	{
		HelloGL* helloGL = nullptr;
	}

	void Init(HelloGL* gl)
	{
		helloGL = gl;
	}

	void Display()
	{
		if (helloGL != nullptr) {
			helloGL->Display();
		}
	}

	void Timer(int preferredRefresh)
	{
		int updateTime = glutGet(GLUT_ELAPSED_TIME);
		helloGL->Update();
		updateTime = glutGet(GLUT_ELAPSED_TIME) - updateTime;
		glutTimerFunc(preferredRefresh - updateTime, GLUTCallbacks::Timer, preferredRefresh);
	}

	void KeyboardDown(unsigned char key, int x, int y)
	{
		helloGL->KeyboardDown(key, x, y);
	}

	void KeyboardUp(unsigned char key, int x, int y)
	{
		helloGL->KeyboardUp(key, x, y);
	}

	void KeyboardSpecialDown(int key, int x, int y)
	{
		helloGL->KeyboardSpecialDown(key, x, y);
	}

	void KeyboardSpecialUp(int key, int x, int y)
	{
		helloGL->KeyboardSpecialUp(key, x, y);
	}

	void Mouse(int button, int state, int x, int y)
	{
		helloGL->Mouse(button, state, x, y);
	}

	void Motion(int x, int y)
	{
		helloGL->Motion(x, y);
	}

	void BaseMenu(int item)
	{
		helloGL->BaseMenu(item);
	}

	void SceneMenu(int item)
	{
		helloGL->SceneMenu(item);
	}
	void TextureMenu(int item)
	{
		helloGL->TextureMenu(item);
	}
}