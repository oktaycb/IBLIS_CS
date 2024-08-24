#include "../../precompiled.h"

#define CSGO_ANIM_LOWER_REALIGN_DELAY 1.1f

float Resolver::GetAwayAngle(LagRecord* pRecord)
{
	QAngle flAwayAngle;
	Math::VectorAngles(g_Client.m_LocalPlayer->m_vecOrigin() - pRecord->m_Origin, flAwayAngle);
	return flAwayAngle.y;
}

float Resolver::GetRotationYaw(LagRecord* pRecord, float flYaw)
{
	return Math::NormalizedAngle(Math::CalcAngle(pRecord->m_Origin, g_Client.m_LocalPlayer->Weapon_ShootPosition()).y - flYaw);
}

// TODO@Monster: Check Balance Adjustment for Last LBY.
void Resolver::PredictAnglePosition(LagRecord* pRecord, AimPlayer* pData)
{
	pData->m_ResolverMode = AimPlayer::RESOLVER::LEGIT;
	pRecord->m_Client.m_EyeAngles = pRecord->m_EyeAngles;

	if (!g_Config.b(g_Items.m_Aimbot.m_General.m_AntiAimPrediction)) {
		Reset(pData, true);
		return;
	}

	if (pData->m_Records.size() >= 2) {
		LagRecord* pPrevious = pData->m_Records[1].get();

		if (pPrevious && pPrevious->m_Dormant)
			g_Resolver.Reset(pData, false);
	}

	SetMode(pData, pRecord);
	MatchShot(pData, pRecord);

	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 32.f };

	Vector Weapon_ShootPosition = pRecord->m_Player->Weapon_ShootPosition();

	pData->m_AdaptiveAngles.clear();
	pData->m_ValidAdaptiveAngles = false;

	pData->m_AdaptiveAngles.emplace_back(g_Resolver.GetAwayAngle(pRecord) + 180.f);
	pData->m_AdaptiveAngles.emplace_back(g_Resolver.GetAwayAngle(pRecord) - 90.f);
	pData->m_AdaptiveAngles.emplace_back(g_Resolver.GetAwayAngle(pRecord) + 90.f);

	for (auto pIterator = pData->m_AdaptiveAngles.begin(); pIterator != pData->m_AdaptiveAngles.end(); ++pIterator)
	{
		Vector vecStart = g_Client.m_LocalPlayer->Weapon_ShootPosition();
		Vector vecEnd{ Weapon_ShootPosition.x + std::cos(Math::DEG2RAD(pIterator->m_Yaw)) * RANGE, Weapon_ShootPosition.y + std::sin(Math::DEG2RAD(pIterator->m_Yaw)) * RANGE, Weapon_ShootPosition.z };
		Vector vecDirection = vecEnd - vecStart;

		float flLenght = vecDirection.Normalize();
		if (flLenght <= 0.f)
			continue;

		for (float flStepLenght{ 0.f }; flStepLenght < flLenght; flStepLenght += STEP) {
			if (!(g_Csgo.m_EngineTrace->GetPointContents(vecStart + (vecDirection * flStepLenght), MASK_SHOT_HULL) & MASK_SHOT_HULL))
				continue;

			pData->m_ValidAdaptiveAngles = true;

			float flMultiplier = 1.f;

			if (flStepLenght > (flLenght * 0.5f))
				flMultiplier = 1.25f;

			if (flStepLenght > (flLenght * 0.75f))
				flMultiplier = 1.25f;

			if (flStepLenght > (flLenght * 0.9f))
				flMultiplier = 2.f;

			pIterator->m_Distance += (STEP * flMultiplier);
		}
	}

	std::sort(pData->m_AdaptiveAngles.begin(), pData->m_AdaptiveAngles.end(),
		[](const AdaptiveAngle& a, const AdaptiveAngle& b) {
			return a.m_Distance > b.m_Distance;
		});

	pRecord->m_UnsafeVelocityTransition = g_Resolver.UnsafeVelocityTransitionDetection(pRecord, pData);
	pRecord->m_FakeFlick = g_Resolver.IsFakeFlick(pRecord, pData);

	if (pRecord->m_Resolved = g_Resolver.PredictLowerbodyUpdates(pRecord, pData)) {
		pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;

		if (pData->m_ResolverMode != AimPlayer::RESOLVER::RESOLVED)
			pData->m_ResolverMode = AimPlayer::RESOLVER::RESOLVED;
	}

	else if (pData->m_MissedShots > 0) {
		if (pData->m_ValidAdaptiveAngles && pData->m_FreestandMissedShots <= 1) {
			pRecord->m_EyeAngles.y = pData->m_LastAngle = pData->m_AdaptiveAngles.front().m_Yaw;

			if (pData->m_ResolverMode != AimPlayer::RESOLVER::FREESTAND)
				pData->m_ResolverMode = AimPlayer::RESOLVER::FREESTAND;
		}
		else {
			float flBruteforceAngle{ 0.f };
			float flLastRotationYaw = GetRotationYaw(pRecord, pData->m_LastAngle);

			if (fabsf(flLastRotationYaw) >= 45.f && fabsf(flLastRotationYaw) <= 135.f)
			{
				switch (pData->m_MissedShots % 4) {
				case 0: flBruteforceAngle = -90.f; break;
				case 1: flBruteforceAngle = 180.f; break;
				case 2: flBruteforceAngle = 90.f; break;
				case 3: flBruteforceAngle = 0.f; break;
				}

				if (flLastRotationYaw >= 45.f && flLastRotationYaw <= 135.f)
					pRecord->m_EyeAngles.y = pData->m_LastAngle + flBruteforceAngle + 90.f;
				else pRecord->m_EyeAngles.y = pData->m_LastAngle - flBruteforceAngle - 90.f;
			}
			else if (fabsf(flLastRotationYaw) > 135.f)
			{
				switch (pData->m_MissedShots % 4) {
				case 0: flBruteforceAngle = -90.f; break;
				case 1: flBruteforceAngle = 180.f; break;
				case 2: flBruteforceAngle = 0.f; break;
				case 3: flBruteforceAngle = 90.f; break;
				}

				if (flLastRotationYaw > 135.f)
					pRecord->m_EyeAngles.y = pData->m_LastAngle + flBruteforceAngle + 90.f;
				else pRecord->m_EyeAngles.y = pData->m_LastAngle - flBruteforceAngle - 90.f;
			}
			else if (fabsf(flLastRotationYaw) < 45.f)
			{
				switch (pData->m_MissedShots % 4) {
				case 0: flBruteforceAngle = -90.f; break;
				case 1: flBruteforceAngle = 90.f; break;
				case 2: flBruteforceAngle = 180.f; break;
				case 3: flBruteforceAngle = 0.f; break;
				}

				if (flLastRotationYaw < 45.f)
					pRecord->m_EyeAngles.y = pData->m_LastAngle + flBruteforceAngle + 90.f;
				else pRecord->m_EyeAngles.y = pData->m_LastAngle - flBruteforceAngle - 90.f;
			}

			pData->m_ResolverMode = AimPlayer::RESOLVER::BRUTEFORCE;

			float flLowerbodyYawDifference = fabs(pRecord->m_EyeAngles.y - pRecord->m_LowerbodyYaw);
			if (flLowerbodyYawDifference <= 35.f) {
				pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::BRUTEFORCE_BROKEN)
					pData->m_ResolverMode = AimPlayer::RESOLVER::BRUTEFORCE_BROKEN;
			}
		}
	}

	else if (!pRecord->m_Shot && fabsf(GetRotationYaw(pRecord, pRecord->m_Client.m_EyeAngles.y)) < 90.f && fabsf(pRecord->m_EyeAngles.x) < 45.f)
		pData->m_ResolverMode = AimPlayer::RESOLVER::LEGIT;

	else if (fabs(pData->m_WalkRecord.m_LowerbodyYaw) > 0.f)
	{
		if (pData->m_IsLowerbodyYawBroken && !pRecord->m_FakeFlick)
		{
			if (pData->m_ResolverMode != AimPlayer::RESOLVER::BROKEN)
				pData->m_ResolverMode = AimPlayer::RESOLVER::BROKEN;

			pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;
		}
		else if (pData->m_CountLowerbodyYawChanges > 0 && !pRecord->m_FakeFlick)
		{
			if (fabs(pData->m_CombineBreakerDifferences + (pData->m_FirstLowerbodyYaw - pData->m_WalkRecord.m_LowerbodyYaw)) <= 45.f)
			{
				pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::PREDICTION_BROKEN)
					pData->m_ResolverMode = AimPlayer::RESOLVER::PREDICTION_BROKEN;
			}
			else {
				pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw - (pData->m_FirstLowerbodyYaw - pData->m_WalkRecord.m_LowerbodyYaw) + pData->m_CombineBreakerDifferences;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::PREDICTION)
					pData->m_ResolverMode = AimPlayer::RESOLVER::PREDICTION;

			}
		}
		else {
			if (fabs(pData->m_WalkRecord.m_LowerbodyYaw - pRecord->m_LowerbodyYaw) <= 45.f)
			{
				pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::LAST_BROKEN)
					pData->m_ResolverMode = AimPlayer::RESOLVER::LAST_BROKEN;
			}
			else {
				pRecord->m_EyeAngles.y = pData->m_WalkRecord.m_LowerbodyYaw;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::LAST)
					pData->m_ResolverMode = AimPlayer::RESOLVER::LAST;

			}
		}

		pData->m_LastAngle = pRecord->m_EyeAngles.y;
	}
	else {
		if (pData->m_Player->GetSequenceActivity(pRecord->m_Layers[ANIMATION_LAYER_ADJUST].GetSequence()) == ACT_CSGO_IDLE_TURN_BALANCEADJUST && pRecord->m_Layers[ANIMATION_LAYER_ADJUST].GetWeight())
		{
			if (fabs(pData->m_AdaptiveAngles.front().m_Yaw - pRecord->m_LowerbodyYaw) > 120.f) {
				pRecord->m_EyeAngles.y = pData->m_AdaptiveAngles.front().m_Yaw;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::FREESTAND_DELTA)
					pData->m_ResolverMode = AimPlayer::RESOLVER::FREESTAND_DELTA;
			}
			else {
				pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw - 180.f;

				if (pData->m_ResolverMode != AimPlayer::RESOLVER::DELTA)
					pData->m_ResolverMode = AimPlayer::RESOLVER::DELTA;
			}
		}
		else {
			pRecord->m_EyeAngles.y = pRecord->m_LowerbodyYaw;

			if (pData->m_ResolverMode != AimPlayer::RESOLVER::BROKEN)
				pData->m_ResolverMode = AimPlayer::RESOLVER::BROKEN;

		}

		pData->m_LastAngle = pRecord->m_EyeAngles.y;
	}

	static float flGroundAngle = 0.f;

	if (fabs(flGroundAngle) > 0.f && pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_AIR)
		pRecord->m_EyeAngles.y -= static_cast<float>(flGroundAngle - g_Resolver.GetAwayAngle(pRecord));
	else flGroundAngle = g_Resolver.GetAwayAngle(pRecord);

	//if (fabs(flGroundAngle) > 0.f && pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_AIR)
	//	g_notify.add(XOR("GetAwayAngle: ") + std::to_string(static_cast<float>(flGroundAngle - g_Resolver.GetAwayAngle(pRecord))) + "\n");
}

