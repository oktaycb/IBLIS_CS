#include "../../precompiled.h"

void CAntiAim::AntiAim()
{
	if (!Enabled())
		return;

	const bool bAntiUntrusted = g_Config.b(g_Items.m_AntiAim.m_General.m_AntiUntrusted);

	switch (m_Config.m_Pitch) {
	case 1: g_Client.m_Command->m_ViewAngles.x = bAntiUntrusted ? 89.f : -540.f; break;
	case 2: g_Client.m_Command->m_ViewAngles.x = bAntiUntrusted ? -89.f : 540.f; break;
	case 3: g_Client.m_Command->m_ViewAngles.x = g_Csgo.RandomFloat(bAntiUntrusted ? -89.f : -540.f, bAntiUntrusted ? 89.f : 541.f); break;
	}

	if (m_Config.m_Yaw > 0) {
		DesiredYaw();

		g_Client.m_Command->m_ViewAngles.y = m_Config.m_DesiredAngle;
	}
	else if (m_Config.m_FakeYaw)
		m_Config.m_DesiredAngle = g_Client.m_Command->m_ViewAngles.y;

	if (m_Config.m_FakeYaw)
	{
		if (*g_Client.m_SendPacket && g_Client.m_bOldSendPacket)
			*g_Client.m_SendPacket = false;

		if (!(*g_Client.m_SendPacket))
			DoRealAntiAim();

		else DoFakeAntiAim();
	}
	else DoRealAntiAim();
}

bool CAntiAim::Enabled()
{
	if (!(m_Config.m_Enable = g_Config.b(g_Items.m_AntiAim.m_General.m_Enable)))
		return false;

	if (g_Client.IsShooting())
		return false;

	if (g_Csgo.m_GameRules->m_bFreezePeriod() || (g_Client.m_Flags & FL_FROZEN) || (g_Client.m_Command->m_Buttons & IN_USE) || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_NOCLIP)
		return false;

	m_Config.m_LowerbodyBreak = false;
	m_Config.m_LowerbodyBreakAmount = 0;

	switch (g_Client.m_MovementMode) {
	case Client::STAND:
		m_Config.m_Pitch = g_Config.i(g_Items.m_AntiAim.m_Stand.m_Pitch);
		m_Config.m_Yaw = g_Config.i(g_Items.m_AntiAim.m_Stand.m_Yaw);
		m_Config.m_FakeYaw = g_Config.i(g_Items.m_AntiAim.m_Stand.m_FakeYaw);
		m_Config.m_LowerbodyBreakAmount = g_Config.i(g_Items.m_AntiAim.m_Stand.m_LowerbodyBreakAmount);
		m_Config.m_HideShots = g_Config.b(g_Items.m_AntiAim.m_Stand.m_HideShots);

		m_Config.m_Freestanding = g_Config.b(g_Items.m_AntiAim.m_Stand.m_Freestanding);
		m_Config.m_LowerbodyBreak = g_Config.b(g_Items.m_AntiAim.m_Stand.m_LowerbodyBreak);
		break;
	case Client::WALK:
		m_Config.m_Pitch = g_Config.i(g_Items.m_AntiAim.m_Move.m_Pitch);
		m_Config.m_Yaw = g_Config.i(g_Items.m_AntiAim.m_Move.m_Yaw);
		m_Config.m_FakeYaw = g_Config.i(g_Items.m_AntiAim.m_Move.m_FakeYaw);
		m_Config.m_HideShots = g_Config.b(g_Items.m_AntiAim.m_Move.m_HideShots);

		m_Config.m_Freestanding = g_Config.b(g_Items.m_AntiAim.m_Move.m_Freestanding);
		break;
	case Client::AIR:
		m_Config.m_Pitch = g_Config.i(g_Items.m_AntiAim.m_Air.m_Pitch);
		m_Config.m_Yaw = g_Config.i(g_Items.m_AntiAim.m_Air.m_Yaw);
		m_Config.m_FakeYaw = g_Config.i(g_Items.m_AntiAim.m_Air.m_FakeYaw);
		m_Config.m_HideShots = g_Config.b(g_Items.m_AntiAim.m_Air.m_HideShots);

		m_Config.m_Freestanding = g_Config.b(g_Items.m_AntiAim.m_Air.m_Freestanding);
		break;
	}

	return true;
}

