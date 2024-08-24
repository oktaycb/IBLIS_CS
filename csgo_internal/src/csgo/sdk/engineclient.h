#pragma once

#define NET_FRAMES_BACKUP	128		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)

class INetChannel {
public:
	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	enum {
		FLOW_OUTGOING = 0,
		FLOW_INCOMING = 1,
		MAX_FLOWS = 2
	};

	enum indices : size_t {
		GETLATENCY = 9,
		GETAVGLATENCY = 10,
		PROCESSPACKET = 41,
		SENDNETMSG = 42,
		SETCHOKED = 47,
		SENDDATAGRAM = 48,
		GETDROPNUMBER = 52,
	};

public:
	__forceinline int GetDropNumber() {
		return util::get_method< int(__thiscall*)(decltype(this)) >(this, GETDROPNUMBER)(this);
	}

	__forceinline float GetLatency(int flow) {
		return util::get_method< float(__thiscall*)(decltype(this), int) >(this, GETLATENCY)(this, flow);
	}

	__forceinline float GetAvgLatency(int flow) {
		return util::get_method< float(__thiscall*)(decltype(this), int) >(this, GETAVGLATENCY)(this, flow);
	}


	__forceinline bool SendNetMsg(void* msg, bool rel = false, bool voice = false) {
		return util::get_method< bool(__thiscall*)(decltype(this), void*, bool, bool) >(this, SENDNETMSG)(this, msg, rel, voice);
	}

	__forceinline int SendDatagram(void* data = nullptr)
	{
		return util::get_method< int(__thiscall*)(decltype(this), void*) >(this, SENDDATAGRAM)(this, data);
	}

private:
	PAD(0x14);

public:
	bool m_ProcessingMessages;		// 0x0014
	bool m_ShouldDelete;			// 0x0015
private:
	PAD(0x2);
public:
	int	m_OutSequenceNr;				// 0x0018 last send outgoing sequence number
	int	m_InSequence;				// 0x001C last received incoming sequnec number
	int	m_OutSequenceAck;			// 0x0020 last received acknowledge outgoing sequnce number
	int m_OutReliableState;			// 0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int	m_InReliableState;			// 0x0028 state of incoming reliable data
	int	m_ChokedPackets;			// 0x002C number of choked packets

private:
	PAD(0x414);					// 0x0030
};

class ReadBF;
class CNetMessage
{
public:
	virtual	~CNetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel* netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(ReadBF& buffer) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(void* buffer) = 0;	// returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char* GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel* GetNetChannel(void) const = 0;
	virtual const char* ToString(void) const = 0; // returns a human readable string about message content
};

class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets(int flow) const = 0;
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

class INetMessage {
public:
	virtual ~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void SetNetChannel(INetChannel* netchan) = 0; // netchannel this message is from/for
	virtual void SetReliable(bool state) = 0;             // set to true if it's a reliable message

	virtual bool Process(void) = 0; // calles the recently set handler to process this message

	virtual bool ReadFromBuffer(void*& buffer) = 0; // returns true if parsing was OK
	virtual bool WriteToBuffer(void*& buffer) = 0; // returns true if writing was OK

	virtual bool IsReliable(void) const = 0; // true, if message needs reliable handling

	virtual int GetType(void) const = 0;         // returns module specific header tag eg svc_serverinfo
	virtual int GetGroup(void) const = 0;        // returns net message group of this message
	virtual const char* GetName(void) const = 0; // returns network message name, eg "svc_serverinfo"
	virtual INetChannel* GetNetChannel(void) const = 0;
	virtual const char* ToString(void) const = 0; // returns a human readable string about message content
};

struct player_info_t {
	uint64_t      m_data_map;
	union {
		int64_t   m_xuid;
		struct {
			int   m_xuid_low;
			int   m_xuid_high;
		};
	};
	char          m_Name[128];
	int           m_user_id;
	char          m_guid[33];
	uint32_t      m_friends_id;
	char          m_friends_name[128];
	bool          m_fake_player;
	bool          m_is_hltv;
	uint32_t      m_custom_files[4];
	uint8_t       m_files_downloaded;
};

