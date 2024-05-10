#include "InputManager.h"

namespace InputManager
{
    bool keys[256];
    bool specialKeys[246];

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

    bool IsKeyDown(int key)
    {
        return keys[key];
    }

    bool IsSpecialKeyDown(int key)
    {
        return specialKeys[key];
    }
}