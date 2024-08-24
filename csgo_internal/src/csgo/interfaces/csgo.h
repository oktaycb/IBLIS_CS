#pragma once

// predeclares.
class CCSGOPlayerAnimState;
class C_CSGameRules;

class CSGO {
public:
	using TraceToExit_t = bool(__fastcall*)(Vector&, CGameTrace&, float, float, float, float, float, float, CGameTrace*);
	using MD5_PseudoRandom_t = uint32_t(__thiscall *)(uint32_t);
	using GetGlowObjectManager_t = CGlowObjectManager * (__cdecl *)();
	using RandomSeed_t = void(__cdecl *)(int);
	using RandomInt_t = int(__cdecl *)(int, int);
	using RandomFloat_t = float(__cdecl *)(float, float);
	using IsBreakableEntity_t = bool(__thiscall *)(Entity *);
	using AngleMatrix_t = void(__fastcall *)(const QAngle &, matrix3x4_t &);
	using LoadFromBuffer_t = bool(__thiscall *)(KeyValues *, const char *, const char *, void *, void *, void *);
	using ServerRankRevealAll_t = bool(__cdecl *)(CCSUsrMsg_ServerRankRevealAll *);
	using IsReady_t = void(__cdecl *)();
	using ShowAndUpdateSelection_t = void(__thiscall *)(CHudElement *, int, Weapon *, bool);
	using GetEconItemView_t = C_EconItemView * (__thiscall *)(Weapon *);
	using GetStaticData_t = CEconItemDefinition * (__thiscall *)(C_EconItemView *);
	using ConcatTransforms_t = void(__fastcall *)();
	using BeamAlloc_t = Beam_t * (__thiscall *)(IViewRenderBeams *, bool);
	using SetupBeam_t = void(__stdcall *)(Beam_t *, const BeamInfo_t &);
	using ClearNotices_t = void(__thiscall *)(KillFeed_t *);
	using GetShotgunSpread_t = void(__stdcall *)(int, int, int, float *, float *);
	using UpdateAnimationState_t = void(__vectorcall*)(CCSGOPlayerAnimState*, void*, float, float, float, bool);
	using ResetAnimationState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	using ModifyEyePosition_t = void(__thiscall*)(CCSGOPlayerAnimState*, void*, Vector&);
	using Simulate_t = void(__thiscall*)(void*, float);
	using ClientThink_t = void(__thiscall*)(void*);

public:

	// module objects.
	PE::Module m_kernel32_dll;
	PE::Module m_user32_dll;
	PE::Module m_shell32_dll;
	PE::Module m_shlwapi_dll;
	PE::Module m_client_dll;
	PE::Module m_engine_dll;
	PE::Module m_materialsystem_dll;
	PE::Module m_server_dll;
	PE::Module m_vstdlib_dll;
	PE::Module m_tier0_dll;
	PE::Module m_serverbrowser_dll;
	PE::Module m_datacache_dll;
	PE::Module m_shaderapidx9_dll;

public:
	// interface ptrs.
	IDirect3DDevice9 *m_Device;
	CHLClient *m_Client;
	ICvar *m_CVar;
	IVEngineClient *m_Engine;
	IClientEntityList *m_EntityList;
	IInputSystem *m_InputSystem;
	ISurface *m_Surface;
	IPanel *m_Panel;
	IEngineVGui *m_EngineVGui;
	CPrediction *m_Prediction;
	IEngineTrace *m_EngineTrace;
	CGameMovement *m_GameMovement;
	IVRenderView *m_RenderView;
	CViewRender *m_ViewRender;
	IVModelRender *m_ModelRender;
	IMaterialSystem *m_MaterialSystem; 
	CStudioRenderContext *m_StudioRender;
	IVModelInfo *m_ModelInfo;
	IVDebugOverlay *m_DebugOverlay;
	IPhysicsSurfaceProps *m_PhysProps;
	IGameEventManager2 *m_GameEvents;
	ILocalize *m_Localize;
	INetworkStringTableContainer *m_NetworkStringTable;
	IEngineSound *m_EngineSound;

