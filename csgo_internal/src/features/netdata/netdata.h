#pragma once

class NetData {
private:
	std::array < CNetvarData, MULTIPLAYER_BACKUP > m_aCompressData = { };

public:
	void Store(Player* pPlayer, int nCommand);
	void Apply(Player* pPlayer, int nCommand);
	void Reset();
};

inline NetData g_Netdata;