void Resolver::PredictFootYaw(LagRecord* pRecord, AimPlayer* pData)
{
	pRecord->m_Client.m_FootYaw = pRecord->m_Player->m_PlayerAnimStateCSGO()->flFootYaw;

	//if (pRecord->m_ResolverMode == RESOLVE_STAND && !pRecord->m_Velocity.IsValid() && !pRecord->m_FakeFlick && pData->m_FakeFlickMissedShots == 1) {
	//	pData->m_Player->m_PlayerAnimStateCSGO()->flFootYaw = pRecord->m_EyeAngles.y - (pRecord->m_EyeAngles.y - pRecord->m_Client.m_FootYaw) * -1;
	//	return;
	//}

	//if (!pRecord->m_FakeFlick || !pRecord->m_Velocity.IsValid())
	//	return;

	//if (!pData->m_ValidAdaptiveAngles || pData->m_AdaptiveAngles.empty() || pData->m_AdaptiveBack)
	//	return;

	//pData->m_Player->m_PlayerAnimStateCSGO()->flFootYaw = pData->m_AdaptiveAngles.front().m_Yaw;
}

bool Resolver::PredictLowerbodyUpdates(LagRecord* pRecord, AimPlayer* pData)
{
	switch (pRecord->m_ResolverMode) {
	case RESOLVE_STAND:
		// TODO@Monster: remove this after ff checks.
		if (pData->m_Records.size() <= 1 || !pData->m_Records[1].get() || pData->m_Records[1].get()->m_Dormant || pData->m_FakeFlickMissedShots > 1)
			return false;

		if (pRecord->m_LowerbodyYaw != pData->m_Records[1].get()->m_LowerbodyYaw) {
			pData->m_NextUpdateTime = CSGO_ANIM_LOWER_REALIGN_DELAY;
			pData->m_NextLowerbodyYawUpdate = g_Csgo.m_Globals->m_Curtime + pData->m_NextUpdateTime;

			Save(pRecord, pData);
			return true;
		}

		if (g_Csgo.m_Globals->m_Curtime > pData->m_NextLowerbodyYawUpdate && pData->m_ServerUpdatedLowerbodyYaw)
		{
			pData->m_NextUpdateTime = CSGO_ANIM_LOWER_REALIGN_DELAY;
			pData->m_NextLowerbodyYawUpdate = g_Csgo.m_Globals->m_Curtime + pData->m_NextUpdateTime;
			return true;
		}

		break;
	case RESOLVE_WALK:
		if (!pRecord->m_FakeFlick)
			std::memcpy(&pData->m_WalkRecord, pRecord, sizeof(LagRecord));

		return Reset(pData, !pRecord->m_FakeFlick);
		break;
	case RESOLVE_AIR:
		Reset(pData);

		// Check if was moving.
		if (pData->m_MissedShots == 0)
			return true;
		break;
	default:
		break;
	}

	return false;
}

