#include "../../../precompiled.h"

bool Hooks::InPrediction() {
	Stack stack;
	QAngle* angles;

	static Address CalcPlayerView_ret1{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C")) };
	static Address CalcPlayerView_ret2{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06")) };
	static Address MaintainSequenceTransitionsAddress{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 74 17 8B 87")) };
	static Address SetupBonesTiming{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05")) };

	if (stack.ReturnAddress() == MaintainSequenceTransitionsAddress)
		return true;

	if (stack.ReturnAddress() == SetupBonesTiming)
		return false;

	if (!g_Client.m_Processing)
		return g_Hooks.m_Prediction.GetOldMethod< InPrediction_t >(CPrediction::INPREDICTION)(this);

	if (stack.ReturnAddress() == CalcPlayerView_ret1 && g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_PUNCH])
		return true;

	if (stack.ReturnAddress() == CalcPlayerView_ret2 && g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_PUNCH]) {
		angles = reinterpret_cast<QAngle*>(*(reinterpret_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1));

		if (angles) {
			*angles -= g_Client.m_LocalPlayer->m_viewPunchAngle()
				+ (g_Client.m_LocalPlayer->m_aimPunchAngle() * g_Csgo.weapon_recoil_scale->GetFloat())
				* g_Csgo.view_recoil_tracking->GetFloat();
		}

		return true;
	}

	return g_Hooks.m_Prediction.GetOldMethod< InPrediction_t >(CPrediction::INPREDICTION)(this);
}

int32_t GetTickRate()
{
	return (int32_t)(1.0f / g_Csgo.m_Globals->m_Interval);
}

void PerformRevolverTime(Player* pPlayer, Command* pCmd)
{
	static float m_TickRecord[MULTIPLAYER_BACKUP];
	static bool m_InAttack[MULTIPLAYER_BACKUP];
	static bool m_CanFire[MULTIPLAYER_BACKUP];

	m_TickRecord[pCmd->m_CommandNumber % MULTIPLAYER_BACKUP] = pPlayer->m_nTickBase();
	m_InAttack[pCmd->m_CommandNumber % MULTIPLAYER_BACKUP] = pCmd->m_Buttons & IN_ATTACK || pCmd->m_Buttons & IN_ATTACK2;
	m_CanFire[pCmd->m_CommandNumber % MULTIPLAYER_BACKUP] = g_Client.CanFireWeapon(0, false);

	auto pCombatWeapon = pPlayer->GetActiveWeapon();
	if (!pCombatWeapon)
		return;

	float flPostponeFireReadyTime = FLT_MAX;
	if (pCombatWeapon->m_iItemDefinitionIndex() == REVOLVER)
	{
		int nCommand = pCmd->m_CommandNumber;
		int nLowestCommand = nCommand - (GetTickRate() >> 1);
		int nCheckCommand = nCommand - 1;
		int nFireCommand = 0;

		while (nCheckCommand > nLowestCommand)
		{
			nFireCommand = nCheckCommand;

			if (!m_InAttack[nCheckCommand % MULTIPLAYER_BACKUP] || !m_CanFire[nCheckCommand % MULTIPLAYER_BACKUP])
				break;

			nCheckCommand--;
		}

		if (nFireCommand)
		{
			int nOffset = 1 - (-0.03348f / g_Csgo.m_Globals->m_Interval);
			if (nCommand - nFireCommand >= nOffset)
				flPostponeFireReadyTime = game::TICKS_TO_TIME(m_TickRecord[(nFireCommand + nOffset) % MULTIPLAYER_BACKUP]) + 0.2f;
		}
	}

	pCombatWeapon->m_flPostponeFireReadyTime() = flPostponeFireReadyTime;
}

void Hooks::RunCommand(Entity* pEntity, Command* pCommands, IMoveHelper* pMoveHelper) {
	Player* pPlayer = reinterpret_cast<Player*>(pEntity);
	if (!pPlayer || pPlayer->GetClientNetworkable()->GetIndex() != g_Csgo.m_Engine->GetLocalPlayer())
		return g_Hooks.m_Prediction.GetOldMethod< RunCommand_t >(CPrediction::RUNCOMMAND)(this, pEntity, pCommands, pMoveHelper);

	if (pCommands->m_TickCount >= (g_Csgo.m_Globals->m_TickCount + std::round(1 / g_Csgo.m_Globals->m_Interval) + 8)) {
		pCommands->m_Predicted = true;

		if (!g_Csgo.m_Prediction->m_EnginePaused && g_Csgo.m_Globals->m_FrameTime > 0)
			++pPlayer->m_nTickBase();

		return pPlayer->SetOrigin(pPlayer->m_vecOrigin());
	}

	PerformRevolverTime(pPlayer, pCommands);

	if (g_Hooks.m_Prediction.GetOldMethod< RunCommand_t >(CPrediction::RUNCOMMAND))
		g_Hooks.m_Prediction.GetOldMethod< RunCommand_t >(CPrediction::RUNCOMMAND)(this, pEntity, pCommands, pMoveHelper);

	g_InputPrediction.UpdateViewmodelData();
}

// TODO@Monster: Add server stuff, remove client.
void Hooks::FinishMove(Entity* pEntity, Command* pCommands, CMoveData* pMoveHelper)
{
	if (g_Hooks.m_Prediction.GetOldMethod< FinishMove_t >(CPrediction::FINISHMOVE))
		g_Hooks.m_Prediction.GetOldMethod< FinishMove_t >(CPrediction::FINISHMOVE)(this, pEntity, pCommands, pMoveHelper);

	float flPitch = pMoveHelper->m_vecAngles[PITCH];
	if (flPitch > 180.0f)
		flPitch -= 360.0f;
	pMoveHelper->m_vecAngles[PITCH] = std::clamp(flPitch, -90.f, 90.f);

	Player* pPlayer = reinterpret_cast<Player*>(pEntity);
	if (pPlayer)
		pPlayer->SetBodyPitch(pMoveHelper->m_vecAngles[PITCH]);
}

void Hooks::Update(int iStartFrame, bool bValidFrame, int iIncomingAcknowledged, int iOutGoingCommand)
{
	CPrediction* pPrediction = reinterpret_cast<CPrediction*>(this);
	if (!pPrediction)
		return g_Hooks.m_Prediction.GetOldMethod< Update_t >(CPrediction::UPDATE)(this, iStartFrame, bValidFrame, iIncomingAcknowledged, iOutGoingCommand);

	float flTimeStamp = g_Csgo.m_Engine->GetLastTimeStamp();
	bool bTimeStampChanged = pPrediction->m_LastServerWorldTimeStamp != flTimeStamp;
	bool bReceivedNewWorldUpdate = true;

	pPrediction->m_EnginePaused = g_Csgo.m_Engine->IsPaused();
	pPrediction->m_LastServerWorldTimeStamp = flTimeStamp;

	g_Csgo.cl_pred_optimize->m_nFlags &= ~FCVAR_HIDDEN;
	g_Csgo.cl_pred_optimize->SetValue(0);

	if (pPrediction->m_PreviousStartFrame == iStartFrame && g_Csgo.cl_pred_optimize->GetBool() && g_Csgo.cl_predict->GetInt() && bTimeStampChanged)
		bReceivedNewWorldUpdate = false;

	pPrediction->m_PreviousStartFrame = iStartFrame;
	pPrediction->m_SavedVars = *g_Csgo.m_Globals;
	pPrediction->_Update(0, bReceivedNewWorldUpdate, bValidFrame, iIncomingAcknowledged, iOutGoingCommand);

	*g_Csgo.m_Globals = pPrediction->m_SavedVars;
}

void Hooks::SetupMove(Entity* pEntity, Command* pCommands, IMoveHelper* pMoveHelper, CMoveData* data)
{
	return g_Hooks.m_Prediction.GetOldMethod< SetupMove_t >(CPrediction::SETUPMOVE)(this, pEntity, pCommands, pMoveHelper, data);
}