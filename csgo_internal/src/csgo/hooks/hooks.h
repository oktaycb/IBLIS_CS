#pragma once

class Hooks {
public:
	void Init();
	void RemoveHook();

public:
	// forward declarations
	class IRecipientFilter;

	// prototypes.
	using ColorModulate_t = void(__thiscall*)(void*, float, float, float);
	using TraceRay_t = void(__thiscall*)(void*, const Ray&, unsigned int, ITraceFilter*, trace_t*);
	using ClipRayToCollideable_t = void(__thiscall*)(void*, const Ray&, uint32_t, ICollideable*, trace_t*);
	using PaintTraverse_t = void(__thiscall*)(void*, vgui::VPANEL, bool, bool);
	using DoPostScreenSpaceEffects_t = bool(__thiscall*)(void*, CViewSetup*);
	using CreateMove_t = void(__thiscall*)(void*, int, float, bool);
	using LevelInitPostEntity_t = void(__thiscall*)(void*);
	using LevelShutdown_t = void(__thiscall*)(void*);
	using LevelInitPreEntity_t = void(__thiscall*)(void*, const char*);
	using IN_KeyEvent_t = int(__thiscall*)(void*, int, int, const char*);
	using FrameStageNotify_t = void(__thiscall*)(void*, Stage_t);
	using UpdateClientSideAnimation_t = void(__thiscall*)(void*);
	using StandardBlendingRules_t = void(__thiscall*)(void*, int, int, int, int, int);
	using ShouldInterpolate_t = bool(__thiscall*)(void*);
	using IsSelfAnimating_t = bool(__thiscall*)(void*);
	using IsPlayer_t = bool(__thiscall*)(void*);
	using IsFollowingEntity_t = bool(__thiscall*)(void*);
	using AccumulateLayers_t = void(__thiscall*)(void*, void*, Vector[], Quaternion[], float);
	using PhysicsSimulate_t = void(__thiscall*)(void*);
	using EstimateAbsVelocity_t = void(__thiscall*)(void*, Vector&);
	using Simulate_t = bool(__thiscall*)(void*);
	using SetupBones_t = bool(__thiscall*)(void*, matrix3x4a_t*, int, int, float);
	using Interpolate_t = bool(__thiscall*)(void*, float);
	using GetActiveWeapon_t = Weapon * (__thiscall*)(void*);
	using CalcView_t = void(__thiscall*)(void*, Vector&, Vector&, float&, float&, float&);
	using SetPlayerUnderwater_t = void(__thiscall*)(void*, bool);
	using CalcAddViewmodelCameraAnimation_t = void(__thiscall*)(void*, Vector&, QAngle&);
	using Weapon_ShootPosition_t = Vector&(__thiscall*)(void*, Vector&);
	using EyePosition_t = Vector&(__thiscall*)(void*, Vector&);
	using DoExtraBoneProcessing_t = void(__thiscall*)(void*, int, int, int, int, int, int);
	using SetupMove_t = void(__thiscall*)(void*, Entity*, Command*, IMoveHelper*, CMoveData*);
	using ProcessMovement_t = void(__thiscall*)(void*, Entity*, CMoveData*);
	using OnJump_t = void(__thiscall*)(void*, float);
	using BuildTransformations_t = void(__thiscall*)(void*, CStudioHdr*, int, int, int, int, int);
	using InPrediction_t = bool(__thiscall*)(void*);
	using OverrideView_t = void(__thiscall*)(void*, CViewSetup*);
	using LockCursor_t = void(__thiscall*)(void*);
	using RunCommand_t = void(__thiscall*)(void*, Entity*, Command*, IMoveHelper*);
	using FinishMove_t = void(__thiscall*)(void*, Entity*, Command*, CMoveData*);
	using Update_t = void(__thiscall*)(void*, int, bool, int, int);
	using ProcessPacket_t = void(__thiscall*)(void*, void*, bool);
	using SendDatagram_t = int(__thiscall*)(void*, void*);
	using SendNetMsg_t = bool(__thiscall*)(void*, INetMessage&, bool, bool);
	using SetChoked_t = void(__thiscall*)(void*);
	using PlaySound_t = void(__thiscall*)(void*, const char*);
	using GetScreenSize_t = void(__thiscall*)(void*, int&, int&);
	using Push3DView_t = void(__thiscall*)(void*, CViewSetup&, int, void*, void*);
	using DrawModelExecute_t = void(__thiscall*)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	using ComputeShadowDepthTextures_t = void(__thiscall*)(void*, const CViewSetup&, bool);
	using GetInt_t = int(__thiscall*)(void*);
	using GetBool_t = bool(__thiscall*)(void*);
	using IsConnected_t = bool(__thiscall*)(void*);
	using IsHLTV_t = bool(__thiscall*)(void*);
	using IsPaused_t = bool(__thiscall*)(void*);
	using IsPlayingTimeDemo_t = bool(__thiscall*)(void*);
	using OnEntityCreated_t = void(__thiscall*)(void*, Entity*);
	using OnEntityDeleted_t = void(__thiscall*)(void*, Entity*);
	using RenderSmokeOverlay_t = void(__thiscall*)(void*, bool);
	using ShouldDrawFog_t = bool(__thiscall*)(void*);
	using Render2DEffectsPostHUD_t = void(__thiscall*)(void*, const CViewSetup&);
	using OnRenderStart_t = void(__thiscall*)(void*);
	using FireEvents_t = void(__thiscall*)(void*);
	using OverrideConfig_t = bool(__thiscall*)(void*, MaterialSystem_Config_t*, bool);
	using TempEntities_t = bool(__thiscall*)(void*, void*);
	using PacketStart_t = void(__thiscall*)(void*, int, int); 
	using PacketEnd_t = void(__thiscall*)(void*);
	using EmitSound_t = void(__thiscall*)(void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, float, int, int, int, const Vector*, const Vector*, void*, bool, float, int);
	using ListLeavesInBox_t = int(__thiscall*)(void*, Vector&, Vector&, unsigned short*, int);
	using WriteUsercmdDeltaToBuffer_t = bool(__thiscall*)(void*, int32_t, WriteBF*, int32_t, int32_t, bool);

public:
	int						ListLeavesInBox(Vector& mins, Vector& maxs, unsigned short* pList, int listMax);
	void					TraceRay(const Ray& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace);
	void					ClipRayToCollideable(const Ray& ray, unsigned int fMask, ICollideable* pCollide, trace_t* pTrace);
	void					PacketStart(int m_iIncomingSequence, int m_iOutgoingAcknowledged);
	void					PacketEnd();
	bool                    SVCMsg_TempEntities(void* msg);
	void                    PaintTraverse(vgui::VPANEL panel, bool repaint, bool force);
	bool                    DoPostScreenSpaceEffects(CViewSetup* setup);
	void                    CreateMove(int iSequence, float flFrametime, bool bIsActive, bool& bSendPacket);
	void __stdcall			CreateMove_Proxy(int iSequence, float flFrameTime, bool bIsActive);
	void                    LevelInitPostEntity();
	void                    LevelShutdown();
	void                    LevelInitPreEntity(const char* map);
	void                    FrameStageNotify(Stage_t stage);
	bool					WriteUsercmdDeltaToBuffer(int32_t iSlot, WriteBF* pBuffer, int32_t iFrom, int32_t iTo, bool bNewCmd);
	void                    UpdateClientSideAnimation();
	void					CalcView(Vector& eyeOrigin, Vector& eyeAngles, float& zNear, float& zFar, float& fov);
	Weapon*					GetActiveWeapon();
	bool					ShouldInterpolate();
	Vector&					Weapon_ShootPosition(Vector& vecPos);
	Vector&					EyePosition(Vector& vecPos);
	bool					IsSelfAnimating();
	void					AccumulateLayers(void* boneSetup, Vector pos[], Quaternion q[], float currentTime);
	bool					IsFollowingEntity();
	void					PhysicsSimulate();
	bool                    InPrediction();
	bool                    ShouldDrawFog();
	void                    OverrideView(CViewSetup* view);
	void                    PlaySound(const char* name);
	void					SetupMove(Entity* ent, Command* cmd, IMoveHelper* helper, CMoveData* data);
	void				    OnJump(float stamina);
	void                    ProcessMovement(Entity* ent, CMoveData* data);
	void                    RunCommand(Entity* ent, Command* cmd, IMoveHelper* helper);
	void					FinishMove(Entity* pEntity, Command* pCommands, CMoveData* pMoveHelper);
	void					Update(int iStartFrame, bool bValidFrame, int iIncomingAcknowledged, int iOutGoingCommand);
	void					SetChoked();
	int                     SendDatagram(void* data);
	bool					SendNetMsg(INetMessage& msg, bool bForceReliable, bool bVoice);
	void                    ProcessPacket(void* packet, bool header);
	void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone);
	void                    ComputeShadowDepthTextures(const CViewSetup& view, bool unk);
	bool					NetShowFragmentsGetBool();
	int                     CsmShadowGetInt();
	void                    DoExtraBoneProcessing(int a2, int a3, int a4, int a5, int a6, int a7);
	void                    BuildTransformations(CStudioHdr* hdr, int a3, int a4, int a5, int a6, int a7);
	bool                    SetupBones(matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	bool                    Interpolate(float currentTime);
	void					FireEvents();
	bool                    IsConnected();
	bool                    IsHLTV();
	bool                    IsPaused();
	bool					IsPlayingTimeDemo();
	void                    EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);
	void                    RenderSmokeOverlay(bool unk);
	void                    OnRenderStart();
	void                    Render2DEffectsPostHUD(const CViewSetup& setup);
	bool                    OverrideConfig(MaterialSystem_Config_t* config, bool update);
	void                    PostDataUpdate(DataUpdateType_t type);
	void                    StandardBlendingRules(int a2, int a3, int a4, int a5, int a6);
	static HRESULT WINAPI   Present(IDirect3DDevice9* device, const RECT* src, const RECT* dst, HWND dst_window_override, const RGNDATA* dirty_region);
	static HRESULT WINAPI   Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);
	static LRESULT WINAPI   WndProc(HWND wnd, uint32_t msg, WPARAM wp, LPARAM lp);
