#include "../../precompiled.h"

//#define DEBUG

bool Client::IsUpdatingAnimations() 
{
	if (!m_Processing || g_Csgo.m_ClientState->m_DeltaTick < 0)
	{
		ResetAnimationData();
		return false;
	}
	
#ifdef DEBUG
	g_Client.m_LocalPlayer->DrawServerHitbox();
#endif // DEBUG

	return m_ChokedCommands == 0;
}

void Client::UpdateAnimations() 
{
	if (!IsUpdatingAnimations())
		return g_InputPrediction.Finish();

	const float flCurtime = g_Csgo.m_Globals->m_Curtime;
	const float flRealtime = g_Csgo.m_Globals->m_RealTime;
	const float flFrametime = g_Csgo.m_Globals->m_FrameTime;
	const float flAbsFrametime = g_Csgo.m_Globals->m_AbsFrametime;
	const int iFramecount = g_Csgo.m_Globals->m_FrameCount;
	const int iTickcount = g_Csgo.m_Globals->m_TickCount;
	const float flInterpAmount = g_Csgo.m_Globals->m_InterpolationAmount;

	g_Csgo.m_Globals->m_Curtime = game::TICKS_TO_TIME(g_Client.m_LocalPlayer->m_nTickBase());
	g_Csgo.m_Globals->m_RealTime = game::TICKS_TO_TIME(g_Client.m_LocalPlayer->m_nTickBase());
	g_Csgo.m_Globals->m_FrameCount = g_Client.m_LocalPlayer->m_nTickBase();
	g_Csgo.m_Globals->m_TickCount = g_Client.m_LocalPlayer->m_nTickBase();
	g_Csgo.m_Globals->m_FrameTime = g_Csgo.m_Globals->m_Interval;
	g_Csgo.m_Globals->m_AbsFrametime = g_Csgo.m_Globals->m_Interval;
	g_Csgo.m_Globals->m_InterpolationAmount = 0.f;

	SetUpAnimationData(false);
	UpdateServerSideAnimations();
	SetUpAnimationData(true);

	SetUpLowerbodyYawTarget();
	SetUpAnimationLayers();

	g_Csgo.m_Globals->m_Curtime = flCurtime;
	g_Csgo.m_Globals->m_RealTime = flRealtime;
	g_Csgo.m_Globals->m_FrameTime = flFrametime;
	g_Csgo.m_Globals->m_AbsFrametime = flAbsFrametime;
	g_Csgo.m_Globals->m_FrameCount = iFramecount;
	g_Csgo.m_Globals->m_TickCount = iTickcount;
	g_Csgo.m_Globals->m_InterpolationAmount = flInterpAmount;

	return g_InputPrediction.Finish();
}

void Client::UpdateServerSideAnimations()
{
	if (m_SpawnTime != m_LocalPlayer->m_flSpawnTime()) {
		m_SpawnTime = m_LocalPlayer->m_flSpawnTime();

		m_LocalPlayer->m_PlayerAnimStateCSGO()->Reset();
	}

	m_LocalPlayer->m_PlayerAnimStateCSGO()->Update(m_EyeAngle);

	m_LocalPlayer->GetPoseParameters(m_PoseParameters.data());
	m_LocalPlayer->GetAnimLayers(m_AnimationLayers.data());

	if (m_LocalPlayer->m_nSequence() != -1)
		m_LocalPlayer->OnLatchInterpolatedVariables(LATCH_ANIMATION_VAR);

	if (m_LocalPlayer->m_bKilledByTaser())
		m_LocalPlayer->HandleTaserAnimation();

	// TODO@Monster: That is a ghetto fix and causes problems, do it like the server does.
	m_LocalPlayer->GetViewModel(m_LocalPlayer->m_nViewModelIndex())->UpdateAllViewmodelAddons();
}

void Client::SetUpAnimationData(bool bUpdated)
{
	if (bUpdated) {
		m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw = Math::NormalizedAngle(Math::NormalizeYaw(m_LocalPlayer->m_PlayerAnimStateCSGO()->flEyeYaw - std::clamp(Math::NormalizeYaw(m_LocalPlayer->m_PlayerAnimStateCSGO()->flEyeYaw - m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw), -58.f, 58.f)));
		return;
	}

	m_EyeAngle = m_Command->m_ViewAngles;

	if (!g_AntiAim.m_Config.m_HideShots)
		if (m_ShotCommandNumber < GetModifableCommand()->m_CommandNumber && m_ShotCommandNumber >= GetModifableCommand()->m_CommandNumber - g_FakeLag.m_ChokedCommands)
			m_EyeAngle = g_Csgo.m_Input->m_Commands[m_ShotCommandNumber % 150].m_ViewAngles;

	m_LocalPlayer->m_flLowerBodyYawTarget() = m_LowerBodyYaw;

	if (m_EyeAngle != m_ViewAngles) {
		m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw = m_LocalPlayer->m_flLowerBodyYawTarget();

		if (fabs(std::clamp(Math::NormalizeYaw(m_EyeAngle.y - m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw), -58.f, 58.f)) < 58.f)
			m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw = m_EyeAngle.y;
	}

	// TODO@Monster: remove this so landing animations are perfectly fine.
	m_LocalPlayer->m_PlayerAnimStateCSGO()->bJumping = (m_LocalPlayer->GetSequenceActivity(m_AnimationLayers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetSequence()) == ACT_CSGO_JUMP) && !(m_LocalPlayer->m_fFlags() & FL_ONGROUND);
}

