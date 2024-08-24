#include "../../../precompiled.h"

void Hooks::ClipRayToCollideable(const Ray& ray, unsigned int fMask, ICollideable* pCollide, trace_t* pTrace) {
	if (pCollide == nullptr || pTrace == nullptr)
		return g_Hooks.m_EngineTrace.GetOldMethod< ClipRayToCollideable_t >(IEngineTrace::CLIPRAYTOCOLLIDEABLE)(this, ray, fMask, pCollide, pTrace);

	// backup maxs.
	auto BackupOBBMaxsRoll = pCollide->OBBMaxs().z;

	// if the player is holding a knife and ducking in air we can still trace and hit him.
	pCollide->OBBMaxs().z += 5.0f;

	// call original.
	g_Hooks.m_EngineTrace.GetOldMethod< ClipRayToCollideable_t >(IEngineTrace::CLIPRAYTOCOLLIDEABLE)(this, ray, fMask, pCollide, pTrace);

	// restore maxs.
	pCollide->OBBMaxs().z = BackupOBBMaxsRoll;
}

void Hooks::TraceRay(const Ray& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace) {
	if (!pTrace || !g_Client.m_Autowalling)
		return g_Hooks.m_EngineTrace.GetOldMethod< TraceRay_t >(IEngineTrace::TRACERAY)(this, ray, fMask, pTraceFilter, pTrace);

	g_Hooks.m_EngineTrace.GetOldMethod< TraceRay_t >(IEngineTrace::TRACERAY)(this, ray, fMask, pTraceFilter, pTrace);
	pTrace->m_Surface.m_Flags |= SURF_SKY;
}