
#pragma once
#include "../../../precompiled.h"
#include "../../../util/singleton.h"

class keyhandler : public singleton<keyhandler>
{
public:
	bool is_key_down(int key);
	bool is_key_up(int key);
	bool is_key_pressed(int key);
	bool auto_check(std::string key);
private:
	bool keys[256] = { false };
	bool pressed[256] = { false };
	bool toggles[256] = { false };
};
