#include "../../precompiled.h"

// TODO@Monster: Check for backtrack misses.
void Shots::OnFrameStage() {

	if (!g_Client.m_Processing) {
		if (!m_Shots.empty())
			m_Shots.clear();

		return;
	}

	for (size_t i = 0u; i < m_Shots.size(); i++)
	{
		auto Shot = &m_Shots[i];
		if (!Shot->m_HasBeenFired || !Shot->m_HasBeenRegistered)
			continue;

		if (!Shot->m_Player || !Shot->m_Record || Shot->m_WantedHitbox < Hitboxes_t::HEAD || Shot->m_WantedHitbox >= Hitboxes_t::MAX)
		{
			m_Shots.erase(m_Shots.begin() + i);
			continue;
		}

		AimPlayer* pData = &g_Aimbot.m_Players[Shot->m_Player->GetClientNetworkable()->GetIndex() - 1];
		if (!pData)
		{
			m_Shots.erase(m_Shots.begin() + i);
			continue;
		}

		if (Shot->m_HasBeenHurted)
		{
			m_Shots.erase(m_Shots.begin() + i);
			continue;
		}

		if (!Shot->m_Player->IsAlive())
		{
			m_Shots.erase(m_Shots.begin() + i);
			continue;
		}

		bool bMissedShotDueToOcclusion = false;

		if (Shot->m_StartPosition.Delta(Shot->m_Impacts.back()) < Shot->m_StartPosition.Delta(Shot->m_EndPosition))
		{
			const float flClientYaw = Math::AngleNormalize(Math::CalcAngle(Shot->m_StartPosition, Shot->m_EndPosition).y);
			const float flServerYaw = Math::AngleNormalize(Math::CalcAngle(Shot->m_StartPosition, Shot->m_Impacts.back()).y);

			Penetration::PenetrationInput_t m_Input;

			m_Input.m_Damage = m_Input.m_PenetrationDamage = Shot->m_Damage;
			m_Input.m_Penetrate = true;
			m_Input.m_Player = Shot->m_Player;
			m_Input.m_From = g_Client.m_LocalPlayer;
			m_Input.m_EyePosition = Shot->m_StartPosition;

			if (fabs(Math::AngleNormalize(fabs(Math::AngleDiff(flClientYaw, flServerYaw)))) <= 5.0f)
			{
				m_Input.m_Position = Shot->m_Impacts.back();

				Penetration::PenetrationOutput_t pServerOutput;
				Penetration::Run(&m_Input, &pServerOutput);

				m_Input.m_Position = Shot->m_EndPosition;

				Penetration::PenetrationOutput_t pClientOutput;
				Penetration::Run(&m_Input, &pClientOutput);

				if (pClientOutput.m_Penetration && pClientOutput.m_Damage > 0.f && !pServerOutput.m_Penetration && pServerOutput.m_Damage <= 0.f)
					bMissedShotDueToOcclusion = true;
			}
		}

		std::string sMissSituation;
		std::string sMissReason;

		if (Shot->m_Record->m_ResolverMode == Resolver::Modes::RESOLVE_AIR)
			sMissSituation = "in air";
		
		else if (Shot->m_Record->m_ResolverMode == Resolver::Modes::RESOLVE_WALK)
			sMissSituation = "moving";
		
		else if (Shot->m_Record->m_ResolverMode == Resolver::Modes::RESOLVE_STAND)
			sMissSituation = "standing";

		std::string sResolverState;

		if (pData->m_ResolverMode == AimPlayer::RESOLVER::LEGIT)
			sResolverState = "legit";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST)
			sResolverState = "last moving lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_BROKEN)
			sResolverState = "last broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_FREESTAND)
			sResolverState = "last freestanding lby";

		else if (Shot->m_Record->m_Resolved || pData->m_ResolverMode == AimPlayer::RESOLVER::RESOLVED)
			sResolverState = Shot->m_Record->m_ResolverMode == Resolver::Modes::RESOLVE_STAND ? "lby update" : Shot->m_Record->m_ResolverMode == Resolver::Modes::RESOLVE_WALK ? "moving lby" : "static lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION)
			sResolverState = "lby prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_BROKEN)
			sResolverState = "broken lby prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_FREESTAND)
			sResolverState = "lby freestand prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_LAYER)
			sResolverState = "layer prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BROKEN)
			sResolverState = "broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND)
			sResolverState = "freestand";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND_DELTA)
			sResolverState = "freestand delta";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND_BROKEN)
			sResolverState = "freestand broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE)
			sResolverState = "bruteforce";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE_BROKEN)
			sResolverState = "bruteforce broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::DELTA)
			sResolverState = "delta";

		Shot->m_DidIntersectPlayer = g_Aimbot.CanHit(Shot->m_StartPosition, Shot->m_StartPosition + (Shot->m_Impacts.back() - Shot->m_StartPosition).Normalized() * Shot->m_StartPosition.Delta(Shot->m_Record->m_Origin) * 1.1f, Shot->m_Player, Shot->m_WantedHitbox, Shot->m_Record);
		if (Shot->m_DidIntersectPlayer) {
			pData->m_MissedShots++;

			if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND && Shot->m_Record->m_ResolverMode != Resolver::Modes::RESOLVE_AIR)
				pData->m_FreestandMissedShots++;

			if (Shot->m_Record->m_ResolverMode != Resolver::Modes::RESOLVE_AIR && (Shot->m_Record->m_Resolved || Shot->m_Record->m_Legit)) {
				if (!Shot->m_Record->m_Velocity.IsValid() && !Shot->m_Record->m_Legit) {
					sMissReason = "desync";
					pData->m_FakeFlickMissedShots++;
				}
				else sMissReason = "animation mismatch";

			}
			else sMissReason = "resolver";
		}

		else if (g_Client.m_PredictionError)
			sMissReason = "prediction error";

		else if (bMissedShotDueToOcclusion)
			sMissReason = "occlusion";

		else if (Shot->m_Record->m_BrokeLagCompensation)
			sMissReason = "lag compensation";

		else if (g_Aimbot.GetHitChance() == 100 || g_Csgo.weapon_accuracy_nospread->GetBool())
			sMissReason = "engine prediction";

		else sMissReason = "spread";

		player_info_t pInfo;
		if (!g_Csgo.m_Engine->GetPlayerInfo(Shot->m_Player->GetClientNetworkable()->GetIndex(), &pInfo))
			continue;

		// TODO@Monster: show server if differs from each other.
		//if (g_Config.vb(g_Items.m_Misc.m_Notifications).at(0)) 
		{
			std::string out = tfm::format(XOR("Shot missed to %s due to %s for %i damage in the %s while %s with %s resolver and history was %i.\n"), pInfo.m_Name, sMissReason, static_cast<int>(Shot->m_Damage), m_Groups[static_cast<size_t>(static_cast<Hitgroups_t>(game::HitboxToHitgroup(Shot->m_WantedHitbox)))], sMissSituation, sResolverState, std::max(g_Client.m_TickCount - Shot->m_ShotTick, 0));
			g_notify.add(out);
		}

		m_Shots.erase(m_Shots.begin() + i);
	}
}

