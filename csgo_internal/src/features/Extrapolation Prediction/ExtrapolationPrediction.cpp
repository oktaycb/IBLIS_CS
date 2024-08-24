#include "../../precompiled.h"

auto GetPing = []() -> int
{
	static float m_AvgLatency = 0;
	m_AvgLatency = g_Client.m_LatencyOutgoing; 

	if (g_Csgo.m_Engine->IsPlayingDemo())
		m_AvgLatency = 0.0f; 

	float flAdjust = 0.0f;

	if (g_Csgo.cl_updaterate->GetFloat() > 0.001f)
	{
		flAdjust = -0.5f / g_Csgo.cl_updaterate->GetFloat();

		m_AvgLatency += flAdjust;
	}

	flAdjust *= 1000.0f;
	m_AvgLatency = std::max(0.0f, m_AvgLatency);

	return int(m_AvgLatency * 1000.0f);
};

Vector CExtrapolation::SmoothVector(Vector& vecPosition, Vector& vecLastPosition)
{
	return (vecPosition + vecLastPosition) * 0.5f;
}

float CExtrapolation::PredictionTime(LagRecord* pRecord, LagRecord* pPrevious)
{
	// calculate the time it will take for a message to travel from the client to the server and back, based on the player's ping
	float roundTripTime = GetPing() / 1000.0f; // ping is in milliseconds, so divide by 1000 to convert to seconds.

	// future position a certain amount of time into the future (determined by the "flPredictionTime" variable).
	float flPredictionTime = g_Csgo.m_Globals->m_Interval;

	// that the object can travel based on its velocity and acceleration.
	float maxDistance = pRecord->m_Velocity.Length() * flPredictionTime + Acceleration(pRecord, pPrevious).Length();

	// clamp the prediction time to the maximum distance that the object can travel.
	flPredictionTime = std::min(flPredictionTime, maxDistance / pRecord->m_Velocity.Length());

	// adjust the prediction time based on the player's latency
	flPredictionTime += roundTripTime;
	return flPredictionTime;
}

Vector CExtrapolation::Acceleration(LagRecord* pRecord, LagRecord* pPrevious)
{
	float flTime = PredictionTime(pRecord, pPrevious);

	// accounting for acceleration and latency.
	return 0.5f * SmoothVector(pRecord->m_Acceleration, pPrevious->m_Acceleration) * flTime * flTime;
}

Vector CExtrapolation::FuturePosition(LagRecord* pRecord, LagRecord* pPrevious)
{
	// use extrapolation to predict the object's future position and latency.
	return pRecord->m_Origin + pRecord->m_Velocity * PredictionTime(pRecord, pPrevious);
}

void CExtrapolation::CollisionDetection(Vector& vecPosition, LagRecord* pRecord)
{
	CGameTrace            pTrace;
	CTraceFilterWorldOnly pFilter;

	g_Csgo.m_EngineTrace->TraceRay(Ray(pRecord->m_Origin, vecPosition, pRecord->m_OBBMins, pRecord->m_OBBMaxs), CONTENTS_SOLID, &pFilter, &pTrace);

	if (pTrace.m_Fraction != 1.f)
	{
		for (int i{}; i < 2; ++i)
		{
			vecPosition = pTrace.m_EndPosition + (pRecord->m_Velocity * (g_Csgo.m_Globals->m_Interval * (1.f - pTrace.m_Fraction)));

			g_Csgo.m_EngineTrace->TraceRay(Ray(pTrace.m_EndPosition, vecPosition, pRecord->m_OBBMins, pRecord->m_OBBMaxs), CONTENTS_SOLID, &pFilter, &pTrace);
			if (pTrace.m_Fraction == 1.f)
				break;
		}
	}
}

void CExtrapolation::HybridPrediction(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious)
{
	// Use dead reckoning to smooth out the movements of the object
	Vector deadReckonedPosition = pPrevious->m_Origin + SmoothVector(pRecord->m_Velocity, pPrevious->m_Velocity) * (pRecord->m_SimulationTime - pPrevious->m_SimulationTime);

	// Use ping-based prediction to adjust the prediction based on the player's latency
	float predictionAdjustment = std::max(0.0f, 1.0f - GetPing() / 500.0f); // adjust the prediction based on the player's latency

	// Combine the extrapolated and dead reckoned positions using a weighted average
	Vector hybridPosition = vecPosition * predictionAdjustment + deadReckonedPosition * (1.0f - predictionAdjustment);

	// update the position.
	vecPosition = hybridPosition;
}

void CExtrapolation::Interpolation(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious) {
	float flInterpolationAmount = std::clamp(0.5f, 0.f, 1.f);

	// use client-side interpolation to smooth out the transitions between updates from the server.
	vecPosition = pPrevious->m_Origin + (vecPosition - pPrevious->m_Origin) * flInterpolationAmount;
}

void CExtrapolation::UpdatePosition(Vector& vecPosition, LagRecord* pRecord, LagRecord* pPrevious)
{
	// use a simple extrapolation technique for high ping.
	Vector vecExtrapolatedPosition = FuturePosition(pRecord, pPrevious);

	// optimize the prediction for different network conditions.
	int iPing = GetPing();
	if (iPing < 200) {
		// use a simple extrapolation technique for high ping.
		vecExtrapolatedPosition = FuturePosition(pRecord, pPrevious) + Acceleration(pRecord, pPrevious);

		// combine the extrapolated and dead reckoned positions using a weighted average
		HybridPrediction(vecExtrapolatedPosition, pRecord, pPrevious);

		// use client-side interpolation to smooth out the transitions between updates from the server.
		Interpolation(vecExtrapolatedPosition, pRecord, pPrevious);
	}
	else if (iPing < 400) 
		// use a simple extrapolation technique for high ping.
		vecExtrapolatedPosition = FuturePosition(pRecord, pPrevious) + Acceleration(pRecord, pPrevious);

	// If a collision is detected, set the optimized position to the current position to prevent the object from moving.
	CollisionDetection(vecExtrapolatedPosition, pRecord);

	// update the position.
	vecPosition = vecExtrapolatedPosition;
}