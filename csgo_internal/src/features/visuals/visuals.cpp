#include "../../precompiled.h"

//#define DEVELOPING

void newvisuals::Init()
{
	if (!g_Client.m_LocalPlayer)
		return;

	g_notify.think();

	// update screen size.
	g_Csgo.m_Engine->GetScreenSize(g_Client.m_Width, g_Client.m_Height);

	for (int i{ 1 }; i <= g_Csgo.m_EntityList->GetHighestEntityIndex(); ++i) {
		Entity* pEntity = g_Csgo.m_EntityList->GetClientEntity(i);

		if (pEntity == nullptr)
			continue;

		m_Weapon = pEntity->as< Weapon* >();
		if (m_Weapon) {
			DrawWorldItems();
			DrawProjectiles();
			// g_Grenades.think_warning(m_Weapon);
		}

		if (!pEntity->IsPlayer())
			continue;

		m_Player = pEntity->as< Player* >();
		if (!m_Player)
			continue;

		// draw player esp.
		Draw();
	}

#if DEVELOPING
	// note@ : use this func when debugging.
	DebugScreen();
#else	
	PenCrosshair();
	DrawHitmarker();
	PreverseKillFeed();
#endif
	Others();
}

void newvisuals::Draw()
{
	// note@ : only draw player visuals inside this scope.
	if (!m_Player || !m_Player->IsPlayer())
		return;

	// gather information.
	m_Enemy = m_Player->enemy(g_Client.m_LocalPlayer);
	m_Teammate = m_Player->teammate(g_Client.m_LocalPlayer);
	m_Local = m_Player->IsLocalPlayer();
	m_Dormant = m_Player->GetClientNetworkable()->IsDormant();

	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_Enabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_Enabled), g_Config.b(g_Items.m_Visuals.m_Local.m_Enabled)))
		return;

	// set dormancy times.
	constexpr float MAX_DORMANT_TIME = 10.f;
	constexpr float DORMANT_FADE_TIME = MAX_DORMANT_TIME / 2.f;

	// get player index.
	m_Index = m_Player->GetClientNetworkable()->GetIndex();
	if (!m_Index)
		return;

	// get player info.
	if (!g_Csgo.m_Engine->GetPlayerInfo(m_Index, &m_PlayerInfo))
		return;

	// draw when we hit a player.
	DrawDamage();

	// get reference to array variable.
	float& flOpacity = m_Opacities[m_Index - 1];

	const float flStep = (1.f / 0.5f) * g_Csgo.m_Globals->m_FrameTime;

	if (m_Player->IsAlive())
	{
		if (!m_Dormant)
			flOpacity += flStep;
		else
			flOpacity -= flStep;
	}
	else flOpacity -= flStep;

	Math::clamp(flOpacity, 0.f, 1.f);

	// we have a dead player and their opacity is 0.
	if (!m_Player->IsAlive() && !flOpacity)
		return;

	bool bDormantEsp = ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_DormantEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_DormantEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_DormantEnabled));

	if (!flOpacity && !bDormantEsp)
		return;

	// stay for x seconds max.
	float flDeltaTime = g_Csgo.m_Globals->m_Curtime - m_Player->m_flSimulationTime();
	if (m_Dormant && flDeltaTime > MAX_DORMANT_TIME)
		return;

	// calculate alphas.
	m_Alpha = static_cast<int>(220 * flOpacity);
	m_LowAlpha = static_cast<int>(159 * flOpacity);

	if (m_Dormant && bDormantEsp) {
		m_Alpha = 127;
		m_LowAlpha = 100;

		// fade.
		if (flDeltaTime > DORMANT_FADE_TIME) {
			// for how long have we been fading?
			float faded = (flDeltaTime - DORMANT_FADE_TIME);
			float scale = 1.f - (faded / DORMANT_FADE_TIME);

			m_Alpha *= scale;
			m_LowAlpha *= scale;
		}
	}

	// don't draw if we can't get the collidible area.
	if (!GetBox()) {

		// draw out of view.
		DrawOutOfView();

		return;
	}

	// draw resolver info.
	//Resolver();

	// draw box esp.
	DrawBox();

	// draw name esp.
	DrawName();

	// draw health bar.
	DrawHealthBar();

	// draw SetUpFlashedReaction.
	DrawFlashEffect();

	// draw skeleton.
	DrawSkeleton();

	// draw flags.
	DrawFlags();

	// draw ammo bar.
	DrawBottomBars();
}

bool newvisuals::GetBox()
{
	Vector vecMin, vecMax, vecOut;
	float flLeft, flBottom, flRight, flTop;
	matrix3x4_t& tran_frame = m_Player->GetCoordinateFrame();

	// get hitbox bounds.
	vecMin = m_Player->GetCollideable()->OBBMins();
	vecMax = m_Player->GetCollideable()->OBBMaxs();

	// set a longer box
	m_BoxLonger = 3;

	Vector2D vecScreenBoxes[8];

	// transform mins and maxes to points. 
	Vector points[] =
	{
		{ vecMin.x, vecMin.y, vecMin.z },
		{ vecMin.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMin.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMin.y, vecMax.z },
		{ vecMax.x, vecMin.y, vecMax.z }
	};

	// transform points to 3-dimensional space.
	for (int i = 0; i <= 7; i++) {
		Math::VectorTransform(points[i], tran_frame, vecOut);
		if (!Math::WorldToScreen(vecOut, vecScreenBoxes[i]))
			return false;
	}

	// generate an array to clamp later.
	Vector2D vecBoxArray[] = {
		vecScreenBoxes[3],
		vecScreenBoxes[5],
		vecScreenBoxes[0],
		vecScreenBoxes[4],
		vecScreenBoxes[2],
		vecScreenBoxes[1],
		vecScreenBoxes[6],
		vecScreenBoxes[7]
	};

	// state the position and size of the box.
	flLeft = vecScreenBoxes[3].x,
	flBottom = vecScreenBoxes[3].y,
	flRight = vecScreenBoxes[3].x,
	flTop = vecScreenBoxes[3].y;

	// clamp the box sizes.
	for (int i = 0; i <= 7; i++) {
		if (flLeft > vecBoxArray[i].x)
			flLeft = vecBoxArray[i].x;

		if (flBottom < vecBoxArray[i].y)
			flBottom = vecBoxArray[i].y;

		if (flRight < vecBoxArray[i].x)
			flRight = vecBoxArray[i].x;

		if (flTop > vecBoxArray[i].y)
			flTop = vecBoxArray[i].y;
	}

	// state the box bounds.
	m_Box.x = flLeft;
	m_Box.y = flTop;
	m_Box.w = flRight - flLeft;
	m_Box.h = flBottom - flTop;

	return true;
}

void newvisuals::DrawBox()
{
	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_BoxEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_BoxEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_BoxEnabled)))
		return;

	Color colBox = ConfigureTeams(g_Config.c(g_Items.m_Visuals.m_Enemies.m_BoxColor), g_Config.c(g_Items.m_Visuals.m_Allies.m_BoxColor), g_Config.c(g_Items.m_Visuals.m_Local.m_BoxColor)).OverrideAlpha(m_Alpha);
	switch (ConfigureTeams(g_Config.i(g_Items.m_Visuals.m_Enemies.m_BoxType), g_Config.i(g_Items.m_Visuals.m_Allies.m_BoxType), g_Config.i(g_Items.m_Visuals.m_Local.m_BoxType)))
	{
	case 0:
		new_render.Rect(m_Box.x - 1.0f, m_Box.y + 1.0f - m_BoxLonger, m_Box.w, m_Box.h + m_BoxLonger + 4, { 10, 10, 10, m_LowAlpha });
		new_render.Rect(m_Box.x, m_Box.y - m_BoxLonger, m_Box.w, m_Box.h + m_BoxLonger + 4, colBox);
		break;
	case 1:
		new_render.CornerBox(m_Box.x, m_Box.y - m_BoxLonger, m_Box.w, m_Box.h + m_BoxLonger + 4, colBox, { 10, 10, 10, m_LowAlpha });
		break;
	case 2:
		new_render.Box3D(m_Player->GetCollideable()->OBBMaxs(), m_Player->m_vecOrigin(), m_Player->m_angEyeAngles().y, 25, colBox);
		break;
	}
}

