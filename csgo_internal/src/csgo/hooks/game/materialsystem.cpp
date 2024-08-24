#include "../../../precompiled.h"

bool Hooks::OverrideConfig( MaterialSystem_Config_t* config, bool update ) {
	if(g_Config.vb(g_Items.m_Visuals.m_World.m_BrightnessAdjustment).at(1))
		config->m_nFullbright = true;

	//static int   old_knife = 0;
	//static bool  old_knife_override = false;
	//static int   old_glove = 0;
	//static bool  old_glove_override = false;

	//if (c_config::get()->b["skin_override_knife"] && old_knife != c_config::get()->i["knife"] ||
	//	old_knife_override != c_config::get()->b["skin_override_knife"] ||
	//	c_config::get()->b["skin_override_gloves"] && old_glove != c_config::get()->i["glove"] ||
	//	old_glove_override != c_config::get()->b["skin_override_gloves"]) {

	//	//g_skins.m_update = true;

	//	old_knife = c_config::get()->i["knife"];
	//	old_knife_override = c_config::get()->b["skin_override_knife"];
	//	old_glove = c_config::get()->i["glove"];
	//	old_glove_override = c_config::get()->b["skin_override_gloves"];
	//}

	return g_Hooks.m_MaterialSystem.GetOldMethod< OverrideConfig_t >( IMaterialSystem::OVERRIDECONFIG )( this, config, update );
}