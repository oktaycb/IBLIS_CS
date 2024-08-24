#include "../../../precompiled.h"

void Hooks::ComputeShadowDepthTextures( const CViewSetup &view, bool unk ) {
	if( !unk )
		return g_Hooks.m_ClientShadowMgr.GetOldMethod< ComputeShadowDepthTextures_t >( IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES )( this, view, unk );

	if(g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_TEAMMATES]) {
		for( int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i ) {
			Player* player = g_Csgo.m_EntityList->GetClientEntity< Player* >( i );

			if( !player )
				continue;

			if( player->IsLocalPlayer( ) )
				continue;

			if( !player->enemy( g_Client.m_LocalPlayer ) ) {
				// disable all rendering at the root.
				player->m_bReadyToDraw( ) = false;

				// now stop rendering their weapon.
				Weapon* weapon = player->GetActiveWeapon( );
				if( weapon )
					weapon->m_bReadyToDraw( ) = false;
			}
		}
	}

	return g_Hooks.m_ClientShadowMgr.GetOldMethod< ComputeShadowDepthTextures_t >( IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES )( this, view, unk );
}