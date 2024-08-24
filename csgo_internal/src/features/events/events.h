#pragma once

class Listener : public IGameEventListener2 {
	using game_event_t = void( __cdecl* )( IGameEvent* );

private:
	std::map< std::string, std::vector< game_event_t > > m_Events;

public:
	// ctor.
	__forceinline Listener( ) : m_Events{} {
		m_debug_id = 42;
	}

	void Init( );

	__forceinline void add( const std::string& name, game_event_t fn ) {
		m_Events[ name ].push_back( fn );
	}

	__forceinline void RegisterEvents( ) {
		if( m_Events.empty( ) )
			return;

		for( const auto& e : m_Events )
			g_Csgo.m_GameEvents->AddListener( this, e.first.data( ), false );
	}

	__forceinline void RemoveListener( ) {
		g_Csgo.m_GameEvents->RemoveListener( this );
	}

	void FireGameEvent( IGameEvent* evt ) override {
		const std::string name = evt->GetName( );

		if( m_Events.count( name ) != 0 ) {
			const auto& events = m_Events[ name ];
			for( const auto& e : events )
				e( evt );
		}
	}
};

namespace events {
	void round_start( IGameEvent* evt );
	void round_end( IGameEvent* evt );
	void round_prestart(IGameEvent* evt);
	void player_hurt( IGameEvent* evt );
	void bullet_impact( IGameEvent* evt );
	void weapon_fire(IGameEvent* evt);
	void item_purchase( IGameEvent* evt );
	void player_death( IGameEvent* evt );
	void player_given_c4( IGameEvent* evt );
	void bomb_beginplant( IGameEvent* evt );
	void bomb_abortplant( IGameEvent* evt );
	void bomb_planted( IGameEvent* evt );
    void bomb_beep( IGameEvent* evt );
	void bomb_begindefuse( IGameEvent* evt );
	void bomb_abortdefuse( IGameEvent* evt );
    void bomb_defused( IGameEvent* evt );
    void bomb_exploded( IGameEvent* evt );
}

inline Listener g_Listener;