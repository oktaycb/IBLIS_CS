#include "../../../precompiled.h"

void Hooks::PaintTraverse(vgui::VPANEL vPanel, bool repaint, bool force) {
	static vgui::VPANEL vPanelID{ 0 };

	if (g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SCOPE] && g_Client.m_Processing)
	{
		static vgui::VPANEL vZoom{ 0 };

		if (!vZoom && RT_HASH(g_Csgo.m_Panel->GetName(vPanel)) == HASH("HudZoom"))
			vZoom = vPanel;

		if (vZoom == vPanel)
			return;
	}

	g_Hooks.m_Panel.GetOldMethod< PaintTraverse_t >(IPanel::PAINTTRAVERSE)(this, vPanel, repaint, force);

	if (!vPanelID)
	{
		if (RT_HASH(g_Csgo.m_Panel->GetName(vPanel)) == HASH("FocusOverlayPanel"))
			vPanelID = vPanel;
	}
	else if (vPanelID == vPanel)
	{
		g_Csgo.m_Panel->SetMouseInputEnabled(vPanel, g_Menu.IsOpen());

		new_render.SceneStart();
#ifdef LEGACY
		new_visuals.Init();
#endif // LEGACY
		new_render.SceneEnd();
	}
}