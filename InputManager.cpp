#include "InputManager.h"

namespace InputManager
{
    bool keys[256];
    bool mouse[3];

    void OnKeyboardDown(unsigned char key)
    {
        keys[key] = true;
    }

    void OnKeyboardUp(unsigned char key)
    {
        keys[key] = false;
    }

    bool IsKeyDown(int key)
    {
        return keys[key];
    }
}