void newvisuals::DrawFlashEffect()
{
	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_FlashEffectEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_FlashEffectEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_FlashEffectEnabled)))
		return;

	const C_AnimationLayer* pLayerFlashedAct = &m_Player->m_AnimOverlay()[ANIMATION_LAYER_FLASHED];

	if (pLayerFlashedAct->GetWeight() <= 0.f)
		return;

	const float flMin = 360 * pLayerFlashedAct->GetWeight() / 1.f;

	// note@ : (only for first commit) didn't delete the commented out ones 
	//just because we could switch the flMin behaviour upon the game mode ( hvh or legit )
	//can do it in the future once we add legit bot. 

	//static bool bMinReachedMax[65] = {};

	//if (iMin == 360)
	//	bMinReachedMax[m_Index] = true;
	//else if (iMin == 0)
	//	bMinReachedMax[m_Index] = false;

	new_render.CircleFilled({ m_Box.x, m_Box.y - m_BoxLonger }, 12, { 10, 10, 10, m_LowAlpha });

	//const int iCalcMin = bMinReachedMax[m_Index] ? iMin : 360;
	const int r = std::min((510.f * (360 - flMin)) / 360.f, 255.f);
	const int g = std::min((510.f * flMin) / 360.f, 255.f);
	new_render.Arc({ m_Box.x, m_Box.y - m_BoxLonger }, 5.f, flMin, 0.f, { r, g, 0, m_Alpha }, 10.f);
}

void newvisuals::DrawName()
{
	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_NameEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_NameEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_NameEnabled)))
		return;

	// long names are not allowed.
	std::string strName{ std::string(m_PlayerInfo.m_Name).substr(0, 24) };

	const Vector2D& vecTextSize = new_render.TextSize(strName, CRender::Fonts::m_esp_font);
	const float flCentre = vecTextSize.x / 2; // todo@ : add font flags instead.

	new_render.StringShadow({ m_Box.x - flCentre + m_Box.w / 2.f, m_Box.y - vecTextSize.y - (m_BoxLonger + 4.f) }, //note@ : since we are using freetype bitmap glyphs, we need to add box_longer + 4.f (box_longer + 1.f default).
		CRender::Fonts::m_esp_font, { 10, 10, 10, m_LowAlpha }, { 255, 255, 255, m_Alpha }, strName);
}

void newvisuals::DrawHealthBar()
{
	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_HealthBarEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_HealthBarEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_HealthBarEnabled)))
		return;

	const int h = m_Box.h;
	const int y = m_Box.y;

	const int hp = m_MaxEntity[m_Index] > m_Player->m_iHealth() ? m_MaxEntity[m_Index] -= Easings::OutQuint((1.f / 0.5f) * g_Csgo.m_Globals->m_FrameTime) : m_MaxEntity[m_Index] = std::min(100, m_Player->m_iHealth());

	const int r = std::min((510 * (100 - hp)) / 100, 255);
	const int g = std::min((510 * hp) / 100, 255);

	new_render.RectFilled(m_Box.x - 7, y - 2, 4, h + 3 + 2, { 10, 10, 10, m_LowAlpha });

	bool bOverrideHealthCol = ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_HealthBarOverrideEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_HealthBarOverrideEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_HealthBarOverrideEnabled));
	Color colHealth = bOverrideHealthCol ? ConfigureTeams(g_Config.c(g_Items.m_Visuals.m_Enemies.m_HealthBarOverrideColor), g_Config.c(g_Items.m_Visuals.m_Allies.m_HealthBarOverrideColor), g_Config.c(g_Items.m_Visuals.m_Local.m_HealthBarOverrideColor)) : Color(r, g, 0, m_Alpha);
	colHealth.a() = m_Alpha;

	const int iFillHeight = hp * h / 100.f;
	new_render.Rect(m_Box.x - 6, y + h - iFillHeight - 1, 2, iFillHeight + 3, colHealth);

	// if hp is below max, draw a string.
	if (hp < 100)
		new_render.StringShadow({ m_Box.x - 19, y + h - iFillHeight - 5 },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, { 255, 255, 255, m_Alpha }, std::to_string(hp));
}

void newvisuals::DrawBottomBars()
{
	struct BottomBars {
		Color m_TextColor;
		Color m_RectColor;
		std::string m_Text;
		std::string m_Icon;
		std::string m_MovableText;
		float m_Bar = 0.f;
		bool m_NoBar = false;
		int m_Space = 5;
		int m_IconSpace = 6;
	};

	std::vector< BottomBars > bottoms = {};
	BottomBars weaponIcon = {};
	BottomBars weaponText = {};
	BottomBars playerDistance = {};

	if (ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_AmmoBarEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_AmmoBarEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_AmmoBarEnabled)))
	{
		Weapon* pWeapon = m_Player->GetActiveWeapon();

		if (pWeapon && pWeapon->IsGun()) 
		{
			BottomBars ammoBar = {};

			// the maxclip1 in the weaponinfo
			const int iCurrentClip = pWeapon->m_iClip1();
			const int iMaxClip = pWeapon->GetWpnData()->m_max_clip1;

			// we don't want a weapon to have no max clip. (for weird servers).
			if (iMaxClip != -1) 
			{
				const C_AnimationLayer* pLayerWeaponAct = &m_Player->m_AnimOverlay()[ANIMATION_LAYER_WEAPON_ACTION];

				// set reload state.
				const bool bReload = (pLayerWeaponAct->GetWeight() != 0.f) && (m_Player->GetSequenceActivity(pLayerWeaponAct->GetSequence()) == 967);

				// check for reloading.
				float flReloadScale = bReload ? pLayerWeaponAct->GetCycle() : static_cast<float>(iCurrentClip) / iMaxClip;

				ammoBar.m_Bar = static_cast<int>(std::round(m_Box.w - 2) * flReloadScale) - 1;

				if (!bReload)
				{
					if (iMaxClip != iCurrentClip)
					{
						ammoBar.m_MovableText = std::to_string(iCurrentClip);
						ammoBar.m_TextColor = { 255, 255, 255, m_Alpha };
					}
				}

				ammoBar.m_RectColor = { 50, 145, 235, m_Alpha };

				// add ammo_bar to our stack.
				bottoms.push_back(ammoBar);
			}
		}
	}

	if (ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_LBYBarEnabled), false, false))
	{
		AimPlayer* pAimbotData = &g_Aimbot.m_Players[m_Player->GetClientNetworkable()->GetIndex() - 1];

		//  : assign & push stack in this scope.
		if (!pAimbotData->m_Records.empty() && (pAimbotData && pAimbotData->m_WalkRecord.m_SimulationTime))
		{
			BottomBars LBYTimer = {};
			const LagRecord* pCurrentLagRecord = pAimbotData->m_Records.front().get();

			if (pCurrentLagRecord)
			{
				// calculate box width
				float flLBYCycle = std::clamp<float>(pAimbotData->m_NextLowerbodyYawUpdate - pCurrentLagRecord->m_SimulationTime, 0.f, pAimbotData->m_NextUpdateTime);

				LBYTimer.m_Bar = (m_Box.w * flLBYCycle) / pAimbotData->m_NextUpdateTime;
				if (LBYTimer.m_Bar > 0.f)
				{
					Color colLBY = ConfigureTeams(g_Config.c(g_Items.m_Visuals.m_Enemies.m_LBYColor), g_Config.c(g_Items.m_Visuals.m_Allies.m_LBYColor), g_Config.c(g_Items.m_Visuals.m_Local.m_LBYColor));
					colLBY.a() = m_Alpha;

					LBYTimer.m_RectColor = colLBY;

					// add LBYTimer to our stack.
					bottoms.push_back(LBYTimer);
				}
			}
		}
	}

	if (ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_DistanceEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_DistanceEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_DistanceEnabled)))
	{
		int iDistance = (((m_Player->m_vecOrigin() - g_Client.m_LocalPlayer->m_vecOrigin()).LengthSqr()) * 0.0625) * 0.001;

		if (iDistance > 0) {
			playerDistance.m_Text = tfm::format(XOR("%i FT"), iDistance);
			playerDistance.m_TextColor = { 255, 255, 255, m_Alpha };
			playerDistance.m_NoBar = true;

			weaponText.m_Space += 2;
			weaponIcon.m_IconSpace += 2;

			bottoms.push_back(playerDistance);
		}
	}

	if (ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_WeaponTextEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_WeaponTextEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_WeaponTextEnabled)))
	{
		Weapon* pWeapon = m_Player->GetActiveWeapon();
		if (pWeapon) {
			std::string strWeaponName = pWeapon->GetLocalizedName();

			// transform lowercase to uppercase.
			std::transform(strWeaponName.begin(), strWeaponName.end(), strWeaponName.begin(), ::toupper);

			weaponText.m_Text = strWeaponName;
			weaponText.m_TextColor = { 255, 255, 255, m_Alpha };
			weaponText.m_NoBar = true;

			// move down for weapon icon.
			weaponIcon.m_IconSpace += 2;

			bottoms.push_back(weaponText);
		}
	}

	if (ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_WeaponIconEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_WeaponIconEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_WeaponIconEnabled)))
	{
		Weapon* pWeapon = m_Player->GetActiveWeapon();
		if (pWeapon) {
			std::string strWeaponIcon = tfm::format(XOR("%c"), pWeapon->GetIcon(pWeapon->m_iItemDefinitionIndex()));

			weaponIcon.m_Icon = strWeaponIcon;
			weaponIcon.m_TextColor = { 255, 255, 255, m_Alpha };
			weaponIcon.m_NoBar = true;

			bottoms.push_back(weaponIcon);
		}
	}

	for (size_t i = 0u; i < bottoms.size(); ++i)
	{
		auto& iterate = bottoms.at(i);

		const Vector2D& vecTextSize = new_render.TextSize(iterate.m_Text, CRender::Fonts::m_esp_small_font);
		const Vector2D& vecIconSize = new_render.TextSize(iterate.m_Icon, CRender::Fonts::m_weapon_font);
		// const Vector2D& vecMovableTextSize = new_render.TextSize(iterate.movable_text, CRender::Fonts::m_esp_small_font);

		int iOffset = i * (vecTextSize.y - 4);
		int iOffsetText = i * (vecTextSize.y - 4);

		int iCentre = vecTextSize.x / 2;
		int iCentreIcon = vecIconSize.x / 2;
		// int iCentreMovable = movable_text_size.x / 2;

		// draw. 
		if (!iterate.m_NoBar)
		{
			new_render.RectFilled(m_Box.x, m_Box.y + 7 + m_Box.h + iOffset, m_Box.w - 1, 4, { 10, 10, 10, m_LowAlpha });
			new_render.Rect(m_Box.x + 1, m_Box.y + 8 + m_Box.h + iOffset, iterate.m_Bar, 2, iterate.m_RectColor);
		}

		new_render.StringShadow({ m_Box.x + static_cast<int>(iterate.m_Bar), m_Box.y + 4 + m_Box.h + iOffset },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_MovableText);

		new_render.StringShadow({ m_Box.x - iCentre + m_Box.w / 2, m_Box.y + iterate.m_Space + m_Box.h + iOffset },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Text);

		new_render.StringShadow({ m_Box.x - iCentreIcon + m_Box.w / 2, m_Box.y + iterate.m_IconSpace + m_Box.h + iOffset },
			CRender::Fonts::m_weapon_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Icon);
	}
}

