#include "../../precompiled.h"
#include "EnginePrediction.h"

void CEnginePrediction::Update()
{
	if (g_Csgo.m_ClientState->m_DeltaTick < 0 /*|| g_Client.m_Stage != FRAME_NET_UPDATE_END*/)
		return;

	return g_Csgo.m_Prediction->Update(g_Csgo.m_ClientState->m_DeltaTick, g_Csgo.m_ClientState->m_DeltaTick > 0, g_Csgo.m_ClientState->m_LastCommandAck, g_Csgo.m_ClientState->m_LastOutgoingCommand + g_Csgo.m_ClientState->m_ChokedCommands);
}

void CEnginePrediction::Backup() {
	if (!g_Client.GetLocalPlayer())
		return;

	m_AbsOrigin = g_Client.GetLocalPlayer()->GetAbsOrigin();
	m_AbsVelocity = g_Client.GetLocalPlayer()->m_vecAbsVelocity();

	m_InPrediction = g_Csgo.m_Prediction->m_InPrediction;
	m_FirstTimePredicted = g_Csgo.m_Prediction->m_Split[GET_ACTIVE_SPLITSCREEN_SLOT()].m_FirstTimePredicted;

	m_Curtime = g_Csgo.m_Globals->m_Curtime;
	m_FrameTime = g_Csgo.m_Globals->m_FrameTime;
}

#define CLIENT_DLL

void CPrediction::RunCommand(Player* pPlayer, Command* ucmd, IMoveHelper* pMoveHelper)
{
	const float playerCurTime = game::TICKS_TO_TIME(pPlayer->m_nTickBase());
	const float playerFrameTime = g_Csgo.m_Prediction->m_EnginePaused ? 0.f : g_Csgo.m_Globals->m_Interval;
	if (g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining <= 0.0f)
		g_InputPrediction.m_TimeAllowedForProcessing = 0.0f;
	else if (playerFrameTime > g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining + FLT_EPSILON)
	{
		float flResult = g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining;
		g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining = 0.0f;
		g_InputPrediction.m_TimeAllowedForProcessing = flResult;
	}
	else
	{
		g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining -= playerFrameTime;
		if (g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining < 0.0f)
			g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining = 0.0f;
		g_InputPrediction.m_TimeAllowedForProcessing = playerFrameTime;
	}

	if (g_InputPrediction.m_TimeAllowedForProcessing < playerFrameTime)
		return;

	pMoveHelper->SetHost(pPlayer);

	pPlayer->m_pCurrentCommand() = g_Client.GetModifableCommand();
	pPlayer->m_LastCmd() = *g_Client.GetModifableCommand();

	*g_Csgo.m_nPredictionRandomSeed = g_Client.GetModifableCommand()->m_RandomSeed;
	g_Csgo.m_pPredictionPlayer = pPlayer;

	g_Client.m_Command->m_Buttons |= pPlayer->m_afButtonForced();

	const float serverCurTime = g_Csgo.m_Globals->m_Curtime;
	const float serverFrameTime = g_Csgo.m_Globals->m_FrameTime;
	g_Csgo.m_Globals->m_Curtime = playerCurTime;
	g_Csgo.m_Globals->m_FrameTime = playerFrameTime;

	if (m_EnginePaused && pPlayer->m_MoveType() == MOVETYPE_NOCLIP && g_Csgo.sv_cheats->GetBool() && g_Csgo.sv_noclipduringpause->GetBool())
		g_Csgo.m_Globals->m_FrameTime = g_Csgo.m_Globals->m_Interval;

	g_Csgo.m_GameMovement->StartTrackPredictionErrors(pPlayer);

	SelectItem(pPlayer, g_Client.m_Command);
	LatchInImpulse(pPlayer, g_Client.m_Command);
	CheckMovingGround(pPlayer, g_Csgo.m_Globals->m_FrameTime);

	QAngle vecAngle; GetLocalViewAngles(vecAngle);
	g_InputPrediction.m_MoveData.m_vecOldAngles = vecAngle;

	pPlayer->SetButtonState(g_Client.m_Command->m_Buttons);
	pPlayer->SetLocalViewAngles(g_Client.m_Command->m_ViewAngles);
	pPlayer->SetAimDirection(g_Client.m_Command->m_AimDirection);

	RunPreThink(pPlayer);
	RunThink(pPlayer);
	SetupMove(pPlayer, g_Client.GetModifableCommand(), pMoveHelper, &g_InputPrediction.m_MoveData);

	IVehicle* pVehicle = pPlayer->GetVehicle();
	if (pVehicle)
		pVehicle->ProcessMovement(pPlayer, &g_InputPrediction.m_MoveData);
	else g_Csgo.m_GameMovement->ProcessMovement(pPlayer, &g_InputPrediction.m_MoveData);

	FinishMove(pPlayer, g_Client.GetModifableCommand(), &g_InputPrediction.m_MoveData);
	//RunPostThink(pPlayer);

	//g_Csgo.m_Globals->m_Curtime = serverCurTime;
	//g_Csgo.m_Globals->m_FrameTime = serverFrameTime;

	//pMoveHelper->ProcessImpacts();

	//g_Csgo.m_Globals->m_Curtime = playerCurTime;
	//g_Csgo.m_Globals->m_FrameTime = playerFrameTime;

	g_Csgo.m_GameMovement->FinishTrackPredictionErrors(pPlayer);
	g_Csgo.m_GameMovement->Reset();

	pPlayer->m_pCurrentCommand() = nullptr;

	*g_Csgo.m_nPredictionRandomSeed = -1;
	g_Csgo.m_pPredictionPlayer = nullptr;

	if (g_Csgo.m_Globals->m_FrameTime > 0.f)
		pPlayer->m_nTickBase()++;

	return pMoveHelper->SetHost(nullptr);
}

