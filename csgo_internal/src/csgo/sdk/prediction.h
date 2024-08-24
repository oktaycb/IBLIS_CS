#pragma once

class CMoveData {
public:
	bool        m_bFirstRunOfFunctions : 1;
	bool        m_bGameCodeMovedPlayer : 1;
	bool        m_bNoAirControl : 1;
	CBaseHandle m_nPlayerHandle;
	int         m_nImpulseCommand;
	QAngle       m_vecViewAngles;
	QAngle       m_vecAbsViewAngles;
	int         m_nButtons;
	int         m_nOldButtons;
	float       m_flForwardMove;
	float       m_flSideMove;
	float       m_flUpMove;
	float       m_flMaxSpeed;
	float       m_flClientMaxSpeed;
	Vector      m_vecVelocity;
	Vector      m_vecOldVelocity;
	float       m_unknown;
	QAngle       m_vecAngles;
	QAngle       m_vecOldAngles;
	float       m_outStepHeight;
	Vector      m_outWishVel;
	Vector      m_outJumpVel;
	Vector      m_vecConstraintCenter;
	float       m_flConstraintRadius;
	float       m_flConstraintWidth;
	float       m_flConstraintSpeedFactor;
	bool        m_bConstraintPastRadius;
	Vector      m_vecAbsOrigin;
};

class IMoveHelper {
public:
	enum indices : size_t {
		SETHOST        = 1,
		PROCESSIMPACTS = 4,
	};

	__forceinline void SetHost( Entity* host ) {
		return util::get_method< void( __thiscall* )( decltype( this ), Entity* ) >( this, SETHOST )( this, host );
	}

	__forceinline void ProcessImpacts() {
		return util::get_method< void(__thiscall*)(decltype(this)) >(this, PROCESSIMPACTS)(this);
	}
};

#define MAX_SPLITSCREEN_PLAYERS 1
#define GET_ACTIVE_SPLITSCREEN_SLOT() ( 0 )

class CPrediction {
public:
	enum indices : size_t {
		UPDATE                  = 3,
        POSTNETWORKDATARECEIVED = 6,
		GETLOCALVIEWANGLES      = 12,
		SETLOCALVIEWANGLES      = 13,
		INPREDICTION            = 14,
		CHECKMOVINGGROUND       = 18,
		RUNCOMMAND              = 19,
		SETUPMOVE               = 20,
		FINISHMOVE              = 21,
		_UPDATE					= 24
	};

private:
	PAD( 0x4 );
protected:
	int		m_LastGroundTick;
public:
	bool    m_InPrediction;
	bool    m_FirstTimePredicted;
	bool    m_EnginePaused;		
	bool    m_OldCLPredictValue;		
	int		m_PreviousStartFrame;
	int		m_IncomingPacketNumber;
	float	m_LastServerWorldTimeStamp; 
public:
	struct Split_t
	{
		Split_t()
		{
			m_FirstTimePredicted = false;
			m_CommandsPredicted = 0;
			m_ServerCommandsAcknowledged = 0;
			m_PreviousAckHadErrors = false;
			m_IdealPitch = 0.0f;
			m_LastCommandAcknowledged = 0;
			m_PreviousAckErrorTriggersFullLatchReset = false;
		}

		bool				m_FirstTimePredicted;									// 0x0018
		PAD(0x3);															// 0x0019
		int					m_CommandsPredicted;									// 0x001C
		int					m_ServerCommandsAcknowledged;							// 0x0020
		int					m_PreviousAckHadErrors;									// 0x0024
		float				m_IdealPitch;											// 0x0028
		int					m_LastCommandAcknowledged;								// 0x002C
		bool				m_PreviousAckErrorTriggersFullLatchReset;				// 0x0030
		CUtlVector<EHANDLE> m_EntsWithPredictionErrorsInLastAck;			// 0x0031
		bool				m_PerformedTickShift;									// 0x0045
	};

