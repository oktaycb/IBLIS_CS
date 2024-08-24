#include "../../../precompiled.h"

void Hooks::LevelInitPreEntity(const char* map) {
	float rate{ 1.f / g_Csgo.m_Globals->m_Interval };

	g_Csgo.cl_updaterate->SetValue(rate);
	g_Csgo.cl_cmdrate->SetValue(rate);

	g_Csgo.cl_interp_ratio->SetValue(2.0f);
	g_Csgo.cl_interp->SetValue(g_Csgo.m_Globals->m_Interval);

	g_Csgo.cl_foot_contact_shadows->SetValue(0);

	g_Aimbot.reset();
	g_Visuals.m_hit_start = g_Visuals.m_hit_end = g_Visuals.m_hit_duration = 0.f;

	return g_Hooks.m_Client.GetOldMethod< LevelInitPreEntity_t >(CHLClient::LEVELINITPREENTITY)(this, map);
}

void Hooks::LevelInitPostEntity() {
	g_Client.OnMapload();

	return g_Hooks.m_Client.GetOldMethod< LevelInitPostEntity_t >(CHLClient::LEVELINITPOSTENTITY)(this);
}

void Hooks::LevelShutdown() {
	g_Aimbot.reset();
	g_ShotSystem.m_Shots.clear();

	g_Client.m_Autowalling = false;
	g_Client.m_LocalPlayer = nullptr;
	g_Client.m_Weapon = nullptr;
	g_Client.m_Processing = false;
	g_Client.m_WeaponInfo = nullptr;
	g_Client.m_round_end = false;
	g_Client.m_EyePosition.clear();
	g_Client.m_CommandNumber.clear();
	g_Client.m_TransmissionCorrectionData.clear();
	g_Client.m_TransmissionCommands.clear();

	g_FakePing.m_Sequences.clear();
	g_Network.m_Sequence = 0;

	return g_Hooks.m_Client.GetOldMethod< LevelShutdown_t >(CHLClient::LEVELSHUTDOWN)(this);
}

