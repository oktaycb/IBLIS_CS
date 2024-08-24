#pragma once

enum class HitscanMode : int {
	NORMAL = 0,
	LETHAL = 1,
	LETHAL2 = 2,
	PREFER = 3
};

enum Multipoint_t : int
{
	HEAD,
	UPPER,
	CHEST,
	LOWER,
	ARMS,
	LEGS,
	FEET
};

class HitscanBox
{
public:
	// update these on a call.
	__forceinline HitscanBox(Hitboxes_t index, HitscanMode mode) {
		m_HitboxIndex = index;
		m_HitscanMode = mode;
	}

	Hitboxes_t m_HitboxIndex;
	HitscanMode m_HitscanMode;

	__forceinline bool operator==(const HitscanBox& c) const {
		return m_HitboxIndex == c.m_HitboxIndex && m_HitscanMode == c.m_HitscanMode;
	}
};

struct Backup_t
{
	QAngle			 m_EyeAngles, m_AbsAngles;
	Vector           m_Origin, m_AbsOrigin, m_OBBMaxs, m_OBBMins;
	Vector           m_Velocity, m_AbsVelocity;
	int              m_Flags, m_EFlags, m_Effects;
	float            m_DuckAmount, m_SimulationTime, m_OldSimulationTime, m_LowerbodyYaw, m_FootYaw;
	C_AnimationLayer m_Layers[13];
	float            m_Poses[24];
	matrix3x4a_t* m_CachedBones;
	int				 m_CachedBoneCount;
	bool             m_ClientSideAnimation;
};

class AimPlayer
{
public:
	// essential data.
	Player* m_Player;
	float		m_SpawnTime;
	bool		m_AwaitingFlashedResult;
	std::deque< std::shared_ptr< LagRecord > >	m_Records;

	enum class RESOLVER : int {
		LEGIT = 0,
		RESOLVED,
		LAST,
		LAST_BROKEN,
		LAST_FREESTAND,
		PREDICTION,
		PREDICTION_BROKEN,
		PREDICTION_FREESTAND,
		PREDICTION_LAYER,
		BROKEN,
		FREESTAND,
		FREESTAND_DELTA,
		FREESTAND_BROKEN,
		BRUTEFORCE,
		BRUTEFORCE_BROKEN,
		DELTA,
	};

	enum class LBY : int {
		NORMAL = 0,
		OPPOSITE,
		MANUAL,
		MINIMAL
	};

	struct
	{
		float flReversedLBYYaw{ 0.f };
		float flOldReversedLBYYaw{ 0.f };
	} m_Visuals;

	// resolve data.
	int       m_Shots{ 0 };
	int       m_MissedShots{ 0 };
	int       m_FreestandMissedShots{ 0 };
	int       m_FakeFlickMissedShots{ 0 };
	LagRecord m_WalkRecord{};
	float     m_NextUpdateTime{ 0.f };
	float     m_NextLowerbodyYawUpdate{ 0.f };
	int       m_CountLowerbodyYawChanges{ 0 };
	bool      m_ServerUpdatedLowerbodyYaw{ false };
	bool      m_PredictionOfLowerbodyYaw{ false };
	RESOLVER  m_ResolverMode{ RESOLVER::LEGIT };
	float     m_LastAngle{ 0 };

	std::vector< AdaptiveAngle > m_AdaptiveAngles{};
	bool m_ValidAdaptiveAngles{ false };
	bool m_AdaptiveBack{ false };
	bool m_AdaptiveLeft{ false };
	bool m_AdaptiveRight{ false };

	bool m_IsLowerbodyYawBroken{ false };

	float m_LastPredictedLowerbodyYaw{ 0.f };
	float m_LastPredictedLowerbodyYawDifference{ 0.f };
	float m_AllLowerbodyYawDifferences{ 0.f };
	float m_BrokenLowerbodyYaw{ 0.f };
	float m_FirstBrokenLowerbodyYaw{ 0.f };
	float m_SecondBrokenLowerbodyYaw{ 0.f };
	float m_FirstBrokenLowerbodyYawNormalized{ 0.f };
	float m_SecondBrokenLowerbodyYawNormalized{ 0.f };
	float m_ThirdBrokenLowerbodyYawNormalized{ 0.f };

