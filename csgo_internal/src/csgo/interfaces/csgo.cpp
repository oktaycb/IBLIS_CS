#include "../../precompiled.h"

void CSGO::SetupModulesList()
{
	m_kernel32_dll = PE::GetModule(HASH("kernel32.dll"));
	m_user32_dll = PE::GetModule(HASH("user32.dll"));
	m_shell32_dll = PE::GetModule(HASH("shell32.dll"));
	m_shlwapi_dll = PE::GetModule(HASH("shlwapi.dll"));
	m_client_dll = PE::GetModule(HASH("client.dll"));
	m_engine_dll = PE::GetModule(HASH("engine.dll"));
	m_server_dll = PE::GetModule(HASH("server.dll"));
	m_vstdlib_dll = PE::GetModule(HASH("vstdlib.dll"));
	m_tier0_dll = PE::GetModule(HASH("tier0.dll"));
	m_datacache_dll = PE::GetModule(HASH("datacache.dll"));
	m_shaderapidx9_dll = PE::GetModule(HASH("shaderapidx9.dll"));
	m_materialsystem_dll = PE::GetModule(HASH("materialsystem.dll"));
}

bool CSGO::SetupInterfaceList()
{
	Interfaces pInterfaces;

	m_Client = pInterfaces.get< CHLClient* >(HASH("VClient"));
	if (!m_Client)
		return false;

	m_CVar = pInterfaces.get< ICvar* >(HASH("VEngineCvar"));
	if (!m_CVar)
		return false;

	m_Engine = pInterfaces.get< IVEngineClient* >(HASH("VEngineClient"));
	if (!m_Engine)
		return false;

	m_EntityList = pInterfaces.get< IClientEntityList* >(HASH("VClientEntityList"));
	if (!m_EntityList)
		return false;

	m_InputSystem = pInterfaces.get< IInputSystem* >(HASH("InputSystemVersion"));
	if (!m_InputSystem)
		return false;

	m_Surface = pInterfaces.get< ISurface* >(HASH("VGUI_Surface"));
	if (!m_Surface)
		return false;

	m_Panel = pInterfaces.get< IPanel* >(HASH("VGUI_Panel"));
	if (!m_Panel)
		return false;

	m_EngineVGui = pInterfaces.get< IEngineVGui* >(HASH("VEngineVGui"));
	if (!m_EngineVGui)
		return false;

	m_Prediction = pInterfaces.get< CPrediction* >(HASH("VClientPrediction"));
	if (!m_Prediction)
		return false;

	m_EngineTrace = pInterfaces.get< IEngineTrace* >(HASH("EngineTraceClient"));
	if (!m_EngineTrace)
		return false;

	m_GameMovement = pInterfaces.get< CGameMovement* >(HASH("GameMovement"));
	if (!m_GameMovement)
		return false;

	m_RenderView = pInterfaces.get< IVRenderView* >(HASH("VEngineRenderView"));
	if (!m_RenderView)
		return false;

	m_ModelRender = pInterfaces.get< IVModelRender* >(HASH("VEngineModel"));
	if (!m_ModelRender)
		return false;

	m_MaterialSystem = pInterfaces.get< IMaterialSystem* >(HASH("VMaterialSystem"));
	if (!m_MaterialSystem)
		return false;

	m_MdlCache = pInterfaces.get< IMDLCache* >(HASH("MDLCache"));
	if (!m_MdlCache)
		return false;

	m_StudioRender = pInterfaces.get< CStudioRenderContext* >(HASH("VStudioRender"));
	if (!m_StudioRender)
		return false;

	m_ModelInfo = pInterfaces.get< IVModelInfo* >(HASH("VModelInfoClient"));
	if (!m_ModelInfo)
		return false;

	m_DebugOverlay = pInterfaces.get< IVDebugOverlay* >(HASH("VDebugOverlay"));
	if (!m_DebugOverlay)
		return false;

	m_PhysProps = pInterfaces.get< IPhysicsSurfaceProps* >(HASH("VPhysicsSurfaceProps"));
	if (!m_PhysProps)
		return false;

	m_GameEvents = pInterfaces.get< IGameEventManager2* >(HASH("GAMEEVENTSMANAGER"), 1);
	if (!m_GameEvents)
		return false;

	m_Localize = pInterfaces.get< ILocalize* >(HASH("Localize_"));
	if (!m_Localize)
		return false;

	m_NetworkStringTable = pInterfaces.get< INetworkStringTableContainer* >(HASH("VEngineClientStringTable"));
	if (!m_NetworkStringTable)
		return false;

	m_EngineSound = pInterfaces.get< IEngineSound* >(HASH("IEngineSoundClient"));
	if (!m_EngineSound)
		return false;

	m_MemAlloc = PE::GetExport(m_tier0_dll, HASH("g_pMemAlloc")).get< IMemAlloc* >();
	if (!m_MemAlloc)
		return false;

	return true;
}