void newvisuals::DrawFlags()
{
	std::vector< std::pair< std::string, Color > > flags;

	const auto& vFlags = ConfigureTeams(g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Flags), g_Config.vb(g_Items.m_Visuals.m_Allies.m_Flags), g_Config.vb(g_Items.m_Visuals.m_Local.m_Flags));

	if (vFlags.at(PLAYERFLAG_MONEY))
	flags.push_back({ tfm::format(XOR("$%i"), m_Player->m_iAccount()), Color(80, 200, 60, m_Alpha) });

	if (vFlags.at(PLAYERFLAG_ARMOR)) 
	{
		if ((m_Player->m_bHasHelmet() && m_Player->m_ArmorValue() > 0))
			flags.push_back({ XOR("HK"), Color(255, 255, 255, m_Alpha) });

		// only helmet.
		else if (m_Player->m_bHasHelmet())
			flags.push_back({ XOR("H"), Color(255, 255, 255, m_Alpha) });

		// only kevlar.
		else if (m_Player->m_ArmorValue() > 0)
			flags.push_back({ XOR("K"), Color(255, 255, 255, m_Alpha) });
	}

	if (vFlags.at(PLAYERFLAG_SCOPED) && m_Player->m_bIsScoped())
		flags.push_back({ XOR("ZOOM"), Color(60, 180, 225, m_Alpha) });

	if (vFlags.at(PLAYERFLAG_FLASHED) && m_Player->m_flFlashBangTime() > 0.f)
		flags.push_back({ XOR("BLIND"), Color(60, 180, 225, m_Alpha) });

	const C_AnimationLayer* pLayerWeaponAct = &m_Player->m_AnimOverlay()[ANIMATION_LAYER_WEAPON_ACTION];

	if (vFlags.at(PLAYERFLAG_RELOADING) && (pLayerWeaponAct->GetWeight() != 0.f && m_Player->GetSequenceActivity(pLayerWeaponAct->GetSequence()) == ACT_CSGO_RELOAD))
		flags.push_back({ XOR("RELOAD"), Color(60, 180, 225, m_Alpha) });

	if (vFlags.at(PLAYERFLAG_BOMBCARRIER) && m_Player->m_iPlayerC4(m_Player->GetClientNetworkable()->GetIndex()) > 0)
		flags.push_back({ XOR("C4"), Color(255, 0, 0, m_Alpha) });

	if (vFlags.at(PLAYERFLAG_PING) && m_Player->m_iPing(m_Player->GetClientNetworkable()->GetIndex()) > 0)
		flags.push_back({ tfm::format(XOR("PING: %i"), m_Player->m_iPing(m_Player->GetClientNetworkable()->GetIndex())), Color(255, 180, 105, m_Alpha) });

	AimPlayer* pData = &g_Aimbot.m_Players[m_Player->GetClientNetworkable()->GetIndex() - 1];
	if (pData && !pData->m_Records.empty())
	{
		std::string sResolverState;

		if (pData->m_ResolverMode == AimPlayer::RESOLVER::LEGIT)
			sResolverState = "LEGIT";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::RESOLVED)
			sResolverState = "RESOLVED";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST)
			sResolverState = "LAST LBY";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_BROKEN)
			sResolverState = "LAST BROKEN";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::LAST_FREESTAND)
			sResolverState = "LAST FREESTAND";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION)
			sResolverState = "PREDICTION";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_BROKEN)
			sResolverState = "PREDICTION BROKEN";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_FREESTAND)
			sResolverState = "PREDICTION FREESTAND";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::PREDICTION_LAYER)
			sResolverState = "PREDICTION LAYER";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BROKEN)
			sResolverState = "BROKEN LBY";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND)
			sResolverState = "FREESTAND";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND_DELTA)
			sResolverState = "FREESTAND DELTA";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::FREESTAND_BROKEN)
			sResolverState = "FREESTAND BROKEN";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE)
			sResolverState = "BRUTEFORCE";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::BRUTEFORCE_BROKEN)
			sResolverState = "BRUTEFORCE BROKEN";

		else if (pData->m_ResolverMode == AimPlayer::RESOLVER::DELTA)
			sResolverState = "DELTA";

		flags.push_back({ sResolverState, Color(255, 255, 255, m_Alpha) });
	}

	// iterate flags.
	for (size_t i = 0u; i < flags.size(); ++i) {

		// pair them.
		const auto& iterate = flags.at(i);

		const Vector2D& vecTextSize = new_render.TextSize(iterate.first, CRender::Fonts::m_esp_small_font);

		const int iOffset = i * (vecTextSize.y - 2);

		new_render.StringShadow({ m_Box.x + 3 + m_Box.w, m_Box.y + iOffset - m_BoxLonger - 1 },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, iterate.second, iterate.first);
	}
}