void Hooks::CreateMove(int iSequenceNumber, float flFrametime, bool bIsActive, bool& bSendPacket) {
	g_Hooks.m_Client.GetOldMethod< CreateMove_t >(CHLClient::CREATEMOVE)(this, iSequenceNumber, flFrametime, bIsActive);
	
#ifdef LEGACY
	if (!g_Csgo.m_Engine->IsConnected() || !g_Csgo.m_Engine->IsInGame())
		return g_Movement.m_AutomaticPeek.Reset();

	g_Client.m_Command = g_Csgo.m_Input->GetUserCmd(iSequenceNumber);
	g_Client.m_LocalPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(g_Csgo.m_Engine->GetLocalPlayer());
	g_Client.m_SendPacket = &bSendPacket;
	g_Client.m_Processing = false;

	if (g_Client.GetModifableCommand() == nullptr || !g_Client.GetModifableCommand()->m_CommandNumber)
		return g_Movement.m_AutomaticPeek.Reset();

	if (g_Client.GetLocalPlayer() == nullptr)
		return g_Movement.m_AutomaticPeek.Reset();

	g_InputPrediction.Backup();

	g_Client.GetModifableCommand()->m_RandomSeed = g_Csgo.MD5_PseudoRandom(g_Client.GetModifableCommand()->m_CommandNumber) & 0x7FFFFFFF;
	g_Client.GetModifablePacket() = true;

	g_Client.StartMove();
	g_Client.m_StrafeAngles = g_Client.GetModifableCommand()->m_ViewAngles;

	if (g_Client.m_Shot = g_Client.IsShooting())
		g_Client.m_ShotCommandNumber = g_Client.GetModifableCommand()->m_CommandNumber;

	if (g_Client.m_Processing)
	{
		g_Movement.JumpRelated();
		g_Movement.Strafe();

		g_Client.m_Weapon = g_Client.m_LocalPlayer->GetActiveWeapon();
		if (g_Client.m_Weapon == nullptr)
			return g_Client.UpdateAnimations();

		g_Client.m_WeaponInfo = g_Client.m_Weapon->GetWpnData();
		if (g_Client.m_WeaponInfo == nullptr)
			return g_Client.UpdateAnimations();

		g_Client.m_WeaponType = g_Client.m_WeaponInfo->m_WeaponType;
		g_Client.m_WeaponItemDefinition = g_Client.m_Weapon->m_iItemDefinitionIndex();

		if (!g_TickbaseManipulation.PerformCommand())
		{
			g_Client.m_VerifiedCommand = g_Csgo.m_Input->GetVerifiedUserCmd(iSequenceNumber);
			if (g_Client.m_VerifiedCommand)
			{
				g_Client.m_VerifiedCommand->m_Command = *g_Client.GetModifableCommand();
				g_Client.m_VerifiedCommand->m_CRC = g_Client.GetModifableCommand()->GetChecksum();
			}

			return g_Client.UpdateAnimations();
		}

		if (g_Aimbot.CanAutoStop())
			g_Aimbot.AutomaticStop();

		g_Movement.AutoPeek();
		g_Movement.FastStop();
		g_Movement.FakeWalk();

		g_InputPrediction.Run();

		g_Client.DoMove();
		g_Client.EndMove();

		g_TickbaseManipulation.ManipulateTickbase();

		if (g_Client.m_ChokedCommands >= g_Client.m_MaxLagAmount)
			g_Client.GetModifablePacket() = true;

		g_Client.UpdateRevolverCock();

		// TODO@Monster: Fix this.
		//if (g_TickbaseManipulation.CanSkipInterpolation())
		//	g_Aimbot.RemoveAttack();

		if (g_Client.GetModifablePacket())
			g_Client.m_aCommandList.emplace_back(g_Client.GetModifableCommand()->m_CommandNumber);
	}
	else g_Movement.m_AutomaticPeek.Reset();

	g_Client.m_VerifiedCommand = g_Csgo.m_Input->GetVerifiedUserCmd(iSequenceNumber);
	if (g_Client.m_VerifiedCommand == nullptr)
		return g_Client.UpdateAnimations();

	g_Client.m_VerifiedCommand->m_Command = *g_Client.GetModifableCommand();
	g_Client.m_VerifiedCommand->m_CRC = g_Client.GetModifableCommand()->GetChecksum();

	return g_Client.UpdateAnimations();
#else
	if (!(g_Client.m_LocalPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(g_Csgo.m_Engine->GetLocalPlayer())))
		return;

	if (!(g_Client.m_Command = g_Csgo.m_Input->GetUserCmd(iSequenceNumber)))
		return;

	if (!(g_Client.m_VerifiedCommand = g_Csgo.m_Input->GetVerifiedUserCmd(iSequenceNumber)))
		return;

	g_Client.m_VerifiedCommand->m_Command = *g_Client.GetModifableCommand();
	g_Client.m_VerifiedCommand->m_CRC = g_Client.GetModifableCommand()->GetChecksum();
#endif
}

__declspec(naked) void __stdcall Hooks::CreateMove_Proxy(int iSequenceNumber, float flFrameTime, bool bIsActive)
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[esp + 20]
		push dword ptr[esp + 0Ch + 8]
		push dword ptr[esp + 10h + 4]
		call Hooks::CreateMove
		pop ebx
		retn 0Ch
	}
}

