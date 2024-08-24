#pragma once

class AimPlayer;

class LagCompensation {
public:
	enum LagType : size_t {
		INVALID = 0,
		CONSTANT,
		ADAPTIVE,
		RANDOM,
	};

public:

	bool StartPrediction(AimPlayer* data);
	void AirAccelerate(LagRecord* record, QAngle angle, float fmove, float smove);
	void PlayerMove(LagRecord* record);
};

inline LagCompensation g_LagCompensation;