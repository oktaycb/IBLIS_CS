#include "../../../precompiled.h"

bool Hooks::DoPostScreenSpaceEffects(CViewSetup* setup) {
	new_visuals.Glow();

	return g_Hooks.m_ClientMode.GetOldMethod< DoPostScreenSpaceEffects_t >(IClientMode::DOPOSTSPACESCREENEFFECTS)(this, setup);
}

bool Hooks::ShouldDrawFog() {
	if (g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_FOG])
		return false;

	return g_Hooks.m_ClientMode.GetOldMethod< ShouldDrawFog_t >(IClientMode::SHOULDDRAWFOG)(this);
}

void Hooks::OverrideView(CViewSetup* view) {
	g_Client.m_LocalPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(g_Csgo.m_Engine->GetLocalPlayer());
	if (!g_Client.m_LocalPlayer)
		return g_Hooks.m_ClientMode.GetOldMethod< OverrideView_t >(IClientMode::OVERRIDEVIEW)(this, view);;

	// run thirdperson
	g_Client.Thirdperson();

	// remove post processing effects.
	game::SetPostProcess(g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_POSTPROCESS]);

	// call original.
	g_Hooks.m_ClientMode.GetOldMethod< OverrideView_t >(IClientMode::OVERRIDEVIEW)(this, view);
}