	float m_FirstPercentDifference{ 0.f };
	float m_SecondPercentDifference{ 0.f };

	float m_LowerbodyYawDifferencesCalculation{ 0.f };
	float m_LowerbodyYawDifferencesCalculation2{ 0.f };
	float m_ThirdLowerbodyYawDifferences{ 0.f };
	float m_OldLowerbodyYawDifferences{ 0.f };
	float m_LowerbodyYawDifferences{ 0.f };
	float m_LowerbodyYawDifferencesNormalized{ 0.f };
	float m_LowerbodyYawMinimalDifferences{ 0.f };
	float m_LowerbodyYawCombination{ 0.f };

	// check anti-aim types yaw.
	LBY m_LowerbodyYawBreakers = LBY::NORMAL;
	LBY m_OldLowerbodyYawBreakers = LBY::NORMAL;

	// save body data.
	float m_FirstLowerbodyYaw{ 0.f };
	float m_FirstLowerbodyYawNormalized{ 0.f };
	float m_FirstLowerbodyYawDifference{ 0.f };
	float m_CombineBreakerDifferences{ 0.f };
public:
	void OnNetUpdate(Player* player);
	void OnRoundStart(Player* player);
public:
	bool CanUpdateAnimation();
	void UpdateInterpolation();
	void UpdateEnemiesAnimations();
	void UpdateAlliesAnimations();
public:
	void reset();
};

class AimPosition
{
public:
	Vector      m_MultiPoints = Vector(0.f, 0.f, 0.f), m_CenterPoint = Vector(0.f, 0.f, 0.f);
	float       m_FinalRadius = 0.f, m_Radius = 0.f;

public:
	__forceinline AimPosition(const Vector& vecMultiPoint = Vector(0.f, 0.f, 0.f), const Vector& vecCenterPoint = Vector(0.f, 0.f, 0.f), const float& flFinalRadius = 0.f, const float& flRadius = 0.f)
	{
		m_MultiPoints = vecMultiPoint;
		m_CenterPoint = vecCenterPoint;
		m_FinalRadius = flFinalRadius;
		m_Radius = flRadius;
	}
};

class PlayerData
{
public:
	AimPlayer* m_Data = nullptr;
	float	m_FOV = 0.f, m_Distance = 0.f;

public:
	__forceinline PlayerData(AimPlayer* m_pData = nullptr, const float& m_flFOV = 0.f, const float& m_flDistance = 0.f)
	{
		if (m_Data == nullptr)
			return;

		m_Data = m_pData;
		m_FOV = m_flFOV;
		m_Distance = m_flDistance;
	}

	__forceinline bool IsValid(float m_flFov = 360.f, bool m_bCheckRecord = true) {
		return m_Data && m_FOV <= m_flFov;
	}
};

class AimData
{
public:
	Player* m_Player = nullptr;
	LagRecord* m_Record = nullptr;
	AimPosition	m_AimPositionData = AimPosition();
	Hitboxes_t	m_Hitbox = Hitboxes_t::INVALID;
	float		m_Damage = 0.f, m_FOV = 0.f, m_Distance = 0.f;
	bool        m_LagCompensation = false;

public:
	__forceinline AimData(Player* m_pEntity = nullptr, LagRecord* m_pRecord = nullptr, const AimPosition& pPoints = AimPosition(), const Hitboxes_t& m_iHitbox = Hitboxes_t::INVALID, const float& m_flDamage = 0.f, const float& m_flFOV = 0.f, const float& m_flDistance = 0.f, const bool& m_bLagCompensation = false)
	{
		if (m_pEntity == nullptr || m_pRecord == nullptr || m_iHitbox < Hitboxes_t::HEAD || m_iHitbox >= Hitboxes_t::MAX || m_flDamage <= 0.f)
			return;

		m_Player = m_pEntity;
		m_Record = m_pRecord;
		m_AimPositionData = pPoints;
		m_Hitbox = m_iHitbox;
		m_Damage = m_flDamage;
		m_FOV = m_flFOV;
		m_Distance = m_flDistance;
		m_LagCompensation = m_bLagCompensation;
	}

