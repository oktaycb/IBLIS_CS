#pragma once

class CFakeLag {
public:
	void Init(bool bInitialized = true);
public:
	int m_ChokedCommands = 1;
};

inline CFakeLag g_FakeLag;