void CEnginePrediction::Run() {
	if (!g_Client.m_Processing)
		return;

	g_Csgo.m_Prediction->m_InPrediction = true;
	g_Csgo.m_Prediction->m_Split[GET_ACTIVE_SPLITSCREEN_SLOT()].m_FirstTimePredicted = false;

	g_Client.GetLocalPlayer()->SetAbsOrigin(g_Client.GetLocalPlayer()->m_vecOrigin());
	g_Client.GetLocalPlayer()->SetAbsVelocity(g_Client.GetLocalPlayer()->m_vecVelocity());

	g_Csgo.m_Prediction->RunCommand(g_Client.GetLocalPlayer(), g_Client.GetModifableCommand(), g_Csgo.m_MoveHelper);

	if (g_Csgo.m_Globals->m_FrameTime > 0.f)
		g_Client.GetLocalPlayer()->m_nTickBase()--;
}

void CEnginePrediction::Finish() {
	if (!g_Client.m_Processing)
		return;

	g_Client.GetLocalPlayer()->SetAbsOrigin(m_AbsOrigin);
	g_Client.GetLocalPlayer()->SetAbsVelocity(m_AbsVelocity);

	g_Csgo.m_Prediction->m_Split[GET_ACTIVE_SPLITSCREEN_SLOT()].m_FirstTimePredicted = m_FirstTimePredicted;
	g_Csgo.m_Prediction->m_InPrediction = m_InPrediction;

	g_Csgo.m_Globals->m_Curtime = m_Curtime;
	g_Csgo.m_Globals->m_FrameTime = m_FrameTime;
}

void CEnginePrediction::Repredict()
{
	g_Csgo.m_Prediction->m_Split[GET_ACTIVE_SPLITSCREEN_SLOT()].m_PreviousAckHadErrors = 1;
	g_Csgo.m_Prediction->m_Split[GET_ACTIVE_SPLITSCREEN_SLOT()].m_CommandsPredicted = 0;
}

void CEnginePrediction::CorrectViewmodelData()
{
	if (!g_Client.m_Processing || g_Client.GetLocalPlayer()->m_hViewModel() == 0xFFFFFFFF)
		return;

	Entity* const pViewModel = g_Csgo.m_EntityList->GetClientEntityFromHandle(g_Client.GetLocalPlayer()->m_hViewModel());
	if (!pViewModel)
		return;

	pViewModel->m_flAnimTime() = m_flViewmodelAnimTime;
	pViewModel->m_flCycle() = m_flViewmodelCycle;
}

void CEnginePrediction::UpdateViewmodelData()
{
	if (!g_Client.m_Processing || g_Client.GetLocalPlayer()->m_hViewModel() == 0xFFFFFFFF)
		return;

	Entity* const pViewModel = g_Csgo.m_EntityList->GetClientEntityFromHandle(g_Client.GetLocalPlayer()->m_hViewModel());
	if (!pViewModel)
		return;

	m_flViewmodelCycle = pViewModel->m_flCycle();
	m_flViewmodelAnimTime = pViewModel->m_flAnimTime();
}

//const char* Weapon::GetName() {
//	return util::get_method< const char* (__thiscall*)(void*) >(this, 378)(this);
//}

bool CPrediction::InPrediction(void)
{
	return m_InPrediction;
}

float CPrediction::GetSavedTime()
{
	return m_SavedVars.m_Curtime;
}

void CPrediction::SelectItem(Player* pPlayer, Command* pCmd)
{
	if (!pCmd->m_WeaponSelect)
		return;

	Weapon* pWeapon = dynamic_cast<Weapon*>(g_Csgo.m_EntityList->GetClientEntity(pCmd->m_WeaponSelect));
	if (!pWeapon)
		return;

	WeaponInfo* pWpnData = pWeapon->GetWpnData();
	if (!pWpnData)
		return;

	pPlayer->SelectItem(pWpnData->m_WeaponName, pCmd->m_WeaponSubType);
}