void newvisuals::DrawSkeleton()
{
	if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_SkeletonEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_SkeletonEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_SkeletonEnabled)))
		return;

	const model_t* pModel = nullptr;
	const studiohdr_t* pStudioHdr = nullptr;
	int  iParent = 0u;
	matrix3x4a_t  pMatrix[128] = {};
	Vector        vecBonePos = {}, vecParentPos = {}, vecHeadPos = {};
	Vector2D      vecBonePosScreen = {}, vecParentPosScreen = {}, vecHeadPosScreen = {};

	// get player's model.
	pModel = m_Player->GetClientRenderable()->GetModel();
	if (!pModel)
		return;

	// get studio model.
	pStudioHdr = g_Csgo.m_ModelInfo->GetStudioModel(pModel);
	if (!pStudioHdr)
		return;

	// get bone matrix.
	if (!m_Player->SetupBones(pMatrix, 128, BONE_USED_BY_ANYTHING, g_Csgo.m_Globals->m_Curtime))
		return;

	if (m_Player->GetClientNetworkable()->IsDormant())
		return;

	for (int i = 0; i < pStudioHdr->numbones; ++i) {
		// get bone.
		auto pBone = pStudioHdr->pBone(i);
		if (!pBone || !(pBone->flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		iParent = pBone->parent;
		if (iParent == -1)
			continue;

		// resolve main bone and parent bone positions.
		pMatrix->get_bone(vecBonePos, i);
		pMatrix->get_bone(vecParentPos, iParent);
		//pMatrix->get_bone(head_pos, 8);

		Color colSkeleton = { 255, 255, 255, m_Alpha };

		// world to screen head bone.
		//if (render::WorldToScreen(head_pos, head_pos_screen) && Math::WorldToScreen(parent_pos, parent_pos_screen))
		//	new_render.circle({ head_pos_screen.x, head_pos_screen.y - 3 }, 5, clr, 1.f);

		// world to screen both the bone parent bone then draw.
		if (Math::WorldToScreen(vecBonePos, vecBonePosScreen) && Math::WorldToScreen(vecParentPos, vecParentPosScreen))
			new_render.Line(vecBonePosScreen.x, vecBonePosScreen.y, vecParentPosScreen.x, vecParentPosScreen.y, colSkeleton, 1.f);
	}
}

void newvisuals::DrawWorldItems()
{
	if (!g_Config.b(g_Items.m_Visuals.m_World.m_ItemsEnabled))
		return;

	// should never happen.
	if (!m_Weapon->IsBaseCombatWeapon() || m_Weapon->GetClientNetworkable()->IsDormant())
		return;

	// we don't wan't to draw it on a player.
	if (g_Csgo.m_EntityList->GetClientEntityFromHandle(m_Weapon->m_hOwnerEntity()))
		return;

	// todo@ : use them globally, also make dormant alpha increment/decrement.
	int iLowAlpha = 159;
	int iAlpha = 220;

	struct ItemGround {
		Color m_TextColor;
		Color m_AmmoRectColor;
		std::string m_Text;
		std::string m_Icon;
		bool m_Bar = false;
		int m_Space = 0;
	};

	const WeaponInfo* pWeaponData = m_Weapon->GetWpnData();
	if (!pWeaponData)
		return;

	Vector vecScreen;
	// get any weapon origin to screen.
	if (g_Csgo.m_DebugOverlay->ScreenPosition(m_Weapon->GetAbsOrigin(), vecScreen))
		return;

	std::vector< ItemGround > itemOnGround = {};

	if (g_Config.vb(g_Items.m_Visuals.m_World.m_ItemFlags).at(DROPPED_WEAPON_ICON)) {
		ItemGround weapon = {};

		std::string strWeaponIcon = tfm::format(XOR("%c"), m_Weapon->GetIcon(m_Weapon->m_iItemDefinitionIndex()));

		weapon.m_Icon = strWeaponIcon;
		weapon.m_TextColor = { 255, 255, 255, iAlpha };

		itemOnGround.push_back(weapon);
	}

	if (g_Config.vb(g_Items.m_Visuals.m_World.m_ItemFlags).at(DROPPED_WEAPON_TEXT))
	{
		ItemGround weapon = {};

		std::string strWeaponText = m_Weapon->GetLocalizedName();
		std::transform(strWeaponText.begin(), strWeaponText.end(), strWeaponText.begin(), ::toupper);

		weapon.m_Text = strWeaponText;
		weapon.m_TextColor = { 255, 255, 255, iAlpha };

		itemOnGround.push_back(weapon);
	}

	if (g_Config.vb(g_Items.m_Visuals.m_World.m_ItemFlags).at(DROPPED_WEAPON_AMMO))
	{
		if (m_Weapon->IsGun())
		{
			ItemGround ammo = {};

			// get current/max ammo.
			const std::string strAmmoText = tfm::format(XOR("[%i/%i]"), m_Weapon->m_iClip1(), m_Weapon->m_iPrimaryReserveAmmoCount());
			ammo.m_Text = strAmmoText;
			ammo.m_TextColor = { 255, 255, 255, iAlpha };
			itemOnGround.push_back(ammo);

			ItemGround rectAmmo = {};
			rectAmmo.m_Bar = true;
			rectAmmo.m_AmmoRectColor = { 0, 145, 235, iAlpha };
			itemOnGround.push_back(rectAmmo);
		}
	}

	if (g_Config.vb(g_Items.m_Visuals.m_World.m_ItemFlags).at(DROPPED_WEAPON_DISTANCE))
	{
		ItemGround distance = {};

		const int dist = (((m_Weapon->m_vecOrigin() - g_Client.m_LocalPlayer->m_vecOrigin()).LengthSqr()) * 0.0625) * 0.001;

		const std::string strDistText = tfm::format(XOR("%i FT"), dist);

		distance.m_Text = strDistText;
		distance.m_TextColor = { 255, 255, 255, iAlpha };
		distance.m_Space = 1;
		itemOnGround.push_back(distance);
	}

	for (size_t i = 0u; i < itemOnGround.size(); ++i)
	{
		auto& iterate = itemOnGround.at(i);

		const Vector2D& vecTextSize = new_render.TextSize(iterate.m_Text, CRender::Fonts::m_esp_small_font);
		const Vector2D& vecIconSize = new_render.TextSize(iterate.m_Icon, CRender::Fonts::m_weapon_font);

		const float fOffset = i * (vecTextSize.y - 2);
		const float fOffsetIcon = i * (vecIconSize.y - 2);
		const float fCentre = vecTextSize.x / 2;
		const float fCentreIcon = vecIconSize.x / 2;

		// todo@ : make animated.
		const float fScale = static_cast<float>(m_Weapon->m_iClip1()) / pWeaponData->m_max_clip1;
		const int iFillBar = 40 - 2 * fScale;

		// draw the bar.
		if (iterate.m_Bar)
		{
			new_render.RectFilled(vecScreen.x - 21, vecScreen.y + fOffset + 2, 40, 4, { 10, 10, 10, iLowAlpha });
			new_render.RectFilled(vecScreen.x - 21 + 1, vecScreen.y + fOffset + 3, iFillBar, 2, iterate.m_AmmoRectColor);
		}

		new_render.StringShadow({ vecScreen.x - fCentre , vecScreen.y + fOffset - iterate.m_Space },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Text);

		new_render.StringShadow({ vecScreen.x - fCentreIcon , vecScreen.y - fOffsetIcon - 5.f },
			CRender::Fonts::m_weapon_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Icon);

		//if (m_Weapon->m_iItemDefinitionIndex() == AWP)
		//{
		//	Vector2D vec;
		//	if (Math::WorldToScreen(vecScreen, vec))
		//		new_render.CircleFilled(vec, 30, Color(10, 10, 10, 255));
		//}
	}
}

void newvisuals::DrawProjectiles()
{
	if (m_Weapon->GetClientNetworkable()->IsDormant() || !g_Config.b(g_Items.m_Visuals.m_World.m_ProjectileEnabled))
		return;

	Vector2D vecScreen = {};
	if (!Math::WorldToScreen(m_Weapon->GetAbsOrigin(), vecScreen))
		return;

	struct Projectiles {
		Color m_TextColor = {};
		std::string m_Icon = {};
		std::string m_Text = {};
	};

	std::vector< Projectiles > projectiles = {};

	if (m_Weapon->is(HASH("CBaseCSGrenadeProjectile")))
	{
		const model_t* pModel = m_Weapon->GetClientRenderable()->GetModel();
		if (!pModel)
			return;

		std::string strModelName = m_Weapon->GetClientRenderable()->GetModel()->m_Name;

		if (strModelName.find(XOR("flashbang")) != std::string::npos)
		{
			if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(1))
			{
				Projectiles flashbangIcon = {};
				flashbangIcon.m_Icon = XOR("k");
				flashbangIcon.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
				projectiles.push_back(flashbangIcon);
			}

			if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(0))
			{
				Projectiles flashbang = {};
				flashbang.m_Text = XOR("FLASH");
				flashbang.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
				projectiles.push_back(flashbang);
			}
		}
		else if (strModelName.find(XOR("fraggrenade")) != std::string::npos)
		{
			if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(1))
			{
				Projectiles fragGrenadeIcon = {};
				fragGrenadeIcon.m_Icon = XOR("l");
				fragGrenadeIcon.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
				projectiles.push_back(fragGrenadeIcon);
			}

			if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(0))
			{
				Projectiles fragGrenade = {};
				fragGrenade.m_Text = XOR("FRAG");
				fragGrenade.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
				projectiles.push_back(fragGrenade);
			}
		}
	}

	if (m_Weapon->is(HASH("CDecoyProjectile")))
	{
		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(1))
		{
			Projectiles fragGrenadeIcon = {};
			fragGrenadeIcon.m_Icon = XOR("o");
			fragGrenadeIcon.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenadeIcon);
		}

		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(0))
		{
			Projectiles fragGrenade = {};
			fragGrenade.m_Text = XOR("FLASH");
			fragGrenade.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenade);
		}
	}

	if (m_Weapon->is(HASH("CSmokeGrenadeProjectile")))
	{
		Smoke_t* pSmoke = reinterpret_cast<Smoke_t*>(m_Weapon);
		if (!pSmoke || pSmoke->m_DidSmokeEffect())
			return;

		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(1))
		{
			Projectiles fragGrenadeIcon = {};
			fragGrenadeIcon.m_Icon = XOR("m");
			fragGrenadeIcon.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenadeIcon);
		}

		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(0))
		{
			Projectiles fragGrenade = {};
			fragGrenade.m_Text = XOR("SMOKE");
			fragGrenade.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenade);
		}
	}

	if (m_Weapon->is(HASH("CMolotovProjectile")))
	{
		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(1))
		{
			Projectiles fragGrenadeIcon = {};
			fragGrenadeIcon.m_Icon = m_Weapon->is(HASH("CInferno")) ? XOR("p") : XOR("n");
			fragGrenadeIcon.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenadeIcon);
		}

		if (g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType).at(0))
		{
			Projectiles fragGrenade = {};
			fragGrenade.m_Text = XOR("FIRE");
			fragGrenade.m_TextColor = g_Config.c(g_Items.m_Visuals.m_World.m_ProjectileColor);
			projectiles.push_back(fragGrenade);
		}
	}

	for (size_t i = 0u; i < projectiles.size(); ++i)
	{
		auto& iterate = projectiles.at(i);

		const Vector2D& vecTextSize = new_render.TextSize(iterate.m_Text, CRender::Fonts::m_esp_small_font);
		const Vector2D& vecIconSize = new_render.TextSize(iterate.m_Icon, CRender::Fonts::m_weapon_font);

		const float fOffset = i * (vecTextSize.y - 2);
		const float fOffsetIcon = i * (vecIconSize.y - 2);
		const float fCentre = vecTextSize.x / 2;
		const float fCentreIcon = vecIconSize.x / 2;

		new_render.StringShadow({ vecScreen.x - fCentreIcon , vecScreen.y - fOffsetIcon - 5.f },
			CRender::Fonts::m_weapon_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Icon);

		new_render.StringShadow({ vecScreen.x - fCentre , vecScreen.y + fOffset },
			CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha }, iterate.m_TextColor, iterate.m_Text);
	}
}

