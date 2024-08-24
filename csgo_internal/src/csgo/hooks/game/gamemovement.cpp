#include "../../../precompiled.h"

void Hooks::ProcessMovement(Entity* ent, CMoveData* data)
{
	data->m_bGameCodeMovedPlayer = false;
	return g_Hooks.m_GameMovement.GetOldMethod< ProcessMovement_t >(CGameMovement::PROCESSMOVEMENT)(this, ent, data);
}

void Hooks::OnJump(float stamina)
{
	if (g_Client.m_Processing)
		g_Client.m_LocalPlayer->DoAnimationEvent(PLAYERANIMEVENT_JUMP);
	return g_Hooks.m_GameMovement.GetOldMethod< OnJump_t >(CGameMovement::ONJUMP)(this, stamina);
}

//for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
//	Player* player = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
//	if (!player || !player->IsPlayer() || !player->IsAlive() || player->teammate(g_Client.m_LocalPlayer))
//		continue;

//	// this event means a jump is definitely happening, not just that a jump is desired.
//	player->DoAnimationEvent(PLAYERANIMEVENT_JUMP);
//}

//bool __fastcall C_Hooks::hkTraceFilterForHeadCollision(LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, LPVOID pTraceParams)
//{
//	if (!g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive())
//		return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision(pEcx, pPlayer, pTraceParams);
//
//	if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->EntIndex() - 1 > 63 || pPlayer == g_Globals.m_LocalPlayer)
//		return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision(pEcx, pPlayer, pTraceParams);
//
//	if (fabsf(pPlayer->m_vecOrigin().z - g_Globals.m_LocalPlayer->m_vecOrigin().z) < 10.0f)
//		return false;
//
//	return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision(pEcx, pPlayer, pTraceParams);
//}