	__forceinline bool IsValid(float m_flFov = 360.f, bool m_bCheckRecord = true) {
		return m_Player && m_Record && m_Damage > 0.f && m_Hitbox >= Hitboxes_t::HEAD && m_Hitbox < Hitboxes_t::MAX&& m_Damage > 0.f && m_FOV <= m_flFov;
	}
};

class Aimbot {
public:
	enum AUTOSTOP : int
	{
		AUTOSTOP_BETWEEN = 0,
		AUTOSTOP_FORCE_ACCURACY,
		AUTOSTOP_EARLY
	};

	enum OPTIMIZATION : int
	{
		OFF = 0,
		LOW,
		MEDIUM,
		HIGH,
		MAXIMUM,
	};

	struct {
		std::array < float_t, 256 > m_aInaccuracy;
		std::array < float_t, 256 > m_aSpread;
		std::array < float_t, 256 > m_aMultiValues;
		std::array < float_t, 256 > m_aInaccuracyMultiValues;
		std::array < float_t, 256 > m_aSpreadMultiValues;

		float_t m_AddAccuracy = 0.0f;
		float_t m_LowestAccuracy = 0.0f;

		float_t m_flInaccuracy = 0.0f;
		float_t m_flSpread = 0.0f;
		float_t m_flMinInaccuracy = 0.0f;

		bool m_bHasValidAccuracyData = false;
		bool m_CalculateAccuracy = false;
		bool m_bHasMaximumAccuracy = false;
		bool bCalculateAccuracy = false;
		bool m_HasFired = false;
	} m_AccuracyData;
public:
	struct Config {
		bool m_Enabled{};
		bool m_Silent{};
		bool m_AutoFire{};
		bool m_AutoWall{};
		bool m_AutoStop{};
		bool m_AutoScope{};
		bool m_HighestAccuracyCheck{true};
		bool m_ScaleDamage{};
		bool m_PredictiveDamage{};
		bool m_WaitForResolved{};
		bool m_OverrideDamage{};
		bool m_OverrideHitChance{};
		bool m_ForceBodyAimKey{};
		bool m_ManualHistoryHitboxes{};
		bool m_HitgroupDamage{};
		bool m_TickbaseManipulation{};

		int m_OptimizationLevel{};
		int m_MultipointHeadScale{};
		int m_MultipointBodyScale{};
		int m_AutoStopType{};
		int m_MinimumDamage{};
		int m_HeadMinimumDamage{};
		int m_NeckMinimumDamage{};
		int m_ChestMinimumDamage{};
		int m_StomachMinimumDamage{};
		int m_ArmsMinimumDamage{};
		int m_LegsMinimumDamage{};
		int m_FeetMinimumDamage{};
		int m_OverrideDamageAmount{};
		int m_HitChanceAmount{};
		int m_OverrideHitChanceAmount{};
		int m_MaximumFOV{};
		int m_AddScaleDamage{};
		int m_SlideOnStop{};
		int m_BodyAfterXMisses{};
		int m_BodyOnXHealth{};
		int m_ForceBodyAfterXMisses{};
		int m_LimitTarget{};
		int m_DeterminePlayerAmount{};
		int m_DetermineFramePerSecond{};
		int m_ChargeTime{};

