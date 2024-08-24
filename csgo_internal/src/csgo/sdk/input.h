#pragma once

constexpr int MULTIPLAYER_BACKUP = 150;

enum button_flags_t : int {
	IN_ATTACK    = ( 1 << 0 ),
	IN_JUMP      = ( 1 << 1 ),
	IN_DUCK      = ( 1 << 2 ),
	IN_FORWARD   = ( 1 << 3 ),
	IN_BACK      = ( 1 << 4 ),
	IN_USE       = ( 1 << 5 ),
	IN_CANCEL    = ( 1 << 6 ),
	IN_LEFT      = ( 1 << 7 ),
	IN_RIGHT     = ( 1 << 8 ),
	IN_MOVELEFT  = ( 1 << 9 ),
	IN_MOVERIGHT = ( 1 << 10 ),
	IN_ATTACK2   = ( 1 << 11 ),
	IN_RUN       = ( 1 << 12 ),
	IN_RELOAD    = ( 1 << 13 ),
	IN_ALT1      = ( 1 << 14 ),
	IN_ALT2      = ( 1 << 15 ),
	IN_SCORE     = ( 1 << 16 ),
	IN_SPEED     = ( 1 << 17 ),
	IN_WALK      = ( 1 << 18 ),
	IN_ZOOM      = ( 1 << 19 ),
	IN_WEAPON1   = ( 1 << 20 ),
	IN_WEAPON2   = ( 1 << 21 ),
	IN_BULLRUSH  = ( 1 << 22 ),
	IN_GRENADE1  = ( 1 << 23 ),
	IN_GRENADE2  = ( 1 << 24 ),
	IN_ATTACK3   = ( 1 << 25 ),
};


class CCSUsrMsg_ServerRankRevealAll {
public:
	__forceinline CCSUsrMsg_ServerRankRevealAll( ) : seconds_till_shutdown{} {}
public:
	PAD( 0x8 );
	int seconds_till_shutdown;
};

class Command {
public:
	__forceinline Command( ) { reset( ); }

	__forceinline void reset( ) {
		m_CommandNumber   = 0;
		m_TickCount       = 0;
		m_ViewAngles.clear( );
		m_AimDirection.clear( );
		m_ForwardMove     = 0.f;
		m_SideMove        = 0.f;
		m_UpMove          = 0.f;
		m_Buttons          = 0;
		m_Impulse          = 0;
		m_WeaponSelect    = 0;
		m_WeaponSubType   = 0;
		m_RandomSeed      = 0;
		m_MouseDX          = 0;
		m_MouseDY          = 0;
		m_Predicted        = false;
		m_HeadAngles.clear( );
		m_HeadOffset.clear( );
	}

	__forceinline Command& operator =( const Command& src ) {
		if( this == &src )
			return *this;

		m_CommandNumber		= src.m_CommandNumber;
		m_TickCount			= src.m_TickCount;
		m_ViewAngles		= src.m_ViewAngles;
		m_AimDirection		= src.m_AimDirection;
		m_ForwardMove		= src.m_ForwardMove;
		m_SideMove			= src.m_SideMove;
		m_UpMove			= src.m_UpMove;
		m_Buttons			= src.m_Buttons;
		m_Impulse			= src.m_Impulse;
		m_WeaponSelect		= src.m_WeaponSelect;
		m_WeaponSubType		= src.m_WeaponSubType;
		m_RandomSeed		= src.m_RandomSeed;
		m_MouseDX			= src.m_MouseDX;
		m_MouseDY			= src.m_MouseDY;
		m_Predicted			= src.m_Predicted;
		m_HeadAngles		= src.m_HeadAngles;
		m_HeadOffset		= src.m_HeadOffset;

		return *this;
	}

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &m_CommandNumber, sizeof(m_CommandNumber));
		CRC32_ProcessBuffer(&crc, &m_TickCount, sizeof(m_TickCount));
		CRC32_ProcessBuffer(&crc, &m_ViewAngles, sizeof(m_ViewAngles));
		CRC32_ProcessBuffer(&crc, &m_AimDirection, sizeof(m_AimDirection));
		CRC32_ProcessBuffer(&crc, &m_ForwardMove, sizeof(m_ForwardMove));
		CRC32_ProcessBuffer(&crc, &m_SideMove, sizeof(m_SideMove));
		CRC32_ProcessBuffer(&crc, &m_UpMove, sizeof(m_UpMove));
		CRC32_ProcessBuffer(&crc, &m_Buttons, sizeof(m_Buttons));
		CRC32_ProcessBuffer(&crc, &m_Impulse, sizeof(m_Impulse));
		CRC32_ProcessBuffer(&crc, &m_WeaponSelect, sizeof(m_WeaponSelect));
		CRC32_ProcessBuffer(&crc, &m_WeaponSubType, sizeof(m_WeaponSubType));
		CRC32_ProcessBuffer(&crc, &m_RandomSeed, sizeof(m_RandomSeed));
		CRC32_ProcessBuffer(&crc, &m_MouseDX, sizeof(m_MouseDX));
		CRC32_ProcessBuffer(&crc, &m_MouseDY, sizeof(m_MouseDY));

		CRC32_Final(&crc);

		return crc;
	}

	__forceinline Command( const Command& src ) {
		*this = src;
	}

	// virtual dtor.
	PAD( 0x4 )

