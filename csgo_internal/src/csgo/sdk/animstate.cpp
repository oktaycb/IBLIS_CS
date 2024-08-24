#include "../../precompiled.h"

//#define CLIENT_DLL

void CCSGOPlayerAnimState::Update(QAngle angAngle, bool bForce) {
	static auto func = pattern::find(g_Csgo.m_client_dll, XOR("E8 ? ? ? ? 0F 57 C0 0F 11 86")).rel32< uintptr_t >(0x1);
	if (!func)
		return;

	__asm {
		push  0
		mov	  ecx, this
		movss xmm1, dword ptr[angAngle + 4]
		movss xmm2, dword ptr[angAngle]
		call  func
	}
}

void CCSGOPlayerAnimState::Reset() {
#ifndef CLIENT_DLL
	pPlayer->SetNumAnimOverlays(ANIMATION_LAYER_COUNT);
#endif
	ApplyLayerOrderPreset(get_animlayerpreset(Default), true);

#ifdef CLIENT_DLL
	nLastUpdateFrame = 0;
	flStepHeightLeft = 0;
	flStepHeightRight = 0;
#endif

#ifndef CLIENT_DLL
	flFlashedAmountEaseOutStart = 0;
	flFlashedAmountEaseOutEnd = 0;
#endif

	pWeapon = pPlayer->GetActiveWeapon();
	pWeaponLast = pWeapon;

#ifdef CLIENT_DLL
	pWeaponLastBoneSetup = pWeapon;
	flEyePositionSmoothLerp = 0;
	flStrafeChangeWeightSmoothFallOff = 0;
	bFirstFootPlantSinceInit = true;
#endif

	flLastUpdateTime = 0;
	flLastUpdateIncrement = 0;

	flEyeYaw = 0;
	flEyePitch = 0;
	flFootYaw = 0;
	flFootYawLast = 0;
	flMoveYaw = 0;
	flMoveYawIdeal = 0;
	flMoveYawCurrentToIdeal = 0;

#ifndef CLIENT_DLL
	pPlayer->m_flLowerBodyYawTarget() = 0;
	flLowerBodyRealignTimer = 0;
#endif

	tStandWalkAim.Init();
	tStandWalkAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.4f;
	tStandWalkAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.2f;
	tStandRunAim.Init();
	tStandRunAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.2f;
	tStandRunAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.4f;
	tCrouchWalkAim.Init();
	tCrouchWalkAim.flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
	tCrouchWalkAim.flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;

	flPrimaryCycle = 0;
	flMoveWeight = 0;
	flMoveWeightSmoothed = 0;
	flAnimDuckAmount = 0;
	flDuckAdditional = 0; // for when we duck a bit after landing from a jump
	flRecrouchWeight = 0;

	vecPositionCurrent.Init();
	vecPositionLast.Init();

	vecVelocity.Init();
	vecVelocityNormalized.Init();
	vecVelocityNormalizedNonZero.Init();
	flVelocityLengthXY = 0;
	flVelocityLengthZ = 0;

	flSpeedAsPortionOfRunTopSpeed = 0;
	flSpeedAsPortionOfWalkTopSpeed = 0;
	flSpeedAsPortionOfCrouchTopSpeed = 0;

	flDurationMoving = 0;
	flDurationStill = 0;

	bOnGround = true;
#ifndef CLIENT_DLL
	bJumping = false;
#endif
	flLandAnimMultiplier = 1.0f;
	flLeftGroundHeight = 0;
	bLanding = false;
	flJumpToFall = 0;
	flDurationInAir = 0;

	flWalkToRunTransition = 0;

	bLandedOnGroundThisFrame = false;
	bLeftTheGroundThisFrame = false;
	flInAirSmoothValue = 0;

	bOnLadder = false;
	flLadderWeight = 0;
	flLadderSpeed = 0;

	bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;

	bDefuseStarted = false;
	bPlantAnimStarted = false;
	bTwitchAnimStarted = false;
	bAdjustStarted = false;

	flNextTwitchTime = 0;

	flTimeOfLastKnownInjury = 0;

	flLastVelocityTestTime = 0;
	vecVelocityLast.Init();
	vecTargetAcceleration.Init();
	vecAcceleration.Init();
	flAccelerationWeight = 0;

	flAimMatrixTransition = 0;
	flAimMatrixTransitionDelay = 0;

	bFlashed = 0;

	flStrafeChangeWeight = 0;
	flStrafeChangeTargetWeight = 0;
	flStrafeChangeCycle = 0;
	nStrafeSequence = -1;
	bStrafeChanging = false;
	flDurationStrafing = 0;

	flFootLerp = 0;

	bFeetCrossed = false;

	bPlayerIsAccelerating = false;

#ifndef CLIENT_DLL
	bDeployRateLimiting = false;
#endif

	flDurationMoveWeightIsTooHigh = 0;
	flStaticApproachSpeed = 80;

	flStutterStep = 0;
	nPreviousMoveState = 0;

	flActionWeightBiasRemainder = 0;

	flAimYawMin = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN;
	flAimYawMax = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX;
	flAimPitchMin = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;
	flAimPitchMax = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;

	ActivityModifiers.Purge();
	bFirstRunSinceInit = true;

#ifdef CLIENT_DLL
	flCameraSmoothHeight = 0;
	bSmoothHeightValid = false;
	flLastTimeVelocityOverTen = 0;

	pPlayer->m_nAnimLODflags() = 0;
#endif
}

void CCSGOPlayerAnimState::ModifyEyePosition(Vector& vecInputEyePos)
{
	using ModifyEyePosition_t = void(__thiscall*)(void*, Vector&);

	static auto func = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 58 56 57 8B F9 83 7F 60")).as< ModifyEyePosition_t >();
	if (!func)
		return;

	return func(this, vecInputEyePos);
}

bool CCSGOPlayerAnimState::CacheSequences()
{
	using CacheSequences_t = bool(__thiscall*)(void*);

	static auto func = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 34 53 56 8B F1 57 8B 46 60")).as< CacheSequences_t >();
	if (!func)
		return false;

	return func(this);
}

void CCSGOPlayerAnimState::SetSequence(int nSequence)
{
	if (pPlayer->m_bUseNewAnimstate() && pPlayer->m_PlayerAnimStateCSGO())
		pPlayer->SetSequence(0);
	else pPlayer->SetSequence(nSequence);
}

void CCSGOPlayerAnimState::SetCycle(float flCycle)
{
	if (pPlayer->m_flCycle() != flCycle)
	{
		if (!Math::IsFinite(flCycle))
			return;

		pPlayer->m_flCycle() = flCycle;
		pPlayer->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
	}
}

void CCSGOPlayerAnimState::SetPlaybackrate(float flPlaybackrate)
{
	pPlayer->m_flPlaybackRate() = flPlaybackrate;
}

