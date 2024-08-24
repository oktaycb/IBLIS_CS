#include "../../../precompiled.h"

void Hooks::PlaySound( const char* name ) {
	g_Hooks.m_Surface.GetOldMethod< PlaySound_t >( ISurface::PLAYSOUND )( this, name );

	//// auto accept.
	//if (c_config::get()->b["autoaccept"] && RT_HASH(name) == HASH("!UI/competitive_accept_beep.wav") && !g_Csgo.m_Engine->IsInGame()) {
	//	// accept match.
	//	g_Csgo.IsReady();

	//	// notify user.
	//	g_Csgo.m_Surface->PlaySound(XOR("ui/xp_levelup.wav"));
	//}
}