#include "../../../precompiled.h"

void Hooks::OnRenderStart() {
	if (!g_Csgo.m_ViewRender || !g_Client.m_Weapon  || !g_Client.m_LocalPlayer)
		return g_Hooks.m_ViewRender.GetOldMethod< OnRenderStart_t >(CViewRender::ONRENDERSTART)(this);

	g_Hooks.m_ViewRender.GetOldMethod< OnRenderStart_t >(CViewRender::ONRENDERSTART)(this);

	if (g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfView)) 
	{
		// TODO@: Add animation.
		g_Csgo.m_ViewRender->m_view.m_fov = g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmount);

		if (g_Client.m_CanScope && g_Client.m_LocalPlayer->m_bIsScoped())
		{
			switch (g_Client.m_Weapon->m_zoomLevel())
			{
			case 1:
				g_Csgo.m_ViewRender->m_view.m_fov -= g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmountScoped);
				break;
			case 2:
				g_Csgo.m_ViewRender->m_view.m_fov -= g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmountZoomed);
				break;
			}
		}

		g_Csgo.m_ViewRender->m_view.m_fov = std::clamp(g_Csgo.m_ViewRender->m_view.m_fov, 1.f, 180.f - 1);
	}

	static float defaultviewmodelfov = 70;

	if (g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideViewModel)) {
		g_Csgo.m_ViewRender->m_view.m_viewmodel_fov = g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideViewModelFov);
	}
	else {
		g_Csgo.m_ViewRender->m_view.m_viewmodel_fov = defaultviewmodelfov;
	}

	g_Csgo.m_ViewRender->m_view.m_viewmodel_fov = std::clamp(g_Csgo.m_ViewRender->m_view.m_viewmodel_fov, 1.f, 180.f - 1);
}

void Hooks::Render2DEffectsPostHUD(const CViewSetup& setup) {
	if (!g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_FLASH])
		g_Hooks.m_ViewRender.GetOldMethod< Render2DEffectsPostHUD_t >(CViewRender::RENDER2DEFFECTSPOSTHUD)(this, setup);
}

void Hooks::RenderSmokeOverlay(bool unk) {
	if (!g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SMOKE])
		g_Hooks.m_ViewRender.GetOldMethod< RenderSmokeOverlay_t >(CViewRender::RENDERSMOKEOVERLAY)(this, unk);
}