void CSGO::SetupConvarList()
{
	m_pitch = m_CVar->FindVar(HASH("m_pitch"));
	m_yaw = m_CVar->FindVar(HASH("m_yaw"));
	sensivity = m_CVar->FindVar(HASH("sensitivity"));
	m_MaxUserCommandProcessTicks = m_CVar->FindVar(HASH("sv_maxusrcmdprocessticks"));
	cl_extrapolate = m_CVar->FindVar(HASH("cl_extrapolate"));
	clear = m_CVar->FindVar(HASH("clear"));
	toggleconsole = m_CVar->FindVar(HASH("toggleconsole"));
	name = m_CVar->FindVar(HASH("name"));
	sv_maxunlag = m_CVar->FindVar(HASH("sv_maxunlag"));
	m_Gravity = m_CVar->FindVar(HASH("sv_gravity"));
	sv_jump_impulse = m_CVar->FindVar(HASH("sv_jump_impulse"));
	sv_enablebunnyhopping = m_CVar->FindVar(HASH("sv_enablebunnyhopping"));
	sv_autobunnyhopping = m_CVar->FindVar(HASH("sv_autobunnyhopping"));
	sv_airaccelerate = m_CVar->FindVar(HASH("sv_airaccelerate"));
	sv_cheats = m_CVar->FindVar(HASH("sv_cheats"));
	sv_noclipduringpause = m_CVar->FindVar(HASH("sv_noclipduringpause"));
	sv_friction = m_CVar->FindVar(HASH("sv_friction"));
	sv_stopspeed = m_CVar->FindVar(HASH("sv_stopspeed"));
	sv_maxspeed = m_CVar->FindVar(HASH("sv_maxspeed"));
	cl_interp = m_CVar->FindVar(HASH("cl_interp"));
	cl_interpolate = m_CVar->FindVar(HASH("cl_interpolate"));
	cl_interp_ratio = m_CVar->FindVar(HASH("cl_interp_ratio"));
	sv_maxupdaterate = m_CVar->FindVar(HASH("sv_maxupdaterate"));
	sv_minupdaterate = m_CVar->FindVar(HASH("sv_minupdaterate"));
	sv_client_max_interp_ratio = m_CVar->FindVar(HASH("sv_client_max_interp_ratio"));
	sv_client_min_interp_ratio = m_CVar->FindVar(HASH("sv_client_min_interp_ratio"));
	cl_updaterate = m_CVar->FindVar(HASH("cl_updaterate"));
	cl_cmdrate = m_CVar->FindVar(HASH("cl_cmdrate"));
	cl_lagcompensation = m_CVar->FindVar(HASH("cl_lagcompensation"));
	mp_teammates_are_enemies = m_CVar->FindVar(HASH("mp_teammates_are_enemies"));
	weapon_debug_spread_show = m_CVar->FindVar(HASH("weapon_debug_spread_show"));
	cl_foot_contact_shadows = m_CVar->FindVar(HASH("cl_foot_contact_shadows"));
	molotov_throw_detonate_time = m_CVar->FindVar(HASH("molotov_throw_detonate_time"));
	host_timescale = m_CVar->FindVar(HASH("host_timescale"));
	weapon_molotov_maxdetonateslope = m_CVar->FindVar(HASH("weapon_molotov_maxdetonateslope"));
	weapon_recoil_scale = m_CVar->FindVar(HASH("weapon_recoil_scale"));
	view_recoil_tracking = m_CVar->FindVar(HASH("view_recoil_tracking"));
	cl_fullupdate = m_CVar->FindVar(HASH("cl_fullupdate"));
	r_DrawSpecificStaticProp = m_CVar->FindVar(HASH("r_DrawSpecificStaticProp"));
	cl_crosshair_sniper_width = m_CVar->FindVar(HASH("cl_crosshair_sniper_width"));
	hud_scaling = m_CVar->FindVar(HASH("hud_scaling"));
	sv_clip_penetration_traces_to_players = m_CVar->FindVar(HASH("sv_clip_penetration_traces_to_players"));
	weapon_accuracy_shotgun_spread_patterns = m_CVar->FindVar(HASH("weapon_accuracy_shotgun_spread_patterns"));
	weapon_accuracy_nospread = m_CVar->FindVar(HASH("weapon_accuracy_nospread"));
	net_showfragments = m_CVar->FindVar(HASH("net_showfragments"));
	cl_csm_shadows = m_CVar->FindVar(HASH("cl_csm_shadows"));
	sv_party_mode = m_CVar->FindVar(HASH("sv_party_mode"));
	r_aspectratio = m_CVar->FindVar(HASH("r_aspectratio"));
	cl_camera_height_restriction_debug = m_CVar->FindVar(HASH("cl_camera_height_restriction_debug"));
	sv_penetration_type = m_CVar->FindVar(HASH("sv_penetration_type"));
	ff_damage_bullet_penetration = m_CVar->FindVar(HASH("ff_damage_bullet_penetration"));
	ff_damage_reduction_bullets = m_CVar->FindVar(HASH("ff_damage_reduction_bullets"));
	cl_pred_optimize = m_CVar->FindVar(HASH("cl_pred_optimize"));
	cl_predict = m_CVar->FindVar(HASH("cl_predict"));
	cl_SetupAllBones = m_CVar->FindVar(HASH("cl_SetupAllBones"));
	sv_clockcorrection_msecs = m_CVar->FindVar(HASH("sv_clockcorrection_msecs"));

	m_RainDensity = m_CVar->FindVar(HASH("r_raindensity"));
	m_RainSpeed = m_CVar->FindVar(HASH("r_rainspeed"));
	m_RainLength = m_CVar->FindVar(HASH("r_rainlength"));
	m_RainWidth = m_CVar->FindVar(HASH("r_rainwidth"));
	m_RainAlpha = m_CVar->FindVar(HASH("r_rainalpha"));
	m_RainSideVel = m_CVar->FindVar(HASH("r_RainSideVel"));
}