void CCSGOPlayerAnimState::SetUpVelocity()
{
#ifndef CLIENT_DLL
	Vector vecAbsVelocity = pPlayer->m_vecAbsVelocity();
#else
	Vector vecAbsVelocity = vecVelocity;

	if (g_Csgo.m_Engine->IsHLTV() || g_Csgo.m_Engine->IsPlayingTimeDemo())
		vecAbsVelocity = pPlayer->m_vecAbsVelocity();
	else pPlayer->EstimateAbsVelocity(vecAbsVelocity);

	if (vecAbsVelocity.LengthSqr() > std::sqrtf(CS_PLAYER_SPEED_RUN * CSGO_ANIM_MAX_VEL_LIMIT))
		vecAbsVelocity = vecAbsVelocity.Normalized() * (CS_PLAYER_SPEED_RUN * CSGO_ANIM_MAX_VEL_LIMIT);
#endif

	flVelocityLengthZ = vecAbsVelocity.z;
	vecAbsVelocity.z = 0;

	bPlayerIsAccelerating = (vecVelocityLast.LengthSqr() < vecAbsVelocity.LengthSqr());

	vecVelocity = Math::Approach(vecAbsVelocity, vecVelocity, flLastUpdateIncrement * 2000);
	vecVelocityNormalized = vecVelocity.Normalized();

	flVelocityLengthXY = std::min(vecVelocity.Length(), CS_PLAYER_SPEED_RUN);

	if (flVelocityLengthXY > 0)
		vecVelocityNormalizedNonZero = vecVelocityNormalized;

	float flMaxSpeedRun = pWeapon ? std::max(pWeapon->GetMaxSpeed(), 0.001f) : CS_PLAYER_SPEED_RUN;
	Assert(flMaxSpeedRun > 0);

	flSpeedAsPortionOfRunTopSpeed = Math::clampf(flVelocityLengthXY / flMaxSpeedRun, 0, 1);
	flSpeedAsPortionOfWalkTopSpeed = flVelocityLengthXY / (flMaxSpeedRun * CS_PLAYER_SPEED_WALK_MODIFIER);
	flSpeedAsPortionOfCrouchTopSpeed = flVelocityLengthXY / (flMaxSpeedRun * CS_PLAYER_SPEED_DUCK_MODIFIER);

	if (flSpeedAsPortionOfWalkTopSpeed >= 1)
		flStaticApproachSpeed = flVelocityLengthXY;
	else if (flSpeedAsPortionOfWalkTopSpeed < 0.5f)
		flStaticApproachSpeed = Math::Approach(80, flStaticApproachSpeed, flLastUpdateIncrement * 60);

	bool bStartedMovingThisFrame = false;
	bool bStoppedMovingThisFrame = false;

	if (flVelocityLengthXY > 0) {
		bStartedMovingThisFrame = (flDurationMoving <= 0);
		flDurationStill = 0;
		flDurationMoving += flLastUpdateIncrement;
	}
	else {
		bStoppedMovingThisFrame = (flDurationStill <= 0);
		flDurationMoving = 0;
		flDurationStill += flLastUpdateIncrement;
	}

#ifndef CLIENT_DLL
	if (!bAdjustStarted && bStoppedMovingThisFrame && bOnGround && !bOnLadder && !bLanding && flStutterStep < 50)
	{
		SetLayerSequence(ANIMATION_LAYER_ADJUST, SelectSequenceFromActMods(ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING));
		bAdjustStarted = true;
	}

	if (GetLayerActivity(ANIMATION_LAYER_ADJUST) == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING ||
		GetLayerActivity(ANIMATION_LAYER_ADJUST) == ACT_CSGO_IDLE_TURN_BALANCEADJUST)
	{
		if (bAdjustStarted && flSpeedAsPortionOfCrouchTopSpeed <= 0.25f)
		{
			IncrementLayerCycleWeightRateGeneric(ANIMATION_LAYER_ADJUST);
			bAdjustStarted = !(IsLayerSequenceCompleted(ANIMATION_LAYER_ADJUST));
		}
		else
		{
			bAdjustStarted = false;
			float flWeight = GetLayerWeight(ANIMATION_LAYER_ADJUST);
			SetLayerWeight(ANIMATION_LAYER_ADJUST, Math::Approach(0, flWeight, flLastUpdateIncrement * 5));
			SetLayerWeightRate(ANIMATION_LAYER_ADJUST, flWeight);
		}
	}
#endif
	flFootYawLast = flFootYaw;
	Math::clamp(flFootYaw, -360.f, 360.f);

	float flEyeFootDelta = Math::AngleDiff(flEyeYaw, flFootYaw);
	float flAimMatrixWidthRange = Math::Lerp(Math::clampf(flSpeedAsPortionOfWalkTopSpeed, 0, 1), 1.0f, Math::Lerp(flWalkToRunTransition, CSGO_ANIM_AIM_NARROW_WALK, CSGO_ANIM_AIM_NARROW_RUN));

	if (flAnimDuckAmount > 0.f)
		flAimMatrixWidthRange = Math::Lerp(flAnimDuckAmount * Math::clampf(flSpeedAsPortionOfCrouchTopSpeed, 0.f, 1.f), flAimMatrixWidthRange, CSGO_ANIM_AIM_NARROW_CROUCHMOVING);

	float flTempYawMax = flAimYawMax * flAimMatrixWidthRange;
	float flTempYawMin = flAimYawMin * flAimMatrixWidthRange;

	if (flEyeFootDelta > flTempYawMax)
		flFootYaw = flEyeYaw - abs(flTempYawMax);
	else if (flEyeFootDelta < flTempYawMin)
		flFootYaw = flEyeYaw + abs(flTempYawMin);

	flFootYaw = Math::AngleNormalize(flFootYaw);

	if (bOnGround) {
		if (flVelocityLengthXY > 0.1f) {
			flFootYaw = Math::ApproachAngle(flEyeYaw, flFootYaw, flLastUpdateIncrement * (30.0f + 20.0f * flWalkToRunTransition));

#ifndef CLIENT_DLL
				flLowerBodyRealignTimer = g_Csgo.m_Globals->m_Curtime + (CSGO_ANIM_LOWER_REALIGN_DELAY * 0.2f);
				pPlayer->m_flLowerBodyYawTarget() = flEyeYaw;
#endif
		}
		else {
			flFootYaw = Math::ApproachAngle(pPlayer->m_flLowerBodyYawTarget(), flFootYaw, flLastUpdateIncrement * CSGO_ANIM_LOWER_CATCHUP_IDLE);

#ifndef CLIENT_DLL
			if (g_Csgo.m_Globals->m_Curtime > flLowerBodyRealignTimer && abs(Math::AngleDiff(flFootYaw, flEyeYaw)) > 35.0f) {
				flLowerBodyRealignTimer = g_Csgo.m_Globals->m_Curtime + CSGO_ANIM_LOWER_REALIGN_DELAY;
				pPlayer->m_flLowerBodyYawTarget() = flEyeYaw;
			}
#endif
		}
	}

#ifndef CLIENT_DLL
	if (flVelocityLengthXY <= CS_PLAYER_SPEED_STOPPED && bOnGround && !bOnLadder && !bLanding && flLastUpdateIncrement > 0 && abs(Math::AngleDiff(flFootYawLast, flFootYaw) / flLastUpdateIncrement > CSGO_ANIM_READJUST_THRESHOLD))
	{
		SetLayerSequence(ANIMATION_LAYER_ADJUST, SelectSequenceFromActMods(ACT_CSGO_IDLE_TURN_BALANCEADJUST));
		bAdjustStarted = true;
	}
#endif

#ifdef CLIENT_DLL
	if (GetLayerWeight(ANIMATION_LAYER_ADJUST) > 0)
	{
		IncrementLayerCycle(ANIMATION_LAYER_ADJUST, false);
		IncrementLayerWeight(ANIMATION_LAYER_ADJUST);
	}
#endif

	if (flVelocityLengthXY > 0 && bOnGround)
	{
		float flRawYawIdeal = (atan2(-vecVelocity[1], -vecVelocity[0]) * 180 / Math::pi);
		if (flRawYawIdeal < 0)
			flRawYawIdeal += 360;

		flMoveYawIdeal = Math::AngleNormalize(Math::AngleDiff(flRawYawIdeal, flFootYaw));
	}

	flMoveYawCurrentToIdeal = Math::AngleNormalize(Math::AngleDiff(flMoveYawIdeal, flMoveYaw));

	if (bStartedMovingThisFrame && flMoveWeight <= 0)
	{
		flMoveYaw = flMoveYawIdeal;

		int nMoveSeq = GetLayerSequence(ANIMATION_LAYER_MOVEMENT_MOVE);
		if (nMoveSeq != -1)
		{
			mstudioseqdesc_t& seqdesc = pPlayer->GetModelPtr()->pSeqdesc(nMoveSeq);
			if (seqdesc.numanimtags > 0)
			{
				if (abs(Math::AngleDiff(flMoveYaw, 180)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_N, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, 135)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_NE, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, 90)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_E, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, 45)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_SE, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, 0)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_S, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, -45)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_SW, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, -90)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_W, 0, 1);
				}
				else if (abs(Math::AngleDiff(flMoveYaw, -135)) <= EIGHT_WAY_WIDTH)
				{
					flPrimaryCycle = pPlayer->GetFirstSequenceAnimTag(nMoveSeq, ANIMTAG_STARTCYCLE_NW, 0, 1);
				}
			}
		}
	}
	else if (GetLayerWeight(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE) >= 1.f)
		flMoveYaw = flMoveYawIdeal;
	else
		flMoveYaw = Math::AngleNormalize(flMoveYaw + (flMoveYawCurrentToIdeal * Math::Bias(Math::Lerp(flAnimDuckAmount, Math::clampf(flSpeedAsPortionOfWalkTopSpeed, 0, 1), Math::clampf(flSpeedAsPortionOfCrouchTopSpeed, 0, 1)), 0.18f) + 0.1f));

	tPoseParamMappings[PLAYER_POSE_PARAM_MOVE_YAW].SetValue(pPlayer, flMoveYaw);

	float flAimYaw = Math::AngleDiff(flEyeYaw, flFootYaw);
	if (flAimYaw >= 0 && flAimYawMax != 0)
		flAimYaw = (flAimYaw / flAimYawMax) * 60.0f;
	else if (flAimYawMin != 0)
		flAimYaw = (flAimYaw / flAimYawMin) * -60.0f;

	tPoseParamMappings[PLAYER_POSE_PARAM_BODY_YAW].SetValue(pPlayer, flAimYaw);

	float flPitch = Math::AngleDiff(flEyePitch, 0);
	if (flPitch > 0)
		flPitch = (flPitch / flAimPitchMax) * CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;
	else flPitch = (flPitch / flAimPitchMin) * CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;

	tPoseParamMappings[PLAYER_POSE_PARAM_BODY_PITCH].SetValue(pPlayer, flPitch);
	tPoseParamMappings[PLAYER_POSE_PARAM_SPEED].SetValue(pPlayer, flSpeedAsPortionOfWalkTopSpeed);
	tPoseParamMappings[PLAYER_POSE_PARAM_STAND].SetValue(pPlayer, 1.0f - (flAnimDuckAmount * flInAirSmoothValue));
}