void newvisuals::DrawOutOfView() 
{
	if (!m_Enemy || !g_Config.b(g_Items.m_Visuals.m_OffScreenEnabled) || !g_Config.f(g_Items.m_Visuals.m_OffScreenSize))
		return;

	if (!g_Client.m_LocalPlayer->IsAlive())
		return;

	const float flYaw = Math::DEG2RAD(g_Client.m_ViewAngles.y);
	const Vector vecPosDiff = g_Client.m_LocalPlayer->m_vecOrigin() - m_Player->m_vecOrigin();

	auto x = std::cos(flYaw) * vecPosDiff.y - std::sin(flYaw) * vecPosDiff.x;
	auto y = std::cos(flYaw) * vecPosDiff.x + std::sin(flYaw) * vecPosDiff.y;

	const float flLength = std::sqrt(x * x + y * y);
	x /= flLength;
	y /= flLength;

	const ImVec2 vecPos = ImVec2(g_Client.m_Width / 2, g_Client.m_Height / 2) + ImVec2(x, y) * g_Config.f(g_Items.m_Visuals.m_OffScreenDistance) * 2;

	const float flSize = g_Config.f(g_Items.m_Visuals.m_OffScreenSize) / 2.2f;
	const float flCentre = g_Config.f(g_Items.m_Visuals.m_OffScreenSize) / 2.f;
	const float flCentreY = g_Config.f(g_Items.m_Visuals.m_OffScreenSize) / 2.f + 5.f;

	const ImVec2 points[]
	{
		ImVec2(vecPos.x, vecPos.y) + ImVec2(0.4f * y, -0.4f * x) * flSize,
		ImVec2(vecPos.x, vecPos.y) + ImVec2(1.0f * x,  1.0f * y) * flSize,
		ImVec2(vecPos.x, vecPos.y) + ImVec2(-0.4f * y,  0.4f * x) * flSize
	};

	// long names are not allowed.
	std::string strPlayerName{ std::string(m_PlayerInfo.m_Name).substr(0, 24) };

	const Vector2D& vecTextSize = new_render.TextSize(strPlayerName, CRender::Fonts::m_esp_font);
	const float centre_font = vecTextSize.x / 2;

	if (g_Config.vb(g_Items.m_Visuals.m_OffScreenFlags).at(OOF_NAME))
		new_render.StringShadow({ vecPos.x - centre_font, vecPos.y - flCentreY - 11 },
			CRender::Fonts::m_esp_font, { 10, 10, 10, m_LowAlpha }, { 255, 255, 255, m_Alpha }, strPlayerName);

	const int hp = m_MaxEntity[m_Index] > m_Player->m_iHealth() ? m_MaxEntity[m_Index] -= Easings::OutQuint((1.f / 0.5f) * g_Csgo.m_Globals->m_FrameTime) : m_MaxEntity[m_Index] = std::min(100, m_Player->m_iHealth());
	const int r = std::min((510 * (100 - hp)) / 100, 255);
	const int g = std::min((510 * hp) / 100, 255);
	const int fill = hp * static_cast<int>(g_Config.f(g_Items.m_Visuals.m_OffScreenSize) + 10.f) / 100.f;
	const Color colHp = { r, g, 0, m_Alpha };

	if (g_Config.vb(g_Items.m_Visuals.m_OffScreenFlags).at(OOF_BOX)) { // todo@ : change box type upon box selection.
		// new_render.Rect(vecPos.x - flCentre - 1, vecPos.y - flCentreY, g_Config.f(g_Items.m_Visuals.m_OffScreenSize), g_Config.f(g_Items.m_Visuals.m_OffScreenSize) + 10, { 10, 10, 10, m_LowAlpha });
		new_render.Rect(vecPos.x - flCentre, vecPos.y - flCentreY, g_Config.f(g_Items.m_Visuals.m_OffScreenSize), g_Config.f(g_Items.m_Visuals.m_OffScreenSize) + 10, { 255, 255, 255, m_Alpha });
	}

	if (g_Config.vb(g_Items.m_Visuals.m_OffScreenFlags).at(OOF_HP)) {
		new_render.RectFilled(vecPos.x - flCentre - 7, vecPos.y - flCentreY, 5, g_Config.f(g_Items.m_Visuals.m_OffScreenSize) + 10, { 10, 10, 10, m_LowAlpha });
		new_render.RectFilled(vecPos.x - flCentre - 6, vecPos.y + (flCentreY - 1), 3, -(fill - 2), colHp);
	}

	new_render.PollyLine(points, 3, { 255, 255, 255, m_Alpha });
	new_render.PollyFilled(points, 3, { 255, 255, 255, m_LowAlpha / 2 });
}

