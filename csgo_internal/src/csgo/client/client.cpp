#include "../../precompiled.h"
#include "../../security/network/tcp.h"

void Client::Init()
{
#ifdef _PRODUCTION
	if (!tcp::_client) {
		tcp::termination.m_break = true;
		return;
	}
#endif

	if (!g_Csgo.Init()) 
	{
		tcp::termination.m_break = true;
		return;
	}

#ifdef LEGACY
	g_Csgo.m_Engine->ExecuteClientCmd(XOR("clear"));

	g_Csgo.m_CVar->FindVar(HASH("developer"))->SetValue(0);
	g_Csgo.m_CVar->FindVar(HASH("con_filter_enable"))->SetValue(1);
	g_Csgo.m_CVar->FindVar(HASH("con_filter_text"))->SetValue("[IBLIS.club]");
	g_Csgo.m_CVar->FindVar(HASH("con_filter_text_out"))->SetValue(" ");
	g_Csgo.m_CVar->FindVar(HASH("contimes"))->SetValue(15);

	g_notify.add(XOR("Welcome to IBLIS.club!\n"));
#endif // LEGACY
}

void Client::OnMapload()
{
	g_Netvars.SetupClassData();

	m_LocalPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(g_Csgo.m_Engine->GetLocalPlayer());
	if (!m_LocalPlayer)
		return;

	m_TickRate = static_cast<size_t>(1.f / g_Csgo.m_Globals->m_Interval);

	if (!g_FakePing.m_Sequences.empty())
		g_FakePing.m_Sequences.clear();

	g_Csgo.m_NetChannelInfo = g_Csgo.m_ClientState->m_NetChannel;
	if (g_Csgo.m_NetChannelInfo) {
		g_Hooks.m_NetChannel.Reset();
		g_Hooks.m_NetChannel.Init(g_Csgo.m_NetChannelInfo);
		g_Hooks.m_NetChannel.Add(INetChannel::SETCHOKED, util::force_cast(&Hooks::SetChoked));
		g_Hooks.m_NetChannel.Add(INetChannel::PROCESSPACKET, util::force_cast(&Hooks::ProcessPacket));
		g_Hooks.m_NetChannel.Add(INetChannel::SENDDATAGRAM, util::force_cast(&Hooks::SendDatagram));
		g_Hooks.m_NetChannel.Add(INetChannel::SENDNETMSG, util::force_cast(&Hooks::SendNetMsg));
	}

	return g_skins.load();
}

void Client::StartMove()
{
	VM_TIGER_START;

	m_TickCount = m_Command->m_TickCount;
	m_ViewAngles = m_Command->m_ViewAngles;
	m_Buttons = m_Command->m_Buttons;

	static float m_Framerate = 0;
	m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * g_Csgo.m_Globals->m_AbsFrametime;
	m_FramePerSecond = static_cast<int>(1.f / m_Framerate);
	m_MaxLagAmount		= g_Csgo.m_GameRules->m_bIsValveDS() ? 6 : g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2;
	m_ChokedCommands	= g_Csgo.m_ClientState->m_ChokedCommands;
	m_Lerp				= game::GetClientInterpAmount();
	m_LatencyOutgoing = g_Csgo.m_NetChannelInfo->GetLatency(INetChannel::FLOW_OUTGOING);
	m_LatencyIncoming = g_Csgo.m_NetChannelInfo->GetLatency(INetChannel::FLOW_INCOMING);
	m_LatencyTicks		= game::TIME_TO_TICKS(m_LatencyOutgoing);
	m_ServerTick		= g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick;
	m_ArrivalTick		= m_ServerTick + m_LatencyTicks;

	Math::clamp(m_LatencyOutgoing, 0.f, 1.f);

	m_Processing = GetLocalPlayer() && GetLocalPlayer()->IsAlive() && !tcp::termination.m_break;
	if (!m_Processing) 
		return;

	m_Flags = m_LocalPlayer->m_fFlags();
	m_Shot = false;

	g_InputPrediction.Update();

	VM_TIGER_END;
}