void CCSGOPlayerAnimState::SetUpAimMatrix()
{
	if (flAnimDuckAmount <= 0 || flAnimDuckAmount >= 1)
	{
		bool bPlayerIsWalking = (pPlayer && pPlayer->m_bIsWalking());
		bool bPlayerIsScoped = (pPlayer && pPlayer->m_bIsScoped());

		if (bPlayerIsScoped) {
			tStandWalkAim.flDurationStateHasBeenInValid = tStandWalkAim.flHowLongToWaitUntilTransitionCanBlendOut;
			tStandRunAim.flDurationStateHasBeenInValid = tStandRunAim.flHowLongToWaitUntilTransitionCanBlendOut;
			tCrouchWalkAim.flDurationStateHasBeenInValid = tCrouchWalkAim.flHowLongToWaitUntilTransitionCanBlendOut;
		}

		float flTransitionSpeed = flLastUpdateIncrement * (bPlayerIsScoped ? CSGO_ANIM_SPEED_TO_CHANGE_AIM_MATRIX_SCOPED : CSGO_ANIM_SPEED_TO_CHANGE_AIM_MATRIX);

		tStandWalkAim.UpdateTransitionState(bPlayerIsWalking && !bPlayerIsScoped && flSpeedAsPortionOfWalkTopSpeed > 0.7f && flSpeedAsPortionOfRunTopSpeed < 0.7,
			flLastUpdateIncrement, flTransitionSpeed);

		tStandRunAim.UpdateTransitionState(!bPlayerIsScoped && flSpeedAsPortionOfRunTopSpeed >= 0.7,
			flLastUpdateIncrement, flTransitionSpeed);

		tCrouchWalkAim.UpdateTransitionState(!bPlayerIsScoped && flSpeedAsPortionOfCrouchTopSpeed >= 0.5,
			flLastUpdateIncrement, flTransitionSpeed);
	}

	float flStandIdleWeight = 1;
	float flStandWalkWeight = tStandWalkAim.flBlendValue;
	float flStandRunWeight = tStandRunAim.flBlendValue;
	float flCrouchIdleWeight = 1;
	float flCrouchWalkWeight = tCrouchWalkAim.flBlendValue;

	if (flStandWalkWeight >= 1)
		flStandIdleWeight = 0;

	if (flStandRunWeight >= 1) {
		flStandIdleWeight = 0;
		flStandWalkWeight = 0;
	}

	if (flCrouchWalkWeight >= 1)
		flCrouchIdleWeight = 0;

	if (flAnimDuckAmount >= 1) {
		flStandIdleWeight = 0;
		flStandWalkWeight = 0;
		flStandRunWeight = 0;
	}
	else if (flAnimDuckAmount <= 0) {
		flCrouchIdleWeight = 0;
		flCrouchWalkWeight = 0;
	}

	float flOneMinusDuckAmount = 1.0f - flAnimDuckAmount;

	flCrouchIdleWeight *= flAnimDuckAmount;
	flCrouchWalkWeight *= flAnimDuckAmount;
	flStandWalkWeight *= flOneMinusDuckAmount;
	flStandRunWeight *= flOneMinusDuckAmount;

	if (flCrouchIdleWeight < 1 && flCrouchWalkWeight < 1 && flStandWalkWeight < 1 && flStandRunWeight < 1)
		flStandIdleWeight = 1;

	tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_STAND_IDLE].SetValue(pPlayer, flStandIdleWeight);
	tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK].SetValue(pPlayer, flStandWalkWeight);
	tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN].SetValue(pPlayer, flStandRunWeight);
	tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_IDLE].SetValue(pPlayer, flCrouchIdleWeight);
	tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK].SetValue(pPlayer, flCrouchWalkWeight);

	char szTransitionStandAimMatrix[MAX_ANIMSTATE_ANIMNAME_CHARS];
	sprintf_s(szTransitionStandAimMatrix, "%s_aim", GetWeaponPrefix());
	int nSeqStand = pPlayer->LookupSequence(szTransitionStandAimMatrix);

	C_AnimationLayer* pAimLayer = &pPlayer->m_AnimOverlay()[ANIMATION_LAYER_AIMMATRIX];
	if (pAimLayer && pWeapon) {
		Player* pAimMatrixHolder = pPlayer;
		int nSeq = nSeqStand;

		Weapon* pWeaponWorldModel = pWeapon->GetWeaponWorldModel();
		if (pWeaponWorldModel && pAimLayer->nDispatchedDst != ACT_INVALID)
		{
			pAimMatrixHolder = (Player*)pWeaponWorldModel;
			nSeq = pAimLayer->nDispatchedDst;
		}

		if (nSeq > 0) {
			float flYawIdleMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMIN_IDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN);
			float flYawIdleMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMAX_IDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX);
			float flYawWalkMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMIN_WALK, flYawIdleMin);
			float flYawWalkMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMAX_WALK, flYawIdleMax);
			float flYawRunMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMIN_RUN, flYawWalkMin);
			float flYawRunMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMAX_RUN, flYawWalkMax);
			float flYawCrouchIdleMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMIN_CROUCHIDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN);
			float flYawCrouchIdleMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMAX_CROUCHIDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX);
			float flYawCrouchWalkMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMIN_CROUCHWALK, flYawCrouchIdleMin);
			float flYawCrouchWalkMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_YAWMAX_CROUCHWALK, flYawCrouchIdleMax);

			float flWalkAmt = tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK].GetValue(pPlayer);
			float flRunAmt = tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN].GetValue(pPlayer);
			float flCrouchWalkAmt = tPoseParamMappings[PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK].GetValue(pPlayer);

			flAimYawMin = Math::Lerp(flAnimDuckAmount,
				Math::Lerp(flRunAmt, Math::Lerp(flWalkAmt, flYawIdleMin, flYawWalkMin), flYawRunMin),
				Math::Lerp(flCrouchWalkAmt, flYawCrouchIdleMin, flYawCrouchWalkMin));

			flAimYawMax = Math::Lerp(flAnimDuckAmount,
				Math::Lerp(flRunAmt, Math::Lerp(flWalkAmt, flYawIdleMax, flYawWalkMax), flYawRunMax),
				Math::Lerp(flCrouchWalkAmt, flYawCrouchIdleMax, flYawCrouchWalkMax));

			float flPitchIdleMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMIN_IDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN);
			float flPitchIdleMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMAX_IDLE, CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX);
			float flPitchWalkRunMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMIN_WALKRUN, flPitchIdleMin);
			float flPitchWalkRunMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMAX_WALKRUN, flPitchIdleMax);
			float flPitchCrouchMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMIN_CROUCH, CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN);
			float flPitchCrouchMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMAX_CROUCH, CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX);
			float flPitchCrouchWalkMin = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMIN_CROUCHWALK, flPitchCrouchMin);
			float flPitchCrouchWalkMax = pAimMatrixHolder->GetAnySequenceAnimTag(nSeq, ANIMTAG_AIMLIMIT_PITCHMAX_CROUCHWALK, flPitchCrouchMax);

			flAimPitchMin = Math::Lerp(flAnimDuckAmount, Math::Lerp(flWalkAmt, flPitchIdleMin, flPitchWalkRunMin), Math::Lerp(flCrouchWalkAmt, flPitchCrouchMin, flPitchCrouchWalkMin));
			flAimPitchMax = Math::Lerp(flAnimDuckAmount, Math::Lerp(flWalkAmt, flPitchIdleMax, flPitchWalkRunMax), Math::Lerp(flCrouchWalkAmt, flPitchCrouchMax, flPitchCrouchWalkMax));
		}
	}

	UpdateAnimLayer(ANIMATION_LAYER_AIMMATRIX, nSeqStand, 0.f, 1.f, 0.f);
}

void CCSGOPlayerAnimState::SetUpWeaponAction()
{
	animstate_layer_t nLayer = ANIMATION_LAYER_WEAPON_ACTION;

#ifndef CLIENT_DLL
	bool bDoIncrement = true;

	if (pWeapon && bDeployRateLimiting && GetLayerActivity(nLayer) == ACT_CSGO_DEPLOY && GetLayerCycle(nLayer) >= CSGO_ANIM_DEPLOY_RATELIMIT) {
		SetLayerSequence(nLayer, SelectSequenceFromActMods(ACT_CSGO_DEPLOY));
		SetLayerWeight(nLayer, 0);
		bDoIncrement = bDeployRateLimiting = false;
	}

	if (nAnimstateModelVersion < 2)
	{
		if (flAnimDuckAmount > 0 && GetLayerWeight(nLayer) > 0 && !LayerSequenceHasActMod(nLayer, "crouch"))
		{
			if (GetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH) <= 0)
				SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH, pPlayer->LookupSequence("recrouch_generic"));

			SetLayerWeight(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH, GetLayerWeight(nLayer) * flAnimDuckAmount);
		}
		else
			SetLayerWeight(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH, 0);
	}
	else
	{
		float flTargetRecrouchWeight = 0.f;
		if (GetLayerWeight(nLayer) > 0.f)
		{
			if (GetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH) <= 0)
				SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH, pPlayer->LookupSequence("recrouch_generic"));

			if (LayerSequenceHasActMod(nLayer, "crouch"))
			{
				if (flAnimDuckAmount < 1)
					flTargetRecrouchWeight = GetLayerWeight(nLayer) * (1.0f - flAnimDuckAmount);
			}
			else if (flAnimDuckAmount > 0)
				flTargetRecrouchWeight = GetLayerWeight(nLayer) * flAnimDuckAmount;
		}
		else if (GetLayerWeight(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH) > 0.f)
			flTargetRecrouchWeight = Math::Approach(0, GetLayerWeight(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH), flLastUpdateIncrement * 4.f);

		SetLayerWeight(ANIMATION_LAYER_WEAPON_ACTION_RECROUCH, flTargetRecrouchWeight);
	}

	if (bDoIncrement)
	{
		IncrementLayerCycle(nLayer, false);

		float flWeightPrev = GetLayerWeight(nLayer);
		float flDesiredWeight = GetLayerIdealWeightFromSeqCycle(nLayer);

		SetLayerWeight(nLayer, flDesiredWeight);
		SetLayerWeightRate(nLayer, flWeightPrev);
	}
