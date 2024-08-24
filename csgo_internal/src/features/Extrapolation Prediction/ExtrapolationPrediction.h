#pragma once

enum class PREDICTION
{
	LOW,
	MEDIUM,
	HIGH,
	HYBRID
};

class CExtrapolation {
public:
	Vector SmoothVector(Vector& vecPosition, Vector& vecLastPosition);
	Vector Acceleration(LagRecord* pRecord, LagRecord* pPrevious);
	Vector FuturePosition(LagRecord* pRecord, LagRecord* pPrevious);

	float PredictionTime(LagRecord* pRecord, LagRecord* pPrevious);

	void CollisionDetection(Vector& vecPosition, LagRecord* pRecord);
	void HybridPrediction(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious);
	void Interpolation(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious);
	void UpdatePosition(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious);
};

inline CExtrapolation g_Extrapolation;