bool Client::SetupWeaponData()
{
	Penetration::PenetrationOutput_t pOutput{ };

	if (m_Weapon == nullptr)
		return false;

	if (m_WeaponInfo == nullptr)
		return false;

	if (m_WeaponType == CSWeaponType::WEAPONTYPE_UNKNOWN)
		return false;

	if (m_WeaponItemDefinition == Weapons_t::UNKOWN)
		return false;

	if (m_WeaponType != WEAPONTYPE_GRENADE)
		m_Weapon->UpdateAccuracyPenalty();

	g_Aimbot.CalculateMaxAccuracy();

	if (m_WeaponType != WEAPONTYPE_KNIFE && m_WeaponType != WEAPONTYPE_C4 && m_WeaponType != WEAPONTYPE_GRENADE) {
		Penetration::PenetrationInput_t pInput;
		pInput.m_From = m_LocalPlayer;
		pInput.m_Player = nullptr;
		pInput.m_EyePosition = m_EyePosition;
		pInput.m_Position = pInput.m_EyePosition + (m_ForwardDirection * m_WeaponInfo->m_WeaponRange);
		pInput.m_Damage = pInput.m_PenetrationDamage = 1.f;
		pInput.m_Penetrate = true;

		Penetration::Run(&pInput, &pOutput);
	}

	m_PenetrationData = pOutput;
	m_PlayerFire = g_Csgo.m_Globals->m_Curtime >= m_LocalPlayer->m_flNextAttack() && !g_Csgo.m_GameRules->m_bFreezePeriod() && !(m_Flags & FL_FROZEN);
	m_CanScope = (m_WeaponItemDefinition == AUG || m_WeaponItemDefinition == SG553 || m_WeaponType == WEAPONTYPE_SNIPER_RIFLE);

	return true;
}

void Client::SetupViewAngles()
{
	m_Command->m_ViewAngles = m_ViewAngles;

	if (m_Flags & FL_ONGROUND)  {
		if (!m_LocalPlayer->m_PlayerAnimStateCSGO()->flVelocityLengthXY)
			m_MovementMode = MOVEMENT::STAND;
		else m_MovementMode = MOVEMENT::WALK;
	} else m_MovementMode = MOVEMENT::AIR;

	m_PressingButtons = m_Command->m_Buttons & IN_LEFT || m_Command->m_Buttons & IN_FORWARD || m_Command->m_Buttons & IN_BACK || m_Command->m_Buttons & IN_RIGHT || m_Command->m_Buttons & IN_MOVELEFT || m_Command->m_Buttons & IN_MOVERIGHT || m_Command->m_Buttons & IN_JUMP;
	
	Math::AngleVectors(m_ViewAngles, &m_ForwardDirection);
}

void Client::DoMove() 
{
	VM_TIGER_START;

	if (tcp::termination.m_break)
		return;

	SetupShootPosition(g_InputPrediction.m_MoveData.m_vecAngles);
	MouseCorrection();
	SetupViewAngles();

	if (!SetupWeaponData())
		return;

	g_Grenades.think();
	g_FakeLag.Init();
	g_Aimbot.Init();
	g_AntiAim.HideShots();
	g_AntiAim.AntiAim();
	g_Movement.AutoPeek();

	VM_TIGER_END;
}

void Client::EndMove() 
{
	VM_TIGER_START;

	m_bOldSendPacket = *m_SendPacket;
	m_OldShot = m_Shot;

	if (g_Config.b(g_Items.m_AntiAim.m_General.m_AntiUntrusted))
		m_Command->m_ViewAngles.SanitizeAngle();

	g_Movement.FixMove(m_StrafeAngles);
	if (!GetModifablePacket())
		return;
	
	m_Radar = m_Command->m_ViewAngles;
	m_LagCompensationBreaked = (m_LocalPlayer->m_vecOrigin() - (m_NetPos.empty() ? m_LocalPlayer->m_vecOrigin() : m_NetPos.front().m_Position)).LengthSqr() > 4096.f;
	m_NetPos.emplace_front(g_Csgo.m_Globals->m_Curtime, m_LocalPlayer->m_vecOrigin());

	VM_TIGER_END;

	//if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagActivationType)[0] && g_Client.GetLocalPlayer()->m_vecVelocity().IsValid())
	//	m_LocalPlayer->SetupBones(m_LagMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, g_Csgo.m_Globals->m_Curtime);
}
	
