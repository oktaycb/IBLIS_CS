#pragma once

class Offsets {
public:
	class NetVar {
	public:
		uint32_t m_offset;

		// default ctor.
		__forceinline NetVar() : m_offset{ 0u } {}

		// init offset.
		__forceinline void init(const size_t offset) {
			m_offset = offset;
		}

		// returns the real netvar offset at runtime.
		__forceinline size_t get() const {
			return m_offset;
		}

		__forceinline operator size_t() const {
			return get();
		}
	};
public:
	NetVar m_vecMins;
	NetVar m_vecMaxs;
	NetVar m_Collision;
	NetVar m_CollisionGroup;
	NetVar m_nPrecipType;
	NetVar m_nMaterialHandle;
	NetVar m_iPlayerC4;
	NetVar m_bKilledByTaser;
	NetVar m_nNextThinkTick;
	NetVar m_vecOrigin;
	NetVar m_vecOldOrigin;
	NetVar m_flGravity;
	NetVar m_vecVelocity;
	NetVar m_iTeamNum;
	NetVar m_nSequence;
	NetVar m_nPredictedViewModelSequence;
	NetVar m_flPlayerCycle;
	NetVar m_flCycle;
	NetVar m_flAnimTime;
	NetVar m_flPlaybackRate;
	NetVar m_flC4Blow;
	NetVar m_bBombTicking;
	NetVar m_nForceBone;
	NetVar m_hPlayer;
	NetVar m_fEffects;
	NetVar m_nModelIndex;
	NetVar m_bReadyToDraw;
	NetVar m_vecAbsVelocity;
	NetVar m_lifeState;
	NetVar m_fFlags;
	NetVar m_flFallVelocity;
	NetVar m_MoveType;
	NetVar m_vecLadderNormal;
	NetVar m_bStrafing;
	NetVar m_iMoveState;
	NetVar m_iHealth;
	NetVar m_flThirdpersonRecoil;
	NetVar m_iAccount;
	NetVar m_hViewModel;
	NetVar m_nViewModelIndex;
	NetVar m_bHasDefuser;
	NetVar m_bIsDefusing;
	NetVar m_nHitboxSet;
	NetVar m_angAbsRotation;
	NetVar m_angRotation;
	NetVar m_angNetworkAngles;
	NetVar m_bUseNewAnimstate;
	NetVar m_PlayerAnimState;
	NetVar m_nTickBase;
	NetVar m_nFinalPredictedTick;
	NetVar m_flNextAttack;
	NetVar m_flDuckAmount;
	NetVar m_flDuckSpeed;
	NetVar m_vphysicsCollisionState;
	NetVar m_nSimulationTick;
	NetVar m_bPredictable;
	NetVar m_CommandContext;
	NetVar m_flSimulationTime;
	NetVar m_flOldSimulationTime;
	NetVar m_flLowerBodyYawTarget;
	NetVar m_bWaitForNoAttack;
	NetVar m_iThrowGrenadeCounter;
	NetVar m_fImmuneToGunGameDamageTime;
	NetVar m_bHasHelmet;
	NetVar m_bClientSideAnimation;
	NetVar m_bHasHeavyArmor;
	NetVar m_bIsScoped;
	NetVar m_iPing;
	NetVar m_bIsWalking;
	NetVar m_bDucking;
	NetVar m_bSpotted;
	NetVar m_iObserverMode;
	NetVar m_ArmorValue;
	NetVar m_flMaxspeed;
	NetVar m_surfaceFriction;
	NetVar m_flFlashBangTime;
	NetVar m_flFlashDuration;
	NetVar m_flTimeOfLastInjury;
	NetVar m_nRelativeDirectionOfLastInjury;
	NetVar m_fireCount;
	NetVar m_LastHitGroup;
	NetVar m_angEyeAngles;
	NetVar m_aimPunchAngle;
	NetVar m_viewPunchAngle;
	NetVar m_aimPunchAngleVel;
	NetVar m_vecViewOffset;
	NetVar m_LastCommand;
	NetVar m_pCurrentCommand;
	NetVar m_iEFlags;
	NetVar m_pRagdoll;
	NetVar m_bClientSideRagdoll;
	NetVar m_flPoseParameter;
	NetVar m_hMyWearables;
	NetVar m_BoneCache;
	NetVar m_hObserverTarget;
	NetVar m_hActiveWeapon;
	NetVar m_rgflCoordinateFrame;
	NetVar m_hGroundEntity;
	NetVar m_hVehicle;
	NetVar m_CustomMaterials;
	NetVar m_CustomMaterials2;
	NetVar m_VisualsDataProcessors;
	NetVar m_bCustomMaterialInitialized;
	NetVar m_iAccumulatedBoneMask;
	NetVar m_iPrevBoneMask;
	NetVar m_iItemDefinitionIndex;
	NetVar m_bPlayerUnderwater;
	NetVar m_iClip1;
	NetVar m_iPrimaryReserveAmmoCount;
	NetVar m_Activity;
	NetVar m_fFireDuration;
	NetVar m_iBurstShotsRemaining;
	NetVar m_flNextPrimaryAttack;
	NetVar m_flNextSecondaryAttack;
	NetVar m_flThrowStrength;
	NetVar m_fNextBurstShot;
	NetVar m_zoomLevel;
	NetVar m_flRecoilIndex;
	NetVar m_fAccuracyPenalty;
	NetVar m_weaponMode;
	NetVar m_nFallbackPaintKit;
	NetVar m_nFallbackStatTrak;
	NetVar m_nFallbackSeed;
	NetVar m_flFallbackWear;
	NetVar m_iViewModelIndex;
	NetVar m_nWeaponViewModelIndex;
	NetVar m_iWorldModelIndex;
	NetVar m_iAccountID;
	NetVar m_iItemIDHigh;
	NetVar m_iEntityQuality;
	NetVar m_OriginalOwnerXuidLow;
	NetVar m_OriginalOwnerXuidHigh;
	NetVar m_bPinPulled;
	NetVar m_fThrowTime;
	NetVar m_hWeapon;
	NetVar m_hWeaponWorldModel;
	NetVar m_fLastShotTime;
	NetVar m_flPostponeFireReadyTime;
	NetVar m_hOwnerEntity;
	NetVar m_hOwner;
	NetVar m_flDeathYaw;
	NetVar m_flAbsYaw;
	NetVar m_flConstraintRadius;
	NetVar m_hMyWeapons;
	NetVar m_flVelocityModifier;
	NetVar m_afButtonForced;
	NetVar m_afButtonDisabled;
	NetVar m_bInBuyZone;
	NetVar m_VarMap;
	NetVar m_nSmokeEffectTickBegin;
	NetVar m_bDidSmokeEffect;
	NetVar m_nExplodeEffectTickBegin;
	NetVar m_hThrower;
	NetVar m_flSpawnTime_Grenade;
	NetVar m_nLastNonSkippedFrame;
	NetVar m_nComputedLODframe;
	NetVar m_nAnimLODflags;
	NetVar m_nAnimLODflagsOld;
	NetVar m_EntClientFlags;
	NetVar m_bIsToolRecording;
	NetVar m_pIk;
	NetVar m_nCustomBlendingRuleMask;
public:
	void Init() {
#define NETVAR( member_var, table_hash, prop_hash ) \
				 member_var.init( g_Netvars.get( table_hash, prop_hash ) );

#define OFFSET( member_var, offset ) \
				 member_var.init( offset );

		NETVAR(m_iPlayerC4, HASH("DT_CSPlayerResource"), HASH("m_iPlayerC4"));
		NETVAR(m_hThrower, HASH("DT_BaseCSGrenade"), HASH("m_hThrower"));
		NETVAR(m_nExplodeEffectTickBegin, HASH("DT_BaseCSGrenadeProjectile"), HASH("m_nExplodeEffectTickBegin"));
		NETVAR(m_bDidSmokeEffect, HASH("DT_SmokeGrenadeProjectile"), HASH("m_bDidSmokeEffect"));
		NETVAR(m_nSmokeEffectTickBegin, HASH("DT_SmokeGrenadeProjectile"), HASH("m_nSmokeEffectTickBegin"));
		NETVAR(m_bInBuyZone, HASH("DT_CSPlayer"), HASH("m_bInBuyZone"));
		NETVAR(m_bKilledByTaser, HASH("DT_CSPlayer"), HASH("m_bKilledByTaser"));
		NETVAR(m_nNextThinkTick, HASH("DT_BaseCombatWeapon"), HASH("m_nNextThinkTick"));
		NETVAR(m_vecOrigin, HASH("DT_BasePlayer"), HASH("m_vecOrigin"));
		NETVAR(m_vecVelocity, HASH("DT_CSPlayer"), HASH("m_vecVelocity[0]"));
		NETVAR(m_iTeamNum, HASH("DT_BaseEntity"), HASH("m_iTeamNum"));
		NETVAR(m_nPredictedViewModelSequence, HASH("DT_PredictedViewModel"), HASH("m_nSequence"));
		NETVAR(m_flPlayerCycle, HASH("DT_CSPlayer"), HASH("m_flCycle"));
		NETVAR(m_nSequence, HASH("DT_BaseAnimating"), HASH("m_nSequence"));
		NETVAR(m_flCycle, HASH("DT_BaseAnimating"), HASH("m_flCycle"));
		NETVAR(m_flAnimTime, HASH("DT_CSPlayer"), HASH("m_flAnimTime"));
		NETVAR(m_flPlaybackRate, HASH("DT_BaseAnimating"), HASH("m_flPlaybackRate"));
		NETVAR(m_flC4Blow, HASH("DT_PlantedC4"), HASH("m_flC4Blow"));
		NETVAR(m_bBombTicking, HASH("DT_PlantedC4"), HASH("m_bBombTicking"));
		NETVAR(m_nModelIndex, HASH("DT_BasePlayer"), HASH("m_nModelIndex"));
		NETVAR(m_lifeState, HASH("DT_BasePlayer"), HASH("m_lifeState"));
		NETVAR(m_fFlags, HASH("DT_BasePlayer"), HASH("m_fFlags"));
		NETVAR(m_flFallVelocity, HASH("DT_BasePlayer"), HASH("m_flFallVelocity"));
		NETVAR(m_MoveType, HASH("C_BaseEntity"), HASH("m_MoveType"));
		NETVAR(m_vecLadderNormal, HASH("DT_BasePlayer"), HASH("m_vecLadderNormal"));
		NETVAR(m_bStrafing, HASH("DT_CSPlayer"), HASH("m_bStrafing"));
		NETVAR(m_iMoveState, HASH("DT_CSPlayer"), HASH("m_iMoveState"));
		NETVAR(m_iHealth, HASH("DT_BasePlayer"), HASH("m_iHealth"));
		NETVAR(m_flThirdpersonRecoil, HASH("DT_CSPlayer"), HASH("m_flThirdpersonRecoil"));
		NETVAR(m_iAccount, HASH("DT_CSPlayer"), HASH("m_iAccount"));
		NETVAR(m_hViewModel, HASH("DT_BasePlayer"), HASH("m_hViewModel[0]"));
		NETVAR(m_nViewModelIndex, HASH("DT_BaseViewModel"), HASH("m_nViewModelIndex"));
		NETVAR(m_bHasDefuser, HASH("DT_CSPlayer"), HASH("m_bHasDefuser"));
		NETVAR(m_bIsDefusing, HASH("DT_CSPlayer"), HASH("m_bIsDefusing"));
		NETVAR(m_flGravity, HASH("DT_CSPlayer"), HASH("m_iTeamNum") - 0x14);
		NETVAR(m_nForceBone, HASH("DT_BaseAnimating"), HASH("m_nForceBone"));
		NETVAR(m_nHitboxSet, HASH("DT_BaseAnimating"), HASH("m_nHitboxSet"));
		NETVAR(m_angAbsRotation, HASH("C_BaseEntity"), HASH("m_angAbsRotation"));
		NETVAR(m_angRotation, HASH("C_BaseEntity"), HASH("m_angRotation"));
		NETVAR(m_angNetworkAngles, HASH("C_BaseEntity"), HASH("m_angNetworkAngles"));
		NETVAR(m_nTickBase, HASH("DT_CSPlayer"), HASH("m_nTickBase"));
		NETVAR(m_flNextAttack, HASH("DT_CSPlayer"), HASH("m_flNextAttack"));
		NETVAR(m_flDuckAmount, HASH("DT_BasePlayer"), HASH("m_flDuckAmount"));
		NETVAR(m_flDuckSpeed, HASH("DT_BasePlayer"), HASH("m_flDuckSpeed"));
		NETVAR(m_flSimulationTime, HASH("DT_BaseEntity"), HASH("m_flSimulationTime"));
		NETVAR(m_flLowerBodyYawTarget, HASH("DT_CSPlayer"), HASH("m_flLowerBodyYawTarget"));
		NETVAR(m_bWaitForNoAttack, HASH("DT_CSPlayer"), HASH("m_bWaitForNoAttack"));
		NETVAR(m_iThrowGrenadeCounter, HASH("DT_CSPlayer"), HASH("m_iThrowGrenadeCounter"));
		NETVAR(m_fImmuneToGunGameDamageTime, HASH("DT_CSPlayer"), HASH("m_fImmuneToGunGameDamageTime"));
		NETVAR(m_bHasHelmet, HASH("DT_CSPlayer"), HASH("m_bHasHelmet"));
		NETVAR(m_bClientSideAnimation, HASH("DT_CSPlayer"), HASH("m_bClientSideAnimation"));
		NETVAR(m_bHasHeavyArmor, HASH("DT_CSPlayer"), HASH("m_bHasHeavyArmor"));
		NETVAR(m_bIsScoped, HASH("DT_CSPlayer"), HASH("m_bIsScoped"));
		NETVAR(m_bIsWalking, HASH("DT_CSPlayer"), HASH("m_bIsWalking"));
		NETVAR(m_bDucking, HASH("DT_CSPlayer"), HASH("m_bDucking"));
		NETVAR(m_iPing, HASH("DT_CSPlayerResource"), HASH("m_iPing"));
		NETVAR(m_bSpotted, HASH("DT_BaseEntity"), HASH("m_bSpotted"));
		NETVAR(m_iObserverMode, HASH("DT_BasePlayer"), HASH("m_iObserverMode"));
		NETVAR(m_ArmorValue, HASH("DT_BasePlayer"), HASH("m_ArmorValue"));
		NETVAR(m_flMaxspeed, HASH("DT_BasePlayer"), HASH("m_flMaxspeed"));
		NETVAR(m_surfaceFriction, HASH("C_BasePlayer"), HASH("m_surfaceFriction"));
		NETVAR(m_angEyeAngles, HASH("DT_CSPlayer"), HASH("m_angEyeAngles"));
		NETVAR(m_aimPunchAngle, HASH("DT_CSPlayer"), HASH("m_aimPunchAngle"));
		NETVAR(m_viewPunchAngle, HASH("DT_CSPlayer"), HASH("m_viewPunchAngle"));
		NETVAR(m_aimPunchAngleVel, HASH("DT_CSPlayer"), HASH("m_aimPunchAngleVel"));
		NETVAR(m_vecViewOffset, HASH("DT_CSPlayer"), HASH("m_vecViewOffset[0]"));
		NETVAR(m_flPoseParameter, HASH("DT_CSPlayer"), HASH("m_flPoseParameter"));
		NETVAR(m_hMyWearables, HASH("DT_BaseCombatCharacter"), HASH("m_hMyWearables"));
		NETVAR(m_hObserverTarget, HASH("DT_CSPlayer"), HASH("m_hObserverTarget"));
		NETVAR(m_hActiveWeapon, HASH("DT_BaseCombatCharacter"), HASH("m_hActiveWeapon"));
		NETVAR(m_flTimeOfLastInjury, HASH("DT_BaseCombatCharacter"), HASH("m_flTimeOfLastInjury"));
		NETVAR(m_nRelativeDirectionOfLastInjury, HASH("DT_CSPlayer"), HASH("m_nRelativeDirectionOfLastInjury"));
		NETVAR(m_fireCount, HASH("DT_Inferno"), HASH("m_fireCount"));
		NETVAR(m_LastHitGroup, HASH("DT_BasePlayer"), HASH("m_LastHitGroup"));
		NETVAR(m_hGroundEntity, HASH("DT_CSPlayer"), HASH("m_hGroundEntity"));
		NETVAR(m_hVehicle, HASH("DT_BasePlayer"), HASH("m_hVehicle"));
		NETVAR(m_iItemDefinitionIndex, HASH("DT_BaseAttributableItem"), HASH("m_iItemDefinitionIndex"));
		NETVAR(m_iClip1, HASH("DT_BaseCombatWeapon"), HASH("m_iClip1"));
		NETVAR(m_iPrimaryReserveAmmoCount, HASH("DT_BaseCombatWeapon"), HASH("m_iPrimaryReserveAmmoCount"));
		NETVAR(m_Activity, HASH("CBaseCombatWeapon"), HASH("m_Activity"));
		NETVAR(m_fFireDuration, HASH("CBaseCombatWeapon"), HASH("m_fFireDuration"));
		NETVAR(m_iBurstShotsRemaining, HASH("CWeaponCSBaseGun"), HASH("m_iBurstShotsRemaining"));
		NETVAR(m_flNextPrimaryAttack, HASH("DT_BaseCombatWeapon"), HASH("m_flNextPrimaryAttack"));
		NETVAR(m_flNextSecondaryAttack, HASH("DT_BaseCombatWeapon"), HASH("m_flNextSecondaryAttack"));
		NETVAR(m_flThrowStrength, HASH("DT_BaseCSGrenade"), HASH("m_flThrowStrength"));
		NETVAR(m_fNextBurstShot, HASH("CWeaponCSBaseGun"), HASH("m_fNextBurstShot"));
		NETVAR(m_zoomLevel, HASH("CWeaponCSBaseGun"), HASH("m_zoomLevel"));
		NETVAR(m_flRecoilIndex, HASH("DT_WeaponCSBase"), HASH("m_flRecoilIndex"));
		NETVAR(m_fAccuracyPenalty, HASH("DT_WeaponCSBase"), HASH("m_fAccuracyPenalty"));
		NETVAR(m_weaponMode, HASH("CWeaponCSBase"), HASH("m_weaponMode"));
		NETVAR(m_nFallbackPaintKit, HASH("DT_BaseAttributableItem"), HASH("m_nFallbackPaintKit"));
		NETVAR(m_nFallbackStatTrak, HASH("DT_BaseAttributableItem"), HASH("m_nFallbackStatTrak"));
		NETVAR(m_nFallbackSeed, HASH("DT_BaseAttributableItem"), HASH("m_nFallbackSeed"));
		NETVAR(m_flFallbackWear, HASH("DT_BaseAttributableItem"), HASH("m_flFallbackWear"));
		NETVAR(m_nWeaponViewModelIndex, HASH("DT_BaseCombatWeapon"), HASH("m_nViewModelIndex"));
		NETVAR(m_iViewModelIndex, HASH("DT_BaseCombatWeapon"), HASH("m_iViewModelIndex"));
		NETVAR(m_iWorldModelIndex, HASH("DT_BaseCombatWeapon"), HASH("m_iWorldModelIndex"));
		NETVAR(m_iAccountID, HASH("DT_BaseCombatWeapon"), HASH("m_iAccountID"));
		NETVAR(m_iItemIDHigh, HASH("DT_BaseCombatWeapon"), HASH("m_iItemIDHigh"));
		NETVAR(m_iEntityQuality, HASH("DT_BaseCombatWeapon"), HASH("m_iEntityQuality"));
		NETVAR(m_OriginalOwnerXuidLow, HASH("DT_BaseAttributableItem"), HASH("m_OriginalOwnerXuidLow"));
		NETVAR(m_OriginalOwnerXuidHigh, HASH("DT_BaseAttributableItem"), HASH("m_OriginalOwnerXuidHigh"));
		NETVAR(m_bPinPulled, HASH("DT_BaseCSGrenade"), HASH("m_bPinPulled"));
		NETVAR(m_fThrowTime, HASH("DT_BaseCSGrenade"), HASH("m_fThrowTime"));
		NETVAR(m_hWeapon, HASH("DT_PredictedViewModel"), HASH("m_hWeapon"));
		NETVAR(m_hWeaponWorldModel, HASH("DT_BaseCombatWeapon"), HASH("m_hWeaponWorldModel"));
		NETVAR(m_hOwnerEntity, HASH("DT_CSPlayer"), HASH("m_hOwnerEntity"));
		NETVAR(m_hOwner, HASH("DT_BaseViewModel"), HASH("m_hOwner"));
		NETVAR(m_fLastShotTime, HASH("DT_WeaponCSBaseGun"), HASH("m_fLastShotTime"));
		NETVAR(m_flPostponeFireReadyTime, HASH("DT_WeaponCSBase"), HASH("m_flPostponeFireReadyTime"));
		NETVAR(m_hPlayer, HASH("DT_CSRagdoll"), HASH("m_hPlayer"));
		NETVAR(m_flDeathYaw, HASH("DT_CSRagdoll"), HASH("m_flDeathYaw"));
		NETVAR(m_flAbsYaw, HASH("DT_CSRagdoll"), HASH("m_flAbsYaw"));
		NETVAR(m_flConstraintRadius, HASH("DT_CSPlayer"), HASH("m_flConstraintRadius"));
		NETVAR(m_hMyWeapons, HASH("DT_CSPlayer"), HASH("m_hMyWeapons"));
		NETVAR(m_flFlashDuration, HASH("DT_CSPlayer"), HASH("m_flFlashDuration"));
		NETVAR(m_flVelocityModifier, HASH("DT_CSPlayer"), HASH("m_flVelocityModifier"));
		NETVAR(m_fEffects, HASH("DT_BasePlayer"), HASH("m_fEffects"));
		NETVAR(m_vphysicsCollisionState, HASH("DT_CSPlayer"), HASH("m_vphysicsCollisionState"));
		NETVAR(m_nPrecipType, HASH("DT_Precipitation"), HASH("m_nPrecipType"));
		NETVAR(m_Collision, HASH("DT_BaseEntity"), HASH("m_Collision"));
		NETVAR(m_CollisionGroup, HASH("DT_BaseEntity"), HASH("m_CollisionGroup"));
		NETVAR(m_vecMins, HASH("DT_CollisionProperty"), HASH("m_vecMins"));
		NETVAR(m_vecMaxs, HASH("DT_CollisionProperty"), HASH("m_vecMaxs"));
		NETVAR(m_bClientSideRagdoll, HASH("DT_BaseAnimating"), HASH("m_bClientSideRagdoll"));

		OFFSET(m_nMaterialHandle, g_Netvars.get(HASH("DT_Precipitation"), HASH("m_nPrecipType")) - 36);
		OFFSET(m_rgflCoordinateFrame, g_Netvars.get(HASH("DT_BaseEntity"), HASH("m_CollisionGroup")) - 0x30);
		OFFSET(m_flOldSimulationTime, g_Netvars.get(HASH("DT_BaseEntity"), HASH("m_flSimulationTime")) + 0x4);
		OFFSET(m_CustomMaterials, g_Netvars.get(HASH("DT_BaseCombatWeapon"), HASH("m_Item")) + 0x14);
		OFFSET(m_VisualsDataProcessors, g_Netvars.get(HASH("DT_BaseCombatWeapon"), HASH("m_Item")) + 0x220);
		OFFSET(m_nFinalPredictedTick, g_Netvars.get(HASH("DT_CSPlayer"), HASH("m_nTickBase")) + 0x4);
		OFFSET(m_vecOldOrigin, 0x3A8);
		OFFSET(m_bReadyToDraw, 0x270);
		OFFSET(m_vecAbsVelocity, 0x94);
		OFFSET(m_flFlashBangTime, 0xA2E8);
		OFFSET(m_LastCommand, 0x326C);
		OFFSET(m_pCurrentCommand, 0x3314);
		OFFSET(m_iEFlags, 0xE4);
		OFFSET(m_BoneCache, 0x2900);
		OFFSET(m_CustomMaterials2, 0x9DC);
		OFFSET(m_bCustomMaterialInitialized, 0x32DD);
		OFFSET(m_afButtonForced, 0x3310);
		OFFSET(m_afButtonDisabled, 0x330C);
		OFFSET(m_bPlayerUnderwater, 0x3550);
		OFFSET(m_bUseNewAnimstate, 0x39E1);
		OFFSET(m_nSimulationTick, 0x2A8);
		OFFSET(m_bPredictable, 0x2EE);
		OFFSET(m_CommandContext, 0x34D0);
		OFFSET(m_PlayerAnimState, 0x3870);
		OFFSET(m_flSpawnTime_Grenade, 0x29B0);
		OFFSET(m_VarMap, 0x24);

		OFFSET(m_iAccumulatedBoneMask, 0x268C);
		OFFSET(m_iPrevBoneMask, 0x2688);

		OFFSET(m_nLastNonSkippedFrame, 0xA5C);

		OFFSET(m_nComputedLODframe, 0xA2C);
		OFFSET(m_nAnimLODflagsOld, 0xA28);
		OFFSET(m_nAnimLODflags, 0xA24);
		OFFSET(m_nCustomBlendingRuleMask, 0xA20);

		OFFSET(m_EntClientFlags, 0x64);
		OFFSET(m_bIsToolRecording, 0x291);
		OFFSET(m_pIk, 0x265C); // 0x2660?
		OFFSET(m_pRagdoll, 0x9F4); // 0x2660?

		// don't want these macros outside of this file.
#undef NETVAR
#undef OFFSET
	}
};

inline Offsets g_Offsets;