	IClientMode *m_ClientMode;
	CGlobalVarsBase *m_Globals;
	IMDLCache* m_MdlCache;
	CInput *m_Input;
	IMoveHelper *m_MoveHelper;
	INetChannel *m_NetChannelInfo;

	CGlowObjectManager *m_GlowObjectManager;
	CClientState *m_ClientState;
	void* m_HookableClientState; // todo: change it.
	CGame *m_Game;
	IMemAlloc *m_MemAlloc;
	IClientShadowMgr *m_ClientShadowMgr;
	// IClientEntityListener** m_entity_listeners;
	CHud *m_Hud;
	C_CSGameRules* m_GameRules;
	IViewRenderBeams *m_Beams;
	CPlayerResource* m_PlayerResource;
	void *m_Radar;

public:
	// convars.
	ConVar* m_pitch;
	ConVar* m_yaw;
	ConVar* sensivity;
	ConVar* m_MaxUserCommandProcessTicks;
	ConVar* cl_extrapolate;
	ConVar *clear;
	ConVar *toggleconsole;
	ConVar *name;
	ConVar *sv_maxunlag;
	ConVar *m_Gravity;
	ConVar *sv_jump_impulse;
	ConVar* sv_autobunnyhopping;
	ConVar* sv_enablebunnyhopping;
	ConVar* sv_airaccelerate;
	ConVar* sv_cheats;
	ConVar* sv_noclipduringpause;
	ConVar* sv_friction;
	ConVar* sv_stopspeed;
	ConVar* sv_maxspeed;
	ConVar* sv_client_min_interp_ratio;
	ConVar* sv_client_max_interp_ratio;
	ConVar* sv_minupdaterate;
	ConVar* sv_maxupdaterate;
	ConVar* cl_interp;
	ConVar* cl_interp_ratio;
	ConVar* cl_interpolate;
	ConVar *cl_updaterate;
	ConVar *cl_cmdrate;
	ConVar *cl_lagcompensation;
	ConVar* cl_foot_contact_shadows;
	ConVar *mp_teammates_are_enemies;
	ConVar *weapon_debug_spread_show;
	ConVar *net_showfragments;
	ConVar* cl_csm_shadows;
	ConVar *molotov_throw_detonate_time;
	ConVar* host_timescale;
	ConVar *weapon_molotov_maxdetonateslope;
	ConVar *weapon_recoil_scale;
	ConVar *view_recoil_tracking;
	ConVar *cl_fullupdate;
	ConVar *r_DrawSpecificStaticProp;
	ConVar *cl_crosshair_sniper_width;
	ConVar *hud_scaling;
	ConVar *sv_clip_penetration_traces_to_players;
	ConVar *weapon_accuracy_shotgun_spread_patterns;
	ConVar* weapon_accuracy_nospread;
	ConVar* sv_party_mode;
	ConVar* r_aspectratio;
	ConVar* cl_camera_height_restriction_debug;
	ConVar* sv_penetration_type;
	ConVar* ff_damage_bullet_penetration;
	ConVar* ff_damage_reduction_bullets;
	ConVar* cl_pred_optimize;
	ConVar* cl_predict;
	ConVar* sv_clockcorrection_msecs;
	ConVar* cl_SetupAllBones;