#else
	if (GetLayerWeight(nLayer) > 0)
	{
		IncrementLayerCycle(nLayer, false);
		IncrementLayerWeight(nLayer);
	}
#endif

	//set weapon sequenceand cycle so dispatched events hit
	C_AnimationLayer* pWeaponLayer = &pPlayer->m_AnimOverlay()[nLayer];
	if (pWeaponLayer && pWeapon)
	{
		Player* pWeaponWorldModel = (Player*)pWeapon->GetWeaponWorldModel();
		if (pWeaponWorldModel)
		{
			if (pWeaponLayer->nDispatchedDst > 0 && pWeaponLayer->GetWeight() > 0) // fixme: does the weight check make 0-frame events fail? Added a check below just in case.
			{
				pWeaponWorldModel->SetSequence(pWeaponLayer->nDispatchedDst);
				pWeaponWorldModel->m_flPlaybackRate() = pWeaponLayer->GetPlaybackRate();

				if (pWeaponWorldModel->m_flCycle() != pWeaponLayer->GetCycle()) {
					pWeaponWorldModel->m_flCycle() = pWeaponLayer->GetCycle();
					pWeaponWorldModel->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
				}
#ifndef CLIENT_DLL
				//pWeaponWorldModel->DispatchAnimEvents(pWeaponWorldModel);
#endif
			}
			else
			{
#ifndef CLIENT_DLL
				//if (pWeaponWorldModel->m_flCycle() != 0)
				//	pWeaponWorldModel->DispatchAnimEvents(pWeaponWorldModel);
#endif
				pWeaponWorldModel->SetSequence(0);
				pWeaponWorldModel->m_flPlaybackRate() = 0;

				if (pWeaponWorldModel->m_flCycle() != 0.f) {
					pWeaponWorldModel->m_flCycle() = 0.f;
					pWeaponWorldModel->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
				}
			}
		}
	}
}