		std::unordered_map<int, bool> m_HitgroupMinimumDamage;
		std::vector< bool> m_Hitbox;
		std::unordered_map<int, bool> m_HistoryHitbox;
		std::unordered_map<int, bool> m_BodyAimHitbox;
		std::unordered_map<int, bool> m_BodyAimHistoryHitbox;
		std::vector<bool> m_Multipoint;
		std::unordered_map<int, bool> m_Optimization;
		std::unordered_map<int, bool> m_OptimizationConditions;
		std::vector< bool> m_AutoStopFlags;
		std::vector< bool> m_AutoStopConditions;
		std::vector< bool> m_PreferBodyAim;
		std::unordered_map<int, bool> m_PreferHeadAim;
		std::unordered_map<int, bool> m_ForceBodyAim;
	} m_Config;
public:
	std::array< AimPlayer, 64 > m_Players;

	__forceinline void reset() {
		// reset all players data.
		for (auto& p : m_Players)
			p.reset();
	}

	__forceinline bool IsValidTarget(Player* player) {
		return player && player->IsPlayer() && player->IsAlive() && player->enemy(g_Client.m_LocalPlayer);
	}

public:
	std::vector<HitscanBox> m_Hitboxes;
	std::deque<Player*> m_PlayerData;
	std::deque< Player* > m_OptimizedTargets;
	std::vector<AimData> m_Data;
	std::vector<Vector> m_OptimizedPoints;
	std::vector<Vector> m_Points;
	std::vector<AimPosition> m_Multipoints;

public:
	AimData m_FirstTarget, m_LastTarget;
	bool m_AutoStopMovement = false;

	void Multipoints(Player* pPlayer, LagRecord* record, Hitboxes_t it);
	AimData GetAimPoint(Player* pPlayer, LagRecord* record);
	void SetUpHitboxes(Player* pPlayer, AimPlayer* pData, bool bBacktracking);
	void RemoveAttack();
	void AddAttack();
	void AddMultipoints(Vector vecPoint, Vector vecCenter, int& iMultiPointAmount, float flHeadFinalRadius = 0.f, float flRadius = 0.f);
	void Init();
public:
	void OptimizeMultipoint(AimData& Info);
	int GetHitChance();
	int GetMinimumDamage(Player* pPlayer, int nGroup = -1);
	bool CanHit(Vector start, Vector end, Player* player, Hitboxes_t box, LagRecord* record);
	bool IsPointAccurate(AimData data);
	void CalculateMaxAccuracy();
	bool IsMaxAccuracy();
	void AutomaticStop();
	bool CanAutoStop();
	bool IsAutoFiring(bool bStab);
	bool Enabled();

private:
	Vector Extrapolation(AimData output);

private:
	int AdjustTickCount(AimData output, ICollideable* pCollideable);
	int TickLatency();

	bool bRevolver = false;
private:
	struct Table_t {
		uint8_t Swing[2][2][2];
		uint8_t Stab[2][2];
	};

	const Table_t m_KnifeDamages{ { { { 25, 90 }, { 21, 76 } }, { { 40, 90 }, { 34, 76 } } }, { { 65, 180 }, { 55, 153 } } };

	std::array< QAngle, 12 > m_KnifeAngles{
		QAngle{ 0.f, 0.f, 0.f }, QAngle{ 0.f, -90.f, 0.f }, QAngle{ 0.f, 90.f, 0.f }, QAngle{ 0.f, 180.f, 0.f },
		QAngle{ -80.f, 0.f, 0.f }, QAngle{ -80.f, -90.f, 0.f }, QAngle{ -80.f, 90.f, 0.f }, QAngle{ -80.f, 180.f, 0.f },
		QAngle{ 80.f, 0.f, 0.f }, QAngle{ 80.f, -90.f, 0.f }, QAngle{ 80.f, 90.f, 0.f }, QAngle{ 80.f, 180.f, 0.f }
	};

	bool CanKnife(Player* pPlayer, QAngle vecAngles, bool& stab);
	bool KnifeTrace(Vector vecDirection, bool stab, CGameTrace* trace);
	bool KnifeIsBehind(Player* pPlayer);
};

inline Aimbot g_Aimbot;