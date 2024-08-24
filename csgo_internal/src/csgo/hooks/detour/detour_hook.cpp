#include "../../../precompiled.h"

void Detours::Init()
{
	m_Interpolate = (decltype(&Interpolate))DetourFunction(reinterpret_cast<byte*>(g_Csgo.Interpolate), reinterpret_cast<byte*>(Interpolate));
	m_CL_Move = (decltype(&CL_Move))DetourFunction(reinterpret_cast<byte*>(g_Csgo.CL_Move), reinterpret_cast<byte*>(CL_Move));
	//m_CLCMsg_Mov2e = (decltype(&CLCMsg_Mov2e))DetourFunction(reinterpret_cast<byte*>(g_Csgo.CLCMsg_Move), reinterpret_cast<byte*>(CLCMsg_Mov2e));
	m_ReportHit = (decltype(&ReportHit))DetourFunction(reinterpret_cast<byte*>(g_Csgo.ReportHit), reinterpret_cast<byte*>(ReportHit));
	m_ColorModulate = (decltype(&ColorModulate))DetourFunction(reinterpret_cast<byte*>(g_Csgo.ColorModulate), reinterpret_cast<byte*>(ColorModulate));
	m_IsSelfAnimating = (decltype(&IsSelfAnimating))DetourFunction(reinterpret_cast<byte*>(g_Csgo.IsSelfAnimating), reinterpret_cast<byte*>(IsSelfAnimating));
	m_AddBaseAnimatingInterpolatedVars = (decltype(&AddBaseAnimatingInterpolatedVars))DetourFunction(reinterpret_cast<byte*>(g_Csgo.AddBaseAnimatingInterpolatedVars), reinterpret_cast<byte*>(AddBaseAnimatingInterpolatedVars));
	m_IsUsingStaticPropDebugModes = (decltype(&IsUsingStaticPropDebugModes))DetourFunction(reinterpret_cast<byte*>(g_Csgo.IsUsingStaticPropDebugModes), reinterpret_cast<byte*>(IsUsingStaticPropDebugModes));
	m_UpdateClientSideAnimation = (decltype(&UpdateClientSideAnimation))DetourFunction(reinterpret_cast<byte*>(g_Csgo.UpdateClientSideAnimation), reinterpret_cast<byte*>(UpdateClientSideAnimation));
	m_UpdateClientSideAnimations = (decltype(&UpdateClientSideAnimations))DetourFunction(reinterpret_cast<byte*>(g_Csgo.UpdateClientSideAnimations), reinterpret_cast<byte*>(UpdateClientSideAnimations));
	m_StudioFrameAdvance = (decltype(&StudioFrameAdvance))DetourFunction(reinterpret_cast<byte*>(g_Csgo.StudioFrameAdvance), reinterpret_cast<byte*>(StudioFrameAdvance));
	m_CheckForSequenceChange = (decltype(&CheckForSequenceChange))DetourFunction(reinterpret_cast<byte*>(g_Csgo.CheckForSequenceChange), reinterpret_cast<byte*>(CheckForSequenceChange));
	m_ShouldSkipAnimationFrame = (decltype(&ShouldSkipAnimationFrame))DetourFunction(reinterpret_cast<byte*>(g_Csgo.ShouldSkipAnimationFrame), reinterpret_cast<byte*>(ShouldSkipAnimationFrame));
	m_UpdateAnimationState = (decltype(&UpdateAnimationState))DetourFunction(reinterpret_cast<byte*>(g_Csgo.UpdateAnimationState), reinterpret_cast<byte*>(UpdateAnimationState));
	m_ResetAnimationState = (decltype(&ResetAnimationState))DetourFunction(reinterpret_cast<byte*>(g_Csgo.ResetAnimationState), reinterpret_cast<byte*>(ResetAnimationState));
	m_ModifyEyePosition = (decltype(&ModifyEyePosition))DetourFunction(reinterpret_cast<byte*>(g_Csgo.ModifyEyePosition), reinterpret_cast<byte*>(ModifyEyePosition));
}

