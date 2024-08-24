#include "../../precompiled.h"

void events::round_start(IGameEvent* evt) {
	g_Client.m_round_end = false;
	g_Client.m_round_start = true;

	// remove notices.
	if (c_config::get()->b["killfeed"]) {
		KillFeed_t* feed = (KillFeed_t*)g_Csgo.m_Hud->FindElement(HASH("SFHudDeathNoticeAndBotStatus"));
		if (feed)
			g_Csgo.ClearNotices(feed);
	}

	for (int i{ 1 }; i <= g_Csgo.m_EntityList->GetHighestEntityIndex(); ++i) {
		Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity<Player*>(i);
		if (pPlayer == nullptr || !pPlayer->IsPlayer())
			continue;

		g_SoundESP.Reset(pPlayer);
		g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1].reset();
	}

	g_Client.m_PoseParameters.fill(0.f);
	g_Client.m_AnimationLayers.fill(C_AnimationLayer());

	g_Client.m_SpawnTime = 0.f;
	g_Client.m_CommandNumber.clear();
	g_Client.m_TransmissionCorrectionData.clear();
	g_Client.m_TransmissionCommands.clear();

	g_Visuals.m_c4_planted = false;
	g_Visuals.m_planted_c4 = nullptr;
	g_Visuals.bombexploded = false;
	g_Visuals.bombedefused = false;

	g_Visuals.m_draw.fill(false);
	g_Visuals.m_opacities.fill(0.f);
	g_Visuals.m_offscreen_damage.fill(OffScreenDamageData_t());

	// buybot.
	{
		std::vector<std::string> first =
		{
			("galilar"),
			("famas"),
			("ak47"),
			("m4a1"),
			("m4a1_silencer"),
			("ssg08"),
			("aug"),
			("sg556"),
			("awp"),
			("scar20"),
			("g3sg1"),
			("nova"),
			("xm1014"),
			("mag7"),
			("m249"),
			("negev"),
			("mac10"),
			("mp9"),
			("mp7"),
			("ump45"),
			("p90"),
			("bizon")
		};

		std::vector<std::string> second =
		{
			("glock"),
			("hkp2000"),
			("usp_silencer"),
			("elite"),
			("p250"),
			("tec9"),
			("fn57"),
			("deagle")
		};

		std::vector<std::string> third =
		{
			("vest"),
			("vesthelm"),
			("taser"),
			("defuser"),
			("heavyarmor"),
			("molotov"),
			("incgrenade"),
			("decoy"),
			("flashbang"),
			("hegrenade"),
			("smokegrenade")
		};

		auto buy1 = g_Config.vb(g_Items.m_Misc.m_AutoBuyMain);
		auto buy2 = g_Config.vb(g_Items.m_Misc.m_AutoBuySecondary);
		auto buy3 = g_Config.vb(g_Items.m_Misc.m_AutoBuyMisc);

		const auto buy_ = [](const std::string& what) -> void
		{
			g_Csgo.m_Engine->ExecuteClientCmd(tfm::format(XOR("buy %s"), what.c_str()).data());
		};

		if (buy1[0])
			buy_(first.at(0));
		if (buy1[1])
			buy_(first.at(1));
		if (buy1[2])
			buy_(first.at(2));
		if (buy1[3])
			buy_(first.at(3));
		if (buy1[4])
			buy_(first.at(4));
		if (buy1[5])
			buy_(first.at(5));
		if (buy1[6])
			buy_(first.at(6));
		if (buy1[7])
			buy_(first.at(7));
		if (buy1[8])
			buy_(first.at(8));
		if (buy1[9])
			buy_(first.at(9));
		if (buy1[10])
			buy_(first.at(10));
		if (buy1[11])
			buy_(first.at(11));
		if (buy1[12])
			buy_(first.at(12));
		if (buy1[13])
			buy_(first.at(13));
		if (buy1[14])
			buy_(first.at(14));
		if (buy1[15])
			buy_(first.at(15));
		if (buy1[16])
			buy_(first.at(16));
		if (buy1[17])
			buy_(first.at(17));
		if (buy1[18])
			buy_(first.at(18));
		if (buy1[19])
			buy_(first.at(19));
		if (buy1[20])
			buy_(first.at(20));
		if (buy1[21])
			buy_(first.at(21));

		if (buy2[0])
			buy_(second.at(0));
		if (buy2[1])
			buy_(second.at(1));
		if (buy2[2])
			buy_(second.at(2));
		if (buy2[3])
			buy_(second.at(3));
		if (buy2[4])
			buy_(second.at(4));
		if (buy2[5])
			buy_(second.at(5));
		if (buy2[6])
			buy_(second.at(6));
		if (buy2[7])
			buy_(second.at(7));

		if (buy3[0])
			buy_(third.at(0));
		if (buy3[1])
			buy_(third.at(1));
		if (buy3[2])
			buy_(third.at(2));
		if (buy3[3])
			buy_(third.at(3));
		if (buy3[4])
			buy_(third.at(4));
		if (buy3[5])
			buy_(third.at(5));
		if (buy3[6])
			buy_(third.at(6));
		if (buy3[7])
			buy_(third.at(7));
		if (buy3[8])
			buy_(third.at(8));
		if (buy3[9])
			buy_(third.at(9));
		if (buy3[10])
			buy_(third.at(10));
	}

	// update all players.
	for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
		Player* player = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
		if (!player || player->IsLocalPlayer())
			continue;

		AimPlayer* data = &g_Aimbot.m_Players[i - 1];
		data->OnRoundStart(player);
	}

	// clear shots
	g_ShotSystem.m_hits.clear();
	g_ShotSystem.m_Shots.clear();

	// clear origins.
	g_Client.m_NetPos.clear();
}