	Split_t									m_Split[MAX_SPLITSCREEN_PLAYERS];		//0x18
	CGlobalVarsBase							m_SavedVars;							//0x4C
	bool									m_PlayerOriginTypedescriptionSearched;
	CUtlVector< const typedescription_t* >	m_PlayerOriginTypeDescription;
	void*									m_pPDumpPanel;
public:
	// virtual methods
	__forceinline void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command) {
		return util::get_method< void(__thiscall*)(void*, int, bool, int, int) >(this, UPDATE)(this, startframe, validframe, incoming_acknowledged, outgoing_command);
	}

	__forceinline void _Update(int nSlot, bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command) {
		return util::get_method< void(__thiscall*)(void*, int, bool, bool, int, int) >(this, _UPDATE)(this, nSlot, received_new_world_update, validframe, incoming_acknowledged, outgoing_command);
	}

	__forceinline void CheckMovingGround(Entity* player, double frametime) {
		return util::get_method< void(__thiscall*)(decltype(this), Entity*, double) >(this, CHECKMOVINGGROUND)(this, player, frametime);
	}

	__forceinline void GetLocalViewAngles(const QAngle& ang) {
		return util::get_method< void(__thiscall*)(decltype(this), const QAngle&) >(this, GETLOCALVIEWANGLES)(this, ang);
	}

	__forceinline void SetLocalViewAngles( const QAngle& ang ) {
		return util::get_method< void( __thiscall* )( decltype( this ), const QAngle& ) >( this, SETLOCALVIEWANGLES )( this, ang );
	}

	__forceinline void SetupMove( Entity* player, Command* cmd, IMoveHelper* helper, CMoveData* data ) {
		return util::get_method< void( __thiscall* )( decltype( this ), Entity*, Command*, IMoveHelper*, CMoveData* ) >( this, SETUPMOVE )( this, player, cmd, helper, data );
	}

	__forceinline void FinishMove( Entity* player, Command* cmd, CMoveData* data ) {
		return util::get_method< void( __thiscall* )( decltype( this ), Entity*, Command*, CMoveData* ) >( this, FINISHMOVE )( this, player, cmd, data );
	}
	
	// engine prediction.
	bool InPrediction(void);
	float GetSavedTime();
	void RunCommand(Player* player, Command* ucmd, IMoveHelper* moveHelper);
	void SelectItem(Player* player, Command* cmd);
	void LatchInImpulse(Player* player, Command* cmd);
	void ServerRunCommand(Player* player, Command* ucmd, IMoveHelper* moveHelper);
	void StartCommand(Player* player, Command* cmd);
	void RunPreThink(Player* player);
	void RunThink(Player* player);
	void RunPostThink(Player* player);
	void FinishCommand(Player* player);
};

class CGameMovement {
public:
	// indexes for virtuals and hooks
	enum indices : size_t {
		PROCESSMOVEMENT             = 1,
		RESET                       = 2,
		STARTTRACKPREDICTIONERRORS  = 3,
		FINISHTRACKPREDICTIONERRORS = 4,
		SETUPMOVEMENTBOUNDS         = 12,
		ONJUMP						= 31,
	};

	virtual						~CGameMovement(void) {}
	virtual void				ProcessMovement(Entity* pPlayer, CMoveData* pMove) = 0;
	virtual void				Reset(void) = 0;
	virtual void				StartTrackPredictionErrors(Entity* pPlayer) = 0;
	virtual void				FinishTrackPredictionErrors(Entity* pPlayer) = 0;
	virtual void				DiffPrint(char const* fmt, ...) = 0;
	virtual Vector const&		GetPlayerMins(bool ducked) const = 0;
	virtual Vector const&		GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const&		GetPlayerViewOffset(bool ducked) const = 0;
	virtual bool				IsMovingPlayerStuck(void) const = 0;
	virtual Entity*				GetMovingPlayer(void) const = 0;
	virtual void				UnblockPusher(Entity* pPlayer, Entity* pPusher) = 0;
	virtual void				SetupMovementBounds(CMoveData* pMove) = 0;
};