void Client::SetupShootPosition(QAngle angAngle)
{
	int m_nBodyPitchPoseParam = m_LocalPlayer->LookupPoseParameter(XOR("body_pitch"));	
	float flOldBodyPitch = m_LocalPlayer->m_flPoseParameter()[m_nBodyPitchPoseParam];

	float flPitch = angAngle.IsZero() ? m_LocalPlayer->EyeAngles()[PITCH] : angAngle[PITCH];
	if (flPitch > 180.0f)
		flPitch -= 360.0f;

	if (!m_PoseParameters.empty())
		m_LocalPlayer->SetPoseParameters(m_PoseParameters.data());
	
	if (!m_AnimationLayers.empty())
		m_LocalPlayer->SetAnimLayers(m_AnimationLayers.data());

	m_LocalPlayer->SetAbsAngles(QAngle(0.f, m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw, 0.f));
	m_LocalPlayer->SetBodyPitch((flPitch + 90.0f) / 180.0f);

	matrix3x4a_t aMatrix[MAXSTUDIOBONES];

	m_LocalPlayer->SetupBones(aMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, g_Csgo.m_Globals->m_Curtime);
	m_LocalPlayer->SetBodyPitch(flOldBodyPitch);

	std::memcpy(m_LocalPlayer->m_BoneCache().m_pCachedBones, aMatrix, sizeof(matrix3x4_t) * m_LocalPlayer->m_BoneCache().m_CachedBoneCount);
	
	m_LocalPlayer->m_iMostRecentModelBoneCounter() = g_Csgo.ModelBoneCounter;
	return m_LocalPlayer->Weapon_ShootPosition(m_EyePosition);
}

void Client::print(const std::string text, ...) {
	va_list     list;
	int         size;
	std::string buf;

	if (text.empty() || !g_Csgo.m_CVar)
		return;

	va_start(list, text);

	size = std::vsnprintf(0, 0, text.c_str(), list);
	buf.resize(size);

	std::vsnprintf(buf.data(), size + 1, text.c_str(), list);

	va_end(list);

	Color colPrint = g_Config.c(g_Items.m_AccentColor);
	g_Csgo.m_CVar->ConsoleColorPrintf(colPrint, XOR("[IBLIS.club] "));
	g_Csgo.m_CVar->ConsoleColorPrintf(colors::white, buf.c_str());
}

bool Client::CanAttack() {
	if (!m_PlayerFire || !m_Weapon || 
		m_WeaponType == WEAPONTYPE_GRENADE || 
		m_WeaponType != WEAPONTYPE_KNIFE && m_Weapon->m_iClip1() < 1)
		return false;
	return true;
}

bool Client::CanFireWeapon(int iShiftAmount, bool revolver, bool bDontCheckBetweenShots) {
	if (!CanAttack())
		return false;

	if (bDontCheckBetweenShots)
		return true;

	float_t flServerTime = game::TICKS_TO_TIME(m_LocalPlayer->m_nTickBase() - iShiftAmount);

	if (m_WeaponItemDefinition == REVOLVER && revolver)
		return m_Weapon->m_flPostponeFireReadyTime() <= flServerTime;

	else if (m_Weapon->m_iBurstShotsRemaining() > 0 && (m_WeaponItemDefinition == GLOCK || m_WeaponItemDefinition == FAMAS))
		return m_Weapon->m_fNextBurstShot() <= flServerTime;
	
	return m_Weapon->m_flNextPrimaryAttack() <= flServerTime;
}

