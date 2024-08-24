#include "../../precompiled.h"

void CFakeLag::Init(bool bInitialized)
{
	m_ChokedCommands = 1;

	if (!g_Client.m_NetPos.empty() && (g_Client.m_LocalPlayer->m_vecOrigin() - g_Client.m_NetPos.front().m_Position).LengthSqr() > 64.f * 64.f) {
		g_Client.GetModifablePacket() = true;
		return;
	}
	
	if (!g_Config.b(g_Items.m_AntiAim.m_General.m_Enable) || g_Client.m_MaxLagAmount <= 1 || g_Csgo.m_MaxUserCommandProcessTicks->GetInt() == 1)
		return;

	if (g_Config.b(g_Items.m_AntiAim.m_Stand.m_Fakelag.m_Enable) && g_Client.m_MovementMode == Client::MOVEMENT::STAND) 
		m_ChokedCommands = g_Config.i(g_Items.m_AntiAim.m_Stand.m_Fakelag.m_Limit);

	else if (g_Config.b(g_Items.m_AntiAim.m_Move.m_Fakelag.m_Enable) && g_Client.m_MovementMode == Client::MOVEMENT::WALK)
		m_ChokedCommands = g_Config.i(g_Items.m_AntiAim.m_Move.m_Fakelag.m_Limit);

	else if (g_Config.b(g_Items.m_AntiAim.m_Air.m_Fakelag.m_Enable) && g_Client.m_MovementMode == Client::MOVEMENT::AIR)
		m_ChokedCommands = g_Config.i(g_Items.m_AntiAim.m_Air.m_Fakelag.m_Limit);
	
	else m_ChokedCommands = 1;

	if (g_AntiAim.m_Config.m_HideShots)
		--m_ChokedCommands;

	g_Client.GetModifablePacket() = g_Client.m_ChokedCommands >= m_ChokedCommands;

	if (g_AntiAim.m_Config.m_HideShots)
		++m_ChokedCommands;

	if (g_Config.b(g_Items.m_Misc.m_FakeWalk) && g_KeyInput.Key(g_Items.m_Misc.m_FakeWalkKeyBinds, g_Items.m_Misc.m_FakeWalkBindsMode)) {
		g_Client.GetModifablePacket() = false;
	}

	if (g_Client.IsShooting())
		g_Client.GetModifablePacket() = !g_AntiAim.m_Config.m_HideShots;

	if (g_TickbaseManipulation.GetActiveExploit()) {
		g_Client.GetModifablePacket() = true;
	}

	if (c_config::get()->b["fakelag_disable_prediction_error"] && (g_Client.m_LocalPlayer->m_flVelocityModifier() < 1.f || g_Client.m_PredictionError)) {
		g_Client.GetModifablePacket() = true;
	}
}