void CAntiAim::DoRealAntiAim() {
	if (g_Csgo.m_Globals->m_Curtime > g_Client.m_NextLowerBodyYawUpdateTime && m_Config.m_LowerbodyBreak && !m_Config.m_FakeFlick) {
		g_Client.m_Command->m_ViewAngles.y += m_Config.m_LowerbodyBreakAmount;
		return;
	}

	switch (m_Config.m_Yaw) {
	case 2:
		g_Client.m_Command->m_ViewAngles.y += g_Csgo.RandomFloat(-m_Config.m_JitterRange / 2.f, m_Config.m_JitterRange / 2.f);
		break;
	case 3:
		Math::clamp(m_Config.m_RotateRange, 0.f, 180.f);

		g_Client.m_Command->m_ViewAngles.y = (m_Config.m_DesiredAngle - m_Config.m_RotateRange / 2.f);
		g_Client.m_Command->m_ViewAngles.y += std::fmod(g_Csgo.m_Globals->m_Curtime * (m_Config.m_RotateSpeed * 20.f), m_Config.m_RotateRange);
		break;
	}

	if (m_Config.m_FakeFlick)
		return;

	if (m_Config.m_Distortion)
		g_Client.m_Command->m_ViewAngles.y += (((sin(g_Csgo.m_Globals->m_Curtime * (m_Config.m_DistortionSpeed / 10.f)) + 1) / 2) * m_Config.m_DistortionAmount) - (m_Config.m_DistortionAmount / 2.f);
}


void CAntiAim::DoFakeAntiAim() {
	switch (m_Config.m_FakeYaw)
	{
	case 1:
		g_Client.m_Command->m_ViewAngles.y = m_Config.m_DesiredAngle + 180.f;
		g_Client.m_Command->m_ViewAngles.y += g_Csgo.RandomFloat(-90.f, 90.f);
		break;
	case 2:
		g_Client.m_Command->m_ViewAngles.y = m_Config.m_DesiredAngle + 180.f;
		break;
	case 3:
		g_Client.m_Command->m_ViewAngles.y = g_Csgo.RandomFloat(-180.f, 180.f);
		break;
	}
}

void CAntiAim::DesiredYaw()
{
	m_Config.m_View = g_Client.m_Command->m_ViewAngles.y;

	if (m_Config.m_YawBase > NULL) {

		float  best_fov{ std::numeric_limits< float >::max() };
		float  best_dist{ std::numeric_limits< float >::max() };
		float  fov, dist;
		Player* target, * best_target{ nullptr };

		for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
			target = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);

			if (!g_Aimbot.IsValidTarget(target) || target->GetClientNetworkable()->IsDormant())
				continue;

			if (m_Config.m_YawBase == 1)
			{
				fov = Math::GetFOV(g_Client.m_ViewAngles, Math::CalcAngle(g_Client.m_EyePosition, target->WorldSpaceCenter()));
				dist = (target->m_vecOrigin() - g_Client.m_LocalPlayer->m_vecOrigin()).LengthSqr();

				if (g_Csgo.weapon_accuracy_nospread->GetInt() == 1 ? (dist < best_fov) : fov < best_fov) {
					best_fov = g_Csgo.weapon_accuracy_nospread->GetInt() == 1 ? dist : fov;
					best_target = target;
				}


				if (best_target) {
					QAngle angle;
					Math::VectorAngles(best_target->m_vecOrigin() - g_Client.m_LocalPlayer->m_vecOrigin(), angle);
					m_Config.m_View = angle.y;
				}
			}
		}

		if (m_Config.m_YawBase == 2) {
			m_Config.m_View = 0.f;
			return;
		}
	}

	m_Config.m_DesiredAngle = m_Config.m_View + 180.f;

	if (m_Config.m_Freestanding)
		FreeStandYaw();
}