bool CSGO::SetupSignatureList()
{
#ifdef LEGACY
	m_MoveHelper = pattern::find(m_client_dll, XOR("8B 0D ? ? ? ? 8B 46 08 68")).add(2).get< IMoveHelper* >(2);
	if (!m_MoveHelper)
		return false;

	m_Game = pattern::find(m_engine_dll, XOR("A1 ? ? ? ? B9 ? ? ? ? FF 75 08 FF 50 34")).add(1).get< CGame* >();
	if (!m_Game)
		return false;

	m_Hud = pattern::find(m_client_dll, XOR("B9 ? ? ? ? 0F 94 C0 0F B6 C0 50 68")).add(0x1).get().as< CHud* >();
	if (!m_Hud)
		return false;

	m_ClientShadowMgr = pattern::find(m_client_dll, XOR("A1 ? ? ? ? FF 90 ? ? ? ? 6A 00")).add(1).get().as< IClientShadowMgr* >();
	if (!m_ClientShadowMgr)
		return false;

	m_PlayerResource = pattern::find(m_client_dll, XOR("83 3D ? ? ? ? 00 53 56 8B D9 C7 44 24 0C 00 00 00 00 57")).add(2).get().as< CPlayerResource* >();
	if (!m_PlayerResource)
		return false;

	ResetAnimationState = pattern::find(m_client_dll, XOR("56 6A 01 68 ? ? ? ? 8B F1")).as< uintptr_t >();
	if (!ResetAnimationState)
		return false;

	ColorModulate = pattern::find(m_materialsystem_dll, XOR("55 8B EC 83 EC ? 56 8B F1 8A 46")).as< uintptr_t >();
	if (!ColorModulate)
		return false;

	IsUsingStaticPropDebugModes = pattern::find(m_engine_dll, XOR("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")).as< uintptr_t >();
	if (!IsUsingStaticPropDebugModes)
		return false;

	ReportHit = pattern::find(m_client_dll, XOR("55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01")).as< uintptr_t >();
	if (!ReportHit)
		return false;

	CL_Move = pattern::find(m_engine_dll, XOR("55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A")).as<uintptr_t>();
	if (!CL_Move)
		return false;

	CLCMsg_Move = pattern::find(m_engine_dll, XOR("55 8B EC 83 EC 30 53 8B D9 57")).as< uintptr_t >();
	if (!CLCMsg_Move)
		return false;

	IsSelfAnimating = pattern::find(m_client_dll, XOR("80 B9 8C 28 00")).as<uintptr_t>();
	if (!IsSelfAnimating)
		return false;

	AddBaseAnimatingInterpolatedVars = pattern::find(m_client_dll, XOR("53 56 8B F1 6A 01")).as<uintptr_t>();
	if (!AddBaseAnimatingInterpolatedVars)
		return false;

	RandomSeed = PE::GetExport(m_vstdlib_dll, HASH("RandomSeed")).as< RandomSeed_t >();
	if (!RandomSeed)
		return false;

	RandomInt = PE::GetExport(m_vstdlib_dll, HASH("RandomInt")).as< RandomInt_t >();
	if (!RandomInt)
		return false;

	RandomFloat = PE::GetExport(m_vstdlib_dll, HASH("RandomFloat")).as< RandomFloat_t >();
	if (!RandomFloat)
		return false;

	m_ClientState = **reinterpret_cast<CClientState***> ((*reinterpret_cast<DWORD**> (m_Engine))[12] + 0x10);
	if (!m_ClientState)
		return false;

	m_HookableClientState = reinterpret_cast<DWORD**>(reinterpret_cast<DWORD>(g_Csgo.m_ClientState) + 0x8);
	if (!m_HookableClientState)
		return false;

	m_ViewRender = pattern::find(m_client_dll, XOR("8B 0D ? ? ? ? 8B 01 FF 50 4C 8B 06")).add(2).get< CViewRender* >(2);
	if (!m_ViewRender)
		return false;

	m_GameRules = pattern::find(m_client_dll, XOR("8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 75 6B")).add(0x2).get< C_CSGameRules* >();
	if (!m_GameRules)
		return false;

	m_Beams = pattern::find(m_client_dll, XOR("8D 04 24 50 A1 ? ? ? ? B9")).add(0x5).get< IViewRenderBeams* >();
	if (!m_Beams)
		return false;

	GetGlowObjectManager = pattern::find(m_client_dll, XOR("A1 ? ? ? ? A8 01 75 4B")).as< GetGlowObjectManager_t >();
	if (!GetGlowObjectManager)
		return false;

	UpdateAnimationState = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24")).as< uintptr_t >();
	if (!UpdateAnimationState)
		return false;

	ModifyEyePosition = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 58 56 57 8B F9 83 7F 60")).as< uintptr_t >();
	if (!ModifyEyePosition)
		return false;

	ShouldSkipAnimationFrame = pattern::find(m_client_dll, XOR("57 8B F9 8B 07 8B 80 60")).as< uintptr_t >();
	if (!ShouldSkipAnimationFrame)
		return false;

	CheckForSequenceChange = pattern::find(m_client_dll, XOR("55 8B EC 51 53 8B 5D 08 56 8B F1 57 85")).as< uintptr_t >();
	if (!CheckForSequenceChange)
		return false;

	UpdateClientSideAnimation = pattern::find(m_client_dll, XOR("56 8B F1 80 BE 8C")).as< uintptr_t >();
	if (!UpdateClientSideAnimation)
		return false;

	UpdateClientSideAnimations = pattern::find(m_client_dll, XOR("8B 0D ? ? ? ? 53 56 57 8B 99")).as< uintptr_t >();
	if (!UpdateClientSideAnimations)
		return false;

	StudioFrameAdvance = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 08 56 8B F1 57 80")).as< uintptr_t >();
	if (!StudioFrameAdvance)
		return false;

	Interpolate = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 57 83")).as< uintptr_t >();
	if (!Interpolate)
		return false;

	m_GlowObjectManager = GetGlowObjectManager();
	if (!m_GlowObjectManager)
		return false;
#else
	//ResetAnimationState = pattern::find(m_client_dll, XOR("56 6A 01 68 ? ? ? ? 8B F1")).as< uintptr_t >();
	//if (!ResetAnimationState)
	//	return false;

	//m_MoveHelper = pattern::find(m_client_dll, XOR("8B 0D ? ? ? ? 8B 46 08 68")).add(2).get< IMoveHelper* >(2);
	//if (!m_MoveHelper)
	//	return false;

	//ReportHit = pattern::find(m_client_dll, XOR("53 56 57 8B F9 33 F6 8B 5F 1C 84 DB 74")).as< uintptr_t >();
	//if (!ReportHit)
	//	return false;

	//CL_Move = pattern::find(m_engine_dll, XOR("55 8B EC 81 EC ? ? ? ? 53 56 8A")).as<uintptr_t>();
	//if (!CL_Move)
	//	return false;

	//IsSelfAnimating = pattern::find(m_client_dll, XOR("80 ? ? ? ? ? ? 74 03")).as<uintptr_t>();
	//if (!IsSelfAnimating)
	//	return false;

	m_ClientState = **reinterpret_cast<CClientState***> ((*reinterpret_cast<DWORD**> (m_Engine))[12] + 0x10);
	if (!m_ClientState)
		return false;

	//UpdateAnimationState = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3")).as< uintptr_t >();
	//if (!UpdateAnimationState)
	//	return false;

	//ModifyEyePosition = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14")).as< uintptr_t >();
	//if (!ModifyEyePosition)
	//	return false;

	//ShouldSkipAnimationFrame = pattern::find(m_client_dll, XOR("57 8B F9 8B 07 8B ? ? ? ? ? FF D0 84 C0 75")).as< uintptr_t >();
	//if (!ShouldSkipAnimationFrame)
	//	return false;

	//CheckForSequenceChange = pattern::find(m_client_dll, XOR("55 8B EC 51 53 8B 5D 08 ? ? ? 57 85 DB 0F 84")).as< uintptr_t >();
	//if (!CheckForSequenceChange)
	//	return false;

	//UpdateClientSideAnimation = pattern::find(m_client_dll, XOR("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74")).as< uintptr_t >();
	//if (!UpdateClientSideAnimation)
	//	return false;

	//UpdateClientSideAnimations = pattern::find(m_client_dll, XOR("8B ? ? ? ? ? 53 56 57 8B ? ? ? ? ? 85")).as< uintptr_t >();
	//if (!UpdateClientSideAnimations)
	//	return false;

	//StudioFrameAdvance = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 08 56 8B F1 57")).as< uintptr_t >();
	//if (!StudioFrameAdvance)
	//	return false;
#endif

	return true;
}