void events::round_end(IGameEvent* evt) {
	if (!g_Client.m_LocalPlayer)
		return;

	// get the reason for the round end.
	int reason = evt->m_keys->FindKey(HASH("reason"))->GetInt();

	// reset.
	g_Client.m_round_end = false;
	g_Client.m_round_start = false;

	if (g_Client.m_LocalPlayer->m_iTeamNum() == TEAM_COUNTERTERRORISTS && reason == CSRoundEndReason::CT_WIN)
		g_Client.m_round_end = true;

	else if (g_Client.m_LocalPlayer->m_iTeamNum() == TEAM_TERRORISTS && reason == CSRoundEndReason::T_WIN)
		g_Client.m_round_end = true;
}

void events::round_prestart(IGameEvent* evt) {
	if (!g_Client.m_LocalPlayer)
		return;

	for (int i{ 1 }; i <= g_Csgo.m_EntityList->GetHighestEntityIndex(); ++i) {
		Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity<Player*>(i);
		if (pPlayer == nullptr || !pPlayer->IsPlayer())
			continue;

		g_SoundESP.Reset(pPlayer);
	}
}

void events::player_hurt(IGameEvent* evt) {
	Player* attacker = nullptr;
	Player* victim = nullptr;

	// forward event to shots hurt processing.
	g_ShotSystem.OnHurt(evt);

	//g_InputPrediction.m_flVelocityModifier = 0.501961f;

	// offscreen esp damage stuff.
	if (evt && g_Config.vb(g_Items.m_Misc.m_Notifications).at(1)) {
		attacker = g_Csgo.m_EntityList->GetClientEntity<Player*>(g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("attacker"))->GetInt()));
		victim = g_Csgo.m_EntityList->GetClientEntity<Player*>(g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt()));

		if (!attacker || !victim)
			return;

		if (!attacker->IsPlayer())
			return;

		player_info_t info;
		g_Csgo.m_Engine->GetPlayerInfo(attacker->GetClientNetworkable()->GetIndex(), &info);

		if (attacker != g_Client.m_LocalPlayer && victim == g_Client.m_LocalPlayer) {
			std::string name{ std::string(info.m_Name).substr(0, 24) };
			float damage = evt->m_keys->FindKey(HASH("dmg_health"))->GetInt();
			int hp = evt->m_keys->FindKey(HASH("health"))->GetInt();

			std::string out = tfm::format(XOR("Local harmed by: %s | Given damage: %i | Server hitgroup: %s | Remained health: %i\n"), name, static_cast<int>(damage), g_ShotSystem.m_Groups[evt->m_keys->FindKey(HASH("hitgroup"))->GetInt()], hp);
			g_notify.add(out);
		}

		// a player damaged the local player.
		if (attacker->GetClientNetworkable()->GetIndex() > 0 && attacker->GetClientNetworkable()->GetIndex() < 64 && victim == g_Client.m_LocalPlayer)
			g_Visuals.m_offscreen_damage[attacker->GetClientNetworkable()->GetIndex()] = { 3.f, 0.f, colors::red };
	}
}

void events::bullet_impact(IGameEvent* evt) {
	// get attacker, if its not us, screw it.
	int        attacker;

	if (!g_Client.m_Processing)
		return;

	// forward event to resolver impact processing.
	g_ShotSystem.OnBulletImpact(evt);

	attacker = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// screw this.
	if (!evt || !g_Client.m_LocalPlayer)
		return;

	// decode impact coordinates and convert to vec3
	Vector pos = {
			evt->m_keys->FindKey(HASH("x"))->GetFloat(),
			evt->m_keys->FindKey(HASH("y"))->GetFloat(),
			evt->m_keys->FindKey(HASH("z"))->GetFloat()
	};

	if (g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactServer)) {
		if (attacker == g_Csgo.m_Engine->GetLocalPlayer())
			g_Csgo.m_DebugOverlay->AddBoxOverlay(pos, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f), QAngle(0.0f, 0.0f, 0.0f), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactServerColor).r(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactServerColor).g(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactServerColor).b(), g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactServerColor).a(), 4.0f);
	}
}