void Resolver::MatchShot(AimPlayer* pData, LagRecord* pRecord) {
	if (!pData->m_Player->GetActiveWeapon())
		return;

	if (game::TIME_TO_TICKS(pData->m_Player->GetActiveWeapon()->m_fLastShotTime() + g_Csgo.m_Globals->m_Interval) != game::TIME_TO_TICKS(pRecord->m_SimulationTime))
		return;

	pRecord->m_Shot = true;
}

bool Resolver::UnsafeVelocityTransitionDetection(LagRecord* pRecord, AimPlayer* pData)
{
	if (pData->m_Records.size() <= 1 || !pData->m_Records[1].get() || pData->m_Records[1].get()->m_Dormant)
		return false;

	if (pRecord->m_Velocity.Length2D() <= 0.f || pRecord->m_Velocity.Length() > 35.f || !(pRecord->m_Flags & FL_ONGROUND))
		return false;

	if (pRecord->m_Layers[6].GetWeight() == 0.0f || pRecord->m_Layers[6].GetWeight() == 1.0f)
		return false;

	return pRecord->m_Layers[6].GetWeight() != pData->m_Records[1].get()->m_Layers[6].GetWeight();
}

#define CSGO_FAKE_FLICK_REALIGN_DELAY CSGO_ANIM_LOWER_REALIGN_DELAY - (CSGO_ANIM_LOWER_REALIGN_DELAY * 0.4f)

