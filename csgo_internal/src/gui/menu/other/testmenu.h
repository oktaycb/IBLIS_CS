#pragma once
#include <string>

class IndicateKeybind
{
public:
	IndicateKeybind(std::string keybindstring, int keymode, bool enablebool)
	{
		this->keybindstr = keybindstring;
		this->keybindmode = keymode;
		this->enabled = enablebool;
	}
	std::string keybindstr;
	int keybindmode;
	float alpha;
	bool enabled;
	int posy;
};

class IndicateSpectator
{
public:
	IndicateSpectator(std::string spectatename, bool enablebool)
	{
		this->spectatestr = spectatename;
		this->enabled = enablebool;
	}
	std::string spectatestr;
	float alpha;
	bool enabled;
	int posy;
};

std::vector <IndicateKeybind> keybinds;