void Client::UpdateRevolverCock() {
	if (!m_Weapon || m_Weapon->m_iClip1() < 1 || m_WeaponItemDefinition != REVOLVER)
		return;

	if (m_Command->m_Buttons & IN_ATTACK || m_Command->m_Buttons & IN_ATTACK2)
		return;

	if (!CanFireWeapon(0, false)) {
		m_RevolverCock = g_Csgo.m_Globals->m_Curtime + g_Csgo.m_Globals->m_Interval * 13.f;
		m_Command->m_Buttons &= ~IN_ATTACK;
		return;
	}

	if (m_RevolverCock <= g_Csgo.m_Globals->m_Curtime) {
		if (m_Weapon->m_flNextSecondaryAttack() <= g_Csgo.m_Globals->m_Curtime)
			m_RevolverCock = g_Csgo.m_Globals->m_Curtime + g_Csgo.m_Globals->m_Interval * 13.f;
		else m_Command->m_Buttons |= IN_ATTACK2;
	}
	else m_Command->m_Buttons |= IN_ATTACK;
}

bool Client::IsShooting(bool bCheckRevolver)
{
	if (m_Weapon == nullptr || m_Command == nullptr)
		return false;

	if (m_WeaponType == WEAPONTYPE_GRENADE && !m_Weapon->m_bPinPulled() && m_Weapon->m_fThrowTime() > 0.f && g_Csgo.m_Globals->m_Curtime >= m_Weapon->m_fThrowTime())
		return true;

	if (!CanFireWeapon(0, bCheckRevolver))
		return false;

	if (m_Command->m_Buttons & IN_ATTACK)
		return true;

	if (m_Command->m_Buttons & IN_ATTACK2 && (m_WeaponType == WEAPONTYPE_KNIFE && m_WeaponItemDefinition != ZEUS || m_WeaponItemDefinition == REVOLVER))
		return true;

	return false;
}

void Client::UnlockHiddenConvars()
{
	auto p = **reinterpret_cast<ConVar***>(g_Csgo.m_CVar + 0x34);
	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext) {
		c->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		c->m_nFlags &= ~FCVAR_HIDDEN;
	}
}

void Client::Thirdperson() 
{
	QAngle                         offset;
	Vector                         origin, forward;
	static CTraceFilterSimple_game filter{ };
	CGameTrace                     tr;

	// for whatever reason overrideview also gets called from the main menu.
	if (!g_Csgo.m_Engine->IsInGame())
		return;

	// camera should be in thirdperson.
	if (g_Config.b(g_Items.m_Visuals.m_ThirdpersonEnabled) && g_KeyInput.Key(g_Items.m_Visuals.m_ThirdpersonKey, g_Items.m_Visuals.m_ThirdpersonKeyMode))
	{
		// if alive and not in thirdperson already switch to thirdperson.
		if (g_Client.m_Processing && !g_Csgo.m_Input->CAM_IsThirdPerson())
			g_Csgo.m_Input->CAM_ToThirdPerson();

		// if dead and spectating in firstperson switch to thirdperson.
		else if (g_Client.m_LocalPlayer->m_iObserverMode() == 4) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (g_Csgo.m_Input->CAM_IsThirdPerson()) {
				g_Csgo.m_Input->CAM_ToFirstPerson();
				g_Csgo.m_Input->m_CameraOffset.z = 0.f;
			}

			g_Client.m_LocalPlayer->m_iObserverMode() = 5;
		}
	}

	// camera should be in firstperson.
	else if (g_Csgo.m_Input->CAM_IsThirdPerson()) {
		g_Csgo.m_Input->CAM_ToFirstPerson();
		g_Csgo.m_Input->m_CameraOffset.z = 0.f;
	}

	// if after all of this we are still in thirdperson.
	if (g_Csgo.m_Input->CAM_IsThirdPerson()) {
		// get camera angles.
		g_Csgo.m_Engine->GetViewAngles(offset);

		// get our viewangle's forward directional vector.
		Math::AngleVectors(offset, &forward);

		// cam_idealdist convar.
		offset.z = g_Config.i(g_Items.m_Visuals.m_ThirdpersonDistance);

		// start pos.
		origin = g_Client.m_EyePosition;

		// setup trace filter and trace.
		filter.SetPassEntity(g_Client.m_LocalPlayer);

		g_Csgo.m_EngineTrace->TraceRay(
			Ray(origin, origin - (forward * offset.z), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f }),
			MASK_NPCWORLDSTATIC,
			(ITraceFilter*)&filter,
			&tr
		);

		// adapt distance to travel time.
		Math::clamp(tr.m_Fraction, 0.f, 1.f);
		offset.z *= tr.m_Fraction;

		static float old_frac = 0;

		// animated thirdperson based on distance
		old_frac = Math::fInterpolate(old_frac, tr.m_Fraction, (tr.m_Fraction < old_frac&& old_frac - tr.m_Fraction > 0.1f) ? 0.125f : (g_Csgo.m_Globals->m_FrameTime * 10.4f));

		offset.z *= old_frac;

		// override camera angles.
		g_Csgo.m_Input->m_CameraOffset = { offset.x, offset.y, offset.z };
	}
}

