#pragma once

class C_CSGameRules {
public:
    // helper methods.
    // only valid in game.
    __forceinline C_CSGameRules *get_gamerules( ) {
        if( !g_Csgo.m_Engine->IsInGame() )
            return nullptr;

        return Address( this ).to< C_CSGameRules * >( );
    }

private:
    template< typename t >
	__forceinline t get( Address offset ) {
        C_CSGameRules *gr{ get_gamerules( ) };
        if( !gr )
            return t{}; // note - dex; probably a bad solution...

		return *( t *)( ( uintptr_t )gr + offset );
	}

public:
	__forceinline bool m_bFreezePeriod() {
		return get< bool >(0x20);

		// note - dex; doesnt work for some reason
		// return get< bool >( g_Netvars.get( HASH( "DT_CSGameRules" ), HASH( "m_bFreezePeriod" ) ) );
	}

	__forceinline bool m_bIsValveDS() {
		return get< bool >(0x75);
	}
};