bool Resolver::IsFakeFlick(LagRecord* pRecord, AimPlayer* pData)
{
	static bool bFakeFlick = false;
	static float flCurtime = 0.f;

	if (pRecord->m_UnsafeVelocityTransition)
	{
		bFakeFlick = true;
		flCurtime = g_Csgo.m_Globals->m_Curtime + CSGO_FAKE_FLICK_REALIGN_DELAY;
	}
	else if (flCurtime <= g_Csgo.m_Globals->m_Curtime)
		bFakeFlick = false;

	if (pRecord->m_Velocity.Length2D() > 0.f && !pRecord->m_UnsafeVelocityTransition || !(pRecord->m_Flags & FL_ONGROUND)) {
		bFakeFlick = false;
		flCurtime = 0.f;
	}

	return bFakeFlick;
}

// TODO@Monster: Use animation state instead of this perhaps.
bool Resolver::IsSafetyCheck(LagRecord* pRecord, AimPlayer* pData)
{
	static bool bOnce = false;
	static bool bBeenFakeFlicking = false;
	static float flFlickingTime = 0.f;

	if (pRecord->m_FakeFlick) {
		bBeenFakeFlicking = false;

		if (!bOnce) {
			flFlickingTime = g_Csgo.m_Globals->m_Curtime + CSGO_FAKE_FLICK_REALIGN_DELAY;
			bOnce = true;
		}
		else if (flFlickingTime <= g_Csgo.m_Globals->m_Curtime)
			bBeenFakeFlicking = true;
	}

	const bool bDisable = pRecord->m_Velocity.IsValid() && (pRecord->m_Velocity.Length2D() < 5.f || pRecord->m_Velocity.Length2D() > 35.f || !pRecord->m_UnsafeVelocityTransition) || !(pRecord->m_Flags & FL_ONGROUND);
	if (bDisable)
	{
		bOnce = false;
		bBeenFakeFlicking = false;
		flFlickingTime = 0.f;
	}

	return bBeenFakeFlicking;
}