public:
	int     m_CommandNumber;
	int     m_TickCount;
	QAngle  m_ViewAngles;
	Vector  m_AimDirection;
	float   m_ForwardMove;
	float   m_SideMove;
	float   m_UpMove;
	int     m_Buttons;
	uint8_t m_Impulse;
	int     m_WeaponSelect;
	int     m_WeaponSubType;
	int     m_RandomSeed;
	short   m_MouseDX;
	short   m_MouseDY;
	bool    m_Predicted;
	Vector  m_HeadAngles;
	Vector  m_HeadOffset;
};

class VerifiedCommand {
public:
	Command   m_Command;
	CRC32_t    m_CRC;
};

class IInputSystem {
public:
	enum indices : size_t {
		ENABLEINPUT          = 11,
		ISBUTTONDOWN         = 15,
		GETBUTTONPRESSEDTICK = 16,
		GETCURSORPOSITION    = 56,
	};

public:
	__forceinline void EnableInput( bool enable ) {
		return util::get_method< void( __thiscall* )( decltype( this ), bool ) >( this, ENABLEINPUT )( this, enable );
	}

	__forceinline void GetCursorPosition( int* x, int* y ) {
		return util::get_method< void( __thiscall* )( decltype( this ), int*, int* ) >( this, GETCURSORPOSITION )( this, x, y );
	}
};

class CInput {
public:
	enum indices : size_t {
		CREATEMOVE                = 3,
		WRITEUSERCMDDELTATOBUFFER = 5,
		ENCODEUSERCMDTOBUFFER     = 6,
		GETUSERCMD                = 8,
		CAMISTHIRDPERSON          = 32,
		CAMTOTHIRDPERSON          = 35,
		CAMTOFIRSTPERSON          = 36,
	};
#ifdef LEGACY
private:
	void* vtable;									// 0x00
public:
	bool              m_trackir;					// 0x04
	bool              m_mouse_init;					// 0x05
	bool              m_mouse_active;				// 0x06
	bool              m_joystick_adv_init;			// 0x07
private:
	PAD(0x2C);									// 0x08
	void* m_keys;						// 0x34
	PAD(0x6C);									// 0x38
public:
	bool              m_camera_intercepting_mouse;	// 0x9C
	bool              m_camera_in_third_person;		// 0x9D
	bool              m_camera_moving_with_mouse;	// 0x9E
	Vector            m_CameraOffset;				// 0xA0
	bool              m_camera_distance_move;		// 0xAC
	int               m_camera_old_x;				// 0xB0
	int               m_camera_old_y;				// 0xB4
	int               m_camera_x;					// 0xB8
	int               m_camera_y;					// 0xBC
	bool              m_camera_is_orthographic;		// 0xC0
	QAngle             m_previous_view_angles;		// 0xC4
	QAngle             m_previous_view_angles_tilt;	// 0xD0
	float             m_last_forward_move;			// 0xDC
	int               m_clear_input_state;			// 0xE0
	Command* m_Commands;					// 0xEC
	VerifiedCommand* m_VerifiedCommands;					// 0xF0
#else
private:
	PAD(0xC);									// 0x0
public:
    bool				m_TrackIRAvailable;		// 0xC
    bool				m_MouseInitialized;		// 0xD
    bool				m_MouseActive;			// 0xE
private:
	PAD(0x9A);									// 0xF
public:
    bool				m_CameraInThirdPerson;	// 0xA9
private:
	PAD(0x2);									// 0xAA
public:
    Vector				m_CameraOffset;			// 0xAC
private:
	PAD(0x38);									// 0xB8
public:
    Command*			m_Commands;
	VerifiedCommand*	m_VerifiedCommands;
#endif
public:
	__forceinline int CAM_IsThirdPerson( int slot = -1 ) {
		return util::get_method< int( __thiscall* )( decltype( this ), int ) >( this, CAMISTHIRDPERSON )( this, slot );
	}

	__forceinline void CAM_ToThirdPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOTHIRDPERSON )( this );
	}

	__forceinline void CAM_ToFirstPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOFIRSTPERSON )( this );
	}

	__forceinline Command* GetUserCmd(int iSlot, int iSequenceNumber) {
		return util::get_method< Command* (__thiscall*)(decltype(this), int, int) >(this, GETUSERCMD)(this, iSlot, iSequenceNumber);
	}
public:

	__forceinline Command* GetUserCmd(int iSequenceNumber) {
		return &m_Commands[iSequenceNumber % MULTIPLAYER_BACKUP];
	}

	__forceinline VerifiedCommand* GetVerifiedUserCmd(int iSequenceNumber) {
		return &m_VerifiedCommands[iSequenceNumber % MULTIPLAYER_BACKUP];
	}
};