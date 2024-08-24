#pragma once

class CNetvarData
{
public:
	EHANDLE m_hGroundEntity;

	float_t m_flPostponeFireReadyTime = 0.f;
	float_t m_flRecoilIndex = 0.f;
	float_t m_flAccuracyPenalty = 0.f;
	float_t m_flDuckAmount = 0.f;
	float_t m_flDuckSpeed = 0.f;
	float_t m_flFallVelocity = 0.f;
	float_t m_flVelocityModifier = 0.f;

	int32_t m_nRenderMode = 0;
	int32_t m_nTickbase = 0;
	int32_t m_fFlags = 0;

	Vector m_vecOrigin = Vector(0.f, 0.f, 0.f);
	Vector m_vecVelocity = Vector(0.f, 0.f, 0.f);
	Vector m_vecBaseVelocity = Vector(0.f, 0.f, 0.f);
	Vector m_vecViewOffset = Vector(0.f, 0.f, 0.f);
	Vector m_vecAimPunchAngleVel = Vector(0.f, 0.f, 0.f);

	QAngle m_angAimPunchAngle = QAngle(0.f, 0.f, 0.f);
	QAngle m_angViewPunchAngle = QAngle(0.f, 0.f, 0.f);
public:
	__forceinline CNetvarData() :
		m_hGroundEntity{},
		m_flPostponeFireReadyTime{}, m_flRecoilIndex{}, m_flAccuracyPenalty{}, m_flDuckAmount{}, m_flDuckSpeed{}, m_flFallVelocity{},
		m_nRenderMode{}, m_nTickbase{}, m_fFlags{},
		m_vecOrigin{}, m_vecVelocity{}, m_vecBaseVelocity{}, m_vecViewOffset{}, m_vecAimPunchAngleVel{},
		m_angAimPunchAngle{}, m_angViewPunchAngle{} {};
};

class CEnginePrediction {
public:
	float m_Curtime;
	float m_FrameTime;
public:
	float_t m_flVelocityModifier = 1.f;
	bool m_bOverrideVelocityModifier = false;
	int32_t m_iLastCommand = 0;
	Vector m_AbsOrigin = Vector(0, 0, 0);
	Vector m_AbsVelocity = Vector(0, 0, 0);
public:
	bool m_FirstTimePredicted;
	bool m_InPrediction;
	CMoveData m_MoveData;
public:
	float m_MovementTimeForUserCmdProcessingRemaining;
	float m_TimeAllowedForProcessing;
public:
	void RunCommand(Player* player, Command* ucmd, IMoveHelper* moveHelper);
	void Update();
	void Backup();
	void Run();
	void Finish();
public:
	void Repredict();
	void RemoveClientJumpSound();
public:
	void CorrectViewmodelData();
	void UpdateViewmodelData();
	void InputDataToMap();
public:
	float m_flViewmodelCycle;
	float m_flViewmodelAnimTime;
};

inline CEnginePrediction g_InputPrediction;