	ConVar* m_RainSpeed;
	ConVar* m_RainLength;
	ConVar* m_RainWidth;
	ConVar* m_RainDensity;
	ConVar* m_RainAlpha;
	ConVar* m_RainSideVel;

public:
	// functions.
	GetGlowObjectManager_t   GetGlowObjectManager;
	MD5_PseudoRandom_t	     MD5_PseudoRandom;
	TraceToExit_t			 TraceToExit;
	Simulate_t				 CClient_Precipitation_Simulate;
	ClientThink_t			 CClient_Precipitation_ClientThink;
	Address                  SetAbsAngles;
	Address					 DisablePostProcess;
	Address				     SetAbsOrigin;
	Address                  InvalidateBoneCache;
	Address                  CacheBoneData;
	Address				     LockStudioHdr;
	RandomSeed_t             RandomSeed;
	RandomInt_t              RandomInt;
	RandomFloat_t            RandomFloat;
	IsBreakableEntity_t      IsBreakableEntity;
	Address	                 SetAbsVelocity;
	Address	                 CalcAbsoluteVelocity;
	AngleMatrix_t            AngleMatrix;
	Address					 DoProceduralFootPlant;
	Address					 ComputeHitboxSurroundingBox;
	Address					 GetSequenceActivity;
	Address					 AttachmentHelper;
	LoadFromBuffer_t		 LoadFromBuffer;
	ServerRankRevealAll_t    ServerRankRevealAll;
	Address					 InvalidatePhysicsRecursive;
	Address					 Reinitialize;
	Address					 IndexModelSequences;
	Address					 ActivityListVersion;
	IsReady_t				 IsReady;
	ShowAndUpdateSelection_t ShowAndUpdateSelection;
	GetEconItemView_t        GetEconItemView;
	GetStaticData_t          GetStaticData;
	Address					 TEFireBullets;
	BeamAlloc_t              BeamAlloc;
	SetupBeam_t              SetupBeam;
	ClearNotices_t           ClearNotices;
	GetShotgunSpread_t       GetShotgunSpread;

	Address	m_nCachedBonesPosition;
	Address	m_nCachedBonesRotation;

	uintptr_t	Interpolate;
	uintptr_t	UpdateClientSideAnimation;
	uintptr_t	UpdateClientSideAnimations;
	uintptr_t	StudioFrameAdvance;
	uintptr_t	IsUsingStaticPropDebugModes;
	uintptr_t	ColorModulate;
	uintptr_t	AlphaModulate;
	uintptr_t	ReportHit;
	uintptr_t	CL_Move;
	uintptr_t	IsSelfAnimating;
	uintptr_t	AddBaseAnimatingInterpolatedVars;
	uintptr_t   UpdateAnimationState;
	uintptr_t   ModifyEyePosition;
	uintptr_t   CLCMsg_Move;
	uintptr_t   ResetAnimationState;
	uintptr_t   ShouldSkipAnimationFrame;
	uintptr_t   CheckForSequenceChange;

	size_t BoneSetupLock;
	size_t BoneAccessor;
	size_t AnimOverlay;
	size_t SpawnTime;
	size_t MostRecentModelBoneCounter;
	size_t ModelBoneCounter;
	size_t LastBoneSetupTime;
	size_t IsLocalPlayer;
	size_t PlayerAnimState;
	size_t studioHdr;

	Address UTIL_TraceLine;
	Address CTraceFilterSimple_vmt;
	Address CTraceFilterSkipTwoEntities_vmt;

	int *m_nPredictionRandomSeed;
	Player *m_pPredictionPlayer;

public:
	HWND m_Window;

	void SetupModulesList();
	bool SetupInterfaceList();
	void SetupConvarList();
	bool SetupSignatureList();
	bool SetupVirtualList();
	bool SetupFunctionList();

	bool Init();
};

inline CSGO g_Csgo;

enum class Hitboxes_t : int {
	INVALID = -1,
	HEAD,
	NECK,
	LOWER_NECK,
	PELVIS,
	BODY,
	THORAX,
	CHEST,
	UPPER_CHEST,
	R_THIGH,
	L_THIGH,
	R_CALF,
	L_CALF,
	R_FOOT,
	L_FOOT,
	R_HAND,
	L_HAND,
	R_UPPER_ARM,
	R_FOREARM,
	L_UPPER_ARM,
	L_FOREARM,
	MAX
};