void Detours::RemoveDetours()
{
	DetourRemove(reinterpret_cast<byte*>(m_Interpolate), reinterpret_cast<byte*>(Interpolate));
	//DetourRemove(reinterpret_cast<byte*>(m_CLCMsg_Mov2e), reinterpret_cast<byte*>(CLCMsg_Mov2e));
	DetourRemove(reinterpret_cast<byte*>(m_CL_Move), reinterpret_cast<byte*>(CL_Move));
	DetourRemove(reinterpret_cast<byte*>(m_ReportHit), reinterpret_cast<byte*>(ReportHit));
	DetourRemove(reinterpret_cast<byte*>(m_AddBaseAnimatingInterpolatedVars), reinterpret_cast<byte*>(AddBaseAnimatingInterpolatedVars));
	DetourRemove(reinterpret_cast<byte*>(m_IsSelfAnimating), reinterpret_cast<byte*>(IsSelfAnimating));
	DetourRemove(reinterpret_cast<byte*>(m_ColorModulate), reinterpret_cast<byte*>(ColorModulate));
	DetourRemove(reinterpret_cast<byte*>(m_IsUsingStaticPropDebugModes), reinterpret_cast<byte*>(IsUsingStaticPropDebugModes));
	DetourRemove(reinterpret_cast<byte*>(m_UpdateClientSideAnimation), reinterpret_cast<byte*>(UpdateClientSideAnimation));
	DetourRemove(reinterpret_cast<byte*>(m_UpdateClientSideAnimations), reinterpret_cast<byte*>(UpdateClientSideAnimations));
	DetourRemove(reinterpret_cast<byte*>(m_StudioFrameAdvance), reinterpret_cast<byte*>(StudioFrameAdvance));
	DetourRemove(reinterpret_cast<byte*>(m_CheckForSequenceChange), reinterpret_cast<byte*>(CheckForSequenceChange));
	DetourRemove(reinterpret_cast<byte*>(m_ShouldSkipAnimationFrame), reinterpret_cast<byte*>(ShouldSkipAnimationFrame));
	DetourRemove(reinterpret_cast<byte*>(m_UpdateAnimationState), reinterpret_cast<byte*>(UpdateAnimationState));
	DetourRemove(reinterpret_cast<byte*>(m_ResetAnimationState), reinterpret_cast<byte*>(ResetAnimationState));
	DetourRemove(reinterpret_cast<byte*>(m_ModifyEyePosition), reinterpret_cast<byte*>(ModifyEyePosition));
}

bool __fastcall Detours::Interpolate(void* ecx, void* edx, float currentTime)
{
	auto pBaseEntity = reinterpret_cast<Player*>(ecx);
	if (!pBaseEntity)
		return g_Detours.m_Interpolate(ecx, edx, currentTime);

	auto pViewModelOwner = pBaseEntity->get< ulong_t >(g_Offsets.m_hOwner) & 0xFFF;
	if (!pViewModelOwner)
		return g_Detours.m_Interpolate(ecx, edx, currentTime);

	Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity<Player*>(pViewModelOwner);
	if (!pPlayer || !pPlayer->IsPlayer() || !pPlayer->IsAlive() || !pPlayer->IsLocalPlayer())
		return g_Detours.m_Interpolate(ecx, edx, currentTime);

	const auto m_FinalPredictedTick = pPlayer->m_nFinalPredictedTick();
	const auto m_InterpolationAmount = g_Csgo.m_Globals->m_InterpolationAmount;

	pPlayer->m_nFinalPredictedTick() = pPlayer->m_nTickBase();

	if (g_TickbaseManipulation.CanSkipInterpolation())
		g_Csgo.m_Globals->m_InterpolationAmount = 0.0f;

	const bool bSetuped = g_Detours.m_Interpolate(ecx, edx, currentTime);

	if (g_TickbaseManipulation.CanSkipInterpolation())
		g_Csgo.m_Globals->m_InterpolationAmount = m_InterpolationAmount;

	pPlayer->m_nFinalPredictedTick() = m_FinalPredictedTick;
	return bSetuped;
}

