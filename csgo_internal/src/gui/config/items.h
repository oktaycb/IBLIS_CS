#pragma once

class Items
{
public:
	struct Aimbot
	{
		struct General
		{
			ADD_CFG(General::m_Enabled, bool, false);
			ADD_CFG(General::m_AutoFire, bool, true);
			ADD_CFG(General::m_AutoWall, bool, true);
			ADD_CFG(General::m_AutoScope, bool, true);
			ADD_CFG(General::m_AutoStop, bool, true);
			ADD_CFG(General::m_Silent, bool, true);
			ADD_CFG(General::m_ScaleDamage, bool, true);
			ADD_CFG(General::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, General::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, General::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, General::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, General::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, General::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, General::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, General::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, General::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, General::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, General::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, General::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, General::m_AutoStopConditions, false);

			ADD_CFG(General::m_MultipointHeadScale, int, 70);
			ADD_CFG(General::m_MultipointBodyScale, int, 70);
			ADD_CFG(General::m_MaximumFOV, int, 360);
			ADD_CFG(General::m_MinimumDamage, int, 0);
			ADD_CFG(General::m_HitChanceAmount, int, 70);
			ADD_CFG(General::m_OverrideDamageAmount, int, 0);
			ADD_CFG(General::m_OverrideHitChanceAmount, int, 70);

