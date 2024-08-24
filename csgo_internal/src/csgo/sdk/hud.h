#pragma once

struct NoticeText_t {
	wchar_t text[ 512 ];
	int unk0; // 0x400
	float unk1; // 0x404
	float unk2; // 0x408
	int unk3;   // 0x40C
	float time; // 0x410
	int unk4;	// 0x414
	float fade; // 0x418
	int unk5;   // 0x41C
};

struct KillFeed_t {
	PAD( 0x7C );
	CUtlVector< NoticeText_t > notices;
};

class CHudElement {
public:
	__forceinline const char* GetName( ) {
		return util::get_method< const char*( __thiscall* )( void* ) >( this, 12 )( this );
	}
};

class CHud {
public:
	PAD( 0x1C );
	CUtlVector< CHudElement* > list;

public:
	__forceinline CHudElement* FindElement( hash_t hash ) {
		auto size = list.Count( );
		for( auto i = 0; i < size; ++i ) {
			if( RT_HASH( list[ i ]->GetName( ) ) == hash )
				return list[ i ];
		}

		return nullptr;
	}
};