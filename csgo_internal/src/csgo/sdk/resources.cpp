#include "../../precompiled.h"

int CPlayerResource::GetPing(int idx) {
    return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g_Csgo.m_PlayerResource) + g_Offsets.m_iPing + (idx * 4));
}