bool CSGO::SetupVirtualList()
{
#ifdef LEGACY
	m_Globals = util::get_method(m_Client, CHLClient::INIT).add(0x1B).get< CGlobalVarsBase* >(2);
#else
	m_Globals = util::get_method(m_Client, CHLClient::INIT).add(0x1F).get< CGlobalVarsBase* >(2);
#endif
	if (!m_Globals)
		return false;

	m_ClientMode = util::get_method(m_Client, CHLClient::HUDPROCESSINPUT).add(0x5).get< IClientMode* >(2);
	if (!m_ClientMode)
		return false;

#ifdef LEGACY
	m_Input = util::get_method(m_Client, CHLClient::INACTIVATEMOUSE).at< CInput* >(0x1);
	if (!m_Input)
		return false;
#else
	m_Input = pattern::find(m_client_dll, XOR("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")).add(0x1).as< CInput* >();
	if (!m_Input)
		return false;
#endif

	m_nPredictionRandomSeed = util::get_method(m_Prediction, CPrediction::RUNCOMMAND).add(0x30).get< int* >();
	if (!m_nPredictionRandomSeed)
		return false;

	m_pPredictionPlayer = util::get_method(m_Prediction, CPrediction::RUNCOMMAND).add(0x54).get< Player* >();
	if (!m_pPredictionPlayer)
		return false;

	return true;
}