void __fastcall Detours::AddBaseAnimatingInterpolatedVars(Player* m_Player, void* edx)
{
	if (!m_Player || !m_Player->IsPlayer() || !m_Player->IsAlive() || !m_Player->ShouldInterpolate())
		return g_Detours.m_AddBaseAnimatingInterpolatedVars(m_Player, edx);

	const auto bClientSideAnimation = m_Player->m_bClientSideAnimation();
	m_Player->m_bClientSideAnimation() = true;

	if (g_Detours.m_AddBaseAnimatingInterpolatedVars)
		g_Detours.m_AddBaseAnimatingInterpolatedVars(m_Player, edx);

	m_Player->m_bClientSideAnimation() = bClientSideAnimation;
}

bool __fastcall Detours::IsSelfAnimating(Player* m_Player, void* edx) {
	return false;
}

// TODO@Monster: Rebuilt and understand why it fucks up the game when called with disabled client animations.
// engine prediction is probably the reason.
void __fastcall Detours::StudioFrameAdvance(Player* m_Player, void* edx)
{
	return;
}

// PURPOSE: remove the interpolation on the C_BaseAnimating::ClientSideAnimationChanged, C_BaseAnimating::MaintainSequenceTransitions but second one is not called due to InPrediction hook.
void __fastcall Detours::CheckForSequenceChange(void* ecx, void* edx, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate) {
	return g_Detours.m_CheckForSequenceChange(ecx, edx, hdr, nCurSequence, bForceNewSequence, g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation) ? bInterpolate : false);
}

bool Detours::IsUsingStaticPropDebugModes() {
	return true;
}

bool __fastcall Detours::ShouldSkipAnimationFrame(Player* m_Player, void* edx) {
	return false;
}

// NOTE: These are not from C_CSPlayer class.
void __fastcall Detours::UpdateClientSideAnimation(Player* m_Player, void* edx)
{
	// TODO@Do for other players too.
	if (!m_Player || !m_Player->IsPlayer() || !m_Player->IsAlive() || !m_Player->IsLocalPlayer())
		return g_Detours.m_UpdateClientSideAnimation(m_Player, edx);

	return;
}

// NOTE: Static version of client animation.
void __fastcall Detours::UpdateClientSideAnimations(Player* m_Player, void* edx)
{
	// TODO@Do for other players too.
	if (!m_Player || !m_Player->IsPlayer() || !m_Player->IsAlive() || !m_Player->IsLocalPlayer())
		return g_Detours.m_UpdateClientSideAnimations(m_Player, edx);

	return;
}

void __vectorcall Detours::UpdateAnimationState(CCSGOPlayerAnimState* m_AnimationState, void* edx, QAngle angAngle, bool bForce)
{
	if (!m_AnimationState->pPlayer || !m_AnimationState->pPlayer->IsAlive() || !m_AnimationState->CacheSequences())
		return;

	// To do: Make it work with enemy players without animation issues on server side codes.
	//if (!m_AnimationState->pPlayer->IsLocalPlayer())
	//	return g_Detours.m_UpdateAnimationState(m_AnimationState, edx, angAngle, bForce);

	if (m_AnimationState->pPlayer->teammate(g_Client.m_LocalPlayer) && g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_TEAMMATES])
		return;

	angAngle.x = Math::AngleNormalize(angAngle.x + m_AnimationState->pPlayer->m_flThirdpersonRecoil());

	m_AnimationState->flLastUpdateIncrement = fmax(0.0f, g_Csgo.m_Globals->m_Curtime - m_AnimationState->flLastUpdateTime);
	m_AnimationState->flEyeYaw = Math::AngleNormalize(angAngle.y);
	m_AnimationState->flEyePitch = Math::AngleNormalize(angAngle.x);
	m_AnimationState->vecPositionCurrent = m_AnimationState->pPlayer->GetAbsOrigin();
	m_AnimationState->pWeapon = m_AnimationState->pPlayer->GetActiveWeapon();

	if (m_AnimationState->pWeapon != m_AnimationState->pWeaponLast || m_AnimationState->bFirstRunSinceInit)
	{
		for (int i = 0; i < ANIMATION_LAYER_COUNT; i++)
		{
			C_AnimationLayer* pLayer = &m_AnimationState->pPlayer->m_AnimOverlay()[i];
			if (pLayer)
			{
				pLayer->pDispatchedStudioHdr = NULL;
				pLayer->nDispatchedSrc = ACT_INVALID;
				pLayer->nDispatchedDst = ACT_INVALID;
			}
		}
	}

	m_AnimationState->flAnimDuckAmount = Math::clampf(Math::Approach(Math::clampf(m_AnimationState->pPlayer->m_flDuckAmount() + m_AnimationState->flDuckAdditional, 0.f, 1.f), m_AnimationState->flAnimDuckAmount, m_AnimationState->flLastUpdateIncrement * 6.0f), 0.f, 1.f);

	m_AnimationState->SetSequence(0);
	m_AnimationState->SetPlaybackrate(0);
	m_AnimationState->SetCycle(0);

	m_AnimationState->SetUpVelocity();
	m_AnimationState->SetUpAimMatrix();
	m_AnimationState->SetUpWeaponAction();
	m_AnimationState->SetUpMovement();
	m_AnimationState->SetUpWholeBodyAction();
	m_AnimationState->SetUpAliveloop();
	m_AnimationState->SetUpFlashedReaction();
	m_AnimationState->SetUpFlinch();
	m_AnimationState->SetUpLean();

	m_AnimationState->pPlayer->SetAbsAngles(QAngle(0.f, m_AnimationState->flFootYaw, 0.f));
	m_AnimationState->bFirstRunSinceInit = false;
	m_AnimationState->pWeaponLast = m_AnimationState->pWeapon;
	m_AnimationState->vecPositionLast = m_AnimationState->vecPositionCurrent;
	m_AnimationState->flLastUpdateTime = g_Csgo.m_Globals->m_Curtime;
	m_AnimationState->nLastUpdateFrame = g_Csgo.m_Globals->m_FrameCount;
}

