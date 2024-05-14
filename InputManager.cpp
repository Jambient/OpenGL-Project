#include "InputManager.h"

namespace InputManager
{
    bool keys[256];
    bool specialKeys[246];
    bool mouse[3];

    void OnKeyboardDown(unsigned char key)
    {
        keys[key] = true;
    }

    void OnKeyboardUp(unsigned char key)
    {
        keys[key] = false;
    }

    void OnKeyboardSpecialDown(int key)
    {
        specialKeys[key] = true;
    }

    void OnKeyboardSpecialUp(int key)
    {
        specialKeys[key] = false;
    }

    void OnMouseEvent(int button, int state)
    {
        mouse[button] = !state;
    }

    bool IsKeyDown(int key)
    {
        return keys[key];
    }

    bool IsSpecialKeyDown(int key)
    {
        return specialKeys[key];
    }

    bool IsMouseButtonDown(int button)
    {
        return mouse[button];
    }
}