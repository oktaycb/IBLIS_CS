#include "../../precompiled.h"

enum ADVANCED_ACTIVITY : int
{
	ACTIVITY_NONE = 0,
	ACTIVITY_JUMP,
	ACTIVITY_LAND
};

void AimPlayer::UpdateEnemiesAnimations()
{
	LagRecord* pRecord = m_Records.front().get();
	if (pRecord == nullptr)
		return;

	CCSGOPlayerAnimState* pAnimationState = m_Player->m_PlayerAnimStateCSGO();
	if (pAnimationState == nullptr)
		return;

	m_ResolverMode = AimPlayer::RESOLVER::LEGIT;

	Backup_t pBackup;

	pBackup.m_Origin = m_Player->m_vecOrigin();
	pBackup.m_AbsOrigin = m_Player->GetAbsOrigin();
	pBackup.m_Velocity = m_Player->m_vecVelocity();
	pBackup.m_AbsVelocity = m_Player->m_vecAbsVelocity();
	pBackup.m_Flags = m_Player->m_fFlags();
	pBackup.m_EFlags = m_Player->m_iEFlags();
	pBackup.m_DuckAmount = m_Player->m_flDuckAmount();
	pBackup.m_LowerbodyYaw = m_Player->m_flLowerBodyYawTarget();
	pBackup.m_SimulationTime = m_Player->m_flSimulationTime();
	m_Player->GetAnimLayers(pBackup.m_Layers);

	const float flCurtime = g_Csgo.m_Globals->m_Curtime;
	const float flRealtime = g_Csgo.m_Globals->m_RealTime;
	const float flFrametime = g_Csgo.m_Globals->m_FrameTime;
	const float flAbsFrametime = g_Csgo.m_Globals->m_AbsFrametime;
	const int iFramecount = g_Csgo.m_Globals->m_FrameCount;
	const int iTickcount = g_Csgo.m_Globals->m_TickCount;
	const float flInterpAmount = g_Csgo.m_Globals->m_InterpolationAmount;

	g_Csgo.m_Globals->m_Curtime = pRecord->m_SimulationTime;
	g_Csgo.m_Globals->m_RealTime = pRecord->m_SimulationTime;
	g_Csgo.m_Globals->m_FrameCount = game::TIME_TO_TICKS(pRecord->m_SimulationTime);
	g_Csgo.m_Globals->m_TickCount = game::TIME_TO_TICKS(pRecord->m_SimulationTime);
	g_Csgo.m_Globals->m_FrameTime = g_Csgo.m_Globals->m_Interval;
	g_Csgo.m_Globals->m_AbsFrametime = g_Csgo.m_Globals->m_Interval;
	g_Csgo.m_Globals->m_InterpolationAmount = 0.f;

	pRecord->m_Legit = game::IsFakePlayer(m_Player->GetClientNetworkable()->GetIndex());

	pRecord->m_Resolved = false;
	pRecord->m_FakeWalk = false;
	pRecord->m_Dormant = false;
	pRecord->m_Shot = false;

	if (pRecord->m_ChokedCommands > 1) {
		if (m_Player->m_fFlags() & FL_ONGROUND &&
			pRecord->m_Layers[12].GetWeight() > 0.0f &&
			pRecord->m_Layers[6].GetWeight() <= 0.f &&
			pRecord->m_Layers[6].GetPlaybackRate() <= 0.f)
			pRecord->m_FakeWalk = true;

		if (m_Records.size() >= 2) {
			LagRecord* pPrevious = m_Records[1].get();

			if (pPrevious && !pPrevious->m_Dormant) {
				int32_t iActivityTick = 0;
				int32_t iActivityType = 0;

				if (pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].GetWeight() > 0.0f && pPrevious->m_Layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].GetWeight() <= 0.0f)
				{
					int32_t iLandSequence = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].GetSequence();
					if (iLandSequence > 2)
					{
						int32_t iLandActivity = m_Player->GetSequenceActivity(iLandSequence);
						if (iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY)
						{
							float_t flCurrentCycle = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].GetCycle();
							float_t flCurrentRate = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].GetPlaybackRate();

							if (flCurrentCycle > 0.0f && flCurrentRate > 0.0f)
							{
								float_t flLandTime = (flCurrentCycle / flCurrentRate);
								if (flLandTime > 0.0f)
								{
									iActivityTick = game::TIME_TO_TICKS(pRecord->m_SimulationTime - flLandTime) + 1;
									iActivityType = ACTIVITY_LAND;
								}
							}
						}
					}
				}

				if (pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetCycle() > 0.0f && pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetPlaybackRate() > 0.0f)
				{
					int32_t iJumpSequence = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetSequence();
					if (iJumpSequence > 2)
					{
						int32_t iJumpActivity = m_Player->GetSequenceActivity(iJumpSequence);
						if (iJumpActivity == ACT_CSGO_JUMP)
						{
							float_t flCurrentCycle = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetCycle();
							float_t flCurrentRate = pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetPlaybackRate();

							if (flCurrentCycle > 0.0f && flCurrentRate > 0.0f)
							{
								float_t flJumpTime = (flCurrentCycle / flCurrentRate);
								if (flJumpTime > 0.0f)
								{
									iActivityTick = game::TIME_TO_TICKS(pRecord->m_SimulationTime - flJumpTime) + 1;
									iActivityType = ACTIVITY_JUMP;
								}
							}
						}
					}
				}

				int32_t iCurrentSimulationTick = game::TIME_TO_TICKS(pRecord->m_SimulationTime);
				if (iActivityType > ACTIVITY_NONE)
				{
					bool bIsOnGround = m_Player->m_fFlags() & FL_ONGROUND;
					if (iActivityType == ACTIVITY_JUMP)
					{
						if (iCurrentSimulationTick == iActivityTick - 1)
							bIsOnGround = true;
						else if (iCurrentSimulationTick == iActivityTick)
						{
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].SetCycle(0.0f);
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].SetSequence(pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetSequence());
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].SetPlaybackRate(pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetPlaybackRate());

							bIsOnGround = false;
						}

					}
					else if (iActivityType == ACTIVITY_LAND)
					{
						if (iCurrentSimulationTick == iActivityTick - 1)
							bIsOnGround = false;
						else if (iCurrentSimulationTick == iActivityTick)
						{
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].SetCycle(0.0f);
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].SetSequence(pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetSequence());
							m_Player->m_AnimOverlay()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].SetPlaybackRate(pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetPlaybackRate());

							bIsOnGround = true;
						}
					}

					if (bIsOnGround)
						m_Player->m_fFlags() |= FL_ONGROUND;
					else 
						m_Player->m_fFlags() &= ~FL_ONGROUND;
				}
			}
		}
	}

	if (pRecord->m_ChokedCommands > 0) {
		if (pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_MOVE].GetPlaybackRate() <= 0.f ||
			pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_MOVE].GetWeight() <= 0.f)
			pRecord->m_Velocity.clear();
		else if (m_Records.size() >= 2) {
			LagRecord* pPrevious = m_Records[1].get();

			if (pPrevious && !pPrevious->m_Dormant) {
				pRecord->m_Velocity = (pRecord->m_Origin - pPrevious->m_Origin) * (1.f / game::TICKS_TO_TIME(pRecord->m_ChokedCommands));

				auto flCurrentDirection = Math::RAD2DEG(std::atan2f(pRecord->m_Velocity.y, pRecord->m_Velocity.x));
				auto flPreviousDirection = Math::RAD2DEG(std::atan2f(pPrevious->m_Velocity.y, pPrevious->m_Velocity.x));
				auto flDelta = Math::normalize_float(flCurrentDirection - flPreviousDirection);

				if (m_Player->m_fFlags() & FL_ONGROUND && pRecord->m_Velocity.IsValid() && std::abs(flDelta) < 1.0f && std::abs(pRecord->m_DuckAmount - pPrevious->m_DuckAmount) <= 0.0f && pRecord->m_Layers[6].GetPlaybackRate() > pPrevious->m_Layers[6].GetPlaybackRate() && pRecord->m_Layers[6].GetWeight() > pPrevious->m_Layers[6].GetWeight())
				{
					auto flWeight = pRecord->m_Layers[6].GetWeight();
					if (flWeight <= 0.7f && flWeight > 0.0f)
					{
						const auto flVelocitySpeed = pRecord->m_Velocity.Length2D();
						if (flVelocitySpeed != 0.0f)
						{
							float flMultiplier = 1;
							if (m_Player->m_fFlags() & 6) flMultiplier = 0.34f;
							else if (pRecord->m_FakeWalk) flMultiplier = 0.52f;

							pRecord->m_Velocity.x = (pRecord->m_Velocity.x / flVelocitySpeed) * (flWeight * (pRecord->m_Player->GetMaxPlayerSpeed() * flMultiplier));
							pRecord->m_Velocity.y = (pRecord->m_Velocity.y / flVelocitySpeed) * (flWeight * (pRecord->m_Player->GetMaxPlayerSpeed() * flMultiplier));
						}
					}
				}

				float flAnimationSpeed = -1.f;

				if (m_Player->m_fFlags() & FL_ONGROUND && pRecord->m_Layers[11].GetWeight() > 0.0f && pRecord->m_Layers[11].GetWeight() < 1.0f)
				{
					auto flMaxSpeed = (1.0f - pRecord->m_Layers[11].GetWeight()) * 0.35f;
					if (flMaxSpeed > 0.0f && flMaxSpeed < 1.0f)
						flAnimationSpeed = flMaxSpeed + 0.55f;
				}

				if (m_Player->m_fFlags() & FL_ONGROUND && pRecord->m_Velocity.IsValid() && flAnimationSpeed > 0.f)
				{
					float flVelocitySpeed = pRecord->m_Velocity.Length2D();
					auto flAdjustedVelocity = (flAnimationSpeed * pRecord->m_Player->GetMaxPlayerSpeed()) / flVelocitySpeed;

					pRecord->m_Velocity.x *= flAdjustedVelocity;
					pRecord->m_Velocity.y *= flAdjustedVelocity;
				}

				if (!m_Player->m_iMoveState() && pRecord->m_Velocity.Length() > 1.5f && (pRecord->m_Velocity.Length() >= pPrevious->m_Velocity.Length() || fabsf(pRecord->m_Velocity.Length() - pPrevious->m_Velocity.Length()) < 0.5f))
					m_Player->m_iMoveState() = pPrevious->m_MoveState == 0 ? 2 : pPrevious->m_MoveState;
			}

			char szWeaponMoveSeq[64];
			sprintf_s(szWeaponMoveSeq, "move_%s", pAnimationState->GetWeaponPrefix());

			int nWeaponMoveSeq = m_Player->LookupSequence(szWeaponMoveSeq);
			if (nWeaponMoveSeq == -1)
				nWeaponMoveSeq = m_Player->LookupSequence("move");

			float flSequenceCycleRate = m_Player->GetSequenceCycleRate(m_Player->GetModelPtr(), nWeaponMoveSeq);
			float flSequenceGroundSpeed = fmax(m_Player->GetSequenceMoveDist(m_Player->GetModelPtr(), nWeaponMoveSeq) / (1.0f / flSequenceCycleRate), 0.001f);

			static float_t flVelocityLength = 0.f;
			static bool bCalculated = false;

			if (!bCalculated && pAnimationState->flVelocityLengthXY > 260.0f * 0.52f) {
				flVelocityLength = pRecord->m_Layers[6].GetPlaybackRate();
				bCalculated = true;
			}

			if (bCalculated && pAnimationState->flVelocityLengthXY <= 260.0f * 0.52f)
				bCalculated = false;

			static bool bWalkToRunTransitionState = 0;
			static float flWalkToRunTransition = 0;

			if (flWalkToRunTransition > 0.f && flWalkToRunTransition < 1.f)
			{
				if (bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
					flWalkToRunTransition += pAnimationState->flLastUpdateIncrement * 2.f;

				else flWalkToRunTransition -= pAnimationState->flLastUpdateIncrement * 2.f;

				flWalkToRunTransition = std::clamp(flWalkToRunTransition, 0.f, 1.f);
			}

			if (pRecord->m_Layers[6].GetPlaybackRate() > flVelocityLength && bWalkToRunTransitionState == ANIM_TRANSITION_RUN_TO_WALK)
			{
				bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;
				flWalkToRunTransition = std::max(0.01f, flWalkToRunTransition);
			}
			else if (pRecord->m_Layers[6].GetPlaybackRate() < flVelocityLength && bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
			{
				bWalkToRunTransitionState = ANIM_TRANSITION_RUN_TO_WALK;
				flWalkToRunTransition = std::min(0.99f, flWalkToRunTransition);
			}

			const float flModifiedMoveCycleRate = (pRecord->m_Layers[6].GetPlaybackRate() / flSequenceCycleRate) / Math::Lerp(flWalkToRunTransition, 1.0f, 0.85f) * flSequenceGroundSpeed;

			if (!pRecord->m_Velocity.IsValid())
				pRecord->m_Velocity = Vector(0.f, (flModifiedMoveCycleRate / pAnimationState->flLastUpdateIncrement), 0.f);
		}
	}

	pRecord->m_AnimationVelocity = pRecord->m_Velocity;

	if (pRecord->m_ChokedCommands > 1) {
		if (m_Records.size() >= 2) {
			LagRecord* pPrevious = m_Records[1].get();

			if (pPrevious && !pPrevious->m_Dormant) {
				const float flSimulationTimeDifference = pRecord->m_SimulationTime - pPrevious->m_SimulationTime;
				const float flDuckAmountDifference = pRecord->m_DuckAmount - pPrevious->m_DuckAmount;
				const float flDuckAmountChange = game::TICKS_TO_TIME(flDuckAmountDifference / flSimulationTimeDifference);

				m_Player->m_flDuckAmount() = pPrevious->m_DuckAmount + flDuckAmountChange;

				if (!pRecord->m_FakeWalk) {
					Vector vecVelocityDifference = pRecord->m_Velocity - pPrevious->m_Velocity;
					Vector vecVelocityChange = game::TICKS_TO_TIME(vecVelocityDifference / flSimulationTimeDifference);

					pRecord->m_AnimationVelocity = pPrevious->m_Velocity + vecVelocityChange;
				}
			}
		}
	}

	m_Player->SetSimulationTime(pRecord->m_SimulationTime);
	m_Player->SetAnimLayers(pRecord->m_Layers);
	m_Player->SetVelocity(pRecord->m_AnimationVelocity);
	m_Player->SetOrigin(pRecord->m_Origin);

	if (!pRecord->m_Legit)
		g_Resolver.PredictAnglePosition(pRecord, this);

	if (m_SpawnTime != m_Player->m_flSpawnTime()) {
		m_SpawnTime = m_Player->m_flSpawnTime();

		m_Player->m_flLowerBodyYawTarget() = 0;
		pAnimationState->Reset();
	}

	pAnimationState->bJumping = m_Player->GetSequenceActivity(pRecord->m_Layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].GetSequence()) == ACT_CSGO_JUMP && !(m_Player->m_fFlags() & FL_ONGROUND);

	if (m_AwaitingFlashedResult) {
		m_AwaitingFlashedResult = false;

		pAnimationState->flFlashedAmountEaseOutStart = pRecord->m_SimulationTime;
		pAnimationState->flFlashedAmountEaseOutEnd = pRecord->m_SimulationTime + m_Player->m_flFlashDuration();
	}

	pAnimationState->Update(pRecord->m_EyeAngles);

	m_Player->InvalidatePhysicsRecursive(POSITION_CHANGED | ANGLES_CHANGED | ANIMATION_CHANGED);

	if (m_Player->m_bKilledByTaser())
		m_Player->HandleTaserAnimation();

	if (m_Player->m_nSequence() != -1)
		m_Player->OnLatchInterpolatedVariables(LATCH_ANIMATION_VAR);

	m_Player->GetPoseParameters(pRecord->m_Poses);
	pRecord->m_AbsAngles = m_Player->GetAbsAngles();

	m_Player->m_vecOrigin() = pBackup.m_Origin;
	m_Player->m_vecVelocity() = pBackup.m_Velocity;
	m_Player->m_vecAbsVelocity() = pBackup.m_AbsVelocity;
	m_Player->m_fFlags() = pBackup.m_Flags;
	m_Player->m_iEFlags() = pBackup.m_EFlags;
	m_Player->m_flDuckAmount() = pBackup.m_DuckAmount;
	m_Player->m_flLowerBodyYawTarget() = pBackup.m_LowerbodyYaw;
	m_Player->m_flSimulationTime() = pBackup.m_SimulationTime;
	m_Player->SetAbsOrigin(pBackup.m_AbsOrigin);
	m_Player->SetAnimLayers(pBackup.m_Layers);

	g_Bones.SetupBones(m_Player, pRecord);

	g_Csgo.m_Globals->m_Curtime = flCurtime;
	g_Csgo.m_Globals->m_RealTime = flRealtime;
	g_Csgo.m_Globals->m_FrameTime = flFrametime;
	g_Csgo.m_Globals->m_AbsFrametime = flAbsFrametime;
	g_Csgo.m_Globals->m_FrameCount = iFramecount;
	g_Csgo.m_Globals->m_TickCount = iTickcount;
	g_Csgo.m_Globals->m_InterpolationAmount = flInterpAmount;
}