void InjectInGame() {
	g_Csgo.m_Engine->ExecuteClientCmd(XOR("clear"));

	if (!g_Csgo.m_Engine->IsInGame())
		return;

	g_Csgo.m_Engine->ExecuteClientCmd(XOR("retry"));

	// Wait 3 Second Niggers.
	return std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

bool CSGO::Init()
{
	VM_DIFF_START;

	while (!(g_Csgo.m_Window = FindWindowA(XOR("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	while (!GetModuleHandleA(XOR("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	SetupModulesList();

	if (!SetupInterfaceList())
		return false;

	if (!SetupVirtualList())
		return false;

#ifdef LEGACY
	InjectInGame();
#endif // LEGACY

	if (!SetupSignatureList())
		return false;

	SetupConvarList();

	m_Device = **pattern::find(m_shaderapidx9_dll, XOR("A1 ? ? ? ? 50 8B 08 FF 51 0C")).add(0x1).as<IDirect3DDevice9***>();
	if (!m_Device)
		return false;
#ifdef LEGACY
	CClient_Precipitation_Simulate = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 1C 53 8B D9 F3")).as< Simulate_t >();
	if (!CClient_Precipitation_Simulate)
		return false;

	CClient_Precipitation_ClientThink = pattern::find(m_client_dll, XOR("A1 ? ? ? ? 83 C1 F4 F3 0F 10 48 14 E9 ? ? ? ?")).as< ClientThink_t >();
	if (!CClient_Precipitation_ClientThink)
		return false;

	MD5_PseudoRandom = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 70 6A 58")).as< MD5_PseudoRandom_t >();
	if (!MD5_PseudoRandom)
		return false;

	SetAbsAngles = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	if (!SetAbsAngles)
		return false;

	DisablePostProcess = pattern::find(m_client_dll, XOR("80 3D ? ? ? ? ? 53 56 57 0F 85")).add(0x2);
	if (!DisablePostProcess)
		return false;

	LockStudioHdr = pattern::find(m_client_dll, XOR("55 8B EC 51 53 8B D9 56 57 8D B3"));
	if (!LockStudioHdr)
		return false;

	SetAbsOrigin = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	if (!SetAbsOrigin)
		return false;

	IsBreakableEntity = pattern::find(m_client_dll, XOR("55 8B EC 51 56 8B F1 85 F6 74 68 83 BE")).as< IsBreakableEntity_t >();
	if (!IsBreakableEntity)
		return false;

	SetAbsVelocity = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1"));
	if (!SetAbsVelocity)
		return false;

	CalcAbsoluteVelocity = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7 87"));
	if (!CalcAbsoluteVelocity)
		return false;

	AngleMatrix = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 8B 07 89 46 0C")).rel32(0x1).as< AngleMatrix_t >();
	if (!AngleMatrix)
		return false;

	ComputeHitboxSurroundingBox = pattern::find(m_client_dll, XOR("E9 ? ? ? ? 32 C0 5D")).rel32(0x1);
	if (!ComputeHitboxSurroundingBox)
		return false;

	Reinitialize = pattern::find(m_client_dll, XOR("55 8B EC 56 8B F1 8B 06 85 C0 74 11 6A ? 50 E8"));
	if (!Reinitialize)
		return false;

	IndexModelSequences = pattern::find(m_client_dll, XOR("57 8B F9 85 FF 74 6E E8 ? ? ? ? 84 C0 74 65 56 8B CF"));
	if (!IndexModelSequences)
		return false;

	ActivityListVersion = pattern::find(m_client_dll, XOR("3B 05 ? ? ? ? 7D 07 8B CE E8 ? ? ? ? 8B 46 04 85 C0"));
	if (!ActivityListVersion)
		return false;

	GetSequenceActivity = pattern::find(m_client_dll, XOR("53 56 8B F1 8B DA 85 F6 74 55"));
	if (!GetSequenceActivity)
		return false;

	AttachmentHelper = pattern::find(m_client_dll, XOR("55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4"));
	if (!AttachmentHelper)
		return false;

	LoadFromBuffer = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 88 44 24 0F 8B 56 FC")).rel32(0x1).as< LoadFromBuffer_t >();
	if (!LoadFromBuffer)
		return false;

	ServerRankRevealAll = pattern::find(m_client_dll, XOR("55 8B EC 8B 0D ? ? ? ? 68")).as< ServerRankRevealAll_t >();
	if (!ServerRankRevealAll)
		return false;

	InvalidatePhysicsRecursive = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 89 5E 18")).rel32(0x1);
	if (!InvalidatePhysicsRecursive)
		return false;

	IsReady = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 59 C2 08 00 51 E8")).rel32(0x1).as< IsReady_t >();
	if (!IsReady)
		return false;

	ShowAndUpdateSelection = pattern::find(m_client_dll, XOR("E8 ? ? ? ? A1 ? ? ? ? F3 0F 10 40 ? C6 83")).rel32(0x1).as< ShowAndUpdateSelection_t >();
	if (!ShowAndUpdateSelection)
		return false;

	GetEconItemView = pattern::find(m_client_dll, XOR("8B 81 ? ? ? ? 81 C1 ? ? ? ? FF 50 04 83 C0 40 C3")).as< GetEconItemView_t >();
	if (!GetEconItemView)
		return false;

	GetStaticData = pattern::find(m_client_dll, XOR("55 8B EC 51 56 57 8B F1 E8 ? ? ? ? 0F B7 8E")).as< GetStaticData_t >();
	if (!GetStaticData)
		return false;

	BeamAlloc = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 8B F0 85 F6 74 7C")).rel32< BeamAlloc_t >(0x1);
	if (!BeamAlloc)
		return false;

	SetupBeam = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 8B 07 33 C9")).rel32< SetupBeam_t >(0x1);
	if (!SetupBeam)
		return false;

	ClearNotices = pattern::find(m_client_dll, XOR("E8 ? ? ? ? 68 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 8B F0 85 F6 74 19")).rel32< ClearNotices_t >(0x1);
	if (!ClearNotices)
		return false;

	GetShotgunSpread = pattern::find(m_client_dll, XOR("E8 ? ? ? ? EB 38 83 EC 08")).rel32< GetShotgunSpread_t >(1);
	if (!GetShotgunSpread)
		return false;

	BoneSetupLock = pattern::find(m_client_dll, XOR("EB 35 FF 15 ? ? ? ? 8D 97 ? ? ? ? 89 44 ? ? 3B 02")).add(10).to< size_t >();
	if (!BoneSetupLock)
		return false;

	BoneAccessor = pattern::find(m_client_dll, XOR("8D 81 ? ? ? ? 50 8D 84 24")).add(2).to< size_t >();
	if (!BoneAccessor)
		return false;

	AnimOverlay = pattern::find(m_client_dll, XOR("8B 80 ? ? ? ? 8D 34 C8")).add(2).to< size_t >();
	if (!AnimOverlay)
		return false;

	SpawnTime = pattern::find(m_client_dll, XOR("F3 0F 5C 88 ? ? ? ? 0F")).add(4).to< size_t >();
	if (!SpawnTime)
		return false;

	IsLocalPlayer = pattern::find(m_client_dll, XOR("74 ? 8A 83 ? ? ? ? 88")).add(4).to< size_t >();
	if (!IsLocalPlayer)
		return false;

	PlayerAnimState = pattern::find(m_client_dll, XOR("8B 8E ? ? ? ? 85 C9 74 3E")).add(2).to< size_t >();
	if (!PlayerAnimState)
		return false;

	studioHdr = pattern::find(m_client_dll, XOR("8B 86 ? ? ? ? 89 44 24 10 85 C0")).add(2).to< size_t >();
	if (!studioHdr)
		return false;

	UTIL_TraceLine = pattern::find(m_client_dll, XOR("55 8B EC 83 E4 F0 83 EC 7C 56 52"));
	if (!UTIL_TraceLine)
		return false;

	CTraceFilterSimple_vmt = UTIL_TraceLine.add(0x3D).to();
	if (!CTraceFilterSimple_vmt)
		return false;

	CTraceFilterSkipTwoEntities_vmt = pattern::find(m_client_dll, XOR("E8 ? ? ? ? F3 0F 10 84 24 ? ? ? ? 8D 84 24 ? ? ? ? F3 0F 58 44 24")).rel32(1).add(0x59).to();
	if (!CTraceFilterSkipTwoEntities_vmt)
		return false;

	InvalidateBoneCache = pattern::find(m_client_dll, XOR("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"));
	if (!InvalidateBoneCache)
		return false;

	ModelBoneCounter = *g_Csgo.InvalidateBoneCache.add(0xA).to< size_t* >();
	if (!ModelBoneCounter)
		return false;

	LastBoneSetupTime = InvalidateBoneCache.add(0x11).to< size_t >();
	if (!LastBoneSetupTime)
		return false;

	MostRecentModelBoneCounter = InvalidateBoneCache.add(0x1B).to< size_t >();
	if (!MostRecentModelBoneCounter)
		return false;
#endif
	// some weird tier0 stuff that prevents me from debugging properly...
#ifdef _DEBUG
	Address debugbreak = pattern::find(g_Csgo.m_client_dll, XOR("CC F3 0F 10 4D ? 0F 57 C0"));

	DWORD old;
	VirtualProtect(debugbreak, 1, PAGE_EXECUTE_READWRITE, &old);

	debugbreak.set< uint8_t >(0x90);

	VirtualProtect(debugbreak, 1, old, &old);
#endif
	new_render.Init();
	g_Config.Init();

#ifdef LEGACY
	g_Chams.GenerateMaterials();
	g_Netvars.Init();
	g_Offsets.Init();
	g_skins.Run();
#endif
	g_Hooks.Init();
#ifdef LEGACY
	g_Listener.Init();
	g_ListenerEntity.Init();
	g_Detours.Init();
#endif

	VM_DIFF_END;
	return true;
}

bool game::IsBreakable(Entity* ent) {
	bool        ret;
	ClientClass* cc;
	const char* name;
	char* takedmg, old_takedmg;

	static size_t m_takedamage_offset{ *(size_t*)((uintptr_t)g_Csgo.IsBreakableEntity + 38) };

	// skip null ents and the world ent.
	if (!ent || !ent->GetClientNetworkable() || ent->GetClientNetworkable()->GetIndex() == 0)
		return false;

	// get m_takedamage and save old m_takedamage.
	takedmg = (char*)((uintptr_t)ent + m_takedamage_offset);
	old_takedmg = *takedmg;

	// get clientclass.
	cc = ent->GetClientNetworkable()->GetClientClass();

	if (cc) {
		// get clientclass network name.
		name = cc->m_pNetworkName;

		// CBreakableSurface, CBaseDoor, ...
		if (name[1] != 'F'
			|| name[4] != 'c'
			|| name[5] != 'B'
			|| name[9] != 'h') {
			*takedmg = DAMAGE_YES;
		}
	}

	ret = g_Csgo.IsBreakableEntity(ent);
	*takedmg = old_takedmg;

	return ret;
}

int game::HitboxToHitgroup(Hitboxes_t index) {
	switch (index) {
	case Hitboxes_t::HEAD:
		return (int)Hitgroups_t::HEAD;

	case Hitboxes_t::NECK:
	case Hitboxes_t::LOWER_NECK:
		return (int)Hitgroups_t::NECK;

	case Hitboxes_t::PELVIS:
	case Hitboxes_t::BODY:
		return (int)Hitgroups_t::STOMACH;

	case Hitboxes_t::THORAX:
	case Hitboxes_t::CHEST:
	case Hitboxes_t::UPPER_CHEST:
		return (int)Hitgroups_t::CHEST;

	case Hitboxes_t::L_HAND:
	case Hitboxes_t::L_UPPER_ARM:
	case Hitboxes_t::L_FOREARM:
		return (int)Hitgroups_t::LEFTARM;

	case Hitboxes_t::R_HAND:
	case Hitboxes_t::R_UPPER_ARM:
	case Hitboxes_t::R_FOREARM:
		return (int)Hitgroups_t::RIGHTARM;

	case Hitboxes_t::L_THIGH:
	case Hitboxes_t::L_CALF:
		return (int)Hitgroups_t::LEFTLEG;

	case Hitboxes_t::R_THIGH:
	case Hitboxes_t::R_CALF:
		return (int)Hitgroups_t::RIGHTLEG;

	case Hitboxes_t::L_FOOT:
		return (int)Hitgroups_t::LEFTFOOT;

	case Hitboxes_t::R_FOOT:
		return (int)Hitgroups_t::RIGHTFOOT;

	default:
		return (int)Hitgroups_t::GENERIC;
	}
}