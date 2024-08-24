#include "../../../precompiled.h"

#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )

int Hooks::ListLeavesInBox(Vector& vecMins, Vector& vecMaxs, unsigned short* pList, int listMax) 
{
	Stack stack;

	static const auto InsertIntoTreeCallListLeavesInBoxAddress{ pattern::find(g_Csgo.m_client_dll, XOR("FF 52 ?? 8B 7D 08 8B 74 24 10")).add(0x3) };

	if (!g_Config.b(g_Items.m_Visuals.m_DisableModelOcculusion) || stack.ReturnAddress() != InsertIntoTreeCallListLeavesInBoxAddress)
		return g_Hooks.m_BSP.GetOldMethod< ListLeavesInBox_t >(ISpatialQuery::LISTLEAVESINBOX)(this, vecMins, vecMaxs, pList, listMax);

	auto pInfo = *reinterpret_cast<RenderableInfo_t**>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14);
	if (!pInfo || !pInfo->pRenderable)
		return g_Hooks.m_BSP.GetOldMethod< ListLeavesInBox_t >(ISpatialQuery::LISTLEAVESINBOX)(this, vecMins, vecMaxs, pList, listMax);

	auto pBaseEntity = (Player*)pInfo->pRenderable->GetIClientUnknown()->GetBaseEntity();
	if (!pBaseEntity->IsValid())
		return g_Hooks.m_BSP.GetOldMethod< ListLeavesInBox_t >(ISpatialQuery::LISTLEAVESINBOX)(this, vecMins, vecMaxs, pList, listMax);

	pInfo->nFlags &= ~0x100;
	pInfo->nFlags2 |= 0xC0;

	static Vector vecMapMin = Vector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static Vector vecMapMax = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);

	return g_Hooks.m_BSP.GetOldMethod< ListLeavesInBox_t >(ISpatialQuery::LISTLEAVESINBOX)(this, vecMapMin, vecMapMax, pList, listMax);
}