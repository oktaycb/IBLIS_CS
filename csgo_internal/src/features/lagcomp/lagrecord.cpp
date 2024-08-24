#include "../../precompiled.h"

void LagRecord::Store(Player* pPlayer, bool allocate) {
	m_Player = pPlayer;
	m_Immune = pPlayer->m_fImmuneToGunGameDamageTime();
	m_Tick = g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick;

	m_SimulationTime = pPlayer->m_flSimulationTime();
	m_OldSimulationTime = pPlayer->m_flOldSimulationTime();
	m_Flags = pPlayer->m_fFlags();
	m_Origin = pPlayer->m_vecOrigin();
	m_OldOrigin = pPlayer->m_vecOldOrigin();
	m_Velocity = pPlayer->m_vecVelocity();
	m_EyeAngles = pPlayer->m_angEyeAngles();
	m_AbsAngles = pPlayer->GetAbsAngles();
	m_LowerbodyYaw = pPlayer->m_flLowerBodyYawTarget();
	m_FootYaw = pPlayer->m_PlayerAnimStateCSGO() ? pPlayer->m_PlayerAnimStateCSGO()->flFootYaw : 0.f;
	m_OBBMins = pPlayer->GetCollideable()->OBBMins();
	m_OBBMaxs = pPlayer->GetCollideable()->OBBMaxs();
	m_DuckAmount = pPlayer->m_flDuckAmount();
	m_ChokedCommands = game::TIME_TO_TICKS(m_SimulationTime - m_OldSimulationTime);
	m_MoveState = pPlayer->m_iMoveState();
	m_MoveType = pPlayer->m_MoveType();

	pPlayer->GetAnimLayers(m_Layers);

	if (!allocate)
		pPlayer->GetPoseParameters(m_Poses);

	m_CachedBoneCount = pPlayer->m_BoneCache().m_CachedBoneCount;
	m_CachedBones = allocate ? (matrix3x4a_t*)g_Csgo.m_MemAlloc->Alloc(sizeof(matrix3x4a_t) * 128) : pPlayer->m_BoneCache().m_pCachedBones;
}

bool LagRecord::IsValidTime(int iShiftAmount)
{
	int32_t nDeadTime = game::TICKS_TO_TIME(g_Csgo.m_Globals->m_TickCount + game::TIME_TO_TICKS(g_Network.GetLatency()) - iShiftAmount) - 0.2f;
	if (game::TIME_TO_TICKS(m_SimulationTime + g_Client.m_Lerp) < nDeadTime)
		return false;

	const float_t flDeltaTime = fmin(g_Network.GetLatency() + g_Client.m_Lerp, g_Csgo.sv_maxunlag->GetFloat()) - (game::TICKS_TO_TIME(g_Client.m_LocalPlayer->m_nTickBase() - iShiftAmount) - m_SimulationTime);
	return fabs(flDeltaTime) <= 0.2f;
}