void CCSGOPlayerAnimState::SetUpMovement()
{
	if (flWalkToRunTransition > 0 && flWalkToRunTransition < 1)
	{
		if (bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
			flWalkToRunTransition += flLastUpdateIncrement * CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED;
		else flWalkToRunTransition -= flLastUpdateIncrement * CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED;

		flWalkToRunTransition = Math::clampf(flWalkToRunTransition, 0, 1);
	}

	if (flVelocityLengthXY > (CS_PLAYER_SPEED_RUN * CS_PLAYER_SPEED_WALK_MODIFIER) && bWalkToRunTransitionState == ANIM_TRANSITION_RUN_TO_WALK)
	{
		bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;
		flWalkToRunTransition = std::max(0.01f, flWalkToRunTransition);
	}
	else if (flVelocityLengthXY < (CS_PLAYER_SPEED_RUN * CS_PLAYER_SPEED_WALK_MODIFIER) && bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
	{
		bWalkToRunTransitionState = ANIM_TRANSITION_RUN_TO_WALK;
		flWalkToRunTransition = std::min(0.99f, flWalkToRunTransition);
	}

	if (nAnimstateModelVersion < 2)
		tPoseParamMappings[PLAYER_POSE_PARAM_RUN].SetValue(pPlayer, flWalkToRunTransition);
	else
	{
		tPoseParamMappings[PLAYER_POSE_PARAM_MOVE_BLEND_WALK].SetValue(pPlayer, (1.0f - flWalkToRunTransition) * (1.0f - flAnimDuckAmount));
		tPoseParamMappings[PLAYER_POSE_PARAM_MOVE_BLEND_RUN].SetValue(pPlayer, (flWalkToRunTransition) * (1.0f - flAnimDuckAmount));
		tPoseParamMappings[PLAYER_POSE_PARAM_MOVE_BLEND_CROUCH_WALK].SetValue(pPlayer, flAnimDuckAmount);
	}

	char szWeaponMoveSeq[MAX_ANIMSTATE_ANIMNAME_CHARS];
	sprintf_s(szWeaponMoveSeq, "move_%s", GetWeaponPrefix());

	int nWeaponMoveSeq = pPlayer->LookupSequence(szWeaponMoveSeq);
	if (nWeaponMoveSeq == -1)
		nWeaponMoveSeq = pPlayer->LookupSequence("move");

	if (pPlayer->m_iMoveState() != nPreviousMoveState)
		flStutterStep += 10;
	nPreviousMoveState = pPlayer->m_iMoveState();

	flStutterStep = Math::clampf(Math::Approach(0.f, flStutterStep, flLastUpdateIncrement * 40.f), 0.f, 100.f);

	float flTargetMoveWeight = Math::Lerp(flAnimDuckAmount, Math::clampf(flSpeedAsPortionOfWalkTopSpeed, 0.f, 1.f), Math::clampf(flSpeedAsPortionOfCrouchTopSpeed, 0.f, 1.f));

	if (flMoveWeight <= flTargetMoveWeight)
		flMoveWeight = flTargetMoveWeight;
	else flMoveWeight = Math::Approach(flTargetMoveWeight, flMoveWeight, flLastUpdateIncrement * Math::RemapValClamped(flStutterStep, 0.0f, 100.0f, 2, 20));

	Vector vecMoveYawDir;
	Math::AngleVectors(QAngle(0, Math::AngleNormalize(flFootYaw + flMoveYaw + 180), 0), &vecMoveYawDir);
	float flYawDeltaAbsDot = abs(Math::DotProduct(vecVelocityNormalizedNonZero, vecMoveYawDir));
	flMoveWeight *= Math::Bias(flYawDeltaAbsDot, 0.2f);

	float flMoveWeightWithAirSmooth = flMoveWeight * flInAirSmoothValue;
	flMoveWeightWithAirSmooth *= std::max((1.0f - GetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB)), 0.55f);
	Math::clamp(flMoveWeightWithAirSmooth, 0.f, 1.f);

	float flMoveCycleRate = 0.f;
	if (flVelocityLengthXY > 0.f) {
		flMoveCycleRate = pPlayer->GetSequenceCycleRate(pPlayer->GetModelPtr(), nWeaponMoveSeq);
		float flSequenceGroundSpeed = std::max(pPlayer->GetSequenceMoveDist(pPlayer->GetModelPtr(), nWeaponMoveSeq) / (1.0f / flMoveCycleRate), 0.001f);
		flMoveCycleRate *= flVelocityLengthXY / flSequenceGroundSpeed;
		flMoveCycleRate *= Math::Lerp(flWalkToRunTransition, 1.0f, CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER);
	}

	float flLocalCycleIncrement = (flMoveCycleRate * flLastUpdateIncrement);
	flPrimaryCycle = Math::ClampCycle(flPrimaryCycle + flLocalCycleIncrement);

	UpdateAnimLayer(ANIMATION_LAYER_MOVEMENT_MOVE, nWeaponMoveSeq, flLocalCycleIncrement, flMoveWeightWithAirSmooth, flPrimaryCycle);

#ifndef CLIENT_DLL
	//bool moveRight = pPlayer->m_nButtons() & IN_MOVERIGHT;
	//bool moveLeft = pPlayer->m_nButtons() & IN_MOVELEFT;
	//bool moveForward = pPlayer->m_nButtons() & IN_FORWARD;
	//bool moveBackward = pPlayer->m_nButtons() & IN_BACK;

	//Vector vecForward;
	//Vector vecRight;
	//Math::AngleVectors(QAngle(0.f, flFootYaw, 0.f), &vecForward, &vecRight, NULL);
	//vecRight.NormalizeInPlace();

	//float flVelToRightDot = Math::DotProduct(vecVelocityNormalizedNonZero, vecRight);
	//float flVelToForwardDot = Math::DotProduct(vecVelocityNormalizedNonZero, vecForward);

	//bool bStrafeRight = (flSpeedAsPortionOfWalkTopSpeed >= 0.73f && moveRight && !moveLeft && flVelToRightDot < -0.63f);
	//bool bStrafeLeft = (flSpeedAsPortionOfWalkTopSpeed >= 0.73f && moveLeft && !moveRight && flVelToRightDot > 0.63f);
	//bool bStrafeForward = (flSpeedAsPortionOfWalkTopSpeed >= 0.65f && moveForward && !moveBackward && flVelToForwardDot < -0.55f);
	//bool bStrafeBackward = (flSpeedAsPortionOfWalkTopSpeed >= 0.65f && moveBackward && !moveForward && flVelToForwardDot > 0.55f);

	//pPlayer->m_bStrafing() = (bStrafeRight || bStrafeLeft || bStrafeForward || bStrafeBackward);
#endif
	if (pPlayer->m_bStrafing()) {
		if (!bStrafeChanging)
			flDurationStrafing = 0.f;

		bStrafeChanging = true;

		flStrafeChangeWeight = Math::Approach(1.f, flStrafeChangeWeight, flLastUpdateIncrement * 20.f);
		flStrafeChangeCycle = Math::Approach(0.f, flStrafeChangeCycle, flLastUpdateIncrement * 10.f);

		tPoseParamMappings[PLAYER_POSE_PARAM_STRAFE_DIR].SetValue(pPlayer, Math::AngleNormalize(flMoveYaw));
	}
	else if (flStrafeChangeWeight > 0.f)
	{
		flDurationStrafing += flLastUpdateIncrement;

		if (flDurationStrafing > 0.08f)
			flStrafeChangeWeight = Math::Approach(0, flStrafeChangeWeight, flLastUpdateIncrement * 5.f);

		nStrafeSequence = pPlayer->LookupSequence("strafe");
		flStrafeChangeCycle = Math::clampf(flStrafeChangeCycle + flLastUpdateIncrement * pPlayer->GetSequenceCycleRate(pPlayer->GetModelPtr(), nStrafeSequence), 0, 1);
	}

	if (flStrafeChangeWeight <= 0.f)
		bStrafeChanging = false;

	bool bPreviousGroundState = bOnGround;
	bOnGround = (pPlayer->m_fFlags() & FL_ONGROUND);

	bLandedOnGroundThisFrame = (!bFirstRunSinceInit && bPreviousGroundState != bOnGround && bOnGround);
	bLeftTheGroundThisFrame = (bPreviousGroundState != bOnGround && !bOnGround);

	float flDistanceFell = 0.f;
	if (bLeftTheGroundThisFrame)
		flLeftGroundHeight = vecPositionCurrent.z;

	if (bLandedOnGroundThisFrame)
	{
		flDistanceFell = abs(flLeftGroundHeight - vecPositionCurrent.z);
		float flDistanceFallNormalizedBiasRange = Math::Bias(Math::RemapValClamped(flDistanceFell, 12.0f, 72.0f, 0.0f, 1.0f), 0.4f);

		flLandAnimMultiplier = Math::clampf(Math::Bias(flDurationInAir, 0.3f), 0.1f, 1.0f);
		flDuckAdditional = std::max(flLandAnimMultiplier, flDistanceFallNormalizedBiasRange);
	}
	else flDuckAdditional = Math::Approach(0, flDuckAdditional, flLastUpdateIncrement * 2);

	flInAirSmoothValue = Math::Approach(bOnGround ? 1.f : 0.f, flInAirSmoothValue, Math::Lerp(flAnimDuckAmount, CSGO_ANIM_ONGROUND_FUZZY_APPROACH, CSGO_ANIM_ONGROUND_FUZZY_APPROACH_CROUCH) * flLastUpdateIncrement);
	Math::clamp(flInAirSmoothValue, 0.f, 1.f);

	flStrafeChangeWeight *= (1.0f - flAnimDuckAmount);
	flStrafeChangeWeight *= flInAirSmoothValue;
	flStrafeChangeWeight = Math::clampf(flStrafeChangeWeight, 0.f, 1.f);

	if (nStrafeSequence != -1.f)
		UpdateAnimLayer(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE, nStrafeSequence, 0, flStrafeChangeWeight, flStrafeChangeCycle);

	bool bPreviouslyOnLadder = bOnLadder;

	bOnLadder = !bOnGround && pPlayer->m_MoveType() == MOVETYPE_LADDER;

	bool bStartedLadderingThisFrame = (!bPreviouslyOnLadder && bOnLadder);
	bool bStoppedLadderingThisFrame = (bPreviouslyOnLadder && !bOnLadder);

	if (flLadderWeight > 0 || bOnLadder) {
#ifndef CLIENT_DLL
		if (bStartedLadderingThisFrame)
			SetLayerSequence(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, SelectSequenceFromActMods(ACT_CSGO_CLIMB_LADDER));
#endif

		if (abs(flVelocityLengthZ) > 100.f)
			flLadderSpeed = Math::Approach(1, flLadderSpeed, flLastUpdateIncrement * 10.0f);
		else flLadderSpeed = Math::Approach(0, flLadderSpeed, flLastUpdateIncrement * 10.0f);

		Math::clamp(flLadderSpeed, 0.f, 1.f);

		if (bOnLadder)
			flLadderWeight = Math::Approach(1, flLadderWeight, flLastUpdateIncrement * 5.0f);
		else flLadderWeight = Math::Approach(0, flLadderWeight, flLastUpdateIncrement * 10.0f);

		Math::clamp(flLadderWeight, 0.f, 1.f);

		QAngle angLadder; Math::VectorAngles(pPlayer->m_vecLadderNormal(), angLadder);

		tPoseParamMappings[PLAYER_POSE_PARAM_LADDER_YAW].SetValue(pPlayer, Math::AngleDiff(angLadder.y, flFootYaw));
		tPoseParamMappings[PLAYER_POSE_PARAM_LADDER_SPEED].SetValue(pPlayer, flLadderSpeed);

		if (GetLayerActivity(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB) == ACT_CSGO_CLIMB_LADDER)
			SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, flLadderWeight);

		SetLayerCycle(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, GetLayerCycle(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB) + (vecPositionCurrent.z - vecPositionLast.z) * Math::Lerp(flLadderSpeed, 0.010f, 0.004f));

		if (bOnLadder)
		{
			float flIdealJumpWeight = 1.0f - flLadderWeight;
			if (GetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL) > flIdealJumpWeight)
				SetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, flIdealJumpWeight);
		}
	}
	else flLadderSpeed = 0.f;

	if (bOnGround) {
		if (!bLanding && (bLandedOnGroundThisFrame || bStoppedLadderingThisFrame)) 
		{
#ifndef CLIENT_DLL
			SetLayerSequence(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, SelectSequenceFromActMods(flDurationInAir > 1.f ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT));
#endif
			SetLayerCycle(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0.f);
			bLanding = true;
		}

		flDurationInAir = 0.f;

		if (bLanding && GetLayerActivity(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB) != ACT_CSGO_CLIMB_LADDER) 
		{
#ifndef CLIENT_DLL			
			bJumping = false;
#endif
			IncrementLayerCycle(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, false);
			IncrementLayerCycle(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, false);

			tPoseParamMappings[PLAYER_POSE_PARAM_JUMP_FALL].SetValue(pPlayer, 0.f);

			if (IsLayerSequenceCompleted(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB))
			{
				bLanding = false;
				SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0.f);
				SetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, 0.f);
				flLandAnimMultiplier = 1.0f;
			}
			else
			{
				float flLandWeight = GetLayerIdealWeightFromSeqCycle(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB) * flLandAnimMultiplier;
				flLandWeight *= Math::clampf((1.0f - flAnimDuckAmount), 0.2f, 1.0f);

				SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, flLandWeight);

				float flCurrentJumpFallWeight = GetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);
				if (flCurrentJumpFallWeight > 0.f) {
					flCurrentJumpFallWeight = Math::Approach(0, flCurrentJumpFallWeight, flLastUpdateIncrement * 10.0f);
					SetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, flCurrentJumpFallWeight);
				}

			}
		}

#ifndef CLIENT_DLL
		if (!bLanding && !bJumping && flLadderWeight <= 0)
			SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0);
#endif
	}
	else if (!bOnLadder) {
		bLanding = false;

		if (bLeftTheGroundThisFrame || bStoppedLadderingThisFrame) {

#ifndef CLIENT_DLL
			// TODO@Monster: WHY THIS RETARD IS NOT WORKING.
			if (!bJumping) 
			{
				SetLayerSequence(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, SelectSequenceFromActMods(ACT_CSGO_FALL));
			}
#endif

			flDurationInAir = 0.f;
		}

		flDurationInAir += flLastUpdateIncrement;

		IncrementLayerCycle(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, false);

		float flJumpWeight = GetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);
		float flNextJumpWeight = GetLayerIdealWeightFromSeqCycle(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);
		if (flNextJumpWeight > flJumpWeight)
			SetLayerWeight(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, flNextJumpWeight);

		float flLingeringLandWeight = GetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB);
		if (flLingeringLandWeight > 0.f) {
			flLingeringLandWeight *= Math::smoothstep_bounds(0.2f, 0.0f, flDurationInAir);
			SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, flLingeringLandWeight);
		}

		tPoseParamMappings[PLAYER_POSE_PARAM_JUMP_FALL].SetValue(pPlayer, Math::clampf(Math::smoothstep_bounds(0.72f, 1.52f, flDurationInAir), 0, 1));
	}
}

