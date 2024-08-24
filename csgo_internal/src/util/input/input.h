#pragma once

class Input
{
public:
    bool IsKeyDown(int key);
    bool IsKeyUp(int key);
    bool IsToggled(int key);
    bool Key(int key, int mod);
    void OnWndProc(UINT u_msg, WPARAM key);
private:
    std::array<bool, 256> KeysUp = {};
    std::array<bool, 256> KeysDown = {};
};

inline Input g_KeyInput;