void Resolver::Save(LagRecord* pRecord, AimPlayer* pData)
{
	pData->m_ServerUpdatedLowerbodyYaw = true;
	pData->m_OldLowerbodyYawBreakers = pData->m_LowerbodyYawBreakers;
	pData->m_LowerbodyYawBreakers = AimPlayer::LBY::NORMAL;

	pData->m_AllLowerbodyYawDifferences = pData->m_WalkRecord.m_LowerbodyYaw - pRecord->m_LowerbodyYaw;

	if (++pData->m_CountLowerbodyYawChanges == 1)
	{
		pData->m_FirstLowerbodyYaw = pRecord->m_LowerbodyYaw;
		pData->m_FirstLowerbodyYawNormalized = Math::AngleDiff(pData->m_WalkRecord.m_LowerbodyYaw, pRecord->m_LowerbodyYaw);
		pData->m_FirstLowerbodyYawDifference = pData->m_AllLowerbodyYawDifferences;
	}

	pData->m_CombineBreakerDifferences = 0;

	pData->m_SecondBrokenLowerbodyYaw = pData->m_FirstBrokenLowerbodyYaw;
	pData->m_FirstBrokenLowerbodyYaw = Math::NormalizedAngle(pData->m_FirstLowerbodyYawDifference) + Math::NormalizedAngle(pData->m_AllLowerbodyYawDifferences);

	pData->m_SecondBrokenLowerbodyYawNormalized = pData->m_FirstBrokenLowerbodyYawNormalized;
	pData->m_FirstBrokenLowerbodyYawNormalized = Math::NormalizedAngle(pData->m_FirstBrokenLowerbodyYaw);

	pData->m_ThirdLowerbodyYawDifferences = pData->m_OldLowerbodyYawDifferences;
	pData->m_OldLowerbodyYawDifferences = pData->m_LowerbodyYawDifferences;
	pData->m_LowerbodyYawDifferencesCalculation = pData->m_OldLowerbodyYawDifferences - pData->m_ThirdLowerbodyYawDifferences;
	pData->m_LowerbodyYawDifferencesCalculation2 = pData->m_LowerbodyYawDifferences - pData->m_LowerbodyYawDifferencesCalculation;
	pData->m_LowerbodyYawDifferences = pData->m_FirstLowerbodyYawDifference - pData->m_AllLowerbodyYawDifferences;

	bool bBreakLowerbody = true;

	if (fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferencesCalculation2))) == 0 || fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferencesCalculation2))) == 90 || fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferencesCalculation2))) == 180)
		bBreakLowerbody = false;

	if (fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferences))) == 0 || fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferences))) == 90 || fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferences))) == 180)
		bBreakLowerbody = true;

	else if (fabs(round(pData->m_LowerbodyYawDifferencesCalculation)) == round(fabs(pData->m_FirstLowerbodyYawDifference)))
		bBreakLowerbody = true;

	if (fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferencesCalculation2))) != 0 && fabs(round(Math::NormalizedAngle(pData->m_LowerbodyYawDifferencesCalculation2))) < 10)
		bBreakLowerbody = false;

	pData->m_IsLowerbodyYawBroken = round(pData->m_SecondBrokenLowerbodyYaw / 2) == round(pData->m_FirstBrokenLowerbodyYaw)
		|| !bBreakLowerbody && round(fabs(fabs(pData->m_FirstBrokenLowerbodyYawNormalized) + fabs(pData->m_SecondBrokenLowerbodyYawNormalized))) == round(fabs(pData->m_FirstLowerbodyYawNormalized))
		|| !bBreakLowerbody && round(fabs(fabs(pData->m_FirstBrokenLowerbodyYawNormalized) - fabs(pData->m_SecondBrokenLowerbodyYawNormalized))) == round(fabs(pData->m_FirstLowerbodyYawNormalized))
		|| !bBreakLowerbody && round(fabs(fabs(pData->m_SecondBrokenLowerbodyYaw) - fabs(pData->m_FirstBrokenLowerbodyYaw))) == round(fabs(pData->m_FirstLowerbodyYawDifference))
		|| !bBreakLowerbody && round(fabs(fabs(pData->m_SecondBrokenLowerbodyYaw) + fabs(pData->m_FirstBrokenLowerbodyYaw))) == round(fabs(pData->m_FirstLowerbodyYawDifference))
		|| round(fabs(pData->m_AllLowerbodyYawDifferences)) <= 5.f;

	if (round(pData->m_LowerbodyYawDifferences) == 0)
		return;

	pData->m_LowerbodyYawCombination = Math::NormalizedAngle(pData->m_FirstLowerbodyYawDifference + pData->m_AllLowerbodyYawDifferences);
	if (abs(round(pData->m_LowerbodyYawCombination)) < 20 || abs(round(pData->m_LowerbodyYawCombination)) > 70 && abs(round(pData->m_LowerbodyYawCombination)) < 110 || abs(round(pData->m_LowerbodyYawCombination)) > 160)
		pData->m_LowerbodyYawBreakers = AimPlayer::LBY::OPPOSITE;

	if (pData->m_CountLowerbodyYawChanges % 2)
		pData->m_FirstPercentDifference = pData->m_LowerbodyYawDifferences;
	else pData->m_SecondPercentDifference = pData->m_LowerbodyYawDifferences;

	const int iDifferencePercentChangeTotal = round(Math::AngleDiff(pData->m_SecondPercentDifference, pData->m_FirstPercentDifference));
	const float flDifferencePercentChangeTotal = Math::AngleDiff(pData->m_SecondPercentDifference, pData->m_FirstPercentDifference);

	if (fabs(flDifferencePercentChangeTotal - iDifferencePercentChangeTotal) > 0 + 0.01f &&
		fabs(flDifferencePercentChangeTotal - iDifferencePercentChangeTotal) < 1 - 0.01f)
		pData->m_LowerbodyYawBreakers = AimPlayer::LBY::MINIMAL;

	static bool bShouldAddDifference = false;

	if (pData->m_LowerbodyYawBreakers == AimPlayer::LBY::MINIMAL && pData->m_OldLowerbodyYawBreakers == AimPlayer::LBY::OPPOSITE)
		bShouldAddDifference = true;

	if (pData->m_LowerbodyYawBreakers == AimPlayer::LBY::NORMAL)
		bShouldAddDifference = false;

	if (pData->m_LowerbodyYawBreakers == AimPlayer::LBY::OPPOSITE || bShouldAddDifference)
		pData->m_CombineBreakerDifferences = pData->m_FirstLowerbodyYawDifference;
}