void CCSGOPlayerAnimState::SetUpAliveloop()
{
	animstate_layer_t nLayer = ANIMATION_LAYER_ALIVELOOP;

	//if (pPlayer->teammate(g_Client.m_LocalPlayer)) {
	//	IncrementLayerCycle(nLayer, true);
	//	return;
	//}

#ifndef CLIENT_DLL
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayer];
	if (!pLayer)
		return;

	if (GetLayerActivity(nLayer) != ACT_CSGO_ALIVE_LOOP)
	{
		SetLayerSequence(nLayer, SelectSequenceFromActMods(ACT_CSGO_ALIVE_LOOP));
		SetLayerCycle(nLayer, g_Csgo.RandomFloat(0.f, 1.f));
		SetLayerRate(nLayer, pPlayer->GetSequenceCycleRate(pPlayer->m_studioHdr(), pLayer->GetSequence()) * g_Csgo.RandomFloat(0.8f, 1.1f));
	}
	else
	{
		if (pWeapon && pWeaponLast != pWeapon)
		{
			SetLayerSequence(nLayer, SelectSequenceFromActMods(ACT_CSGO_ALIVE_LOOP));
			SetLayerCycle(nLayer, GetLayerCycle(nLayer));
		}
		else if (IsLayerSequenceCompleted(nLayer))
			SetLayerRate(nLayer, pPlayer->GetSequenceCycleRate(pPlayer->m_studioHdr(), pLayer->GetSequence()) * g_Csgo.RandomFloat(0.8f, 1.1f));
		else
			SetLayerWeight(nLayer, Math::RemapValClamped(flSpeedAsPortionOfRunTopSpeed, 0.55f, 0.9f, 1.0f, 0.f));
	}
#endif

	IncrementLayerCycle(nLayer, true);
}

void CCSGOPlayerAnimState::SetUpWholeBodyAction()
{
	animstate_layer_t nLayer = ANIMATION_LAYER_WHOLE_BODY;

	//if (pPlayer->teammate(g_Client.m_LocalPlayer)) {
	//	if (GetLayerWeight(nLayer) > 0)
	//	{
	//		IncrementLayerCycle(nLayer, false);
	//		IncrementLayerWeight(nLayer);
	//	}

	//	return;
	//}

#ifndef CLIENT_DLL
	if (pPlayer->m_iTeamNum() == TEAM_CT && pPlayer->m_bIsDefusing())
	{
		if (!bDefuseStarted)
		{
			SetLayerSequence(nLayer, SelectSequenceFromActMods(pPlayer->m_bHasDefuser() ? ACT_CSGO_DEFUSE_WITH_KIT : ACT_CSGO_DEFUSE));
			bDefuseStarted = true;
		}
		else
			IncrementLayerCycleWeightRateGeneric(nLayer);
	}
	else if (GetLayerActivity(nLayer) == ACT_CSGO_DEFUSE || GetLayerActivity(nLayer) == ACT_CSGO_DEFUSE_WITH_KIT)
	{
		if (GetLayerWeight(nLayer) > 0)
		{
			float flCurrentWeight = GetLayerWeight(nLayer);
			SetLayerWeight(nLayer, Math::Approach(0, flCurrentWeight, flLastUpdateIncrement * CSGO_ANIM_DEFUSE_ABORT_SPEED));
			SetLayerWeightRate(nLayer, flCurrentWeight);
		}

		bDefuseStarted = false;
	}
	else if (GetLayerActivity(nLayer) == ACT_CSGO_PLANT_BOMB)
	{
		if (pWeapon && pWeapon->m_iItemDefinitionIndex() != C4)
			bPlantAnimStarted = false;

		if (bPlantAnimStarted)
		{
			float flWeightPrevious = GetLayerWeight(nLayer);
			IncrementLayerCycle(nLayer, false);
			SetLayerWeight(nLayer, Math::Approach(GetLayerIdealWeightFromSeqCycle(nLayer), flWeightPrevious, flLastUpdateIncrement * CSGO_ANIM_BOMBPLANT_BLEND_RATE));
			SetLayerWeightRate(nLayer, flWeightPrevious);

			bPlantAnimStarted = !(IsLayerSequenceCompleted(nLayer));
		}
		else
		{
			if (GetLayerWeight(nLayer) > 0) {
				float flCurrentWeight = GetLayerWeight(nLayer);
				SetLayerWeight(nLayer, Math::Approach(0, flCurrentWeight, flLastUpdateIncrement * CSGO_ANIM_BOMBPLANT_ABORT_SPEED));
				SetLayerWeightRate(nLayer, flCurrentWeight);
			}

			bPlantAnimStarted = false;
		}
	}
	else
	{
		SetLayerCycle(nLayer, 0.999f);
		SetLayerWeight(nLayer, 0);
		SetLayerWeightRate(nLayer, 0);
	}
#else
	if (GetLayerWeight(nLayer) > 0)
	{
		IncrementLayerCycle(nLayer, false);
		IncrementLayerWeight(nLayer);
	}
#endif
}

void CCSGOPlayerAnimState::SetUpFlashedReaction()
{
	animstate_layer_t nLayer = ANIMATION_LAYER_FLASHED;

	//if (pPlayer->teammate(g_Client.m_LocalPlayer)) {
	//	if (GetLayerWeight(nLayer) > 0)
	//	{
	//		C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayer];
	//		if (pLayer && pLayer->GetWeightDeltaRate() < 0)
	//			IncrementLayerWeight(nLayer);
	//	}
	//	return;
	//}

#ifndef CLIENT_DLL
	if (flFlashedAmountEaseOutEnd < g_Csgo.m_Globals->m_Curtime)
	{
		SetLayerWeight(nLayer, 0);
		bFlashed = false;
	}
	else
	{
		if (!bFlashed)
		{
			SetLayerSequence(nLayer, SelectSequenceFromActMods(ACT_CSGO_FLASHBANG_REACTION));
			bFlashed = true;
		}

		float flFlashedAmount = Math::RemapValClamped(g_Csgo.m_Globals->m_Curtime, flFlashedAmountEaseOutStart, flFlashedAmountEaseOutEnd, 1.f, 0.f);

		SetLayerCycle(nLayer, 0);
		SetLayerRate(nLayer, 0);

		float flWeightPrevious = GetLayerWeight(nLayer);
		float flWeightNew = Math::SimpleSpline(flFlashedAmount);

		SetLayerWeight(nLayer, flWeightNew);
		SetLayerWeightRate(nLayer, (flWeightNew >= flWeightPrevious) ? 0 : flWeightPrevious);
	}
#else
	if (GetLayerWeight(nLayer) > 0)
	{
		C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayer];
		if (pLayer && pLayer->GetWeightDeltaRate() < 0)
			IncrementLayerWeight(nLayer);
	}
#endif
}

void CCSGOPlayerAnimState::SetUpFlinch()
{
	animstate_layer_t nLayer = ANIMATION_LAYER_FLINCH;

	//if (pPlayer->teammate(g_Client.m_LocalPlayer)) {
	//	IncrementLayerCycle(nLayer, false);
	//	return;
	//}

#ifndef CLIENT_DLL
	if (flTimeOfLastKnownInjury < pPlayer->m_flTimeOfLastInjury())
	{
		flTimeOfLastKnownInjury = pPlayer->m_flTimeOfLastInjury();

		bool bNoFlinchIsPlaying = (IsLayerSequenceCompleted(nLayer) || GetLayerWeight(nLayer) <= 0);
		bool bHeadshotIsPlaying = (!bNoFlinchIsPlaying && GetLayerActivity(nLayer) == ACT_CSGO_FLINCH_HEAD);

		if (pPlayer->m_fireCount() & DMG_BURN)
		{
			if (bNoFlinchIsPlaying)
			{
				SetLayerSequence(nLayer, SelectSequenceFromActMods(ACT_CSGO_FLINCH_MOLOTOV));
				UpdateActivityModifiers();
			}
		}
		else if (bNoFlinchIsPlaying || !bHeadshotIsPlaying || pPlayer->m_LastHitGroup() == HEAD)
		{
			RelativeDamagedDirection_t damageDir = pPlayer->m_nRelativeDirectionOfLastInjury();
			bool bLeft = false;
			bool bRight = false;
			switch (damageDir) {
			case DAMAGED_DIR_NONE:
			case DAMAGED_DIR_FRONT: {
				AddActivityModifier("front");
				break;
			}
			case DAMAGED_DIR_BACK: {
				AddActivityModifier("back");
				break;
			}
			case DAMAGED_DIR_LEFT: {
				AddActivityModifier("left");
				bLeft = true;
				break;
			}
			case DAMAGED_DIR_RIGHT: {
				AddActivityModifier("right");
				bRight = true;
				break;
			}
			}
			switch (pPlayer->m_LastHitGroup()) {
			case static_cast<int>(Hitgroups_t::HEAD): {
				AddActivityModifier("head");
				break;
			}
			case static_cast<int>(Hitgroups_t::CHEST): {
				AddActivityModifier("chest");
				break;
			}
			case static_cast<int>(Hitgroups_t::LEFTARM): {
				if (!bLeft) { AddActivityModifier("left"); }
				AddActivityModifier("arm");
				break;
			}
			case static_cast<int>(Hitgroups_t::RIGHTARM): {
				if (!bRight) { AddActivityModifier("right"); }
				AddActivityModifier("arm");
				break;
			}
			case static_cast<int>(Hitgroups_t::GENERIC):
			case static_cast<int>(Hitgroups_t::STOMACH): {
				AddActivityModifier("gut");
				break;
			}
			case static_cast<int>(Hitgroups_t::LEFTLEG): {
				if (!bLeft) { AddActivityModifier("left"); }
				AddActivityModifier("leg");
				break;
			}
			case static_cast<int>(Hitgroups_t::RIGHTLEG): {
				if (!bRight) { AddActivityModifier("right"); }
				AddActivityModifier("leg");
				break;
			}
			}

			SetLayerSequence(nLayer, SelectSequenceFromActMods(pPlayer->m_LastHitGroup() == HEAD ? ACT_CSGO_FLINCH_HEAD : ACT_CSGO_FLINCH));
			UpdateActivityModifiers();
		}
	}

	if (GetLayerSequence(nLayer) > 0)
		SetLayerWeight(nLayer, GetLayerIdealWeightFromSeqCycle(nLayer));
	else
		SetLayerWeight(nLayer, 0);
#endif

	IncrementLayerCycle(nLayer, false);
}