void newvisuals::DrawDamage()
{
	if (!g_Config.b(g_Items.m_Visuals.m_DamageEnabled)) {
		g_ShotSystem.m_DmgIndicator.clear();
		return;
	}

	for (size_t i = 0u; i < g_ShotSystem.m_DmgIndicator.size(); i++)
	{
		auto& iterate = g_ShotSystem.m_DmgIndicator.at(i);
		if (iterate.m_Position.IsZero())
			continue;

		if (iterate.m_EraseTime < g_InputPrediction.m_Curtime)
		{
			if (!(iterate.m_Alpha = std::clamp(--iterate.m_Alpha, 0, 255)))
				g_ShotSystem.m_DmgIndicator.erase(g_ShotSystem.m_DmgIndicator.begin() + i);
		}
		else iterate.m_Alpha = std::clamp(++iterate.m_Alpha, 0, 255);
		
		Vector2D screen;
		if (Math::WorldToScreen(iterate.m_Position, screen))
		{
			const Vector2D& vecTextSize = new_render.TextSize(iterate.m_Output, CRender::Fonts::m_esp_small_font);

			const float centre = vecTextSize.x / 2;
			const int offset = i * (vecTextSize.y - 3);

			iterate.m_AnimationX += (((sin(g_InputPrediction.m_Curtime * (iterate.m_RandAnimVel / 10.f)) + 1) / 2) * iterate.m_RandAnimAmount) - (iterate.m_RandAnimAmount / 2.f);
			iterate.m_AnimationY += (1.f / 0.4f) * ImGui::GetIO().DeltaTime;
			iterate.m_Color.a() = iterate.m_Alpha;

			new_render.CircleFilled({ screen.x + iterate.m_AnimationX, screen.y - iterate.m_AnimationY + offset }, 10, iterate.m_Color);
			new_render.StringShadow({ screen.x - centre + iterate.m_AnimationX, screen.y - iterate.m_AnimationY + offset - 5 },
				CRender::Fonts::m_esp_small_font, { 10, 10, 10, iterate.m_Alpha / 4 }, { 255, 255, 255, iterate.m_Alpha }, iterate.m_Output);
		}
	}
}

void newvisuals::DrawHitmarker() 
{
	if (g_ShotSystem.m_HitmarkerAlpha == 0 || !g_Config.b(g_Items.m_Visuals.m_HitmarkerEnabled))
		return;

	const int x = g_Client.m_Width / 2, y = g_Client.m_Height / 2;

	if (!g_ShotSystem.m_CompleteHitmarkerSize) {
		g_ShotSystem.m_HitmarkerSize += (255 / 8.f) * g_Csgo.m_Globals->m_FrameTime;

		if (g_ShotSystem.m_HitmarkerSize > 25.f)
			g_ShotSystem.m_CompleteHitmarkerSize = true;
	}
	else
	{
		g_ShotSystem.m_HitmarkerSize -= (255 / 8.f) * g_Csgo.m_Globals->m_FrameTime;
		g_ShotSystem.m_HitmarkerSize = std::clamp(g_ShotSystem.m_HitmarkerSize, 15.f, 25.f);

		if (g_ShotSystem.m_HitmarkerSize == 15 && g_Csgo.m_Globals->m_Curtime > g_ShotSystem.m_HitmarkerEraseTime)
			g_ShotSystem.m_HitmarkerAlpha -= (255 / 11.f) * g_Csgo.m_Globals->m_FrameTime;
	}

	float flSize = g_ShotSystem.m_HitmarkerSize;

	// todo@ : maybe triangles? looks cooler imo.
	new_render.Line(x - flSize, y - flSize, x - (flSize / 2), y - (flSize / 2), { 255, 255, 255, g_ShotSystem.m_HitmarkerAlpha }, 1.f);
	new_render.Line(x - flSize, y + flSize, x - (flSize / 2), y + (flSize / 2), { 255, 255, 255, g_ShotSystem.m_HitmarkerAlpha }, 1.f);
	new_render.Line(x + flSize, y + flSize, x + (flSize / 2), y + (flSize / 2), { 255, 255, 255, g_ShotSystem.m_HitmarkerAlpha }, 1.f);
	new_render.Line(x + flSize, y - flSize, x + (flSize / 2), y - (flSize / 2), { 255, 255, 255, g_ShotSystem.m_HitmarkerAlpha }, 1.f);

	// todo@ : add world hitmarker.
}

void newvisuals::PreverseKillFeed()
{
	if (!g_Config.b(g_Items.m_Visuals.m_PreverseKillfeedEnabled) || !g_Csgo.m_Engine->IsInGame() || !g_Client.m_Processing)
		return;

	// get the addr of the killfeed.
	KillFeed_t* pFeed = reinterpret_cast<KillFeed_t*>(g_Csgo.m_Hud->FindElement(HASH("SFHudDeathNoticeAndBotStatus")));
	if (!pFeed)
		return;

	for (auto i = 0; i < pFeed->notices.Count(); ++i) {
		NoticeText_t* pNotice = &pFeed->notices[i];

		// this is a local player kill, delay it.
		if (pNotice->fade == 1.5f)
			pNotice->fade = FLT_MAX;
	}
}

void newvisuals::Glow() {

	if (!g_Client.m_LocalPlayer)
		return;

	for (int i = 0; i < g_Csgo.m_GlowObjectManager->m_object_definitions.Count(); ++i) 
	{
		GlowObjectDefinition_t* obj = &g_Csgo.m_GlowObjectManager->m_object_definitions[i];
		if (!obj->m_Player || !obj->m_Player->IsPlayer())
			continue;

		Player* m_Player = obj->m_Player->as< Player* >();

		if (!ConfigureTeams(g_Config.b(g_Items.m_Visuals.m_Enemies.m_GlowEnabled), g_Config.b(g_Items.m_Visuals.m_Allies.m_GlowEnabled), g_Config.b(g_Items.m_Visuals.m_Local.m_GlowEnabled), m_Player, true))
			continue;

		Color colGlow = ConfigureTeams(g_Config.c(g_Items.m_Visuals.m_Enemies.m_GlowColor), g_Config.c(g_Items.m_Visuals.m_Allies.m_GlowColor), g_Config.c(g_Items.m_Visuals.m_Local.m_GlowColor), m_Player, true);
		obj->m_render_occluded = true;
		obj->m_render_unoccluded = false;
		obj->m_render_full_bloom = false;
		obj->m_color = {colGlow.r() / 255.f, colGlow.g() / 255.f, colGlow.b() / 255.f};
		obj->m_alpha = colGlow.a() / 255.f;
	}
}

