#include "../../../precompiled.h"
//
//std::array<RadarLogs, 65> CRadar::m_Logs = {};
//
//void CRadar::Collect()
//{
//	static auto RadarBase = pattern::find(g_Csgo.m_client_dll, XOR("A1 ? ? ? ? 8B 0C B0 8B 01 FF 50 ? 46 3B 35 ? ? ? ? 7C EA 8B 0D")).add(1);
//	static DWORD RadarHud = *(DWORD*)(*(DWORD*)(RadarBase.as<DWORD>()) + 0x54);
//
//	if (!RadarHud)
//		return;
//
//	for (int i = 0; i < g_Csgo.m_Globals->m_MaxClients; i++)
//	{
//		Player* pEntity = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
//		if (!pEntity || !pEntity->IsAlive() || pEntity->IsLocalPlayer() || pEntity->teammate(g_Client.m_LocalPlayer))
//			continue;
//
//		const auto RadarPlayers = *reinterpret_cast<RadarPlayer*>(RadarHud + (0x174 * (i + 1)) - 0x3C);
//		auto& RadarLog = m_Logs[i];
//
//		if (RadarPlayers.bVisible != RadarLog.LastVisibleStatus || RadarPlayers.vecNetworkOrigin != RadarLog.vecNetworkOrigin)
//		{
//			RadarLog.LastVisibleStatus = RadarPlayers.bVisible;
//
//			if (RadarPlayers.bVisible == 3 || RadarPlayers.bVisible == 1) //spotted by me or teammate
//			{
//				//RadarLog.LastSpottedTime = Utils::get_epoch_time();
//				RadarLog.vecNetworkOrigin = RadarPlayers.vecNetworkOrigin;
//			}
//		}
//
//		//RadarLog.IsValidLog = Utils::get_epoch_time() - RadarLog.LastSpottedTime < EXPIRY_TIME;
//	}
//}