void events::weapon_fire(IGameEvent* evt) {
	if (!g_Client.m_Processing)
		return;

	// screw this.
	if (!evt || !g_Client.m_LocalPlayer)
		return;

	g_ShotSystem.OnWeaponFire(evt);
}

void events::item_purchase(IGameEvent* evt) {
	int           team, purchaser;
	player_info_t info;

	if (!g_Client.m_LocalPlayer || !evt)
		return;

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(1))
		return;

	// only log purchases of the enemy team.
	team = evt->m_keys->FindKey(HASH("team"))->GetInt();
	if (team == g_Client.m_LocalPlayer->m_iTeamNum())
		return;

	// get the player that did the purchase.
	purchaser = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());

	// get player info of purchaser.
	if (!g_Csgo.m_Engine->GetPlayerInfo(purchaser, &info))
		return;

	std::string weapon = evt->m_keys->FindKey(HASH("weapon"))->m_string;
	if (weapon == XOR("weapon_unknown"))
		return;

	// TODO@Monster: Add money amount of gun, etc.
	std::string out = tfm::format(XOR("Player: %s | Has bought %s.\n"), std::string{ info.m_Name }.substr(0, 24), weapon);
	g_notify.add(out);
}

void events::player_death(IGameEvent* evt) {

	if (!evt || !g_Client.m_LocalPlayer)
		return;

	if (!strcmp(evt->GetName(), "player_death")) {
		// get index of player that died.
		int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
		if (!index)
			return;

		// reset opacity scale.
		g_Visuals.m_opacities[index - 1] = 0.f; // THIS SHIT WAS ON SUM CRASHES CUS WE HAD NOT ADDED THE CHECKS LMAAAAAAAAAAAAAOOO ASS CANCER
		g_Visuals.m_draw[index - 1] = false;
	}
}

void events::player_given_c4(IGameEvent* evt) {
	player_info_t info;
	
	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(2))
		return;

	// get the player who received the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;

	std::string out = tfm::format(XOR("Player: %s | Received the bomb.\n"), std::string{ info.m_Name }.substr(0, 24));
	g_notify.add(out);
}

void events::bomb_beginplant(IGameEvent* evt) {
	player_info_t info;

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(2))
		return;

	// get the player who played the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// get player info of purchaser.
	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;

	// TODO@Monster: Add more information of player perhaps, HP?
	std::string out = tfm::format(XOR("Player: %s | Has started planting the bomb.\n"), std::string{ info.m_Name }.substr(0, 24));
	g_notify.add(out);
}

void events::bomb_abortplant(IGameEvent* evt) {
	player_info_t info;

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(2))
		return;

	// get the player who stopped planting the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// get player info of purchaser.
	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;

	// TODO@Monster: Add more information of player perhaps, HP?
	std::string out = tfm::format(XOR("Player: %s | Has stopped planting the bomb.\n"), std::string{ info.m_Name }.substr(0, 24));
	g_notify.add(out);
}

void events::bomb_planted(IGameEvent* evt) {
	Entity* bomb_target;
	std::string   site_name;
	player_info_t info;
	std::string   out;

	// get the func_bomb_target entity and store info about it.
	bomb_target = g_Csgo.m_EntityList->GetClientEntity(evt->m_keys->FindKey(HASH("site"))->GetInt());
	if (bomb_target) {
		site_name = bomb_target->GetBombsiteName();
		g_Visuals.m_last_bombsite = site_name;
	}

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(2))
		return;

	// get the player who stopped planting the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	// get player info of purchaser.
	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;


	// TODO@Monster: Add more information of player perhaps, HP?
	out = tfm::format(XOR("Player: %s | The bomb was planted at %s.\n"), std::string(info.m_Name).substr(0, 24), site_name.c_str());
	g_notify.add(out);
}