public:
	// vmts.
	VMT m_Panel;
	VMT m_ClientMode;
	VMT m_Client;
	VMT m_ClientState;
	VMT m_Engine;
	VMT m_EngineSound;
	VMT m_Prediction;
	VMT m_BSP;
	VMT m_GameMovement;
	VMT m_Surface;
	VMT m_render;
	VMT m_NetChannel;
	VMT m_MessageHandler;
	VMT m_ModelRender;
	VMT m_ClientShadowMgr;
	VMT m_ViewRender;
	VMT m_MaterialSystem;
	VMT m_cl_csm_shadows;
	VMT m_net_show_fragments;
	VMT m_Device;
	VMT m_EngineTrace;

	// player.
	std::array< VMT, 64 > m_Player;
	std::array< VMT, 64 > m_Renderable;
	std::array< VMT, 64 > m_ViewModel;

	// wndproc old ptr.
	WNDPROC m_old_wndproc;

	// old player create fn.
	DoExtraBoneProcessing_t     m_DoExtraBoneProcessing;
	UpdateClientSideAnimation_t m_UpdateClientSideAnimation;
	CalcView_t                  m_CalcView;
	SetPlayerUnderwater_t       m_SetPlayerUnderwater;
	CalcAddViewmodelCameraAnimation_t m_CalcAddViewmodelCameraAnimation;
	Weapon_ShootPosition_t      m_Weapon_ShootPosition;
	EyePosition_t				m_EyePosition;
	GetActiveWeapon_t           m_GetActiveWeapon;
	BuildTransformations_t      m_BuildTransformations;
	StandardBlendingRules_t     m_StandardBlendingRules;
	ShouldInterpolate_t			m_ShouldInterpolate;
	IsSelfAnimating_t			m_IsSelfAnimating;
	IsPlayer_t					m_IsPlayer;
	PhysicsSimulate_t			m_PhysicsSimulate;
	Simulate_t					m_Simulate;
	EstimateAbsVelocity_t		m_EstimateAbsVelocity;
	AccumulateLayers_t			m_AccumulateLayers;
	IsFollowingEntity_t			m_IsFollowingEntity;

	// renderable create fn.
	SetupBones_t m_SetupBones;

	// view model create fn.
	Interpolate_t m_Interpolate;

	// netvar proxies.
	RecvVarProxy_t m_Pitch;
	RecvVarProxy_t m_Body;
	RecvVarProxy_t m_Force;
	RecvVarProxy_t m_AbsYaw;
	RecvVarProxy_t m_VelocityModifier;
	RecvVarProxy_t m_ClientSideAnimation;
	RecvVarProxy_t m_ReceivedFlashDuration;
};

// note - dex; these are defined in player.cpp.
class CustomEntityListener : public IClientEntityListener {
public:
	virtual void OnEntityCreated(Entity* ent) override;
	virtual void OnEntityDeleted(Entity* ent) override;

	__forceinline void Init() {
		g_Csgo.m_EntityList->AddListenerEntity(this);
	}

	__forceinline void RemoveListenerEntity() {
		g_Csgo.m_EntityList->RemoveListenerEntity(this);
	}
};

inline Hooks                g_Hooks;
inline CustomEntityListener g_ListenerEntity;