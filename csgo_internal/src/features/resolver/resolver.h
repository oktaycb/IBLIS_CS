#pragma once

class ShotRecord;

class Resolver {
public:
	enum Modes : size_t {
		RESOLVE_STAND,
		RESOLVE_WALK,
		RESOLVE_AIR,
		RESOLVE_OVERRIDE,
	};

public:
	bool ShouldBalanceAdjust(LagRecord* pRecord);
	bool HasBalanceAdjustment(LagRecord* pRecord);
	void MatchShot(AimPlayer* data, LagRecord* record);
	bool InUseDetection(LagRecord* pRecord);
	float GetRotationYaw(LagRecord* record, float flYaw);
	void PredictAnglePosition(LagRecord* record, AimPlayer* data);
	void PredictFootYaw(LagRecord* pRecord, AimPlayer* pData);
	bool UnsafeVelocityTransitionDetection(LagRecord* pRecord, AimPlayer* pData);
	bool IsFakeFlick(LagRecord* pRecord, AimPlayer* pData);
	bool IsSafetyCheck(LagRecord* pRecord, AimPlayer* pData);
	bool PredictLowerbodyUpdates(LagRecord* pRecord, AimPlayer* data);

	void Save(LagRecord* pRecord, AimPlayer* pData);
	bool Reset(AimPlayer* pData, bool bResetMissedShots = false);

	void SetMode(AimPlayer* pData, LagRecord* pRecord);

	LagRecord* GetLastRecord(AimPlayer* data);
	LagRecord* GetAllRecords(AimPlayer* pData);
	LagRecord* GetIdealRecord(AimPlayer* data);

	float GetAwayAngle(LagRecord* record);

public:
};

inline Resolver g_Resolver;