void Shots::OnBulletImpact(IGameEvent* evt) {
	int        attacker;
	Vector     vecPosition, dir, start, end;
	float      flTime;

	// screw this.
	if (!evt || !g_Client.m_LocalPlayer)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// decode impact coordinates and convert to vec3.
	vecPosition = {
		evt->m_keys->FindKey(HASH("x"))->GetFloat(),
		evt->m_keys->FindKey(HASH("y"))->GetFloat(),
		evt->m_keys->FindKey(HASH("z"))->GetFloat()
	};

	// get prediction flTime at this point.
	flTime = g_Csgo.m_Globals->m_Curtime;

	// add to visual impacts if we have features that rely on it enabled.
	if (c_config::get()->b["impact_beams"])
		m_vis_impacts.push_back({ vecPosition, g_Client.m_LocalPlayer->Weapon_ShootPosition(), g_Client.m_LocalPlayer->m_nTickBase() });

	// we did not take a shot yet.
	if (m_Shots.empty())
		return;

	struct ShotMatch_t { float Delta; ShotRecord* ShotData; };
	ShotMatch_t pMatch{};
	pMatch.Delta = std::numeric_limits< float >::max();
	pMatch.ShotData = nullptr;

	for (auto& Iterate : m_Shots) {

		if (Iterate.m_HasBeenRegistered)
			continue;

		float flDelta = std::abs(flTime - (Iterate.m_CurrentTime + Iterate.m_Latency));
		if (flDelta > 1.f)
			continue;

		if (flDelta < pMatch.Delta) {
			pMatch.Delta = flDelta;
			pMatch.ShotData = &Iterate;
		}
	}

	if (!pMatch.ShotData)
		return;

	pMatch.ShotData->m_HasBeenRegistered = true;
	pMatch.ShotData->m_Impacts.emplace_back(vecPosition);
}