class CEventInfo {
public:
	enum {
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short					m_ClassID;
	float					m_FireDelay;
	const void* m_send_table;
	const ClientClass* m_client_class;
	int						m_bits;
	uint8_t* m_data;
	int						m_Flags;
	PAD(0x18);
	CEventInfo* m_Next;
};
class CClientState {
public:
	enum indices : size_t {
		PACKETSTART = 5,
		PACKETEND = 6,
		TEMPENTITIES = 36,
	};
#ifdef LEGACY
private:
	PAD(0x9C);                                // 0x0000

public:
	INetChannel* m_NetChannel;				// 0x009C

private:
	PAD(0x70);                                // 0x00A0

public:
	int				m_next_message_time;		// 0x0110

public:
	float           m_net_cmd_time;             // 0x0114
	uint32_t        m_server_count;             // 0x0118
private:
	PAD(0x4C);								// 0x011C

public:
	int             m_unk;                      // 0x0168
	int             m_ServerTick;              // 0x016C
	int             m_ClientTick;              // 0x0170
	int             m_DeltaTick;               // 0x0174

private:
	PAD(0x4B30);                              // 0x0178

public:
	float           m_frame_time;               // 0x4CA8
	int             m_LastOutgoingCommand;    // 0x4CAC
	int             m_ChokedCommands;          // 0x4CB0
	int             m_LastCommandAck;         // 0x4CB4
	uint32_t	    m_nSoundSequence;
	PAD(0x134);                               // 0x4CB8
	CEventInfo* m_Events;					// 0x4DEC
#else
private:
	PAD(0x9C);										// 0x0000

public:
	INetChannel*		m_NetChannel;				// 0x009C
	int					m_ChallengeNr;				// 0x00A0
private:
	PAD(0x4);										//0x00A4
public:
	double				m_ConnectTime;
	int					m_RetryNumber;
private:
	PAD(84);                //0x00A4
public:
	int					m_SignonState;            //0x0108
private:
	PAD(0x4);                //0x010C
public:
	float				m_NextCmdTime;            //0x0114
	int					m_ServerCount;            //0x0118
	int					m_CurrentSequence;        //0x011C
private:
	PAD(8);
public:
	struct {
		float			m_ClockOffsets[16];
		int				m_CurClockOffset;
		int				m_ServerTick;
		int				m_ClientTick;
	} m_ClockDriftMgr;
private:
	PAD(68);
public:
	int					m_DeltaTick;                //0x0174
private:
	PAD(4);
public:
	int					m_ViewEntity;            //0x0180
	int					m_PlayerSlot;            //0x0184
	bool				m_Paused;
private:
	PAD(3);
public:
	char				m_LevelName[260];
	char				m_LevelNameShort[40];
private:
	PAD(212);                //0x032C
public:
	int					m_MaxClients;            //0x0388
private:
	PAD(18836);            //0x038C
public:
	float				m_LastServerTickTime;    //0x4D10
	bool				m_InSimulation;            //0x4D14
private:
	PAD(0xB);                //0x4D15
public:
	int					m_OldTickCount;
	float				m_TickRemainder;
	float				m_FrameTime;
	int					m_LastOutgoingCommand;
	int					m_ChokedCommands;
	int					m_LastCommandAck;
	int					m_LastServerTick;
	int					m_CommandAck;
	int					m_SoundSequence;
	int					m_LastProgressPercent;
	bool				m_IsHLTV;
private:
	PAD(75);                //0x4D38
public:
	Vector				m_ViewPoint;            //0x4D88
private:
	PAD(204);                //0x4D9A
public:
	CEventInfo*			m_Events;
#endif
};

class ISPSharedMemory;
class CGamestatsData;
class CSteamAPIContext;
struct Frustum_t;

typedef struct InputContextHandle_t__* InputContextHandle_t;
struct client_textmessage_t;
class model_t;
class SurfInfo;
class IMaterial;
class CSentence;
class CAudioSource;
class AudioState_t;
class ISpatialQuery;
class IMaterialSystem;
class CPhysCollide;
class IAchievementMgr;
class KeyValues;
class Color;
typedef void(*pfnDemoCustomDataCallback)(unsigned char* pData, size_t iSize);

class IVEngineClient {
public:
	enum indices : size_t {
		GETSCREENSIZE = 5,
		GETPLAYERINFO = 8,
		GETPLAYERFORUSERID = 9,
		GETLOCALPLAYER = 12,
		GETLASTTIMESTAMP = 14,
		GETVIEWANGLES = 18,
		SETVIEWANGLES = 19,
		//GETMAXCLIENTS          = 20,
		ISINGAME = 26,
		ISCONNECTED = 27,
		WORLDTOSCREENMATRIX = 37,
		FIREEVENTS = 59,
		GETNETCHANNELINFO = 78,
		ISPLAYINGDEMO = 82,
		ISPLAYINGTIMEDEMO = 84,
		ISPAUSED = 90,
		//ISTAKINGSCREENSHOT     = 92,
		ISHLTV = 93,
		//SETOCCLUSIONPARAMETERS = 96,
		EXECUTECLIENTCMD = 108,
	};

public:
	virtual int                   GetIntersectingSurfaces(const model_t *model, const Vector &vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo *pInfos, const int nMaxInfos) = 0;
	virtual Vector                GetLightForPoint(const Vector &pos, bool bClamp) = 0;
	virtual IMaterial*            TraceLineMaterialAndLighting(const Vector &start, const Vector &end, Vector &diffuseLightColor, Vector& baseColor) = 0;
	virtual const char*           ParseFile(const char *data, char *token, int maxlen) = 0;
	virtual bool                  CopyFile(const char *source, const char *destination) = 0;
	virtual void                  GetScreenSize(int& width, int& height) = 0;
	virtual void                  ServerCmd(const char *szCmdString, bool bReliable = true) = 0;
	virtual void                  ClientCmd(const char *szCmdString) = 0;
	virtual bool                  GetPlayerInfo(int ent_num, player_info_t *pinfo) = 0;
	virtual int                   GetPlayerForUserID(int userID) = 0;
	virtual client_textmessage_t* TextMessageGet(const char *pName) = 0; // 10
	virtual bool                  Con_IsVisible(void) = 0;
	virtual int                   GetLocalPlayer(void) = 0;
	virtual const model_t*        LoadModel(const char *pName, bool bProp = false) = 0;
	virtual float                 GetLastTimeStamp(void) = 0;
	virtual CSentence*            GetSentence(CAudioSource *pAudioSource) = 0; // 15
	virtual float                 GetSentenceLength(CAudioSource *pAudioSource) = 0;
	virtual bool                  IsStreaming(CAudioSource *pAudioSource) const = 0;
	virtual void                  GetViewAngles(QAngle& va) = 0; // 18
	virtual void                  SetViewAngles(QAngle& va) = 0; // 19
	virtual int                   GetMaxClients(void) = 0; // 20
	virtual   const char*         Key_LookupBinding(const char *pBinding) = 0;
	virtual const char*           Key_BindingForKey(int &code) = 0;
	virtual void                  Key_SetBinding(int, char const*) = 0;
	virtual void                  StartKeyTrapMode(void) = 0;
	virtual bool                  CheckDoneKeyTrapping(int &code) = 0;
	virtual bool                  IsInGame(void) = 0;
	virtual bool                  IsConnected(void) = 0;
	virtual bool                  IsDrawingLoadingImage(void) = 0;
	virtual void                  HideLoadingPlaque(void) = 0;
	virtual void                  Con_NPrintf(int pos, const char *fmt, ...) = 0; // 30
	virtual void                  Con_NXPrintf(const struct con_nprint_s *info, const char *fmt, ...) = 0;
	virtual int                   IsBoxVisible(const Vector& mins, const Vector& maxs) = 0;
	virtual int                   IsBoxInViewCluster(const Vector& mins, const Vector& maxs) = 0;
	virtual bool                  CullBox(const Vector& mins, const Vector& maxs) = 0;
	virtual void                  Sound_ExtraUpdate(void) = 0;
	virtual const char*           GetGameDirectory(void) = 0;
	virtual const VMatrix&        WorldToScreenMatrix() = 0;
	virtual const VMatrix&        WorldToViewMatrix() = 0;
	virtual int                   GameLumpVersion(int lumpId) const = 0;
	virtual int                   GameLumpSize(int lumpId) const = 0; // 40
	virtual bool                  LoadGameLump(int lumpId, void* pBuffer, int size) = 0;
	virtual int                   LevelLeafCount() const = 0;
	virtual ISpatialQuery*        GetBSPTreeQuery() = 0;
	virtual void                  LinearToGamma(float* linear, float* gamma) = 0;
	virtual float                 LightStyleValue(int style) = 0; // 45
	virtual void                  ComputeDynamicLighting(const Vector& pt, const Vector* pNormal, Vector& color) = 0;
	virtual void                  GetAmbientLightColor(Vector& color) = 0;
	virtual int                   GetDXSupportLevel() = 0;
	virtual bool                  SupportsHDR() = 0;
	virtual void                  Mat_Stub(IMaterialSystem *pMatSys) = 0; // 50
	virtual void                  GetChapterName(char *pchBuff, int iMaxLength) = 0;
	virtual char const*           GetLevelName(void) = 0;
	virtual char const*           GetLevelNameShort(void) = 0;
	virtual char const*           GetMapGroupName(void) = 0;
	virtual struct IVoiceTweak_s* GetVoiceTweakAPI(void) = 0;
	virtual void                  SetVoiceCasterID(unsigned int someint) = 0; // 56
	virtual void                  EngineStats_BeginFrame(void) = 0;
	virtual void                  EngineStats_EndFrame(void) = 0;
	virtual void                  FireEvents() = 0;
	virtual int                   GetLeavesArea(unsigned short *pLeaves, int nLeaves) = 0;
	virtual bool                  DoesBoxTouchAreaFrustum(const Vector &mins, const Vector &maxs, int iArea) = 0; // 60
	virtual int                   GetFrustumList(Frustum_t **pList, int listMax) = 0;
	virtual bool                  ShouldUseAreaFrustum(int i) = 0;
	virtual void                  SetAudioState(const AudioState_t& state) = 0;
	virtual int                   SentenceGroupPick(int groupIndex, char *name, int nameBufLen) = 0;
	virtual int                   SentenceGroupPickSequential(int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset) = 0;
	virtual int                   SentenceIndexFromName(const char *pSentenceName) = 0;
	virtual const char*           SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int                   SentenceGroupIndexFromName(const char *pGroupName) = 0;
	virtual const char*           SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual float                 SentenceLength(int sentenceIndex) = 0;
	virtual void                  ComputeLighting(const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors = NULL) = 0;
	virtual void                  ActivateOccluder(int nOccluderIndex, bool bActive) = 0;
	virtual bool                  IsOccluded(const Vector &vecAbsMins, const Vector &vecAbsMaxs) = 0; // 74
	virtual int                   GetOcclusionViewId(void) = 0;
	virtual void*                 SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void                  SaveFreeMemory(void *pSaveMem) = 0;
	virtual INetChannelInfo*      GetNetChannelInfo(void) = 0; //78
	virtual void                  DebugDrawPhysCollide(const CPhysCollide *pCollide, IMaterial *pMaterial, const matrix3x4_t& transform, const Color &color) = 0; //79
	virtual void                  CheckPoint(const char *pName) = 0; // 80
	virtual void                  DrawPortals() = 0;
	virtual bool                  IsPlayingDemo(void) = 0;
	virtual bool                  IsRecordingDemo(void) = 0;
	virtual bool                  IsPlayingTimeDemo(void) = 0;
	virtual int                   GetDemoRecordingTick(void) = 0;
	virtual int                   GetDemoPlaybackTick(void) = 0;
	virtual int                   GetDemoPlaybackStartTick(void) = 0;
	virtual float                 GetDemoPlaybackTimeScale(void) = 0;
	virtual int                   GetDemoPlaybackTotalTicks(void) = 0;
	virtual bool                  IsPaused(void) = 0; // 90
	virtual float                 GetTimescale(void) const = 0;
	virtual bool                  IsTakingScreenshot(void) = 0;
	virtual bool                  IsHLTV(void) = 0; //93
	virtual bool                  IsLevelMainMenuBackground(void) = 0;
	virtual void                  GetMainMenuBackgroundName(char *dest, int destlen) = 0;
	virtual void                  SetOcclusionParameters(const int /*OcclusionParams_t*/ &params) = 0; // 96
	virtual void                  GetUILanguage(char *dest, int destlen) = 0;
	virtual int                   IsSkyboxVisibleFromPoint(const Vector &vecPoint) = 0;
	virtual const char*           GetMapEntitiesString() = 0;
	virtual bool                  IsInEditMode(void) = 0; // 100
	virtual float                 GetScreenAspectRatio(int viewportWidth, int viewportHeight) = 0;
	virtual bool                  REMOVED_SteamRefreshLogin(const char *password, bool isSecure) = 0; // 100
	virtual bool                  REMOVED_SteamProcessCall(bool & finished) = 0;
	virtual unsigned int          GetEngineBuildNumber() = 0; // engines build
	virtual const char *          GetProductVersionString() = 0; // mods version number (steam.inf)
	virtual void                  GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;
	virtual bool                  IsHammerRunning() const = 0;
	virtual void                  ExecuteClientCmd(const char *szCmdString) = 0; //108
	virtual bool                  MapHasHDRLighting(void) = 0;
	virtual bool                  MapHasLightMapAlphaData(void) = 0;
	virtual int                   GetAppID() = 0;
	virtual Vector                GetLightForPointFast(const Vector &pos, bool bClamp) = 0;
	virtual void                  ClientCmd_Unrestricted1(char  const*, int, bool);
	virtual void ClientCmd_Unrestricted(const char *szCmdString, const char *newFlag) = 0; // 114, new flag, quick testing shows setting 0 seems to work, haven't looked into it.
	virtual void                  SetRestrictServerCommands(bool bRestrict) = 0;
	virtual void                  SetRestrictClientCommands(bool bRestrict) = 0;
	virtual void                  SetOverlayBindProxy(int iOverlayID, void *pBindProxy) = 0;
	virtual bool                  CopyFrameBufferToMaterial(const char *pMaterialName) = 0;
	virtual void                  ReadConfiguration(const int iController, const bool readDefault) = 0;
	virtual void                  SetAchievementMgr(IAchievementMgr *pAchievementMgr) = 0;
	virtual IAchievementMgr*      GetAchievementMgr() = 0;
	virtual bool                  MapLoadFailed(void) = 0;
	virtual void                  SetMapLoadFailed(bool bState) = 0;
	virtual bool                  IsLowViolence() = 0;
	virtual const char*           GetMostRecentSaveGame(void) = 0;
	virtual void                  SetMostRecentSaveGame(const char *lpszFilename) = 0;
	virtual void                  StartXboxExitingProcess() = 0;
	virtual bool                  IsSaveInProgress() = 0;
	virtual bool                  IsAutoSaveDangerousInProgress(void) = 0;
	virtual unsigned int          OnStorageDeviceAttached(int iController) = 0;
	virtual void                  OnStorageDeviceDetached(int iController) = 0;
	virtual char* const           GetSaveDirName(void) = 0;
	virtual void                  WriteScreenshot(const char *pFilename) = 0;
	virtual void                  ResetDemoInterpolation(void) = 0;
	virtual int                   GetActiveSplitScreenPlayerSlot() = 0;
	virtual int                   SetActiveSplitScreenPlayerSlot(int slot) = 0;
	virtual bool                  SetLocalPlayerIsResolvable(char const *pchContext, int nLine, bool bResolvable) = 0;
	virtual bool                  IsLocalPlayerResolvable() = 0;
	virtual int                   GetSplitScreenPlayer(int nSlot) = 0;
	virtual bool                  IsSplitScreenActive() = 0;
	virtual bool                  IsValidSplitScreenSlot(int nSlot) = 0;
	virtual int                   FirstValidSplitScreenSlot() = 0; // -1 == invalid
	virtual int                   NextValidSplitScreenSlot(int nPreviousSlot) = 0; // -1 == invalid
	virtual ISPSharedMemory*      GetSinglePlayerSharedMemorySpace(const char *szName, int ent_num = (1 << 11)) = 0;
	virtual void                  ComputeLightingCube(const Vector& pt, bool bClamp, Vector *pBoxColors) = 0;
	virtual void                  RegisterDemoCustomDataCallback(const char* szCallbackSaveID, pfnDemoCustomDataCallback pCallback) = 0;
	virtual void                  RecordDemoCustomData(pfnDemoCustomDataCallback pCallback, const void *pData, size_t iDataLength) = 0;
	virtual void                  SetPitchScale(float flPitchScale) = 0;
	virtual float                 GetPitchScale(void) = 0;
	virtual bool                  LoadFilmmaker() = 0;
	virtual void                  UnloadFilmmaker() = 0;
	virtual void                  SetLeafFlag(int nLeafIndex, int nFlagBits) = 0;
	virtual void                  RecalculateBSPLeafFlags(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomNew(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomChanged(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomSkyAbove(void) = 0;
	virtual float                 DSPGetCurrentDASRoomSkyPercent(void) = 0;
	virtual void                  SetMixGroupOfCurrentMixer(const char *szgroupname, const char *szparam, float val, int setMixerType) = 0;
	virtual int                   GetMixLayerIndex(const char *szmixlayername) = 0;
	virtual void                  SetMixLayerLevel(int index, float level) = 0;
	virtual int                   GetMixGroupIndex(char  const* groupname) = 0;
	virtual void                  SetMixLayerTriggerFactor(int i1, int i2, float fl) = 0;
	virtual void                  SetMixLayerTriggerFactor(char  const* char1, char  const* char2, float fl) = 0;
	virtual bool                  IsCreatingReslist() = 0;
	virtual bool                  IsCreatingXboxReslist() = 0;
	virtual void                  SetTimescale(float flTimescale) = 0;
	virtual void                  SetGamestatsData(CGamestatsData *pGamestatsData) = 0;
	virtual CGamestatsData*       GetGamestatsData() = 0;
	virtual void                  GetMouseDelta(int &dx, int &dy, bool b) = 0; // unknown
	virtual   const char*         Key_LookupBindingEx(const char *pBinding, int iUserId = -1, int iStartCount = 0, int iAllowJoystick = -1) = 0;
	virtual int                   Key_CodeForBinding(char  const*, int, int, int) = 0;
	virtual void                  UpdateDAndELights(void) = 0;
	virtual int                   GetBugSubmissionCount() const = 0;
	virtual void                  ClearBugSubmissionCount() = 0;
	virtual bool                  DoesLevelContainWater() const = 0;
	virtual float                 GetServerSimulationFrameTime() const = 0;
	virtual void                  SolidMoved(class IClientEntity *pSolidEnt, class ICollideable *pSolidCollide, const Vector* pPrevAbsOrigin, bool accurateBboxTriggerChecks) = 0;
	virtual void                  TriggerMoved(class IClientEntity *pTriggerEnt, bool accurateBboxTriggerChecks) = 0;
	virtual void                  ComputeLeavesConnected(const Vector &vecOrigin, int nCount, const int *pLeafIndices, bool *pIsConnected) = 0;
	virtual bool                  IsInCommentaryMode(void) = 0;
	virtual void                  SetBlurFade(float amount) = 0;
	virtual bool                  IsTransitioningToLoad() = 0;
	virtual void                  SearchPathsChangedAfterInstall() = 0;
	virtual void                  ConfigureSystemLevel(int nCPULevel, int nGPULevel) = 0;
	virtual void                  SetConnectionPassword(char const *pchCurrentPW) = 0;
	virtual CSteamAPIContext*     GetSteamAPIContext() = 0;
	virtual void                  SubmitStatRecord(char const *szMapName, unsigned int uiBlobVersion, unsigned int uiBlobSize, const void *pvBlob) = 0;
	virtual void                  ServerCmdKeyValues(KeyValues *pKeyValues) = 0; // 203
	virtual void                  SpherePaintSurface(const model_t* model, const Vector& location, unsigned char chr, float fl1, float fl2) = 0; // 204
	virtual bool                  HasPaintmap(void) = 0; // 205
	virtual void                  EnablePaintmapRender() = 0; // 206
	//virtual void                TracePaintSurface( const model_t *model, const Vector& position, float radius, CUtlVector<Color>& surfColors ) = 0;
	virtual void                  SphereTracePaintSurface(const model_t* model, const Vector& position, const Vector &vec2, float radius, /*CUtlVector<unsigned char, CUtlMemory<unsigned char, int>>*/ int& utilVecShit) = 0; // 207
	virtual void                  RemoveAllPaint() = 0; // 208
	virtual void                  PaintAllSurfaces(unsigned char uchr) = 0; // 209
	virtual void                  RemovePaint(const model_t* model) = 0; // 210
	virtual bool                  IsActiveApp() = 0; // 211
	virtual bool                  IsClientLocalToActiveServer() = 0; // 212
	virtual void                  TickProgressBar() = 0; // 213
	virtual InputContextHandle_t  GetInputContext(int /*EngineInputContextId_t*/ id) = 0; // 214
	virtual void                  GetStartupImage(char* filename, int size) = 0; // 215
	virtual bool                  IsUsingLocalNetworkBackdoor(void) = 0; // 216
	virtual void                  SaveGame(const char*, bool, char*, int, char*, int) = 0; // 217
	virtual void                  GetGenericMemoryStats(/* GenericMemoryStat_t */ void **) = 0; // 218
	virtual bool                  GameHasShutdownAndFlushedMemory(void) = 0; // 219
	virtual int                   GetLastAcknowledgedCommand(void) = 0; // 220
	virtual void                  FinishContainerWrites(int i) = 0; // 221
	virtual void                  FinishAsyncSave(void) = 0; // 222
	virtual int                   GetServerTick(void) = 0; // 223
	virtual const char*           GetModDirectory(void) = 0; // 224
	virtual bool                  AudioLanguageChanged(void) = 0; // 225
	virtual bool                  IsAutoSaveInProgress(void) = 0; // 226
	virtual void                  StartLoadingScreenForCommand(const char* command) = 0; // 227
	virtual void                  StartLoadingScreenForKeyValues(KeyValues* values) = 0; // 228
	virtual void                  SOSSetOpvarFloat(const char*, float) = 0;
	virtual void                  SOSGetOpvarFloat(const char*, float &) = 0;
	virtual bool                  IsSubscribedMap(const char*, bool) = 0;
	virtual bool                  IsFeaturedMap(const char*, bool) = 0;
	virtual void                  GetDemoPlaybackParameters(void) = 0;
	virtual int                   GetClientVersion(void) = 0;
	virtual bool                  IsDemoSkipping(void) = 0;
	virtual void                  SetDemoImportantEventData(const KeyValues* values) = 0;
	virtual void                  ClearEvents(void) = 0;
	virtual int                   GetSafeZoneXMin(void) = 0;
	virtual bool                  IsVoiceRecording(void) = 0;
	virtual void                  ForceVoiceRecordOn(void) = 0;
	virtual bool                  IsReplay(void) = 0;
};