void Client::ClanTag() {
	auto SetClanTag = [&](std::string tag) -> void {
		using SetClanTag_t = int(__fastcall*)(const char*, const char*);
		static auto SetClanTagFn = pattern::find(g_Csgo.m_engine_dll, XOR("53 56 57 8B DA 8B F9 FF 15")).as <SetClanTag_t >();

		SetClanTagFn(tag.c_str(), g_Config.i(g_Items.m_Misc.m_ClantagSpammer) == 1 ? XOR("IBLIS.club |") : XOR("IBLIS.club "));
	};

	static bool bResetOnce = false;
	static bool bDone = false;

	if (g_Config.i(g_Items.m_Misc.m_ClantagSpammer) > 0)
	{
		std::string szName = g_Config.i(g_Items.m_Misc.m_ClantagSpammer) == 1 ? XOR("IBLIS.club |") : XOR("IBLIS.club ");
		if (!szName.empty()) 
			SetClanTag(szName.c_str());

		if (!szName.empty())
		{
			if (g_Config.i(g_Items.m_Misc.m_ClantagSpammer) == 1)
				SetClanTag(szName.c_str());
			else {
				std::string szClantag = XOR("");
				for (size_t i = 0u; i < szName.length(); i++)
					szClantag += szName[(i - static_cast<int>(g_InputPrediction.m_Curtime)) % szName.length()];

				if (!szClantag.empty())
					SetClanTag(szClantag.c_str());
			}
		}

		bResetOnce = true;
		return;
	}

	if (bResetOnce) {
		SetClanTag(XOR(""));
		bResetOnce = false;
	}
}

void Client::Skybox()
{
	static auto sv_skyname = g_Csgo.m_CVar->FindVar(HASH("sv_skyname"));
	if (c_config::get()->b["skyboxchange"]) {
		switch (c_config::get()->i["skyboxes"]) {
		case 0:sv_skyname->SetValue(XOR("cs_tibet")); break;
		case 1:sv_skyname->SetValue(XOR("embassy")); break;
		case 2:sv_skyname->SetValue(XOR("italy")); break;
		case 3:sv_skyname->SetValue(XOR("sky_cs15_daylight01_hdr")); break;
		case 4:sv_skyname->SetValue(XOR("sky_csgo_cloudy01")); break;
		case 5:sv_skyname->SetValue(XOR("sky_csgo_night02")); break;
		case 6:sv_skyname->SetValue(XOR("sky_csgo_night02b")); break;
		case 7:sv_skyname->SetValue(XOR("sky_csgo_night_flat")); break;
		case 8:sv_skyname->SetValue(XOR("sky_day02_05_hdr")); break;
		case 9:sv_skyname->SetValue(XOR("sky_day02_05")); break;
		case 10:sv_skyname->SetValue(XOR("sky_l4d_rural02_ldr")); break;
		case 11:sv_skyname->SetValue(XOR("vertigo_hdr")); break;
		case 12:sv_skyname->SetValue(XOR("vertigoblue_hdr")); break;
		case 13:sv_skyname->SetValue(XOR("vertigo")); break;
		case 14:sv_skyname->SetValue(XOR("vietnam")); break;
		case 15:sv_skyname->SetValue(XOR("sky_dust")); break;
		case 16:sv_skyname->SetValue(XOR("jungle")); break;
		case 17:sv_skyname->SetValue(XOR("nukeblank")); break;
		case 18:sv_skyname->SetValue(XOR("office")); break;
		}
	}

	static const auto fog_enable = g_Csgo.m_CVar->FindVar(HASH("fog_enable"));
	fog_enable->SetValue(1);

	static const auto fog_override = g_Csgo.m_CVar->FindVar(HASH("fog_override"));
	fog_override->SetValue(g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFog));

	static const auto fog_color = g_Csgo.m_CVar->FindVar(HASH("fog_color"));
	fog_color->SetValue(std::string(std::to_string(g_Config.c(g_Items.m_Visuals.m_Effects.m_FogColor).r()) + " " + std::to_string(g_Config.c(g_Items.m_Visuals.m_Effects.m_FogColor).g()) + " " + std::to_string(g_Config.c(g_Items.m_Visuals.m_Effects.m_FogColor).b()) + " " + std::to_string(g_Config.c(g_Items.m_Visuals.m_Effects.m_FogColor).a()) + " ").c_str());

	static const auto fog_start = g_Csgo.m_CVar->FindVar(HASH("fog_start"));
	fog_start->SetValue(g_Config.i(g_Items.m_Visuals.m_Effects.m_FogStart));

	static const auto fog_end = g_Csgo.m_CVar->FindVar(HASH("fog_end"));
	fog_end->SetValue(g_Config.i(g_Items.m_Visuals.m_Effects.m_FogEnd));

	static const auto fog_destiny = g_Csgo.m_CVar->FindVar(HASH("fog_maxdensity"));
	fog_destiny->SetValue(0.4f);
}