void Shots::OnHurt(IGameEvent* evt) {
	int         attacker, victim, group, hp;
	float       damage;
	std::string name;

	if (!evt || !g_Client.m_LocalPlayer)
		return;

	attacker = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("attacker"))->GetInt());
	victim = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());

	// skip invalid player indexes.
	// should never happen? world entity could be attacker, or a nade that hits you.
	if (attacker < 1 || attacker > 64 || victim < 1 || victim > 64)
		return;

	// we were not the attacker or we hurt ourselves.
	else if (attacker != g_Csgo.m_Engine->GetLocalPlayer() || victim == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// get hitgroup.
	// players that get naded ( DMG_BLAST ) or stabbed seem to be put as GENERIC.
	group = evt->m_keys->FindKey(HASH("hitgroup"))->GetInt();

	// invalid hitgroups ( note - dex; GEAR isn't really invalid, seems to be set for hands and stuff? ).
	//if (group == static_cast<int>(Hitgroups_t::GEAR))
		//return;

	// get the player that was hurt.
	Player* target = g_Csgo.m_EntityList->GetClientEntity< Player* >(victim);
	if (!target)
		return;

	// get player info.
	player_info_t info;
	if (!g_Csgo.m_Engine->GetPlayerInfo(victim, &info))
		return;

	// get player name;
	name = std::string(info.m_Name).substr(0, 24);

	// get damage reported by the server.
	damage = (float)evt->m_keys->FindKey(HASH("dmg_health"))->GetInt();

	// get remaining hp.
	hp = evt->m_keys->FindKey(HASH("health"))->GetInt();

	// setup headshot marker
	m_headshot = (group == static_cast<int>(Hitgroups_t::HEAD));

	if (g_Config.b(g_Items.m_Visuals.m_HitmarkerEnabled)) {
		m_HitmarkerEraseTime = g_Csgo.m_Globals->m_Curtime + 1;
		m_HitmarkerSize = 0.f;
		m_CompleteHitmarkerSize = false;
		m_HitmarkerAlpha = 255;
	}

	// hitmarker.
	if (c_config::get()->i["hitmarker_type"] > 0) {
		g_Visuals.m_hit_duration = 2.f;
		g_Visuals.m_hit_start = g_Csgo.m_Globals->m_Curtime;
		g_Visuals.m_hit_end = g_Visuals.m_hit_start + g_Visuals.m_hit_duration;

		// bind to draw
		m_Damage = damage;

		// get interpolated origin.
		m_Origin = target->GetAbsOrigin();

		// get Hitbox bounds.
		target->ComputeHitboxSurroundingBox(&m_OBBMins, &m_OBBMaxs);

		// correct x and y coordinates.
		m_OBBMins = { m_Origin.x, m_Origin.y, m_OBBMins.z };
		m_OBBMaxs = { m_Origin.x, m_Origin.y, m_OBBMaxs.z + 8.f };
	}

	if (g_Config.b(g_Items.m_Visuals.m_DamageEnabled) && g_Config.b(g_Items.m_Visuals.m_Enemies.m_Enabled))
	{
		DamageIndicator_t Indicate;
		Indicate.m_Damage = damage;
		Indicate.m_Alpha = 0;
		Indicate.m_Position = target->Weapon_ShootPosition();
		Indicate.m_EraseTime = g_Csgo.m_Globals->m_Curtime + 3.f;
		Indicate.m_Complete = false;
		Indicate.m_AnimationX = 0.f;
		Indicate.m_AnimationY = 0.f;
		Indicate.m_RandAnimAmount = g_Csgo.RandomFloat(0.05f, 0.07f);
		Indicate.m_RandAnimVel = g_Csgo.RandomFloat(20.f, 27.f);
		Indicate.m_Color = Color(g_Csgo.RandomInt(0, 255), g_Csgo.RandomInt(0, 255), g_Csgo.RandomInt(0, 255));
		Indicate.m_Output = XOR("-") + std::to_string(static_cast<int>(damage));
		m_DmgIndicator.emplace_back(Indicate);
	}

	if (g_Config.b(g_Items.m_Visuals.m_Effects.m_EnableHitSound))
		g_Csgo.m_EngineSound->EmitAmbientSound(XOR("buttons/arena_switch_press_02.wav"), 1.f);

	// if we hit a player, mark vis impacts.
	if (!m_vis_impacts.empty()) {
		for (auto& i : m_vis_impacts) {
			if (i.m_tickbase == g_Client.m_LocalPlayer->m_nTickBase())
				i.m_hit_player = true;
		}
	}

	struct ShotMatch_t { float Delta; ShotRecord* ShotData; };
	ShotMatch_t pMatch{};
	pMatch.Delta = std::numeric_limits< float >::max();
	pMatch.ShotData = nullptr;

	for (auto& Iterate : m_Shots) {

		if (Iterate.m_HasBeenHurted)
			continue;

		float flDelta = std::abs(g_Csgo.m_Globals->m_Curtime - (Iterate.m_CurrentTime + Iterate.m_Latency));
		if (flDelta > 1.f)
			continue;

		if (flDelta < pMatch.Delta) {
			pMatch.Delta = flDelta;
			pMatch.ShotData = &Iterate;
		}
	}

	if (pMatch.ShotData) {
		pMatch.ShotData->m_HasBeenHurted = true;
		pMatch.ShotData->m_ServerHitgroup = (Hitgroups_t)group;

		AimPlayer* pData = &g_Aimbot.m_Players[pMatch.ShotData->m_Player->GetClientNetworkable()->GetIndex() - 1];
		if (!pData)
			return;

		LagRecord* pRecord = pData->m_Records.front().get();
		if (pRecord == nullptr)
			return;

		std::string sMissSituation;
		std::string sResolverState;

		if (pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_AIR)
			sMissSituation = "in air";

		else if (pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_WALK)
			sMissSituation = "moving";

		else if (pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_STAND)
			sMissSituation = "standing";

		if (pData->m_ResolverMode == AimPlayer::RESOLVER::LEGIT)
			sResolverState = "legit";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST)
			sResolverState = "last moving lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_BROKEN)
			sResolverState = "last broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_FREESTAND)
			sResolverState = "last freestanding lby";

		else if (pRecord->m_Resolved || pData->m_ResolverMode == AimPlayer::RESOLVER::RESOLVED)
			sResolverState = pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_STAND ? "lby update" : pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_WALK ? "moving lby" : "static lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION)
			sResolverState = "lby prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_BROKEN)
			sResolverState = "broken lby prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_FREESTAND)
			sResolverState = "lby freestanding prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_LAYER)
			sResolverState = "layer prediction";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BROKEN)
			sResolverState = "broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND)
			sResolverState = "freestanding";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE)
			sResolverState = "bruteforce";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE_BROKEN)
			sResolverState = "bruteforce broken lby";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::DELTA)
			sResolverState = "delta";

		// TODO@Monster: show server if differs from each other.
		if (g_Config.vb(g_Items.m_Misc.m_Notifications).at(0)) {
			// TODO@Monster: instead of wanted use differs for original.
			std::string out = tfm::format(XOR("Harmed %s for %i damage in the %s while %s with %s resolver caused remained health for %i and history was %i. (wanted damage was %i, wanted hitgroup was %s)\n"),
				name, static_cast<int>(damage), m_Groups[static_cast<size_t>(pMatch.ShotData->m_ServerHitgroup)], sMissSituation, sResolverState , hp, std::max(g_Client.m_TickCount - pMatch.ShotData->m_ShotTick, 0),
				static_cast<int>(pMatch.ShotData->m_Damage), m_Groups[static_cast<size_t>(static_cast<Hitgroups_t>(game::HitboxToHitgroup(pMatch.ShotData->m_WantedHitbox)))]);
			g_notify.add(out);
		}
	}
	else if (g_Config.vb(g_Items.m_Misc.m_Notifications).at(0)) {
		std::string out = tfm::format(XOR("Harmed %s for %i damage in the %s caused remaining health for %i.\n"),
			name, static_cast<int>(damage), m_Groups[group], hp);
		g_notify.add(out);
	}
}

void Shots::OnWeaponFire(IGameEvent* evt)
{
	int attacker;

	// screw this.
	if (!evt || !g_Client.m_LocalPlayer)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_Csgo.m_Engine->GetLocalPlayer())
		return;

	struct ShotMatch_t { float Delta; ShotRecord* ShotData; };
	ShotMatch_t pMatch{};
	pMatch.Delta = std::numeric_limits< float >::max();
	pMatch.ShotData = nullptr;

	for (auto& Iterate : m_Shots) {

		if (Iterate.m_HasBeenFired)
			continue;

		float flDelta = std::abs(g_Csgo.m_Globals->m_Curtime - (Iterate.m_CurrentTime + Iterate.m_Latency));
		if (flDelta > 1.f)
			continue;

		if (flDelta < pMatch.Delta) {
			pMatch.Delta = flDelta;
			pMatch.ShotData = &Iterate;
		}
	}

	if (pMatch.ShotData)
		pMatch.ShotData->m_HasBeenFired = true;
}