void events::bomb_beep(IGameEvent* evt) {
	Entity* c4;
	Vector             explosion_origin, explosion_origin_adjusted;
	CTraceFilterSimple filter;
	CGameTrace         tr;

	// we have a bomb ent already, don't do anything else.
	if (g_Visuals.m_c4_planted)
		return;

	// bomb_beep is called once when a player plants the c4 and contains the entindex of the C4 weapon itself, we must skip that here.
	c4 = g_Csgo.m_EntityList->GetClientEntity(evt->m_keys->FindKey(HASH("entindex"))->GetInt());
	if (!c4 || !c4->is(HASH("CPlantedC4")))
		return;

	// planted bomb is currently active, grab some extra info about it and set it for later.
	g_Visuals.m_c4_planted = true;
	g_Visuals.m_planted_c4 = c4;
	g_Visuals.m_planted_c4_explode_time = c4->m_flC4Blow();

	// the bomb origin is adjusted slightly inside CPlantedC4::C4Think, right when it's about to explode.
	// we're going to do that here.
	explosion_origin = c4->GetAbsOrigin();
	explosion_origin_adjusted = explosion_origin;
	explosion_origin_adjusted.z += 8.f;

	// setup filter and do first trace.
	filter.SetPassEntity(c4);

	g_Csgo.m_EngineTrace->TraceRay(
		Ray(explosion_origin_adjusted, explosion_origin_adjusted + Vector(0.f, 0.f, -40.f)),
		MASK_SOLID,
		&filter,
		&tr
	);

	// pull out of the wall a bit.
	if (tr.m_Fraction != 1.f)
		explosion_origin = tr.m_EndPosition + (tr.m_Plane.m_Normal * 0.6f);

	// this happens inside CCSGameRules::RadiusDamage.
	explosion_origin.z += 1.f;

	// set all other vars.
	g_Visuals.m_planted_c4_explosion_origin = explosion_origin;

	// todo - dex;  get this radius dynamically... seems to only be available in map bsp file, search string: "info_map_parameters"
	//              info_map_parameters is an entity created on the server, it doesnt seem to have many useful networked vars for clients.
	//
	//              swapping maps between de_dust2 and de_nuke and scanning for 500 and 400 float values will leave you one value.
	//              need to figure out where it's written from.
	//
	// server.dll uses starting 'radius' as damage... the real radius passed to CCSGameRules::RadiusDamage is actually multiplied by 3.5.
	g_Visuals.m_planted_c4_damage = 500.f;
	g_Visuals.m_planted_c4_radius = g_Visuals.m_planted_c4_damage * 3.5f;
	g_Visuals.m_planted_c4_radius_scaled = g_Visuals.m_planted_c4_radius / 3.f;
}

void events::bomb_begindefuse(IGameEvent* evt) {
	player_info_t info;

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(3))
		return;

	// get index of player that started defusing the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;

	bool kit = evt->m_keys->FindKey(HASH("haskit"))->GetBool();

	if (kit) {
		std::string out = tfm::format(XOR("Player: %s | Started defusing with a kit. (5 seconds remaining)\n"), std::string(info.m_Name).substr(0, 24));
		g_notify.add(out);
	}

	else {
		std::string out = tfm::format(XOR("Player: %s | Started defusing without a kit. (10 seconds remaining)\n"), std::string(info.m_Name).substr(0, 24));
		g_notify.add(out);
	}
}

void events::bomb_abortdefuse(IGameEvent* evt) {
	player_info_t info;

	if (!g_Config.vb(g_Items.m_Misc.m_Notifications).at(3))
		return;

	// get index of player that stopped defusing the bomb.
	int index = g_Csgo.m_Engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (index == g_Csgo.m_Engine->GetLocalPlayer())
		return;

	if (!g_Csgo.m_Engine->GetPlayerInfo(index, &info))
		return;

	std::string out = tfm::format(XOR("Player: %s | Stopped defusing\n"), std::string(info.m_Name).substr(0, 24));
	g_notify.add(out);
}

void events::bomb_exploded(IGameEvent* evt) {
	g_Visuals.m_c4_planted = false;
	g_Visuals.m_planted_c4 = nullptr;
	g_Visuals.bombexploded = true;
}

void events::bomb_defused(IGameEvent* evt) {
	g_Visuals.m_c4_planted = false;
	g_Visuals.m_planted_c4 = nullptr;
	g_Visuals.bombedefused = true;
}

void Listener::Init() {
	// link events with callbacks.
	add(XOR("round_start"), events::round_start);
	add(XOR("round_prestart"), events::round_prestart);
	add(XOR("round_end"), events::round_end);
	add(XOR("player_hurt"), events::player_hurt);
	add(XOR("bullet_impact"), events::bullet_impact);
	add(XOR("weapon_fire"), events::weapon_fire);
	add(XOR("item_purchase"), events::item_purchase);
	add(XOR("player_death"), events::player_death);
	add(XOR("player_given_c4"), events::player_given_c4);
	add(XOR("bomb_beginplant"), events::bomb_beginplant);
	add(XOR("bomb_abortplant"), events::bomb_abortplant);
	add(XOR("bomb_planted"), events::bomb_planted);
	add(XOR("bomb_beep"), events::bomb_beep);
	add(XOR("bomb_begindefuse"), events::bomb_begindefuse);
	add(XOR("bomb_abortdefuse"), events::bomb_abortdefuse);
	add(XOR("bomb_exploded"), events::bomb_exploded);
	add(XOR("bomb_defused"), events::bomb_defused);

	RegisterEvents();
}