bool Resolver::Reset(AimPlayer* pData, bool bResetMissedShots)
{
	if (bResetMissedShots)
		pData->m_MissedShots = 0;

	pData->m_ServerUpdatedLowerbodyYaw = false;
	pData->m_ValidAdaptiveAngles = false;
	pData->m_AdaptiveAngles.clear();
	pData->m_NextLowerbodyYawUpdate = 0.f;
	pData->m_FirstLowerbodyYawNormalized = 0;
	pData->m_ThirdBrokenLowerbodyYawNormalized = 0;
	pData->m_SecondBrokenLowerbodyYawNormalized = 0;
	pData->m_FirstBrokenLowerbodyYawNormalized = 0;
	pData->m_SecondBrokenLowerbodyYaw = 0;
	pData->m_FirstBrokenLowerbodyYaw = 0;
	pData->m_IsLowerbodyYawBroken = false;
	pData->m_CountLowerbodyYawChanges = 0;
	pData->m_FirstLowerbodyYaw = 0;
	pData->m_LowerbodyYawBreakers = AimPlayer::LBY::NORMAL;
	pData->m_FirstPercentDifference = 0;
	pData->m_SecondPercentDifference = 0;
	pData->m_AllLowerbodyYawDifferences = 0;
	pData->m_FirstLowerbodyYawDifference = 0;
	pData->m_ThirdLowerbodyYawDifferences = 0;
	pData->m_OldLowerbodyYawDifferences = 0;
	pData->m_LowerbodyYawDifferencesCalculation = 0;
	pData->m_LowerbodyYawDifferencesCalculation2 = 0;
	pData->m_LowerbodyYawDifferences = 0;
	pData->m_CombineBreakerDifferences = 0;
	pData->m_LowerbodyYawCombination = 0;
	return true;
}

