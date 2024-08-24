#include "../../precompiled.h"
#include "sdk.h"

void IEngineTrace::TraceRay(const Ray& ray, uint32_t mask, ITraceFilter* filter, CGameTrace* trace)
{
	g_Client.m_Autowalling = true;
	util::get_method< void(__thiscall*)(decltype(this), const Ray&, uint32_t, ITraceFilter*, CGameTrace*) >(this, IEngineTrace::TRACERAY)(this, ray, mask, filter, trace);
	g_Client.m_Autowalling = false;
}