void Client::SetUpLowerbodyYawTarget()
{
	m_AnimFrame = g_Csgo.m_Globals->m_Curtime - m_Curtime;
	m_Curtime = g_Csgo.m_Globals->m_Curtime;

	if (m_LocalPlayer->m_PlayerAnimStateCSGO()->bOnGround) {
		if (m_LocalPlayer->m_PlayerAnimStateCSGO()->flVelocityLengthXY > 0.1f) {
			m_NextUpdateTime = (CSGO_ANIM_LOWER_REALIGN_DELAY * 0.2f);
			m_NextLowerBodyYawUpdateTime = m_Curtime + m_NextUpdateTime;
			if (m_LowerBodyYaw != m_EyeAngle.y)
				m_LowerBodyYaw = m_EyeAngle.y;

			m_CountForLowerBodyYawUpdate = 0;
		}
		else if (m_Curtime > m_NextLowerBodyYawUpdateTime) {
			if (!m_CountForLowerBodyYawUpdate && fabsf(Math::AngleDiff(m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw, m_EyeAngle.y)) > 35.0f || m_CountForLowerBodyYawUpdate) {
				m_NextUpdateTime = CSGO_ANIM_LOWER_REALIGN_DELAY;
				m_NextLowerBodyYawUpdateTime = m_Curtime + m_NextUpdateTime;
				if (m_LowerBodyYaw != m_EyeAngle.y)
					m_LowerBodyYaw = m_EyeAngle.y;
			}

			++m_CountForLowerBodyYawUpdate;
		}
	}
}

void Client::SetUpAnimationLayers()
{
	if (m_AnimationLayers.empty())
		return;

	m_AnimationLayers[ANIMATION_LAYER_LEAN].SetWeight(0.f);

	const float flWeight = m_AnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].GetWeight();

	if (m_LocalPlayer->m_PlayerAnimStateCSGO()->flVelocityLengthXY <= 0.1f)
		m_AnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].SetWeight(0.0f);
	else m_AnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].SetWeight(flWeight);

	m_AnimationLayers[ANIMATION_LAYER_ALIVELOOP].SetCycle(0.f);

	if (m_LocalPlayer->m_PlayerAnimStateCSGO()->bOnGround && m_LocalPlayer->m_PlayerAnimStateCSGO()->flVelocityLengthXY <= 0.1f)
	{
		m_AnimationLayers[ANIMATION_LAYER_ADJUST].SetWeight(0.0f);
		m_AnimationLayers[ANIMATION_LAYER_ADJUST].SetCycle(0.0f);
	}
}

void Client::SetAbsAngles()
{
	if (!m_Processing || g_Csgo.m_ClientState->m_DeltaTick < 0) 
	{
		ResetAnimationData();
		return;
	}

	if (!m_PoseParameters.empty())
		m_LocalPlayer->SetPoseParameters(m_PoseParameters.data());

	if (!m_AnimationLayers.empty())
		m_LocalPlayer->SetAnimLayers(m_AnimationLayers.data());

	m_LocalPlayer->SetAbsAngles(QAngle(0.f, m_LocalPlayer->m_PlayerAnimStateCSGO()->flFootYaw, 0.f));
}

void Client::ResetAnimationData()
{
	m_EyeAngle = QAngle(0, 0, 0);
	m_Radar = QAngle(0, 0, 0);
	m_BodyUpdate = false;
	m_LagCompensationBreaked = false;
	m_SpawnTime = 0.0f;
	m_LowerBodyYaw = 0.0f;
	m_NextUpdateTime = 0.0f;
	m_NextLowerBodyYawUpdateTime = 0.0f;
	m_Curtime = 0.0f;
	m_AnimFrame = 0.0f;

	m_ClientAnimationUpdate = false;
	m_SetupBones = false;

	m_CountForLowerBodyYawUpdate = 0;

	m_PoseParameters.fill(0.f);
	m_AnimationLayers.fill(C_AnimationLayer());
}
