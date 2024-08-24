#include "../../precompiled.h"

bool LagCompensation::StartPrediction(AimPlayer* data) {
	LagRecord* pRecord = data->m_Records.front().get();
	if (!pRecord)
		return false;

	if (data->m_Records.size() <= 1)
		return false;

	LagRecord* pPrevious = data->m_Records[1].get();
	if (!pPrevious || pPrevious->m_Dormant)
		return false;

	if (!(pRecord->m_BrokeLagCompensation = (pRecord->m_Origin - pPrevious->m_Origin).LengthSqr() >= 4096.f))
		return false;

	return true;
}

void LagCompensation::PlayerMove(LagRecord* pRecord) {
	Vector                start, end, normal;
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;

	// define trace start.
	start = pRecord->m_Origin;

	// move trace end one tick into the future using predicted velocity.
	end = start + (pRecord->m_Velocity * g_Csgo.m_Globals->m_Interval);

	// trace.
	g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, pRecord->m_OBBMins, pRecord->m_OBBMaxs), CONTENTS_SOLID, &filter, &trace);

	// we hit shit
	// we need to fix hit.
	if (trace.m_Fraction != 1.f) {

		// fix sliding on planes.
		for (int i{}; i < 2; ++i) {
			pRecord->m_Velocity -= trace.m_Plane.m_Normal * pRecord->m_Velocity.Dot(trace.m_Plane.m_Normal);

			float adjust = pRecord->m_Velocity.Dot(trace.m_Plane.m_Normal);
			if (adjust < 0.f)
				pRecord->m_Velocity -= (trace.m_Plane.m_Normal * adjust);

			start = trace.m_EndPosition;
			end = start + (pRecord->m_Velocity * (g_Csgo.m_Globals->m_Interval * (1.f - trace.m_Fraction)));

			g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, pRecord->m_OBBMins, pRecord->m_OBBMaxs), CONTENTS_SOLID, &filter, &trace);
			if (trace.m_Fraction == 1.f)
				break;
		}
	}

	// set new final origin.
	pRecord->m_Origin = start = end = trace.m_EndPosition;

	// move endpos 2 units down.
	// this way we can check if we are in/on the ground.
	end.z -= 2.f;

	// trace.
	g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, pRecord->m_OBBMins, pRecord->m_OBBMaxs), CONTENTS_SOLID, &filter, &trace);

	// strip onground flag.
	pRecord->m_Flags &= ~FL_ONGROUND;

	// add back onground flag if we are onground.
	if (trace.hit() && trace.m_Plane.m_Normal.z > 0.7f)
		pRecord->m_Flags |= FL_ONGROUND;
	
	// predict simulation time.
	pRecord->m_SimulationTime += game::TIME_TO_TICKS(g_Csgo.m_Globals->m_TickCount) - pRecord->m_SimulationTime;
}

void LagCompensation::AirAccelerate(LagRecord* pRecord, QAngle angle, float fmove, float flSideMove) {
	Vector fwd, right, wishvel, wishdir;
	float  maxspeed, wishspd, wishspeed, currentspeed, addspeed, accelspeed;

	// determine movement angles.
	Math::AngleVectors(angle, &fwd, &right);

	// zero out z components of movement vectors.
	fwd.z = 0.f;
	right.z = 0.f;

	// Normalize remainder of vectors.
	fwd.Normalize();
	right.Normalize();

	// determine x and y parts of velocity.
	for (int i{}; i < 2; ++i)
		wishvel[i] = (fwd[i] * fmove) + (right[i] * flSideMove);

	// zero out z part of velocity.
	wishvel.z = 0.f;

	// determine maginitude of speed of move.
	wishdir = wishvel;
	wishspeed = wishdir.Normalize();

	// get maxspeed.
	// TODO; maybe global this or whatever its 260 anyway always.
	maxspeed = pRecord->m_Player->m_flMaxspeed();

	// clamp to server defined max speed.
	if (wishspeed != 0.f && wishspeed > maxspeed)
		wishspeed = maxspeed;

	// make copy to preserve original variable.
	wishspd = wishspeed;

	// cap speed.
	if (wishspd > 30.f)
		wishspd = 30.f;

	// determine veer amount.
	currentspeed = pRecord->m_Velocity.Dot(wishdir);

	// see how much to add.
	addspeed = wishspd - currentspeed;

	// if not adding any, done.
	if (addspeed <= 0.f)
		return;

	// Determine acceleration speed after acceleration
	accelspeed = g_Csgo.sv_airaccelerate->GetFloat() * wishspeed * g_Csgo.m_Globals->m_Interval;

	// cap it.
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// add accel.
	pRecord->m_Velocity += (wishdir * accelspeed);
}