void __fastcall Detours::ResetAnimationState(CCSGOPlayerAnimState* m_AnimationState, void* edx)
{
	// To do: Make it work with enemy players without animation issues on server side codes.
	if (!m_AnimationState->pPlayer->IsLocalPlayer())
		return g_Detours.m_ResetAnimationState(m_AnimationState, edx);

#ifndef CLIENT_DLL
	m_AnimationState->pPlayer->SetNumAnimOverlays(ANIMATION_LAYER_COUNT);
#endif
	m_AnimationState->ApplyLayerOrderPreset(get_animlayerpreset(Default), true);

#ifdef CLIENT_DLL
	m_AnimationState->nLastUpdateFrame = 0;
	m_AnimationState->flStepHeightLeft = 0;
	m_AnimationState->flStepHeightRight = 0;
#endif

#ifndef CLIENT_DLL
	m_AnimationState->flFlashedAmountEaseOutStart = 0;
	m_AnimationState->flFlashedAmountEaseOutEnd = 0;
#endif

	m_AnimationState->pWeapon = m_AnimationState->pPlayer->GetActiveWeapon();
	m_AnimationState->pWeaponLast = m_AnimationState->pWeapon;

#ifdef CLIENT_DLL
	m_AnimationState->pWeaponLastBoneSetup = m_AnimationState->pWeapon;
	m_AnimationState->flEyePositionSmoothLerp = 0;
	m_AnimationState->flStrafeChangeWeightSmoothFallOff = 0;
	m_AnimationState->bFirstFootPlantSinceInit = true;
#endif

	m_AnimationState->flLastUpdateTime = 0;
	m_AnimationState->flLastUpdateIncrement = 0;

	m_AnimationState->flEyeYaw = 0;
	m_AnimationState->flEyePitch = 0;
	m_AnimationState->flFootYaw = 0;
	m_AnimationState->flFootYawLast = 0;
	m_AnimationState->flMoveYaw = 0;
	m_AnimationState->flMoveYawIdeal = 0;
	m_AnimationState->flMoveYawCurrentToIdeal = 0;

#ifndef CLIENT_DLL
	m_AnimationState->pPlayer->m_flLowerBodyYawTarget() = 0;
	m_AnimationState->flLowerBodyRealignTimer = 0;
#endif

	m_AnimationState->tStandWalkAim.Init();
	m_AnimationState->tStandWalkAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.4f;
	m_AnimationState->tStandWalkAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.2f;
	m_AnimationState->tStandRunAim.Init();
	m_AnimationState->tStandRunAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.2f;
	m_AnimationState->tStandRunAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.4f;
	m_AnimationState->tCrouchWalkAim.Init();
	m_AnimationState->tCrouchWalkAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
	m_AnimationState->tCrouchWalkAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;

	m_AnimationState->flPrimaryCycle = 0;
	m_AnimationState->flMoveWeight = 0;
	m_AnimationState->flMoveWeightSmoothed = 0;
	m_AnimationState->flAnimDuckAmount = 0;
	m_AnimationState->flDuckAdditional = 0; // for when we duck a bit after landing from a jump
	m_AnimationState->flRecrouchWeight = 0;

	m_AnimationState->vecPositionCurrent.Init();
	m_AnimationState->vecPositionLast.Init();

	m_AnimationState->vecVelocity.Init();
	m_AnimationState->vecVelocityNormalized.Init();
	m_AnimationState->vecVelocityNormalizedNonZero.Init();
	m_AnimationState->flVelocityLengthXY = 0;
	m_AnimationState->flVelocityLengthZ = 0;

	m_AnimationState->flSpeedAsPortionOfRunTopSpeed = 0;
	m_AnimationState->flSpeedAsPortionOfWalkTopSpeed = 0;
	m_AnimationState->flSpeedAsPortionOfCrouchTopSpeed = 0;

	m_AnimationState->flDurationMoving = 0;
	m_AnimationState->flDurationStill = 0;

	m_AnimationState->bOnGround = true;
#ifndef CLIENT_DLL
	m_AnimationState->bJumping = false;
#endif
	m_AnimationState->flLandAnimMultiplier = 1.0f;
	m_AnimationState->flLeftGroundHeight = 0;
	m_AnimationState->bLanding = false;
	m_AnimationState->flJumpToFall = 0;
	m_AnimationState->flDurationInAir = 0;

	m_AnimationState->flWalkToRunTransition = 0;

	m_AnimationState->bLandedOnGroundThisFrame = false;
	m_AnimationState->bLeftTheGroundThisFrame = false;
	m_AnimationState->flInAirSmoothValue = 0;

	m_AnimationState->bOnLadder = false;
	m_AnimationState->flLadderWeight = 0;
	m_AnimationState->flLadderSpeed = 0;

	m_AnimationState->bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;

	m_AnimationState->bDefuseStarted = false;
	m_AnimationState->bPlantAnimStarted = false;
	m_AnimationState->bTwitchAnimStarted = false;
	m_AnimationState->bAdjustStarted = false;

	m_AnimationState->flNextTwitchTime = 0;

	m_AnimationState->flTimeOfLastKnownInjury = 0;

	m_AnimationState->flLastVelocityTestTime = 0;
	m_AnimationState->vecVelocityLast.Init();
	m_AnimationState->vecTargetAcceleration.Init();
	m_AnimationState->vecAcceleration.Init();
	m_AnimationState->flAccelerationWeight = 0;

	m_AnimationState->flAimMatrixTransition = 0;
	m_AnimationState->flAimMatrixTransitionDelay = 0;

	m_AnimationState->bFlashed = 0;

	m_AnimationState->flStrafeChangeWeight = 0;
	m_AnimationState->flStrafeChangeTargetWeight = 0;
	m_AnimationState->flStrafeChangeCycle = 0;
	m_AnimationState->nStrafeSequence = -1;
	m_AnimationState->bStrafeChanging = false;
	m_AnimationState->flDurationStrafing = 0;

	m_AnimationState->flFootLerp = 0;

	m_AnimationState->bFeetCrossed = false;

	m_AnimationState->bPlayerIsAccelerating = false;

#ifndef CLIENT_DLL
	m_AnimationState->bDeployRateLimiting = false;
#endif

	m_AnimationState->flDurationMoveWeightIsTooHigh = 0;
	m_AnimationState->flStaticApproachSpeed = 80;

	m_AnimationState->flStutterStep = 0;
	m_AnimationState->nPreviousMoveState = 0;

	m_AnimationState->flActionWeightBiasRemainder = 0;

	m_AnimationState->flAimYawMin = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN;
	m_AnimationState->flAimYawMax = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX;
	m_AnimationState->flAimPitchMin = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;
	m_AnimationState->flAimPitchMax = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;

	m_AnimationState->ActivityModifiers.Purge();
	m_AnimationState->bFirstRunSinceInit = true;

#ifdef CLIENT_DLL
	m_AnimationState->flCameraSmoothHeight = 0;
	m_AnimationState->bSmoothHeightValid = false;
	m_AnimationState->flLastTimeVelocityOverTen = 0;

	m_AnimationState->pPlayer->m_nAnimLODflags() = 0;
#endif
}

