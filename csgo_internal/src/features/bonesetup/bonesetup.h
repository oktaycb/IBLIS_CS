#pragma once

class Bones {
public:
	bool SetupBones(Player* pPlayer, LagRecord* pRecord);
	bool BuildBones(Player* pPlayer, LagRecord* pRecord, int iBoneMask);
};

inline Bones g_Bones;