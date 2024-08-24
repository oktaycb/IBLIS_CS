#pragma once

struct Hit_t {
	char pad0x8[0x8];
	float m_pos_x;
	float m_pos_y;
	float m_time;
	float m_pos_z;
};

enum Stage_t {
	FRAME_UNDEFINED = -1, // (haven't Run any frames yet)
	FRAME_START,
	FRAME_NET_UPDATE_START, // A network packet is being recieved
	FRAME_NET_UPDATE_POSTDATAUPDATE_START, // Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_END, // Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_END, // We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_RENDER_START, // We're about to start rendering the scene
	FRAME_RENDER_END // We've finished rendering the scene
};

enum class Hitgroups_t {
	UKNOWN = -1,
	GENERIC,
	HEAD,
	CHEST,
	STOMACH,
	LEFTARM,
	RIGHTARM,
	LEFTLEG,
	RIGHTLEG,
	NECK,
	GEAR = 10,
	LEFTFOOT,
	RIGHTFOOT
};

class CHLClient {
public:
	enum indices : size_t {
#ifdef LEGACY
		INIT						= 0,
		LEVELINITPREENTITY			= 5,
		LEVELINITPOSTENTITY			= 6,
		LEVELSHUTDOWN				= 7,
		GETALLCLASSES				= 8,
		HUDPROCESSINPUT				= 10,
		INACTIVATEMOUSE				= 15,
		INKEYEVENT					= 20,
		CREATEMOVE					= 21, 
		WRITEUSERCMDDELTATOBUFFER	= 23,
		RENDERVIEW					= 27,
		FRAMESTAGENOTIFY			= 36,
		DISPATCHUSERMESSAGE			= 37,
#else
		INIT = 0,
		LEVELINITPREENTITY = 5,
		LEVELINITPOSTENTITY = 6,
		LEVELSHUTDOWN = 7,
		GETALLCLASSES = 8,
		HUDPROCESSINPUT = 10,
		INACTIVATEMOUSE = 15,
		INKEYEVENT = 20,
		CREATEMOVE = 22,
		WRITEUSERCMDDELTATOBUFFER = 24,
		RENDERVIEW = 28,
		FRAMESTAGENOTIFY = 37,
		DISPATCHUSERMESSAGE = 38,
#endif
	};

public:
	__forceinline ClientClass* GetAllClasses( ) {
		return util::get_method< ClientClass*( __thiscall* )( decltype( this ) )>( this, GETALLCLASSES )( this );
	}
};