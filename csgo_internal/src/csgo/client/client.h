#pragma once

class NetPos {
public:
	float  m_Curtime;
	Vector m_Position;

public:
	__forceinline NetPos() : m_Curtime{}, m_Position{} {};
	__forceinline NetPos(float time, Vector pos) : m_Curtime{ time }, m_Position{ pos } {};
};

struct CommandNumber
{
	bool m_IsOutgoing = false;
	bool m_IsUsed = false;
	int m_PreviousCommandNumber = 0;
	int m_CommandNumber = 0;
};

struct CorrectionData
{
	int m_CommandNumber = 0;
	int m_TickCount = 0;
	int m_ChokedCommands = 0;
};

class CCommandContext
{
public:
	CUtlVector< Command > cmds;

	int				numcmds;
	int				totalcmds;
	int				dropped_packets;
	bool			paused;
};

class Client {
public:
	static void Init();

	void ProcessUsercmds(Command* cmds, int numcmds, int totalcmds, int dropped_packets, bool paused);
	void SetupShootPosition(QAngle angAngle = QAngle(0.f, 0.f, 0.f));
	void UpdateServerSideAnimations();
	void SetUpAnimationData(bool bUpdated = true);
	void SetUpLowerbodyYawTarget();
	void SetUpAnimationLayers();
	void SetAbsAngles();
	void ResetAnimationData();
	bool IsUpdatingAnimations();
	void UpdateAnimations();

	void PanicMode();
	void DoMove();
	void UnlockHiddenConvars();
	void Thirdperson();
	void ClanTag();
	void Skybox();
	void NoSmoke();
	void MouseCorrection();
	void StartMove();
	void EndMove();

	void OnPaint();
	void OnMapload();
	void OnTick();

	void print(const std::string text, ...);

	bool CanAttack();

	bool CanFireWeapon(int iShiftAmount = 0, bool bCheckRevolver = true, bool bDontCheckBetweenShots = false);

	bool SetupWeaponData();
	void SetupViewAngles();
	bool IsShooting(bool bCheckRevolver = true);
	void UpdateRevolverCock();
	
	float GetVelocityLengthXY(Player* pPlayer);
public:
	enum MOVEMENT {
		STAND = 0,
		WALK,
		AIR,
	};

	MOVEMENT m_MovementMode = MOVEMENT::STAND;

	// local player variables.
	Player* m_LocalPlayer = nullptr;
	bool	         m_Processing = false;
	bool	         m_PredictionError = false;
	bool	         m_NetVarError = false;
	bool			 m_CanScope = false;
	int	             m_Flags = 0;
	Vector	         m_EyePosition;
	bool	         m_PlayerFire = false;
	bool	         m_Shot = false;
	bool	         m_OldShot = false;
	bool		     m_PressingButtons = false;
	float			 m_MaxPlayerSpeed = 260.f;
	float			 m_AccurateSpeed = 0.f;
	int				 m_FrameRate = 0;
	int				 m_FramePerSecond = 0;
	bool             m_Autowalling = false;

	Player* GetLocalPlayer() {
		return m_LocalPlayer;
	}

	// active weapon variables.
	Weapon* m_Weapon = nullptr;
	int         m_WeaponItemDefinition = 0;
	WeaponInfo* m_WeaponInfo = nullptr;
	int         m_WeaponType = 0;

	// revolver variables.
	float		m_RevolverCock = 0.f;

	// general game varaibles.
	bool		m_round_end = false;
	bool		m_round_start = false;
	Stage_t		m_Stage = FRAME_UNDEFINED;
	int			m_MaxLagAmount = 0;
	size_t		m_TickRate = 64u;
	int			m_ChokedCommands = 0;
	bool		m_bOldSendPacket;
	float		m_Lerp = 0.f;
	float		m_LatencyOutgoing = 0.f;
	float		m_LatencyIncoming = 0.f;
	int			 m_LatencyTicks = 0;
	int			m_ServerTick = 0;
	int			m_ArrivalTick = 0;
	int			m_Width, m_Height = 0;
	bool*		m_SendPacket;

	bool& GetModifablePacket() {
		return *m_SendPacket;
	}

	// bones related.
	matrix3x4a_t m_LagMatrix[MAXSTUDIOBONES];
	matrix3x4a_t m_Matrix[MAXSTUDIOBONES];
	std::array < Vector, MAXSTUDIOBONES > m_BoneOrigins;

	// usercommand variables.
	Command* m_Command = nullptr;
	VerifiedCommand* m_VerifiedCommand = nullptr;

	Command* GetModifableCommand() {
		return m_Command;
	}

	// fakelag fix.
	std::deque < int32_t > m_aCommandList;
	std::vector <int> m_CommandNumber;
	std::deque <CommandNumber> m_TransmissionCommands;
	std::deque <CorrectionData> m_TransmissionCorrectionData;
	CUtlVector< CCommandContext > m_CommandContext;

	bool      m_ShotFixed = false;
	int		  m_ShotCommandNumber = 0;
	float	  m_LastVelocityModifier = 0.f;
	int		  m_Sequence = 0;
	int	      m_TickCount = 0;
	int	      m_Buttons = 0;
	QAngle    m_ViewAngles = QAngle(0.f, 0.f, 0.f);
	QAngle	  m_StrafeAngles = QAngle(0.f, 0.f, 0.f);
	Vector	  m_ForwardDirection = Vector(0.f, 0.f, 0.f);

	Penetration::PenetrationOutput_t m_PenetrationData = {};

	std::deque< NetPos >   m_NetPos = {};

	bool   m_PanicButton = false;

	// animation variables.
	QAngle			 m_EyeAngle = QAngle(0, 0, 0), m_Radar = QAngle(0, 0, 0);
	bool			 m_BodyUpdate = false, m_LagCompensationBreaked = false;
	float			 m_SpawnTime = 0.0f, m_LowerBodyYaw = 0.0f, m_NextUpdateTime = 0.0f, m_NextLowerBodyYawUpdateTime = 0.0f, m_Curtime = 0.0f, m_AnimFrame = 0.0f;
	int				 m_CountForLowerBodyYawUpdate = 0;

	std::array < C_AnimationLayer, 13 > m_AnimationLayers;
	std::array < float, 24 > m_PoseParameters;

	// update stuff.
	// update stuff.
	bool m_ClientAnimationUpdate = false;
	bool m_SetupBones = false;
};

inline Client g_Client;