void Client::NoSmoke() {
	std::vector<const char*> vistasmoke_mats =
	{
			XOR("particle/vistasmokev1/vistasmokev1_fire"),
			XOR("particle/vistasmokev1/vistasmokev1_smokegrenade"),
			XOR("particle/vistasmokev1/vistasmokev1_emods"),
			XOR("particle/vistasmokev1/vistasmokev1_emods_impactdust"),
	};

	for (auto mat_s : vistasmoke_mats)
	{
		IMaterial* mat = g_Csgo.m_MaterialSystem->FindMaterial(mat_s, (char*)XOR("Other textures"));

		if (!mat || mat->IsErrorMaterial())
			return;

		if (!g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SMOKE]) {
			mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			return;
		}

		mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	}
}

void Client::MouseCorrection()
{
	QAngle angOldViewAngles;
	g_Csgo.m_Engine->GetViewAngles(angOldViewAngles);

	float delta_x = std::remainderf(/*m_Command->m_ViewAngles.x*/ m_ViewAngles.x - angOldViewAngles.x, 360.0f);
	float delta_y = std::remainderf(/*m_Command->m_ViewAngles.y*/ m_ViewAngles.y - angOldViewAngles.y, 360.0f);

	if (delta_x != 0.0f)
	{
		float mouse_y = -((delta_x / g_Csgo.m_pitch->GetFloat()) / g_Csgo.sensivity->GetFloat());
		short mousedy;
		if (mouse_y <= 32767.0f) {
			if (mouse_y >= -32768.0f) {
				if (mouse_y >= 1.0f || mouse_y < 0.0f) {
					if (mouse_y <= -1.0f || mouse_y > 0.0f)
						mousedy = static_cast<short>(mouse_y);
					else
						mousedy = -1;
				}
				else {
					mousedy = 1;
				}
			}
			else {
				mousedy = 0x8000u;
			}
		}
		else {
			mousedy = 0x7FFF;
		}

		m_Command->m_MouseDY = mousedy;
	}

	if (delta_y != 0.0f)
	{
		float mouse_x = -((delta_y / g_Csgo.m_yaw->GetFloat()) / g_Csgo.sensivity->GetFloat());
		short mousedx;
		if (mouse_x <= 32767.0f) {
			if (mouse_x >= -32768.0f) {
				if (mouse_x >= 1.0f || mouse_x < 0.0f) {
					if (mouse_x <= -1.0f || mouse_x > 0.0f)
						mousedx = static_cast<short>(mouse_x);
					else
						mousedx = -1;
				}
				else {
					mousedx = 1;
				}
			}
			else {
				mousedx = 0x8000u;
			}
		}
		else {
			mousedx = 0x7FFF;
		}

		m_Command->m_MouseDX = mousedx;
	}
}