void CCSGOPlayerAnimState::SetUpLean()
{
	float m_Interval = g_Csgo.m_Globals->m_Curtime - flLastVelocityTestTime;
	if (m_Interval > 0.025f) {
		m_Interval = std::min(m_Interval, 0.1f);
		flLastVelocityTestTime = g_Csgo.m_Globals->m_Curtime;

		vecTargetAcceleration = (pPlayer->m_vecVelocity() - vecVelocityLast) / m_Interval;
		vecTargetAcceleration.z = 0;

		vecVelocityLast = pPlayer->m_vecVelocity();
	}

	vecAcceleration = Math::Approach(vecTargetAcceleration, vecAcceleration, flLastUpdateIncrement * 800.0f);

	QAngle temp;
	Math::VectorAngles(vecAcceleration, Vector(0, 0, 1), temp);

	flAccelerationWeight = Math::clampf((vecAcceleration.Length() / CS_PLAYER_SPEED_RUN) * flSpeedAsPortionOfRunTopSpeed, 0, 1);
	flAccelerationWeight *= (1.0f - flLadderWeight);

	tPoseParamMappings[PLAYER_POSE_PARAM_LEAN_YAW].SetValue(pPlayer, Math::AngleNormalize(flFootYaw - temp[YAW]));

	if (GetLayerSequence(ANIMATION_LAYER_LEAN) <= 0)
		SetLayerSequence(ANIMATION_LAYER_LEAN, pPlayer->LookupSequence("lean"));

	SetLayerWeight(ANIMATION_LAYER_LEAN, flAccelerationWeight);
}

int CCSGOPlayerAnimState::GetLayerSequence(animstate_layer_t nLayerIndex)
{
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		return pLayer->GetSequence();
	return 0;
}

//inline bool CCSGOPlayerAnimState::LayerToIndex(const C_AnimationLayer* pLayer, int& nIndex)
//{
//	return false;
//}

//// todo: find the sig.
//CUtlSymbolTable* g_ActivityModifiersTable = nullptr;

void CCSGOPlayerAnimState::AddActivityModifier(const char* szName)
{
	//if (szName == NULL)
	//{
	//	Assert(0);
	//	return;
	//}

	//char szLookup[32];
	//std::strcpy(szLookup, szName);

	//CUtlSymbol sym = g_ActivityModifiersTable->Find(szLookup);
	//if (!sym.IsValid())
	//{
	//	sym = g_ActivityModifiersTable->AddString(szLookup);
	//}

	//ActivityModifiers.AddToTail(sym);
}

void CCSGOPlayerAnimState::UpdateActivityModifiers(void)
{
	//ActivityModifiers.Purge();

	//AddActivityModifier(GetWeaponPrefix());

	//if (flSpeedAsPortionOfWalkTopSpeed > 0.25f)
	//{
	//	AddActivityModifier("moving");
	//}

	//if (flAnimDuckAmount > 0.55f)
	//{
	//	AddActivityModifier("crouch");
	//}
}

int CCSGOPlayerAnimState::SelectSequenceFromActMods(Activity iAct)
{
	int nSequence = 0;
	bool bisCrouching = flAnimDuckAmount > 0.55f;
	float flRunningSpeed = flSpeedAsPortionOfRunTopSpeed;
	bool bMoving = flRunningSpeed > 0.25f;
	bool bChangingWeapon = pWeaponLast != pWeapon;

	switch (iAct) {
	case ACT_CSGO_JUMP:
		nSequence = bMoving + 17;
		if (!bisCrouching)
			nSequence = bMoving + 15;
		break;
	case ACT_CSGO_ALIVE_LOOP:
		nSequence = 8;

		if (bChangingWeapon)
			nSequence = 9;
		break;
	case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		nSequence = 6;
		break;
	case ACT_CSGO_FALL:
		nSequence = 14;
		break;
	case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		nSequence = 4;
		break;
	case ACT_CSGO_CLIMB_LADDER:
		nSequence = 13;
		break;
	case ACT_CSGO_LAND_LIGHT:
		nSequence = 20;
		if (bMoving)
			nSequence = 22;

		if (bisCrouching)
		{
			nSequence = 21;
			if (bMoving)
				nSequence = 19;
		}
		break;
	case ACT_CSGO_LAND_HEAVY:
		nSequence = 23;
		if (bisCrouching)
			nSequence = 24;
		break;
	}

	return nSequence;

	// todo: find sig.
	//return pPlayer->SelectWeightedSequenceFromModifiers(iAct, ActivityModifiers.Base(), ActivityModifiers.Count());
}

Activity CCSGOPlayerAnimState::GetLayerActivity(animstate_layer_t nLayerIndex)
{
	//MDLCACHE_CRITICAL_SECTION();
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		return pPlayer->GetSequenceActivity(pLayer->GetSequence());
	return ACT_INVALID;
}

float CCSGOPlayerAnimState::GetLayerWeight(animstate_layer_t nLayerIndex)
{
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		return pLayer->GetWeight();
	return 0.f;
}

float CCSGOPlayerAnimState::GetLayerCycle(animstate_layer_t nLayerIndex)
{
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		return pLayer->GetCycle();
	return 0.f;
}

float CCSGOPlayerAnimState::GetLayerIdealWeightFromSeqCycle(animstate_layer_t nLayerIndex)
{
	//MDLCACHE_CRITICAL_SECTION();
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (!pLayer)
		return 0.f;

	mstudioseqdesc_t& seqdesc = pPlayer->GetModelPtr()->pSeqdesc(pLayer->GetSequence());

	float flCycle = pLayer->GetCycle();
	if (flCycle >= 0.999f)
		flCycle = 1;
	float flEaseIn = seqdesc.fadeintime;
	float flEaseOut = seqdesc.fadeouttime;
	float flIdealWeight = 1;

	if (flEaseIn > 0 && flCycle < flEaseIn)
	{
		flIdealWeight = Math::smoothstep_bounds(0, flEaseIn, flCycle);
	}
	else if (flEaseOut < 1 && flCycle > flEaseOut)
	{
		flIdealWeight = Math::smoothstep_bounds(1.0f, flEaseOut, flCycle);
	}

	if (flIdealWeight < 0.0015f)
		return 0;

	return Math::clampf(flIdealWeight, 0.f, 1.f);
}

void CCSGOPlayerAnimState::IncrementLayerCycleWeightRateGeneric(animstate_layer_t nLayerIndex)
{
	float flWeightPrevious = GetLayerWeight(nLayerIndex);
	IncrementLayerCycle(nLayerIndex, false);
	SetLayerWeight(nLayerIndex, GetLayerIdealWeightFromSeqCycle(nLayerIndex));
	SetLayerWeightRate(nLayerIndex, flWeightPrevious);
}

void CCSGOPlayerAnimState::IncrementLayerWeight(animstate_layer_t nLayerIndex)
{
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];

	if (!pLayer)
		return;

	if (abs(pLayer->GetWeightDeltaRate()) <= 0.f)
		return;

	float flCurrentWeight = pLayer->GetWeight();
	flCurrentWeight += flLastUpdateIncrement * pLayer->GetWeightDeltaRate();
	Math::clamp(flCurrentWeight, 0.f, 1.f);
	pLayer->SetWeight(flCurrentWeight);
}

void CCSGOPlayerAnimState::SetLayerRate(animstate_layer_t nLayerIndex, float flRate)
{
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		pLayer->SetPlaybackRate(flRate);
}

void CCSGOPlayerAnimState::SetLayerWeight(animstate_layer_t nLayerIndex, float flWeight)
{
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (!pLayer)
		return;
	pLayer->SetWeight(Math::clampf(flWeight, 0, 1));
#ifndef CLIENT_DLL
	//pLayer->fFlags |= ANIM_LAYER_ACTIVE;
#endif
}

void CCSGOPlayerAnimState::SetLayerWeightRate(animstate_layer_t nLayerIndex, float flPrevious)
{
	if (flLastUpdateIncrement == 0)
		return;

	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (!pLayer)
		return;

	float flNewRate = (pLayer->GetWeight() - flPrevious) / flLastUpdateIncrement;
	pLayer->SetWeightDeltaRate(flNewRate);
}

void CCSGOPlayerAnimState::SetLayerCycle(animstate_layer_t nLayerIndex, float flCycle)
{
	C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		pLayer->SetCycle(Math::clamp_cycle(flCycle));
}