void Resolver::SetMode(AimPlayer* pData, LagRecord* pRecord) {
	pRecord->m_ResolverMode = Modes::RESOLVE_STAND;

	if (!(pData->m_Player->m_fFlags() & FL_ONGROUND))
		pRecord->m_ResolverMode = Modes::RESOLVE_AIR;

	else if (pRecord->m_Velocity.IsValid())
		pRecord->m_ResolverMode = Modes::RESOLVE_WALK;
}

LagRecord* Resolver::GetIdealRecord(AimPlayer* data) {
	if (data->m_Records.empty())
		return nullptr;

	LagRecord* pFirtValid = nullptr;

	for (const auto& pIterator : data->m_Records) {
		if (pIterator.get() == nullptr || pIterator.get()->m_Dormant || pIterator.get()->m_Immune || !pIterator.get()->IsValidTime(g_TickbaseManipulation.GetShiftAmount()))
			continue;

		// first record that was valid, store it for later.
		if (!pFirtValid)
			pFirtValid = pIterator.get();

		return pIterator.get();
	}

	return (pFirtValid) ? pFirtValid : nullptr;
}

LagRecord* Resolver::GetLastRecord(AimPlayer* pData) {
	if (pData->m_Records.empty())
		return nullptr;

	for (auto pIterator = pData->m_Records.crbegin(); pIterator != pData->m_Records.crend(); ++pIterator) {
		if (pIterator->get() == nullptr || pIterator->get()->m_Dormant || pIterator->get()->m_Immune || !pIterator->get()->IsValidTime(g_TickbaseManipulation.GetShiftAmount()))
			continue;

		return pIterator->get();
	}

	return nullptr;
}

//LagRecord* Resolver::GetAllRecords(AimPlayer* pData) {
//	if (pData->m_Records.empty())
//		return nullptr;
//
//	for (auto pIterator : pData->m_Records)
//	{
//		if (pIterator.get()->m_Dormant || pIterator.get()->m_Immune || !pIterator.get()->valid())
//			continue;
//
//		return pIterator.get();
//	}
//
//	return nullptr;
//}