void Client::PanicMode()
{
	g_Csgo.m_Engine->ExecuteClientCmd(XOR("clear"));

	g_ListenerEntity.RemoveListenerEntity();
	g_Listener.RemoveListener();
	g_Detours.RemoveDetours();
	g_Hooks.RemoveHook();

	SetWindowLongPtr(g_Csgo.m_Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_Hooks.m_old_wndproc));

	if (!g_Csgo.m_Engine->IsInGame())
		return;

	return g_Hooks.m_NetChannel.Reset();
}

float Client::GetVelocityLengthXY(Player* m_Player)
{
	if (m_Player->m_AnimOverlay()[6].GetPlaybackRate() <= 0.f)
		return 0.f;

	char szWeaponMoveSeq[64];
	sprintf_s(szWeaponMoveSeq, XOR("move_%s"), m_Player->m_PlayerAnimStateCSGO()->GetWeaponPrefix());

	int nWeaponMoveSeq = m_Player->LookupSequence(szWeaponMoveSeq);
	if (nWeaponMoveSeq == -1)
		nWeaponMoveSeq = m_Player->LookupSequence(XOR("move"));

	float flSequenceCycleRate = m_Player->GetSequenceCycleRate(m_Player->GetModelPtr(), nWeaponMoveSeq);
	float flSequenceGroundSpeed = fmax(m_Player->GetSequenceMoveDist(m_Player->GetModelPtr(), nWeaponMoveSeq) / (1.0f / flSequenceCycleRate), 0.001f);

	static float_t flVelocityLength = 0.f;
	static bool bCalculated = false;

	// TODO@Monster: Try using hardcoded playbackrate numbers here.
	// Example: m_Player->m_AnimOverlay()[6].GetPlaybackRate > 0.00053f, so it will be more accurate than using broken velocity.
	// Playbackrate changes on directions so find a better way, or make it not changed on directions.
	if (!bCalculated && m_Player->m_PlayerAnimStateCSGO()->flVelocityLengthXY > 260.0f * 0.52f) {
		flVelocityLength = m_Player->m_AnimOverlay()[6].GetPlaybackRate();
		bCalculated = true;
	}

	if (bCalculated && m_Player->m_PlayerAnimStateCSGO()->flVelocityLengthXY <= 260.0f * 0.52f)
		bCalculated = false;

	static bool bWalkToRunTransitionState = 0;
	static float flWalkToRunTransition = 0;

	if (flWalkToRunTransition > 0.f && flWalkToRunTransition < 1.f)
	{
		if (bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
			flWalkToRunTransition += m_Player->m_PlayerAnimStateCSGO()->flLastUpdateIncrement * 2.f;

		else flWalkToRunTransition -= m_Player->m_PlayerAnimStateCSGO()->flLastUpdateIncrement * 2.f;

		flWalkToRunTransition = std::clamp(flWalkToRunTransition, 0.f, 1.f);
	}

	if (m_Player->m_AnimOverlay()[6].GetPlaybackRate() > flVelocityLength && bWalkToRunTransitionState == ANIM_TRANSITION_RUN_TO_WALK)
	{
		bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;
		flWalkToRunTransition = std::max(0.01f, flWalkToRunTransition);
	}
	else if (m_Player->m_AnimOverlay()[6].GetPlaybackRate() < flVelocityLength && bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
	{
		bWalkToRunTransitionState = ANIM_TRANSITION_RUN_TO_WALK;
		flWalkToRunTransition = std::min(0.99f, flWalkToRunTransition);
	}

	float flModifiedMoveCycleRate = (m_Player->m_AnimOverlay()[6].GetPlaybackRate() / flSequenceCycleRate) / Math::Lerp(flWalkToRunTransition, 1.0f, 0.85f) * flSequenceGroundSpeed;
	return (flModifiedMoveCycleRate / m_Player->m_PlayerAnimStateCSGO()->flLastUpdateIncrement);
}