void CAntiAim::FreeStandYaw() {

	// constants.
	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 32.f };

	// best target.
	struct AutoTarget_t { float fov; Player* player; };
	AutoTarget_t target{ 180.f + 1.f, nullptr };

	// construct vector of angles to test.
	std::vector< AdaptiveAngle > angles{ };

	angles.emplace_back(m_Config.m_View - 180.f);
	angles.emplace_back(m_Config.m_View + 90.f);
	angles.emplace_back(m_Config.m_View - 90.f);

	// the best angle should be at the front now.
	AdaptiveAngle* best = &angles.front();

	// iterate players.
	for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
		Player* player = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);

		// validate player.
		if (!g_Aimbot.IsValidTarget(player))
			continue;

		// skip dormant players.
		if (player->GetClientNetworkable()->IsDormant())
			continue;

		float fov = Math::GetFOV(g_Client.m_ViewAngles, Math::CalcAngle(g_Client.m_EyePosition, player->WorldSpaceCenter()));

		if (fov < target.fov) {
			target.fov = fov;
			target.player = player;
		}
	}

	if (!target.player)
		return;

	// start the trace at the enemy shoot pos.
	Vector start = target.player->Weapon_ShootPosition();

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid{ false };

	// iterate vector of angles.
	for (auto it = angles.begin(); it != angles.end(); ++it) {

		// compute the 'rough' estimation of where our head will be.
		Vector end{ g_Client.m_EyePosition.x + std::cos(Math::DEG2RAD(it->m_Yaw)) * RANGE,
			g_Client.m_EyePosition.y + std::sin(Math::DEG2RAD(it->m_Yaw)) * RANGE,
			g_Client.m_EyePosition.z };

		// draw a line for debugging purposes.
		// g_Csgo.m_DebugOverlay->AddLineOverlay( start, end, 255, 0, 0, true, 0.1f );

		// compute the direction.
		Vector dir = end - start;
		float len = dir.Normalize();

		// should never happen.
		if (len <= 0.f)
			continue;

		// step thru the total distance, 4 units per step.
		for (float i{ 0.f }; i < len; i += STEP) {
			// get the current step position.
			Vector point = start + (dir * i);

			// get the contents at this point.
			int contents = g_Csgo.m_EngineTrace->GetPointContents(point, MASK_SHOT_HULL);

			// contains nothing that can stop a bullet.
			if (!(contents & MASK_SHOT_HULL))
				continue;

			float mult = 1.f;

			// over 50% of the total Length, prioritize this shit.
			if (i > (len * 0.5f))
				mult = 1.25f;

			// over 75% of the total Length, prioritize this shit.
			if (i > (len * 0.75f))
				mult = 1.25f;

			// over 90% of the total Length, prioritize this shit.
			if (i > (len * 0.9f))
				mult = 2.f;

			// append 'penetrated distance'.
			it->m_Distance += (STEP * mult);

			// mark that we found anything.
			valid = true;
		}
	}

	if (!valid)
		return;

	// put the most distance at the front of the container.
	std::sort(angles.begin(), angles.end(),
		[](const AdaptiveAngle& a, const AdaptiveAngle& b) {
			return a.m_Distance > b.m_Distance;
		});

	m_Config.m_DesiredAngle = Math::NormalizedAngle(best->m_Yaw);
}

void CAntiAim::HideShots()
{
	if (!g_Client.CanFireWeapon() || g_Client.m_ChokedCommands > 0 || g_TickbaseManipulation.GetActiveExploit())
		return;

	g_Client.GetModifablePacket() = false;
	g_Aimbot.RemoveAttack();
}