void CPrediction::LatchInImpulse(Player* pPlayer, Command* pCmd)
{
	if (!pCmd->m_Impulse)
		return;

	IVehicle* pVehicle = pPlayer->GetVehicle();
	if (pVehicle && !pPlayer->UsingStandardWeaponsInVehicle())
		return;

	pPlayer->m_nImpulse() = pCmd->m_Impulse;
}

void CPrediction::RunPreThink(Player* pPlayer)
{
	if (pPlayer->PhysicsRunThink())
		pPlayer->PreThink();
}

void CPrediction::RunThink(Player* pPlayer)
{
	if (pPlayer->m_nNextThinkTick() <= 0 || pPlayer->m_nNextThinkTick() > pPlayer->m_nTickBase())
		return;

	pPlayer->m_nNextThinkTick() = -1;
	pPlayer->CheckHasThinkFunction(false);
	pPlayer->Think();
}

// TODO@Monster: Rebuilt From Server, Understand Why it's broking the fake latency.
void CPrediction::RunPostThink(Player* pPlayer)
{
	pPlayer->PostThink();
}

typedescription_t* GetDatamapEntry(datamap_t* pMap, const char* name)
{
	while (pMap)
	{
		for (int i = 0; i < pMap->m_size; i++)
		{
			if (pMap->m_desc[i].m_Name == nullptr)
				continue;

			if (strcmp(name, pMap->m_desc[i].m_Name) == 0)
				return &pMap->m_desc[i];
		}
		pMap = pMap->m_base;
	}

	return 0;
}

void CEnginePrediction::InputDataToMap()
{
	//auto dmap = g_Client.GetLocalPlayer()->GetPredDescMap();

	//static std::unique_ptr<typedescription_t[]> data(new typedescription_t[dmap->m_size + 1]);
	//std::memcpy(data.get(), dmap->m_desc, dmap->m_size * sizeof typedescription_t);

	//typedescription_t velmod = {};

	//velmod.m_Type = FIELD_FLOAT;
	//velmod.m_Name = "m_flVelocityModifier";
	//velmod.m_offset = g_Offsets.m_flVelocityModifier.get();
	//velmod.m_size = 1;
	//velmod.m_Flags = 0x100; // FTYPEDESC_INSENDTABLE
	//velmod.m_bytes = 4;
	//velmod.m_tolerance = 0.057f; // Don't ask.

	//std::memcpy(&data.get()[dmap->m_size], &velmod, sizeof typedescription_t);

	//dmap->m_desc = data.get();
	//dmap->m_size += 1;
	//dmap->m_optimized_data_map = nullptr; // Forcing the game to recompute flattened chains.

	//g_Csgo.m_Prediction->shutdown_predictables();
	//g_Csgo.m_Prediction->reinit_predictables();
	
	//static size_t m_flVelocityModifier = g_Offsets.m_flVelocityModifier.get();
	//if (!m_flVelocityModifier)
	//	return;

	//datamap_t* pDataMap = g_Client.GetLocalPlayer()->GetPredDescMap();
	//typedescription_t* pTypeDesc = GetDatamapEntry(pDataMap, XOR("m_vphysicsCollisionState"));
	//if (!pTypeDesc)
	//	return;

	//const float flRecoveryRate = 1.f / 2.5f;
	//const float flTolerance = flRecoveryRate * g_Csgo.m_Globals->m_Interval;

	//if (pTypeDesc->m_tolerance == flTolerance)
	//	return;

	//int offset = static_cast<int>(m_flVelocityModifier);
	//if (!offset)
	//	return;

	//pTypeDesc->m_Type = FIELD_FLOAT;
	//pTypeDesc->m_tolerance = flTolerance;
	//pTypeDesc->m_offset = offset;
	//pTypeDesc->m_bytes = sizeof(float);
	//pTypeDesc->m_flat_offset[TD_OFFSET_NORMAL] = offset;

	//pDataMap->m_optimized_data_map = nullptr;
}

void expand_ccsplayer_dmap() {
	auto dmap = g_Client.GetLocalPlayer()->GetPredDescMap();

	static std::unique_ptr<typedescription_t[]> data(new typedescription_t[dmap->m_size + 1]);
	std::memcpy(data.get(), dmap->m_desc, dmap->m_size * sizeof typedescription_t);

	typedescription_t velmod = {};

	velmod.m_Type = FIELD_FLOAT;
	velmod.m_Name = "m_flVelocityModifier";
	velmod.m_offset = g_Offsets.m_flVelocityModifier.get();
	velmod.m_size = 1;
	velmod.m_Flags = 0x100; // FTYPEDESC_INSENDTABLE
	velmod.m_bytes = 4;
	velmod.m_tolerance = 0.057f; // Don't ask.

	std::memcpy(&data.get()[dmap->m_size], &velmod, sizeof typedescription_t);

	dmap->m_desc = data.get();
	dmap->m_size += 1;
	dmap->m_optimized_data_map = nullptr; // Forcing the game to recompute flattened chains.

	//g_Csgo.m_Prediction->shutdown_predictables();
	//g_Csgo.m_Prediction->reinit_predictables();
}