void newvisuals::PenCrosshair() 
{
	if (!g_Config.b(g_Items.m_Visuals.m_PenCrosshairEnabled) || !g_Client.m_Processing || !g_Client.m_Weapon)
		return;

	std::string strPenText = std::to_string(static_cast<int>(g_Client.m_PenetrationData.m_Damage));

	switch (g_Config.i(g_Items.m_Visuals.m_PenCrosshairType))
	{
	case 0: {
		Color colPenColor = {};

		int x = g_Client.m_Width / 2;
		int y = (g_Client.m_Height / 2) + 1;

		if (g_Client.m_PenetrationData.m_Player && g_Client.m_PenetrationData.m_Player->enemy(g_Client.m_LocalPlayer))
			colPenColor = colors::light_blue;
		else if (g_Client.m_PenetrationData.m_Penetration)
			colPenColor = colors::transparent_green;
		else
			colPenColor = colors::transparent_red;

		new_render.CircleFilled({ x, y }, 3, { 10, 10, 10, 159 });

		new_render.CircleFilled({ x, y }, 2, colPenColor);

		const Vector2D& vecTextSize = new_render.TextSize(strPenText, CRender::Fonts::m_esp_font);

		int iCentre = vecTextSize.x / 2;

		if (g_Config.b(g_Items.m_Visuals.m_PenCrosshairDmgEnabled) && g_Client.m_PenetrationData.m_Penetration)
			new_render.StringShadow({ x - iCentre, y + 6 },
				CRender::Fonts::m_esp_small_font, { 10, 10, 10, 159 }, { 255, 255, 255, 220 }, strPenText);
	} break;
	case 1: {
		CGameTrace trace = {};
		CTraceFilterSimple_game filter = {};

		QAngle angViewAngles = QAngle(0.f, 0.f, 0.f);
		g_Csgo.m_Engine->GetViewAngles(angViewAngles);

		Vector vecDirection = Vector(0.f, 0.f, 0.f);
		Math::AngleVectors(angViewAngles, &vecDirection);

		const float fMaxRange = g_Client.m_WeaponInfo->m_WeaponRange * 2;
		Vector vecStart = g_Client.m_EyePosition;
		Vector vecEnd = vecStart + (vecDirection * fMaxRange);

		filter.SetPassEntity(g_Client.m_LocalPlayer); // skip local.
		g_Csgo.m_EngineTrace->TraceRay(Ray(vecStart, vecEnd), MASK_SHOT | CONTENTS_GRATE, reinterpret_cast<ITraceFilter*>(&filter), &trace);

		Color colPen = g_Config.c(g_Items.m_Visuals.m_PenColor);
		Color colGreen = g_Client.m_PenetrationData.m_Penetration ? Color(0, 255, 0, 130) : colPen;
		Vector vecDelta = Vector(colPen.r() - colGreen.r(), colPen.g() - colGreen.g(), colPen.b() - colGreen.b());
		Color blend = Color(colPen.r() - vecDelta.x * (std::clamp(g_Client.m_PenetrationData.m_Damage, 0.f, 50.f) / 50), colPen.g() - vecDelta.y * (std::clamp(g_Client.m_PenetrationData.m_Damage, 0.f, 50.f) / 50), colPen.b() - vecDelta.z * (std::clamp(g_Client.m_PenetrationData.m_Damage, 0.f, 50.f) / 50));

		const float fAnglez = Math::DotProduct(Vector(0, 0, 1), trace.m_Plane.m_Normal);
		const float fInvanglez = Math::DotProduct(Vector(0, 0, -1), trace.m_Plane.m_Normal);
		const float fAngley = Math::DotProduct(Vector(0, 1, 0), trace.m_Plane.m_Normal);
		const float fInvangley = Math::DotProduct(Vector(0, -1, 0), trace.m_Plane.m_Normal);
		const float fAnglex = Math::DotProduct(Vector(1, 0, 0), trace.m_Plane.m_Normal);
		const float fInvanglex = Math::DotProduct(Vector(-1, 0, 0), trace.m_Plane.m_Normal);

		if ((fAnglez > 0.5 || fInvanglez > 0.5))
			new_render.RectFilled3D(trace.m_EndPosition, Vector2D(5, 5), blend, 0);
		else if ((fAngley > 0.5 || fInvanglez > 0.5))
			new_render.RectFilled3D(trace.m_EndPosition, Vector2D(5, 5), blend, 1);
		else if ((fAnglex > 0.5 || fInvanglex > 0.5))
			new_render.RectFilled3D(trace.m_EndPosition, Vector2D(5, 5), blend, 2);

		const Vector2D& vecTextSize = new_render.TextSize(strPenText, CRender::Fonts::m_big_bold_font);

		float flCentre = vecTextSize.x / 2;

		Vector2D vecText = {};
		if (g_Config.b(g_Items.m_Visuals.m_PenCrosshairDmgEnabled) && Math::WorldToScreen(trace.m_EndPosition, vecText) && g_Client.m_PenetrationData.m_Penetration) {
			new_render.StringShadow({ vecText.x - flCentre, vecText.y + 30 }, // todo@ : +30 is a workaround. need to figure out something else.
				CRender::Fonts::m_big_bold_font, { 10, 10, 10, 159 }, { 255, 255, 255, 220 }, strPenText);
		}

	} break;
	}
}

void newvisuals::Resolver()
{
	if (!m_Player->enemy(g_Client.m_LocalPlayer))
		return;

	AimPlayer* pData = &g_Aimbot.m_Players[m_Player->GetClientNetworkable()->GetIndex() - 1];
	if (!pData || pData->m_Records.empty())
		return;

	LagRecord* pRecord = pData->m_Records.front().get();
	if (!pRecord || pRecord->m_Dormant)
		return;

	const auto& pos = m_Player->GetClientRenderable()->GetRenderOrigin();
	Vector2D tmp;

	if (!Math::WorldToScreen(pos, tmp) || pData->m_AdaptiveAngles.empty())
		return;

	Vector2D vecDrawPosition;
	Color cColor;

	const float flLowerbodyYaw = pRecord->m_LowerbodyYaw;
	const float flLastMovingLowerbodyYaw = pData->m_WalkRecord.m_LowerbodyYaw;
	const float flFakeYaw = pRecord->m_Client.m_EyeAngles.y;
	const float flRealYaw = pRecord->m_EyeAngles.y;

	const Vector vecRealYawPos(50.f * cos(Math::DEG2RAD(flRealYaw)) + pos.x, 50.f * sin(Math::DEG2RAD(flRealYaw)) + pos.y, pos.z);
	const Vector vecFreestandingPos(50.f * cos(Math::DEG2RAD(pData->m_AdaptiveAngles.front().m_Yaw)) + pos.x, 50.f * sin(Math::DEG2RAD(pData->m_AdaptiveAngles.front().m_Yaw)) + pos.y, pos.z);
	const Vector vecLowerbodyYawPos(50.f * cos(Math::DEG2RAD(flLowerbodyYaw)) + pos.x, 50.f * sin(Math::DEG2RAD(flLowerbodyYaw)) + pos.y, pos.z);
	const Vector vecLastMovingLowerbodyYawPos(50.f * cos(Math::DEG2RAD(flLastMovingLowerbodyYaw)) + pos.x, 50.f * sin(Math::DEG2RAD(flLastMovingLowerbodyYaw)) + pos.y, pos.z);
	const Vector vecFakeYawPos(50.f * cos(Math::DEG2RAD(flFakeYaw)) + pos.x, 50.f * sin(Math::DEG2RAD(flFakeYaw)) + pos.y, pos.z);

	cColor = { 0, 255, 0, m_LowAlpha };

	if (Math::WorldToScreen(vecLowerbodyYawPos, vecDrawPosition))
	{
		new_render.Line(tmp.x, tmp.y, vecDrawPosition.x, vecDrawPosition.y, cColor);
		new_render.StringShadow({ vecDrawPosition.x, vecDrawPosition.y },CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha / 4 }, cColor, "LBY");
	}

	cColor = { 255, 255, 255, m_LowAlpha };

	if (Math::WorldToScreen(vecFreestandingPos, vecDrawPosition))
	{
		new_render.Line(tmp.x, tmp.y, vecDrawPosition.x, vecDrawPosition.y, cColor);
		new_render.StringShadow({ vecDrawPosition.x, vecDrawPosition.y }, CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha / 4 }, cColor, "FREESTAND");
	}

	cColor = { 255, 255, 0, m_LowAlpha };

	if (Math::WorldToScreen(vecRealYawPos, vecDrawPosition))
	{
		new_render.Line(tmp.x, tmp.y, vecDrawPosition.x, vecDrawPosition.y, cColor);
		new_render.StringShadow({ vecDrawPosition.x, vecDrawPosition.y }, CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha / 4 }, cColor, "REAL");
	}

	cColor = { 255, 0, 0, m_LowAlpha };

	if (Math::WorldToScreen(vecFakeYawPos, vecDrawPosition))
	{
		new_render.Line(tmp.x, tmp.y, vecDrawPosition.x, vecDrawPosition.y, cColor);
		new_render.StringShadow({ vecDrawPosition.x, vecDrawPosition.y }, CRender::Fonts::m_esp_small_font, { 10, 10, 10, m_LowAlpha / 4 }, cColor, "FAKE");
	}
}

