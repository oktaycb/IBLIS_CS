#include "../../precompiled.h"

bool Input::IsKeyDown(int key)
{
    return KeysDown.at(key);
}

bool Input::IsKeyUp(int key)
{
    return KeysUp.at(key);
} 

bool Input::IsToggled(int key)
{
    //note@ : i simply don't want to switch static values for this. loword keystate works fine and it doesn't spam on hold, which is a good thing.
    //could have used hiword for keydown/keyup (holding) with the keystate but i want to see if the users have issues with toggle but (holding) keydown/keyup.
    //if this goes okay, we will probably use this instead of wndproc or csgo's retarded input.
    return LOWORD(GetKeyState(key)); 

    //todo : test this later: (can't test rn since we don't even have a menu )
    //return KeysDown.at(LOWORD(key));
}

bool Input::Key(int key, int mod) {

    switch (g_Config.i(mod)) {
    case 0:
        return true;
    case 1:
        return IsToggled(g_Config.i(key));
    case 2:
        return IsKeyDown(g_Config.i(key));
    case 3:
        return IsKeyUp(g_Config.i(key));
    default:
        return true;
    }
}

void Input::OnWndProc(UINT Msg, WPARAM Key)
{
    switch (Msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        KeysDown.at(Key) = true;
        break;
    case WM_LBUTTONDOWN:
        KeysDown.at(VK_LBUTTON) = true;
        break;

    case WM_LBUTTONUP:
        KeysDown.at(VK_LBUTTON) = false;
        KeysUp.at(VK_LBUTTON) = true;
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        KeysDown.at(Key) = false;
        KeysUp.at(Key) = true;
        break;

    case WM_XBUTTONDOWN:
        KeysDown.at(HIWORD(Key) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2) = true;
        KeysUp.at(HIWORD(Key) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2) = false;
        break;

    case WM_XBUTTONUP:
        KeysDown.at(HIWORD(Key) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2) = false;
        KeysUp.at(HIWORD(Key) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2) = true;
        break;
    }

    for (auto i = 0u; i < KeysDown.size(); i++)
    { 
        if (KeysDown.at(i))
            KeysUp.at(i) = false;
    }
}