void CCSGOPlayerAnimState::ApplyLayerOrderPreset(animlayerpreset nNewPreset, bool bForce)
{
	if (!bForce && pLayerOrderPreset == nNewPreset)
		return;

	pLayerOrderPreset = nNewPreset;

	for (int i = 0; i < ANIMATION_LAYER_COUNT; i++)
	{
		C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[pLayerOrderPreset[i]];
		if (pLayer)
		{
			pLayer->SetOrder(i);
			pLayer->pDispatchedStudioHdr = NULL;
			pLayer->nDispatchedSrc = ACT_INVALID;
			pLayer->nDispatchedDst = ACT_INVALID;
		}
	}
}

void CCSGOPlayerAnimState::UpdateAnimLayer(animstate_layer_t nLayerIndex, int nSequence, float flPlaybackRate, float flWeight, float flCycle)
{
	if (nSequence > 1) {
		C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
		if (!pLayer)
			return;

		pLayer->SetSequence(nSequence);
		pLayer->SetPlaybackRate((flPlaybackRate));
		pLayer->SetCycle((Math::clampf(flCycle, 0.f, 1.f)));
		pLayer->SetWeight((Math::clampf(flWeight, 0.f, 1.f)));
		UpdateLayerOrderPreset(nLayerIndex, nSequence);
#ifndef CLIENT_DLL
		//pLayer->fFlags |= ANIM_LAYER_ACTIVE;
#endif
	}
}

void CCSGOPlayerAnimState::UpdateLayerOrderPreset(animstate_layer_t nLayerIndex, int nSequence)
{
	if (!pPlayer || nLayerIndex != ANIMATION_LAYER_WEAPON_ACTION)
		return;

	if (pPlayer->GetAnySequenceAnimTag(nSequence, ANIMTAG_WEAPON_POSTLAYER, 0) != 0)
		ApplyLayerOrderPreset(get_animlayerpreset(WeaponPost));
	else ApplyLayerOrderPreset(get_animlayerpreset(Default));
}

void CCSGOPlayerAnimState::SetLayerSequence(animstate_layer_t nLayerIndex, int nSequence)
{
	if (nSequence > 1) {
		C_AnimationLayer* pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
		if (pLayer) {
			pLayer->SetCycle(0.f);
			pLayer->SetWeight(0.f);
			pLayer->SetSequence(nSequence);
			pLayer->SetPlaybackRate(pPlayer->GetLayerSequenceCycleRate(pLayer, nSequence));
			UpdateLayerOrderPreset(nLayerIndex, nSequence);
#ifndef CLIENT_DLL
			//pLayer->fFlags |= ANIM_LAYER_ACTIVE;
#endif
		}
	}
}

void CCSGOPlayerAnimState::DoAnimationEvent(PlayerAnimEvent_t animEvent)
{
	UpdateActivityModifiers();

	switch (animEvent) {
	case PLAYERANIMEVENT_THROW_GRENADE_UNDERHAND:
	{
		AddActivityModifier("underhand");
	}
	case PLAYERANIMEVENT_FIRE_GUN_PRIMARY:
	case PLAYERANIMEVENT_THROW_GRENADE:
	{
		if (pWeapon && animEvent == PLAYERANIMEVENT_FIRE_GUN_PRIMARY && pWeapon->m_iItemDefinitionIndex() == C4)
		{
			SetLayerSequence(ANIMATION_LAYER_WHOLE_BODY, SelectSequenceFromActMods(ACT_CSGO_PLANT_BOMB));
			bPlantAnimStarted = true;
		}

		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_PRIMARY));
		break;
	}
	case PLAYERANIMEVENT_FIRE_GUN_PRIMARY_OPT:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_PRIMARY_OPT_1));
		break;
	}
	case PLAYERANIMEVENT_FIRE_GUN_PRIMARY_SPECIAL1:
	case PLAYERANIMEVENT_FIRE_GUN_PRIMARY_OPT_SPECIAL1:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_PRIMARY_OPT_2));
		break;
	}
	case PLAYERANIMEVENT_FIRE_GUN_SECONDARY:
	{
		if (pWeapon->GetWpnData()->m_WeaponType == WEAPONTYPE_SNIPER_RIFLE)
		{
			// hack: sniper rifles use primary fire anim when 'alt' firing, meaning scoped.
			SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_PRIMARY));
		}
		else
		{
			SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_SECONDARY));
		}
		break;
	}
	case PLAYERANIMEVENT_FIRE_GUN_SECONDARY_SPECIAL1:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_FIRE_SECONDARY_OPT_1));
		break;
	}
	case PLAYERANIMEVENT_GRENADE_PULL_PIN:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_OPERATE));
		break;
	}
	case PLAYERANIMEVENT_SILENCER_ATTACH:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_SILENCER_ATTACH));
		break;
	}
	case PLAYERANIMEVENT_SILENCER_DETACH:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_SILENCER_DETACH));
		break;
	}
	case PLAYERANIMEVENT_RELOAD:
	{
		if (pWeapon && pWeapon->GetWpnData()->m_WeaponType == WEAPONTYPE_SHOTGUN && pWeapon->m_iItemDefinitionIndex() != MAG7)
			break;

		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_RELOAD));
		break;
	}
	case PLAYERANIMEVENT_RELOAD_START:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_RELOAD_START));
		break;
	}
	case PLAYERANIMEVENT_RELOAD_LOOP:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_RELOAD_LOOP));
		break;
	}
	case PLAYERANIMEVENT_RELOAD_END:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_RELOAD_END));
		break;
	}
	case PLAYERANIMEVENT_CATCH_WEAPON:
	{
		SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_CATCH));
		break;
	}
	case PLAYERANIMEVENT_CLEAR_FIRING:
	{
		bPlantAnimStarted = false;
		break;
	}
	case PLAYERANIMEVENT_DEPLOY:
	{
		if (pWeapon && GetLayerActivity(ANIMATION_LAYER_WEAPON_ACTION) == ACT_CSGO_DEPLOY &&
			GetLayerCycle(ANIMATION_LAYER_WEAPON_ACTION) < CSGO_ANIM_DEPLOY_RATELIMIT)
		{
			bDeployRateLimiting = true;
		}
		else
		{
			SetLayerSequence(ANIMATION_LAYER_WEAPON_ACTION, SelectSequenceFromActMods(ACT_CSGO_DEPLOY));
		}
		break;
	}
	case PLAYERANIMEVENT_JUMP:
	{
		// TODO@Monster: FIX THIS.
		bJumping = true;
		SetLayerSequence(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL, SelectSequenceFromActMods(ACT_CSGO_JUMP));
		break;
	}
	default:
	{
		break;
	}
	}
}

void CCSGOPlayerAnimState::IncrementLayerCycle(animstate_layer_t nLayerIndex, bool bAllowLoop)
{
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (!pLayer)
		return;

	if (abs(pLayer->GetPlaybackRate()) <= 0)
		return;

	float flCurrentCycle = pLayer->GetCycle();
	flCurrentCycle += flLastUpdateIncrement * pLayer->GetPlaybackRate();

	if (!bAllowLoop && flCurrentCycle >= 1)
		flCurrentCycle = 0.999f;

	pLayer->SetCycle(Math::clamp_cycle(flCurrentCycle));
}

bool CCSGOPlayerAnimState::IsLayerSequenceCompleted(animstate_layer_t nLayerIndex)
{
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (pLayer)
		return (flLastUpdateIncrement * pLayer->GetPlaybackRate()) + pLayer->GetCycle() >= 1.0f;
	return false;
}

bool CCSGOPlayerAnimState::LayerSequenceHasActMod(animstate_layer_t nLayerIndex, const char* szActMod)
{
	auto pLayer = &pPlayer->m_AnimOverlay()[nLayerIndex];
	if (!pLayer)
		return false;

	mstudioseqdesc_t& seqdesc = pPlayer->GetModelPtr()->pSeqdesc(pLayer->GetSequence());
	for (int i = 0; i < seqdesc.numactivitymodifiers; i++) {
		if (!strcmp(seqdesc.pActivityModifier(i)->pszName(), szActMod))
			return true;
	}

	return false;
}

const char* CCSGOPlayerAnimState::GetWeaponPrefix(void)
{
	int nWeaponType = 0; // knife

	pWeapon = pPlayer->GetActiveWeapon();
	if (pWeapon && pWeapon->GetWpnData())
	{
		nWeaponType = (int)pWeapon->GetWpnData()->m_WeaponType;

		int nWeaponID = pWeapon->m_iItemDefinitionIndex();
		if (nWeaponID == MAG7)
		{
			nWeaponType = WEAPONTYPE_RIFLE;
		}
		else if (nWeaponID == ZEUS)
		{
			nWeaponType = WEAPONTYPE_PISTOL;
		}

		if (nWeaponType == WEAPONTYPE_STACKABLEITEM)
		{
			nWeaponType = WEAPONTYPE_GRENADE; // redirect healthshot, adrenaline, etc to the grenade archetype
		}
	}

	return g_szWeaponPrefixLookupTable[Math::clampf(nWeaponType, 0, 9)];
}