void __fastcall Detours::ModifyEyePosition(CCSGOPlayerAnimState* m_AnimationState, void* edx, Vector& vecInputEyePos)
{
	m_AnimationState->bSmoothHeightValid = false;
	return g_Detours.m_ModifyEyePosition(m_AnimationState, edx, vecInputEyePos);
}

void __cdecl Detours::CL_Move(float_t flAccumulatedExtraSamples, bool bIsFinalTick)
{
	g_Network.m_Sequence = 0;
	if (!g_Client.m_Processing)
		return g_Detours.m_CL_Move(flAccumulatedExtraSamples, bIsFinalTick);

	g_TickbaseManipulation.m_Data.m_ShiftingTickbase = false;
	if (g_TickbaseManipulation.m_Data.m_bSkipInterpolation = !g_TickbaseManipulation.IsAllowedToRun())
		return;

	g_Detours.m_CL_Move(flAccumulatedExtraSamples, bIsFinalTick);
	return g_TickbaseManipulation.PerformPackets();
}

void __fastcall Detours::ColorModulate(IMaterial* m_Material, void* edx, float* m_Red, float* m_Green, float* m_Blue) {
	g_Detours.m_ColorModulate(m_Material, edx, m_Red, m_Green, m_Blue);

	if (!m_Material || m_Material->IsErrorMaterial())
		return g_Detours.m_ColorModulate(m_Material, edx, m_Red, m_Green, m_Blue);

	if (g_Config.b(g_Items.m_Visuals.m_World.m_TransparentProps) && strstr(m_Material->GetTextureGroupName(), XOR("StaticProp textures")))
		m_Material->GetShaderParams()[5]->flValue = g_Config.i(g_Items.m_Visuals.m_World.m_TransparentPropsAmount) / 100.f;

	if (g_Config.vb(g_Items.m_Visuals.m_World.m_BrightnessAdjustment).at(0) && (strstr(m_Material->GetTextureGroupName(), XOR("World textures")) || strstr(m_Material->GetTextureGroupName(), XOR("StaticProp textures")) || strstr(m_Material->GetTextureGroupName(), XOR("SkyBox textures")))) {
		*m_Red *= strstr(m_Material->GetTextureGroupName(), XOR("StaticProp textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_PropColor).r() / 255.f) : strstr(m_Material->GetTextureGroupName(), XOR("SkyBox textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_SkyColor).r() / 255.f) : std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_WorldColor).r() / 255.f);
		*m_Green *= strstr(m_Material->GetTextureGroupName(), XOR("StaticProp textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_PropColor).g() / 255.f) : strstr(m_Material->GetTextureGroupName(), XOR("SkyBox textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_SkyColor).g() / 255.f) : std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_WorldColor).g() / 255.f);
		*m_Blue *= strstr(m_Material->GetTextureGroupName(), XOR("StaticProp textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_PropColor).b() / 255.f) : strstr(m_Material->GetTextureGroupName(), XOR("SkyBox textures")) ? std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_SkyColor).b() / 255.f) : std::min(1.f, g_Config.c(g_Items.m_Visuals.m_World.m_WorldColor).b() / 255.f);
	}
}

bool __cdecl Detours::ReportHit(Hit_t* pInfo) {
	if (!pInfo)
		return g_Detours.m_ReportHit(pInfo);

	WorldHitmarkerData_t pData;

	pData.m_alpha = 1.f;
	pData.m_time = g_Csgo.m_Globals->m_Curtime;
	pData.m_pos_x = pInfo->m_pos_x;
	pData.m_pos_y = pInfo->m_pos_y;
	pData.m_pos_z = pInfo->m_pos_z;

	// push pData.
	g_Visuals.m_world_hitmarkers.push_back(pData);

	// call original.
	return g_Detours.m_ReportHit(pInfo);
}

Detours g_Detours{ };