			ADD_CFG(General::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(General::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(General::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(General::m_TargetSelection, int, 0);
			ADD_CFG(General::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(General::m_DetermineXMisses, int, 0);
			ADD_CFG(General::m_DetermineXHealth, int, 0);
			ADD_CFG(General::m_DetermineTarget, int, 0);
			ADD_CFG(General::m_LimitTarget, int, 0);
			ADD_CFG(General::m_MinimumDamageHead, int, 0);
			ADD_CFG(General::m_MinimumDamageNeck, int, 0);
			ADD_CFG(General::m_MinimumDamageChest, int, 0);
			ADD_CFG(General::m_MinimumDamageStomach, int, 0);
			ADD_CFG(General::m_MinimumDamageArms, int, 0);
			ADD_CFG(General::m_MinimumDamageLegs, int, 0);
			ADD_CFG(General::m_MinimumDamageFeet, int, 0);
			ADD_CFG(General::m_ScaleDamageAmount, int, 0);

			ADD_CFG(General::m_OptimizationLevel, int, 4);
			ADD_CFG(General::m_DetermineFPS, int, 60);

			ADD_CFG(General::m_AntiAimPrediction, bool, true);
			ADD_CFG(General::m_SmoothInterpolation, bool, true);

			ADD_CFG(General::m_TickbaseManipulation, bool, false);
			ADD_CFG(General::m_TickbaseManipulationKey, int, 0);
			ADD_CFG(General::m_TickbaseManipulationMode, int, 0);

			ADD_VECTOR_CFG(bool, 2, General::m_TickbaseManipulationSelection, false);

			ADD_CFG(General::m_ForceBody, bool, false);
			ADD_CFG(General::m_ForceBodyKey, int, 0);
			ADD_CFG(General::m_ForceBodyMode, int, 0);

			ADD_CFG(General::m_WaitForLowerbodyYawUpdate, bool, false);
			ADD_CFG(General::m_WaitForLowerbodyYawUpdateKey, int, 0);
			ADD_CFG(General::m_WaitForLowerbodyYawUpdateMode, int, 0);

			ADD_CFG(General::m_OverrideDamage, bool, false);
			ADD_CFG(General::m_OverrideDamageKey, int, 0);
			ADD_CFG(General::m_OverrideDamageMode, int, 0);

			ADD_CFG(General::m_OverrideHitChance, bool, false);
			ADD_CFG(General::m_OverrideHitChanceKey, int, 0);
			ADD_CFG(General::m_OverrideHitChanceMode, int, 0);
		} m_General;

		struct Pistol
		{
			ADD_CFG(Pistol::m_Enabled, bool, false);
			ADD_CFG(Pistol::m_AutoFire, bool, true);
			ADD_CFG(Pistol::m_AutoWall, bool, true);
			ADD_CFG(Pistol::m_AutoScope, bool, true);
			ADD_CFG(Pistol::m_AutoStop, bool, true);
			ADD_CFG(Pistol::m_Silent, bool, true);
			ADD_CFG(Pistol::m_ScaleDamage, bool, true);
			ADD_CFG(Pistol::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, Pistol::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, Pistol::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, Pistol::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, Pistol::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, Pistol::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, Pistol::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, Pistol::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, Pistol::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, Pistol::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Pistol::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Pistol::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, Pistol::m_AutoStopConditions, false);

			ADD_CFG(Pistol::m_MultipointHeadScale, int, 70);
			ADD_CFG(Pistol::m_MultipointBodyScale, int, 50);
			ADD_CFG(Pistol::m_MaximumFOV, int, 360);
			ADD_CFG(Pistol::m_MinimumDamage, int, 10);
			ADD_CFG(Pistol::m_HitChanceAmount, int, 60);
			ADD_CFG(Pistol::m_OverrideDamageAmount, int, 0);
			ADD_CFG(Pistol::m_OverrideHitChanceAmount, int, 60);

			ADD_CFG(Pistol::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(Pistol::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(Pistol::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(Pistol::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(Pistol::m_DetermineXMisses, int, 0);
			ADD_CFG(Pistol::m_DetermineXHealth, int, 0);
			ADD_CFG(Pistol::m_DetermineTarget, int, 0);
			ADD_CFG(Pistol::m_LimitTarget, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageHead, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageNeck, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageChest, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageStomach, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageArms, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageLegs, int, 0);
			ADD_CFG(Pistol::m_MinimumDamageFeet, int, 0);
			ADD_CFG(Pistol::m_ScaleDamageAmount, int, 0);
		} m_Pistol;

		struct HeavyPistol
		{
			ADD_CFG(HeavyPistol::m_Enabled, bool, false);
			ADD_CFG(HeavyPistol::m_AutoFire, bool, true);
			ADD_CFG(HeavyPistol::m_AutoWall, bool, true);
			ADD_CFG(HeavyPistol::m_AutoScope, bool, true);
			ADD_CFG(HeavyPistol::m_AutoStop, bool, true);
			ADD_CFG(HeavyPistol::m_Silent, bool, true);
			ADD_CFG(HeavyPistol::m_ScaleDamage, bool, true);
			ADD_CFG(HeavyPistol::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, HeavyPistol::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, HeavyPistol::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, HeavyPistol::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, HeavyPistol::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, HeavyPistol::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, HeavyPistol::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, HeavyPistol::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, HeavyPistol::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, HeavyPistol::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, HeavyPistol::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, HeavyPistol::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, HeavyPistol::m_AutoStopConditions, false);

			ADD_CFG(HeavyPistol::m_MultipointHeadScale, int, 80);
			ADD_CFG(HeavyPistol::m_MultipointBodyScale, int, 80);
			ADD_CFG(HeavyPistol::m_MaximumFOV, int, 360);
			ADD_CFG(HeavyPistol::m_MinimumDamage, int, 50);
			ADD_CFG(HeavyPistol::m_HitChanceAmount, int, 60);
			ADD_CFG(HeavyPistol::m_OverrideDamageAmount, int, 0);
			ADD_CFG(HeavyPistol::m_OverrideHitChanceAmount, int, 60);

			ADD_CFG(HeavyPistol::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(HeavyPistol::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(HeavyPistol::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(HeavyPistol::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(HeavyPistol::m_DetermineXMisses, int, 0);
			ADD_CFG(HeavyPistol::m_DetermineXHealth, int, 0);
			ADD_CFG(HeavyPistol::m_DetermineTarget, int, 0);
			ADD_CFG(HeavyPistol::m_LimitTarget, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageHead, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageNeck, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageChest, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageStomach, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageArms, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageLegs, int, 0);
			ADD_CFG(HeavyPistol::m_MinimumDamageFeet, int, 0);
			ADD_CFG(HeavyPistol::m_ScaleDamageAmount, int, 0);
		} m_HeavyPistol;

		struct Rifle
		{
			ADD_CFG(Rifle::m_Enabled, bool, false);
			ADD_CFG(Rifle::m_AutoFire, bool, true);
			ADD_CFG(Rifle::m_AutoWall, bool, true);
			ADD_CFG(Rifle::m_AutoScope, bool, true);
			ADD_CFG(Rifle::m_AutoStop, bool, true);
			ADD_CFG(Rifle::m_Silent, bool, true);
			ADD_CFG(Rifle::m_ScaleDamage, bool, true);
			ADD_CFG(Rifle::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, Rifle::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, Rifle::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, Rifle::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, Rifle::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, Rifle::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, Rifle::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, Rifle::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, Rifle::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, Rifle::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Rifle::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Rifle::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, Rifle::m_AutoStopConditions, false);

			ADD_CFG(Rifle::m_MultipointHeadScale, int, 60);
			ADD_CFG(Rifle::m_MultipointBodyScale, int, 60);
			ADD_CFG(Rifle::m_MaximumFOV, int, 360);
			ADD_CFG(Rifle::m_MinimumDamage, int, 0);
			ADD_CFG(Rifle::m_HitChanceAmount, int, 60);
			ADD_CFG(Rifle::m_OverrideDamageAmount, int, 0);
			ADD_CFG(Rifle::m_OverrideHitChanceAmount, int, 60);

			ADD_CFG(Rifle::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(Rifle::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(Rifle::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(Rifle::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(Rifle::m_DetermineXMisses, int, 0);
			ADD_CFG(Rifle::m_DetermineXHealth, int, 0);
			ADD_CFG(Rifle::m_DetermineTarget, int, 0);
			ADD_CFG(Rifle::m_LimitTarget, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageHead, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageNeck, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageChest, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageStomach, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageArms, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageLegs, int, 0);
			ADD_CFG(Rifle::m_MinimumDamageFeet, int, 0);
			ADD_CFG(Rifle::m_ScaleDamageAmount, int, 0);
		} m_Rifle;

		struct Scout
		{
			ADD_CFG(Scout::m_Enabled, bool, false);
			ADD_CFG(Scout::m_AutoFire, bool, true);
			ADD_CFG(Scout::m_AutoWall, bool, true);
			ADD_CFG(Scout::m_AutoScope, bool, true);
			ADD_CFG(Scout::m_AutoStop, bool, true);
			ADD_CFG(Scout::m_Silent, bool, true);
			ADD_CFG(Scout::m_ScaleDamage, bool, true);
			ADD_CFG(Scout::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, Scout::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, Scout::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, Scout::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, Scout::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, Scout::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, Scout::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, Scout::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, Scout::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, Scout::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Scout::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Scout::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, Scout::m_AutoStopConditions, false);

			ADD_CFG(Scout::m_MultipointHeadScale, int, 80);
			ADD_CFG(Scout::m_MultipointBodyScale, int, 60);
			ADD_CFG(Scout::m_MaximumFOV, int, 360);
			ADD_CFG(Scout::m_MinimumDamage, int, 80);
			ADD_CFG(Scout::m_HitChanceAmount, int, 80);
			ADD_CFG(Scout::m_OverrideDamageAmount, int, 0);
			ADD_CFG(Scout::m_OverrideHitChanceAmount, int, 80);

			ADD_CFG(Scout::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(Scout::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(Scout::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(Scout::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(Scout::m_DetermineXMisses, int, 0);
			ADD_CFG(Scout::m_DetermineXHealth, int, 0);
			ADD_CFG(Scout::m_DetermineTarget, int, 0);
			ADD_CFG(Scout::m_LimitTarget, int, 0);
			ADD_CFG(Scout::m_MinimumDamageHead, int, 0);
			ADD_CFG(Scout::m_MinimumDamageNeck, int, 0);
			ADD_CFG(Scout::m_MinimumDamageChest, int, 0);
			ADD_CFG(Scout::m_MinimumDamageStomach, int, 0);
			ADD_CFG(Scout::m_MinimumDamageArms, int, 0);
			ADD_CFG(Scout::m_MinimumDamageLegs, int, 0);
			ADD_CFG(Scout::m_MinimumDamageFeet, int, 0);
			ADD_CFG(Scout::m_ScaleDamageAmount, int, 0);
		} m_Scout;

		struct AWP
		{
			ADD_CFG(AWP::m_Enabled, bool, false);
			ADD_CFG(AWP::m_AutoFire, bool, true);
			ADD_CFG(AWP::m_AutoWall, bool, true);
			ADD_CFG(AWP::m_AutoScope, bool, true);
			ADD_CFG(AWP::m_AutoStop, bool, true);
			ADD_CFG(AWP::m_Silent, bool, true);
			ADD_CFG(AWP::m_ScaleDamage, bool, true);
			ADD_CFG(AWP::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, AWP::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, AWP::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, AWP::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, AWP::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, AWP::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, AWP::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, AWP::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, AWP::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, AWP::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, AWP::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, AWP::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, AWP::m_AutoStopConditions, false);

			ADD_CFG(AWP::m_MultipointHeadScale, int, 60);
			ADD_CFG(AWP::m_MultipointBodyScale, int, 80);
			ADD_CFG(AWP::m_MaximumFOV, int, 360);
			ADD_CFG(AWP::m_MinimumDamage, int, 100);
			ADD_CFG(AWP::m_HitChanceAmount, int, 80);
			ADD_CFG(AWP::m_OverrideDamageAmount, int, 0);
			ADD_CFG(AWP::m_OverrideHitChanceAmount, int, 80);

			ADD_CFG(AWP::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(AWP::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(AWP::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(AWP::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(AWP::m_DetermineXMisses, int, 0);
			ADD_CFG(AWP::m_DetermineXHealth, int, 0);
			ADD_CFG(AWP::m_DetermineTarget, int, 0);
			ADD_CFG(AWP::m_LimitTarget, int, 0);
			ADD_CFG(AWP::m_MinimumDamageHead, int, 0);
			ADD_CFG(AWP::m_MinimumDamageNeck, int, 0);
			ADD_CFG(AWP::m_MinimumDamageChest, int, 0);
			ADD_CFG(AWP::m_MinimumDamageStomach, int, 0);
			ADD_CFG(AWP::m_MinimumDamageArms, int, 0);
			ADD_CFG(AWP::m_MinimumDamageLegs, int, 0);
			ADD_CFG(AWP::m_MinimumDamageFeet, int, 0);
			ADD_CFG(AWP::m_ScaleDamageAmount, int, 0);
		} m_AWP;

		struct Auto
		{
			ADD_CFG(Auto::m_Enabled, bool, false);
			ADD_CFG(Auto::m_AutoFire, bool, true);
			ADD_CFG(Auto::m_AutoWall, bool, true);
			ADD_CFG(Auto::m_AutoScope, bool, true);
			ADD_CFG(Auto::m_AutoStop, bool, true);
			ADD_CFG(Auto::m_Silent, bool, true);
			ADD_CFG(Auto::m_ScaleDamage, bool, true);
			ADD_CFG(Auto::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, Auto::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, Auto::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, Auto::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, Auto::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, Auto::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, Auto::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, Auto::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, Auto::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, Auto::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Auto::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Auto::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, Auto::m_AutoStopConditions, false);

			ADD_CFG(Auto::m_MultipointHeadScale, int, 70);
			ADD_CFG(Auto::m_MultipointBodyScale, int, 70);
			ADD_CFG(Auto::m_MaximumFOV, int, 360);
			ADD_CFG(Auto::m_MinimumDamage, int, 20);
			ADD_CFG(Auto::m_HitChanceAmount, int, 70);
			ADD_CFG(Auto::m_OverrideDamageAmount, int, 0);
			ADD_CFG(Auto::m_OverrideHitChanceAmount, int, 70);

			ADD_CFG(Auto::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(Auto::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(Auto::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(Auto::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(Auto::m_DetermineXMisses, int, 0);
			ADD_CFG(Auto::m_DetermineXHealth, int, 0);
			ADD_CFG(Auto::m_DetermineTarget, int, 0);
			ADD_CFG(Auto::m_LimitTarget, int, 0);
			ADD_CFG(Auto::m_MinimumDamageHead, int, 0);
			ADD_CFG(Auto::m_MinimumDamageNeck, int, 0);
			ADD_CFG(Auto::m_MinimumDamageChest, int, 0);
			ADD_CFG(Auto::m_MinimumDamageStomach, int, 0);
			ADD_CFG(Auto::m_MinimumDamageArms, int, 0);
			ADD_CFG(Auto::m_MinimumDamageLegs, int, 0);
			ADD_CFG(Auto::m_MinimumDamageFeet, int, 0);
			ADD_CFG(Auto::m_ScaleDamageAmount, int, 0);
		} m_Auto;

		struct Taser
		{
			ADD_CFG(Taser::m_Enabled, bool, false);
			ADD_CFG(Taser::m_AutoFire, bool, true);
			ADD_CFG(Taser::m_AutoWall, bool, true);
			ADD_CFG(Taser::m_AutoScope, bool, true);
			ADD_CFG(Taser::m_AutoStop, bool, true);
			ADD_CFG(Taser::m_Silent, bool, true);
			ADD_CFG(Taser::m_ScaleDamage, bool, true);
			ADD_CFG(Taser::m_PredictiveDamage, bool, true);

			ADD_VECTOR_CFG(bool, 7, Taser::m_Hitbox, false);
			ADD_VECTOR_CFG(bool, 7, Taser::m_HitboxHistory, false);
			ADD_VECTOR_CFG(bool, 5, Taser::m_BodyAimHitbox, false);
			ADD_VECTOR_CFG(bool, 5, Taser::m_BodyAimHitboxHistory, false);
			ADD_VECTOR_CFG(bool, 7, Taser::m_Multipoint, false);
			ADD_VECTOR_CFG(bool, 7, Taser::m_Optimization, false);
			ADD_VECTOR_CFG(bool, 2, Taser::m_OptimizationDisablers, false);
			ADD_VECTOR_CFG(bool, 3, Taser::m_PreferBodyAim, false);
			ADD_VECTOR_CFG(bool, 3, Taser::m_DisableBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Taser::m_ForceBodyAim, false);
			ADD_VECTOR_CFG(bool, 2, Taser::m_AutoStopFlags, false);
			ADD_VECTOR_CFG(bool, 1, Taser::m_AutoStopConditions, false);

			ADD_CFG(Taser::m_MultipointHeadScale, int, 20);
			ADD_CFG(Taser::m_MultipointBodyScale, int, 70);
			ADD_CFG(Taser::m_MaximumFOV, int, 360);
			ADD_CFG(Taser::m_MinimumDamage, int, 100);
			ADD_CFG(Taser::m_HitChanceAmount, int, 0);
			ADD_CFG(Taser::m_OverrideDamageAmount, int, 0);
			ADD_CFG(Taser::m_OverrideHitChanceAmount, int, 0);

			ADD_CFG(Taser::m_HistoryHitboxSelection, bool, false);
			ADD_CFG(Taser::m_HitGroupMinimumDamage, bool, false);
			ADD_CFG(Taser::m_HighestAccuracyCheck, bool, false);
			ADD_CFG(Taser::m_DetermineXMissesForceBody, int, 0);
			ADD_CFG(Taser::m_DetermineXMisses, int, 0);
			ADD_CFG(Taser::m_DetermineXHealth, int, 0);
			ADD_CFG(Taser::m_DetermineTarget, int, 0);
			ADD_CFG(Taser::m_LimitTarget, int, 0);
			ADD_CFG(Taser::m_MinimumDamageHead, int, 0);
			ADD_CFG(Taser::m_MinimumDamageNeck, int, 0);
			ADD_CFG(Taser::m_MinimumDamageChest, int, 0);
			ADD_CFG(Taser::m_MinimumDamageStomach, int, 0);
			ADD_CFG(Taser::m_MinimumDamageArms, int, 0);
			ADD_CFG(Taser::m_MinimumDamageLegs, int, 0);
			ADD_CFG(Taser::m_MinimumDamageFeet, int, 0);
			ADD_CFG(Taser::m_ScaleDamageAmount, int, 0);
		} m_Taser;
	} m_Aimbot;

	struct AntiAim
	{
		struct General
		{
			ADD_CFG(General::m_Enable, bool, false);
			ADD_CFG(General::m_AntiUntrusted, bool, true);
		} m_General;

		struct Stand
		{
			struct StandFakelag
			{
				ADD_CFG(StandFakelag::m_Enable, bool, false);
				ADD_CFG(StandFakelag::m_Limit, int, 0);
			} m_Fakelag;

			ADD_CFG(Stand::m_Pitch, int, 1);
			ADD_CFG(Stand::m_Yaw, int, 1);
			ADD_CFG(Stand::m_FakeYaw, int, 1);
			ADD_CFG(Stand::m_LowerbodyBreakAmount, int, 100);
			ADD_CFG(Stand::m_HideShots, bool, true);

			ADD_CFG(Stand::m_Freestanding, bool, true);
			ADD_CFG(Stand::m_LowerbodyBreak, bool, true);
		} m_Stand;

		struct Move
		{
			struct MoveFakelag
			{
				ADD_CFG(MoveFakelag::m_Enable, bool, true);
				ADD_CFG(MoveFakelag::m_Limit, int, 14);
			} m_Fakelag;

			ADD_CFG(Move::m_Pitch, int, 1);
			ADD_CFG(Move::m_Yaw, int, 1);
			ADD_CFG(Move::m_FakeYaw, int, 1);
			ADD_CFG(Move::m_HideShots, bool, false);

			ADD_CFG(Move::m_Freestanding, bool, 1);
		} m_Move;

		struct Air
		{
			struct AirFakelag
			{
				ADD_CFG(AirFakelag::m_Enable, bool, true);
				ADD_CFG(AirFakelag::m_Limit, int, 14);
			} m_Fakelag;

			ADD_CFG(Air::m_Pitch, int, 1);
			ADD_CFG(Air::m_Yaw, int, 1);
			ADD_CFG(Air::m_FakeYaw, int, 1);
			ADD_CFG(Air::m_HideShots, bool, false);

			ADD_CFG(Air::m_Freestanding, bool, false);
		} m_Air;
	} m_AntiAim;

	struct Misc
	{
		ADD_VECTOR_CFG(bool, 5, Misc::m_Notifications, false);
		ADD_VECTOR_CFG(bool, 22, Misc::m_AutoBuyMain, false);
		ADD_VECTOR_CFG(bool, 8, Misc::m_AutoBuySecondary, false);
		ADD_VECTOR_CFG(bool, 11, Misc::m_AutoBuyMisc, false);

		ADD_CFG(Misc::m_AutomaticJump, bool, false);
		ADD_CFG(Misc::m_AutomaticStrafe, bool, false);

		ADD_CFG(Misc::m_ClantagSpammer, int, 0);

		ADD_CFG(Misc::m_FakeLatency, bool, false);
		ADD_CFG(Misc::m_FakeLatencyAmount, int, 200);
		ADD_CFG(Misc::m_FakeLatencyKey, int, 0);
		ADD_CFG(Misc::m_FakeLatencyMode, int, 0);

		ADD_CFG(Misc::m_FakeWalk, bool, false);
		ADD_CFG(Misc::m_FakeWalkKeyBinds, int, 0);
		ADD_CFG(Misc::m_FakeWalkBindsMode, int, 0);

		ADD_CFG(Misc::m_AutomaticPeek, bool, false);
		ADD_CFG(Misc::m_AutomaticPeekKeyBinds, int, 0);
		ADD_CFG(Misc::m_AutomaticPeekKeyBindsMode, int, 0);
		ADD_CFG(Misc::m_UnlockHiddenCvars, bool, false);
	} m_Misc;

	struct Visuals
	{
		struct Enemies
		{
			ADD_VECTOR_CFG(bool, 7, Enemies::m_Flags, false);
			ADD_CFG(Enemies::m_Enabled, bool, false);
			ADD_CFG(Enemies::m_DormantEnabled, bool, false);
			ADD_CFG(Enemies::m_BoxEnabled, bool, false);
			ADD_CFG(Enemies::m_BoxType, int, 0);
			ADD_CFG(Enemies::m_BoxColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Enemies::m_FlashEffectEnabled, bool, false);
			ADD_CFG(Enemies::m_NameEnabled, bool, false);
			ADD_CFG(Enemies::m_HealthBarEnabled, bool, false);
			ADD_CFG(Enemies::m_HealthBarOverrideEnabled, bool, false);
			ADD_CFG(Enemies::m_AmmoBarEnabled, bool, false);
			ADD_CFG(Enemies::m_LBYBarEnabled, bool, false);
			ADD_CFG(Enemies::m_LBYColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Enemies::m_DistanceEnabled, bool, false);
			ADD_CFG(Enemies::m_WeaponTextEnabled, bool, false);
			ADD_CFG(Enemies::m_WeaponIconEnabled, bool, false);
			ADD_CFG(Enemies::m_SkeletonEnabled, bool, false);
			ADD_CFG(Enemies::m_GlowEnabled, bool, false);
			ADD_CFG(Enemies::m_GlowColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Enemies::m_HealthBarOverrideColor, Color, Color(255, 255, 255, 255));

			struct EnemiesChams
			{
				ADD_VECTOR_CFG(bool, 2, EnemiesChams::m_ActivationType, false);
				ADD_VECTOR_CFG(bool, 2, EnemiesChams::m_HistoryActivationType, false);

				ADD_CFG(EnemiesChams::m_ModelSelection, int, 0);
				ADD_CFG(EnemiesChams::m_VisibleColor, Color, Color(255, 255, 255, 255));
				ADD_CFG(EnemiesChams::m_BehindWallColor, Color, Color(255, 255, 255, 255));

				ADD_CFG(EnemiesChams::m_HistoryModelSelection, int, 0);
				ADD_CFG(EnemiesChams::m_HistoryVisibleColor, Color, Color(255, 255, 255, 255));
				ADD_CFG(EnemiesChams::m_HistoryBehindWallColor, Color, Color(255, 255, 255, 255));
			} m_Chams;
		} m_Enemies;

		struct Allies
		{
			ADD_VECTOR_CFG(bool, 7, Allies::m_Flags, false);
			ADD_CFG(Allies::m_Enabled, bool, false);
			ADD_CFG(Allies::m_DormantEnabled, bool, false);
			ADD_CFG(Allies::m_BoxEnabled, bool, false);
			ADD_CFG(Allies::m_BoxType, int, 0);
			ADD_CFG(Allies::m_BoxColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Allies::m_FlashEffectEnabled, bool, false);
			ADD_CFG(Allies::m_NameEnabled, bool, false);
			ADD_CFG(Allies::m_HealthBarEnabled, bool, false);
			ADD_CFG(Allies::m_HealthBarOverrideEnabled, bool, false);
			ADD_CFG(Allies::m_AmmoBarEnabled, bool, false);
			ADD_CFG(Allies::m_LBYBarEnabled, bool, false);
			ADD_CFG(Allies::m_LBYColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Allies::m_DistanceEnabled, bool, false);
			ADD_CFG(Allies::m_WeaponTextEnabled, bool, false);
			ADD_CFG(Allies::m_WeaponIconEnabled, bool, false);
			ADD_CFG(Allies::m_SkeletonEnabled, bool, false);
			ADD_CFG(Allies::m_GlowEnabled, bool, false);
			ADD_CFG(Allies::m_GlowColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Allies::m_HealthBarOverrideColor, Color, Color(255, 255, 255, 255));

			struct AlliesChams
			{
				ADD_VECTOR_CFG(bool, 2, AlliesChams::m_ActivationType, false);
				ADD_VECTOR_CFG(bool, 2, AlliesChams::m_HistoryActivationType, false);

				ADD_CFG(AlliesChams::m_ModelSelection, int, 0);
				ADD_CFG(AlliesChams::m_VisibleColor, Color, Color(255, 255, 255, 255));
				ADD_CFG(AlliesChams::m_BehindWallColor, Color, Color(255, 255, 255, 255));
			} m_Chams;
		} m_Allies;

		struct Local
		{
			ADD_VECTOR_CFG(bool, 7, Local::m_Flags, false);
			ADD_CFG(Local::m_Enabled, bool, false);
			ADD_CFG(Local::m_DormantEnabled, bool, false);
			ADD_CFG(Local::m_BoxEnabled, bool, false);
			ADD_CFG(Local::m_BoxType, int, 0);
			ADD_CFG(Local::m_BoxColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Local::m_FlashEffectEnabled, bool, false);
			ADD_CFG(Local::m_NameEnabled, bool, false);
			ADD_CFG(Local::m_HealthBarEnabled, bool, false);
			ADD_CFG(Local::m_HealthBarOverrideEnabled, bool, false);
			ADD_CFG(Local::m_AmmoBarEnabled, bool, false);
			ADD_CFG(Local::m_LBYBarEnabled, bool, false);
			ADD_CFG(Local::m_LBYColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Local::m_DistanceEnabled, bool, false);
			ADD_CFG(Local::m_WeaponTextEnabled, bool, false);
			ADD_CFG(Local::m_WeaponIconEnabled, bool, false);
			ADD_CFG(Local::m_SkeletonEnabled, bool, false);
			ADD_CFG(Local::m_GlowEnabled, bool, false);
			ADD_CFG(Local::m_GlowColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Local::m_HealthBarOverrideColor, Color, Color(255, 255, 255, 255));

			struct LocalChams
			{
				ADD_VECTOR_CFG(bool, 2, LocalChams::m_ActivationType, false);
				ADD_VECTOR_CFG(bool, 2, LocalChams::m_FakelagActivationType, false);

				ADD_CFG(LocalChams::m_ModelSelection, int, 0);
				ADD_CFG(LocalChams::m_VisibleColor, Color, Color(255, 255, 255, 255));
				ADD_CFG(LocalChams::m_BehindWallColor, Color, Color(255, 255, 255, 255));

				ADD_CFG(LocalChams::m_FakelagModelSelection, int, 0);
				ADD_CFG(LocalChams::m_FakelagVisibleColor, Color, Color(255, 255, 255, 255));
				ADD_CFG(LocalChams::m_FakelagBehindWallColor, Color, Color(255, 255, 255, 255));

				ADD_CFG(LocalChams::m_BlendWhenScoped, bool, false);
			} m_Chams;
		} m_Local;

		struct World {
			ADD_VECTOR_CFG(bool, 3, World::m_BrightnessAdjustment, false);
			ADD_VECTOR_CFG(bool, 2, World::m_ProjectileType, false);
			ADD_VECTOR_CFG(bool, 4, World::m_ItemFlags, false);

			ADD_CFG(World::m_BulletImpactServer, bool, false);
			ADD_CFG(World::m_BulletImpactClient, bool, false);
			ADD_CFG(World::m_TransparentProps, bool, false);
			ADD_CFG(World::m_ProjectileEnabled, bool, false);
			ADD_CFG(World::m_ItemsEnabled, bool, false);

			ADD_CFG(World::m_BulletImpactServerColor, Color, Color(0, 0, 255, 255));
			ADD_CFG(World::m_BulletImpactClientColor, Color, Color(255, 0, 0, 255));
			ADD_CFG(World::m_PropColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(World::m_SkyColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(World::m_WorldColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(World::m_ProjectileColor, Color, Color(255, 255, 255, 255));

			ADD_CFG(World::m_TransparentPropsAmount, int, 0);
		} m_World;

		struct Effects {
			ADD_VECTOR_CFG(bool, 12, Effects::m_Removals, false);

			ADD_CFG(Effects::m_OverrideFieldOfView, bool, false);

			ADD_CFG(Effects::m_OverrideFieldOfViewAmount, int, 90);
			ADD_CFG(Effects::m_OverrideFieldOfViewAmountScoped, int, 50);
			ADD_CFG(Effects::m_OverrideFieldOfViewAmountZoomed, int, 75);

			ADD_CFG(Effects::m_OverrideViewModel, bool, false);
			ADD_CFG(Effects::m_OverrideViewModelFov, int, 70);
			ADD_CFG(Effects::m_EnableHitSound, bool, false);

			ADD_CFG(Effects::m_OverrideFog, bool, false);
			ADD_CFG(Effects::m_FogColor, Color, Color(255, 255, 255, 255));
			ADD_CFG(Effects::m_FogStart, int, 0);
			ADD_CFG(Effects::m_FogEnd, int, 0);
			ADD_CFG(Effects::m_FogDensity, float, 0.f);
			ADD_CFG(Effects::m_PartyMode, bool, false);
		} m_Effects;

		ADD_VECTOR_CFG(bool, 7, Visuals::m_Indicators, false);
		ADD_VECTOR_CFG(bool, 5, Visuals::m_OffScreenFlags, false);
		ADD_CFG(Visuals::m_DamageEnabled, bool, false);
		ADD_CFG(Visuals::m_HitmarkerEnabled, bool, false);
		ADD_CFG(Visuals::m_PreverseKillfeedEnabled, bool, false);
		ADD_CFG(Visuals::m_ThirdpersonEnabled, bool, false);
		ADD_CFG(Visuals::m_ThirdpersonKey, int, 0);
		ADD_CFG(Visuals::m_ThirdpersonKeyMode, int, 0);
		ADD_CFG(Visuals::m_ThirdpersonDistance, int, 100);
		ADD_CFG(Visuals::m_OffScreenEnabled, bool, false);
		ADD_CFG(Visuals::m_OffScreenSize, float, 0.f);
		ADD_CFG(Visuals::m_OffScreenColor, Color, Color(255, 255, 255, 255));
		ADD_CFG(Visuals::m_OffScreenDistance, float, 0.f);
		ADD_CFG(Visuals::m_PenCrosshairEnabled, bool, false);
		ADD_CFG(Visuals::m_PenCrosshairDmgEnabled, bool, false);
		ADD_CFG(Visuals::m_PenCrosshairType, int, 0);
		ADD_CFG(Visuals::m_PenColor, Color, Color(255, 255, 255, 255));
		ADD_CFG(Visuals::m_DisableModelOcculusion, bool, false);
		ADD_CFG(Visuals::m_ForceCrosshair, bool, false);
		ADD_CFG(Visuals::m_AspectRatio, bool, false);
		ADD_CFG(Visuals::m_AspectRatioAmount, float, 1.f);
		ADD_CFG(Visuals::m_AutoPeekIndicator, bool, false);
		ADD_CFG(Visuals::m_AutoPeekIndicatorColor, Color, Color(255, 255, 255, 255));
	} m_Visuals;

	ADD_CFG(m_HitmarkerType, int, 0);

	ADD_CFG(m_Theme, int, 0);

	ADD_CFG(m_KeybindX, int, 0);
	ADD_CFG(m_KeybindY, int, 0);
	ADD_CFG(m_SpecX, int, 0);
	ADD_CFG(m_SpecY, int, 0);

	ADD_CFG(m_GradientStyle, int, 0);

	ADD_CFG(m_AnimationSpeed, float, 3.f);

	// colors
	ADD_CFG(m_AccentColor, Color, Color(255, 255, 255, 255));
	ADD_CFG(m_AccentColor2, Color, Color(255, 255, 255, 255));
};

inline Items g_Items;