void Hooks::FrameStageNotify(Stage_t nStage) {

	if (nStage != FRAME_START)
		g_Client.m_Stage = nStage;

	if (!g_Csgo.m_Engine->IsConnected())
		return g_Hooks.m_Client.GetOldMethod< FrameStageNotify_t >(CHLClient::FRAMESTAGENOTIFY)(this, nStage);

	g_Client.m_LocalPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(g_Csgo.m_Engine->GetLocalPlayer());
	if (!g_Client.m_LocalPlayer)
		return g_Hooks.m_Client.GetOldMethod< FrameStageNotify_t >(CHLClient::FRAMESTAGENOTIFY)(this, nStage);

#ifdef LEGACY
	if (nStage == FRAME_RENDER_START)
	{
		if (g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactClient))
		{
			static auto iLastCount = 0;
			auto& iClientImpactList = g_Client.m_LocalPlayer->get< CUtlVector<clientHitVerify_t> >(0xBA90 - 12);

			for (auto i = iClientImpactList.Count(); i > iLastCount; --i)
				g_Csgo.m_DebugOverlay->AddBoxOverlay(iClientImpactList[i - 1].vecPosition, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f), QAngle(0.0f, 0.0f, 0.0f), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactClientColor).r(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactClientColor).g(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactClientColor).b(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactClientColor).a(), 4.0f);

			if (iClientImpactList.Count() != iLastCount)
				iLastCount = iClientImpactList.Count();
		}

		// todo: remake.
		//g_Visuals.DrawBeams();
		//g_Visuals.DrawSounds();
		g_ShotSystem.OnFrameStage();

		if (g_Csgo.m_Input->CAM_IsThirdPerson())
			g_Csgo.m_Prediction->SetLocalViewAngles(g_Client.m_Radar);

		g_Client.SetAbsAngles();
	}

	g_Hooks.m_Client.GetOldMethod< FrameStageNotify_t >(CHLClient::FRAMESTAGENOTIFY)(this, nStage);

	if (nStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		g_Client.Skybox();
		g_InputPrediction.CorrectViewmodelData();
		g_Client.ClanTag();
		g_skins.think();
	}

	else if (nStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
		g_Client.NoSmoke();
	}

	else if (nStage == FRAME_NET_UPDATE_END) {
		g_Csgo.m_Engine->FireEvents();

		for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
			Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
			if (!pPlayer || !pPlayer->IsPlayer() || !pPlayer->IsAlive() || pPlayer->IsLocalPlayer()) {
				// TODO: Use player_spawn Instead.
				if (pPlayer && g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1].m_SpawnTime)
					g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1].m_SpawnTime = 0.f;
				continue;
			}

			AimPlayer* pData = &g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1];
			if (pData)
				pData->OnNetUpdate(pPlayer);
		}

		g_InputPrediction.InputDataToMap();
	}
#endif // LEGACY
}

static void WriteUsercmd(WriteBF* pBuffer, Command* pToCmd, Command* pFromCmd)
{
	// TODO@Move this.
	using WriteUsercmd_t = void(__fastcall*)(void*, Command*, Command*);
	static WriteUsercmd_t fnWriteUserCmd = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D")).as<WriteUsercmd_t>();

	__asm {
		mov     ecx, pBuffer
		mov     edx, pToCmd
		push    pFromCmd
		call    fnWriteUserCmd
		add     esp, 4
	}
}

bool Hooks::WriteUsercmdDeltaToBuffer(int32_t iSlot, WriteBF* pBuffer, int32_t iFrom, int32_t iTo, bool bIsNewCommand)
{
	if (g_TickbaseManipulation.GetForcedShiftAmount() < 1 || g_TickbaseManipulation.GetShiftMode() != MODE::SHIFT_BUFFER)
		return g_Hooks.m_Client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::WRITEUSERCMDDELTATOBUFFER)(this, iSlot, pBuffer, iFrom, iTo, bIsNewCommand);

	int32_t iExtraCommands = g_TickbaseManipulation.GetForcedShiftAmount();
	g_TickbaseManipulation.ResetShiftAmount();

	int32_t* pNumBackupCommands = (int32_t*)((uintptr_t)(pBuffer)-0x30);
	int32_t* pNumNewCommands = (int32_t*)((uintptr_t)(pBuffer)-0x2C);

	int32_t iNewCommands = *pNumNewCommands;
	int32_t iNextCommand = g_Csgo.m_ClientState->m_ChokedCommands + g_Csgo.m_ClientState->m_LastOutgoingCommand + 1;

	*pNumBackupCommands = 0;

	for (iTo = iNextCommand - iNewCommands + 1; iTo <= iNextCommand; iTo++)
	{
		if (!g_Hooks.m_Client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::WRITEUSERCMDDELTATOBUFFER)(this, iSlot, pBuffer, iFrom, iTo, true))
			return false;

		iFrom = iTo;
	}

	*pNumNewCommands = iNewCommands + iExtraCommands;

	Command* pCmd = g_Csgo.m_Input->GetUserCmd(iSlot, iFrom);
	if (!pCmd)
		return true;

	Command ToCmd = *pCmd;
	Command FromCmd = *pCmd;

	ToCmd.m_CommandNumber++;
	ToCmd.m_TickCount += 200;

	for (int32_t i = iNewCommands; i <= iNewCommands + iExtraCommands; i++)
	{
		WriteUsercmd(pBuffer, &ToCmd, &FromCmd);

		ToCmd.m_TickCount--;
		ToCmd.m_CommandNumber--;

		FromCmd.m_TickCount = ToCmd.m_TickCount + 1;
		FromCmd.m_CommandNumber = ToCmd.m_TickCount + 1;
	}

	return true;
}