void newvisuals::DebugScreen() 
{
	struct DebugVisual {
		Color m_Color; std::string m_Text;
	};

	std::vector< DebugVisual > debugOnScreen = {};

	DebugVisual velocityDebug = {};
	velocityDebug.m_Color = { 255, 255, 255, 255 };
	velocityDebug.m_Text = tfm::format(XOR("velocity is: %f"), g_Client.GetLocalPlayer()->m_vecVelocity().Length());
	debugOnScreen.push_back(velocityDebug);

	DebugVisual globalsFramtime = {};
	globalsFramtime.m_Color = { 255, 255, 255, 255 };
	globalsFramtime.m_Text = tfm::format(XOR("frametime is: %f"), g_Csgo.m_Globals->m_FrameTime);
	debugOnScreen.push_back(globalsFramtime);

	DebugVisual globalsFrameCount = {};
	globalsFrameCount.m_Color = { 255, 255, 255, 255 };
	globalsFrameCount.m_Text = tfm::format(XOR("framecount is: %i"), g_Csgo.m_Globals->m_FrameCount);
	debugOnScreen.push_back(globalsFrameCount);

	DebugVisual globalsTickCount = {};
	globalsTickCount.m_Color = { 255, 255, 255, 255 };
	globalsTickCount.m_Text = tfm::format(XOR("tick_count is: %i"), g_Csgo.m_Globals->m_TickCount);
	debugOnScreen.push_back(globalsTickCount);

	DebugVisual globalsRealTime = {};
	globalsRealTime.m_Color = { 255, 255, 255, 255 };
	globalsRealTime.m_Text = tfm::format(XOR("realtime is: %f"), g_Csgo.m_Globals->m_RealTime);
	debugOnScreen.push_back(globalsRealTime);

	DebugVisual globalsInterval = {};
	globalsInterval.m_Color = { 255, 255, 255, 255 };
	globalsInterval.m_Text = tfm::format(XOR("interval is: %f"), g_Csgo.m_Globals->m_Interval);
	debugOnScreen.push_back(globalsInterval);

	DebugVisual globalsInterpolationAmt = {};
	globalsInterpolationAmt.m_Color = { 255, 255, 255, 255 };
	globalsInterpolationAmt.m_Text = tfm::format(XOR("interpolation amount is: %f"), g_Csgo.m_Globals->m_InterpolationAmount);
	debugOnScreen.push_back(globalsInterpolationAmt);

	const C_AnimationLayer* pLayerFlashedAct = &m_Player->m_AnimOverlay()[ANIMATION_LAYER_FLASHED];
	const bool bFlashed = (pLayerFlashedAct->GetWeight() != 0.f) && (m_Player->GetSequenceActivity(pLayerFlashedAct->GetSequence()) == ACT_CSGO_FLASHBANG_REACTION);
	float flFlashCycle = bFlashed ? pLayerFlashedAct->GetCycle() : 1337.f;

	DebugVisual debugFlashWeight = {};
	debugFlashWeight.m_Color = { 255, 255, 255, 255 };
	debugFlashWeight.m_Text = tfm::format(XOR("pLayerFlashedAct->GetWeight() amount is : % f"), pLayerFlashedAct->GetWeight());
	debugOnScreen.push_back(debugFlashWeight);

	DebugVisual debugFlashCycle = {};
	debugFlashCycle.m_Color = { 255, 255, 255, 255 };
	debugFlashCycle.m_Text = tfm::format(XOR("flFlashCycle is: %f"), flFlashCycle);
	debugOnScreen.push_back(debugFlashCycle);

	if (debugOnScreen.empty())
		return;

	for (size_t i = 0u; i < debugOnScreen.size(); ++i)
	{
		auto& iterate = debugOnScreen[i];

		const Vector2D& vecTextSize = new_render.TextSize(iterate.m_Text, CRender::Fonts::m_big_bold_font);

		int iOffset = i * (vecTextSize.y + 3);
		int iCentre = vecTextSize.x / 2;

		new_render.StringShadow({ (g_Client.m_Width / 2) - iCentre , (g_Client.m_Height / 2) + iOffset },
			CRender::Fonts::m_big_bold_font, { 10, 10, 10, m_LowAlpha }, iterate.m_Color, iterate.m_Text);
	}
}

void newvisuals::Others()
{
	if (g_Config.b(g_Items.m_Misc.m_UnlockHiddenCvars))
		g_Client.UnlockHiddenConvars();

	if (g_Config.b(g_Items.m_Visuals.m_Effects.m_PartyMode))
		g_Csgo.sv_party_mode->SetValue(1);

	g_Csgo.weapon_debug_spread_show->SetValue(g_Config.b(g_Items.m_Visuals.m_ForceCrosshair) && !g_Client.m_LocalPlayer->m_bIsScoped() ? 3 : 0);

	if (g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SCOPE] && g_Client.m_Processing && g_Client.m_LocalPlayer->GetActiveWeapon()
		&& g_Client.m_LocalPlayer->GetActiveWeapon()->GetWpnData()->m_WeaponType == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE && g_Client.m_LocalPlayer->m_bIsScoped()) 
	{
		int x = g_Client.m_Width / 2, y = g_Client.m_Height / 2, size = g_Csgo.cl_crosshair_sniper_width->GetInt();
		if (size > 1) {
			x -= (size / 2);
			y -= (size / 2);
		}

		new_render.RectFilled(0, y, g_Client.m_Width, size, colors::black);
		new_render.RectFilled(x, 0, size, g_Client.m_Height, colors::black);
	}

	static float flBackupAspectRatio = g_Csgo.r_aspectratio->GetFloat();
	if (g_Config.b(g_Items.m_Visuals.m_AspectRatio))
	{
		g_Csgo.r_aspectratio->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		g_Csgo.r_aspectratio->m_nFlags &= ~FCVAR_HIDDEN;
		g_Csgo.r_aspectratio->SetValue(g_Config.f(g_Items.m_Visuals.m_AspectRatioAmount));
	}
	else g_Csgo.r_aspectratio->SetValue(flBackupAspectRatio);

	if (g_Config.b(g_Items.m_Visuals.m_AutoPeekIndicator) && g_Client.m_Processing && g_Config.b(g_Items.m_Misc.m_AutomaticPeek) 
		&& g_KeyInput.Key(g_Items.m_Misc.m_AutomaticPeekKeyBinds, g_Items.m_Misc.m_AutomaticPeekKeyBindsMode)) 
	{
		const Color col = (g_Movement.m_AutomaticPeek.m_ShouldRetrack && !g_Movement.m_AutomaticPeek.m_AutoPeekingDone) ? Color(0, 255, 60, 255) : g_Config.c(g_Items.m_Visuals.m_AutoPeekIndicatorColor);
		new_render.Circle3D(g_Movement.m_AutomaticPeek.m_Origin, 20, col, true);
	}
}