namespace game {
	__forceinline float GetClientInterpAmount() {
		if (g_Csgo.cl_interpolate->GetInt() == 0)
			return 0.0f;

		static const ConVar* pMin = g_Csgo.sv_client_min_interp_ratio;
		static const ConVar* pMax = g_Csgo.sv_client_max_interp_ratio;

		static const ConVar* pMinUpdateRate = g_Csgo.sv_minupdaterate;
		static const ConVar* pMaxUpdateRate = g_Csgo.sv_maxupdaterate;

		float flLerpRatio = g_Csgo.cl_interp_ratio->GetFloat();
		if (flLerpRatio == 0)
			flLerpRatio = 1.0f;

		float flLerpAmount = g_Csgo.cl_interp->GetFloat();

		if (pMin && pMax && pMin->GetFloat() != -1)
			flLerpRatio = std::clamp(flLerpRatio, pMin->GetFloat(), pMax->GetFloat());
		else if (flLerpRatio == 0.f)
			flLerpRatio = 1.0f;

		float flUpdateRate = g_Csgo.cl_updaterate->GetFloat();

		if (pMinUpdateRate && pMaxUpdateRate)
			flUpdateRate = std::clamp(flUpdateRate, pMinUpdateRate->GetFloat(), pMaxUpdateRate->GetFloat());

		return std::max(flLerpAmount, flLerpRatio / flUpdateRate);
	}

	__forceinline int TIME_TO_TICKS(float time) {
		return (int)(0.5f + time / g_Csgo.m_Globals->m_Interval);
	}

	__forceinline float TICKS_TO_TIME(int ticks) {
		return g_Csgo.m_Globals->m_Interval * (float)(ticks);
	}

	__forceinline Vector TICKS_TO_TIME(Vector ticks) {
		return g_Csgo.m_Globals->m_Interval * (Vector)(ticks);
	}

	__forceinline bool IsFakePlayer(int index) {
		player_info_t info;
		if (g_Csgo.m_Engine->GetPlayerInfo(index, &info))
			return info.m_fake_player;

		return false;
	}

	__forceinline bool IsValidHitgroup(int index) {
		if ((index >= static_cast<int>(Hitgroups_t::HEAD) && index <= static_cast<int>(Hitgroups_t::RIGHTLEG)) || index == static_cast<int>(Hitgroups_t::GEAR))
			return true;

		return false;
	}

	__forceinline bool UTIL_TraceToExit(Vector& penetrationEnd, CGameTrace& trEnter, float start_x, float start_y, float start_z, float dir_z, float dir_y, float dir_x, CGameTrace* trExit) {
		static auto func = g_Csgo.TraceToExit;
		if (!func)
			return false;

		_asm {
			push trExit
			push dir_z
			push dir_y
			push dir_x
			push start_z
			push start_y
			push start_x
			mov edx, trEnter
			mov ecx, penetrationEnd
			call func
			add esp, 0x1C
		}
	}

	__forceinline void UTIL_ClipTraceToPlayers(const Vector &start, const Vector &end, uint32_t mask, ITraceFilter *filter, CGameTrace *tr, float range = 0) {
		static auto func = pattern::find(g_Csgo.m_client_dll, XOR("E8 ? ? ? ? 83 C4 14 8A 56 37")).rel32< uintptr_t >(0x1);
		if (!func)
			return;

		__asm {
			mov  ecx, start
			mov	 edx, end
			push range
			push tr
			push filter
			push mask
			call func
			add	 esp, 16
		}
	}

	__forceinline void ConcatTransforms(const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &output) {
		static auto func = pattern::find(g_Csgo.m_client_dll, XOR("E8 ? ? ? ? 0F 28 44 24")).rel32< uintptr_t >(0x1);
		if (!func)
			return;

		__asm {
			push output
			lea  edx, in2
			lea  ecx, in1
			call func
			add  esp, 4
		}
	}

	template<class T = DWORD>
	__forceinline T* FindHudElement(const char* name) {
		static auto pThis = *pattern::find(g_Csgo.m_client_dll, XOR("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")).add(0x1).as< DWORD**>();

		static auto FindHudElement = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39")).as<DWORD(__thiscall*)(void*, const char*)>();
		return (T*)FindHudElement(pThis, name);
	}

	__forceinline void SetPostProcess(bool enable) {
		// get post process address
		static bool* disable_post_process = *g_Csgo.DisablePostProcess.as<bool**>();

		// set it.
		*disable_post_process = enable;
	}

	bool IsBreakable(Entity* ent);
	int	HitboxToHitgroup(Hitboxes_t index);
}