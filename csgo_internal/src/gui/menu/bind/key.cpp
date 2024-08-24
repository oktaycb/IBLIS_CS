#include "../../../precompiled.h"

bool keyhandler::is_key_down(int key) {
	return HIWORD(GetKeyState(key));
}

bool keyhandler::is_key_up(int key) {
	return !HIWORD(GetKeyState(key));
}

bool keyhandler::is_key_pressed(int key) {
	return false;
}

bool keyhandler::auto_check(std::string key) {
	switch (c_config::get()->i[key + "_style"]) {
	case 0:
		return true;
	case 1:
		return this->is_key_down(c_config::get()->i[key]);
	case 2:
		return LOWORD(GetKeyState(c_config::get()->i[key]));
	case 3:
		return this->is_key_up(c_config::get()->i[key]);
	default:
		return true;
	}
}