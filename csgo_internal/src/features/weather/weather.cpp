#include "../../precompiled.h"

#define MAX_POSITION_FLT 32768.0f

// How many bits to use to encode an edict.
#define	MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	MAX_EDICTS					(1<<MAX_EDICT_BITS)

void C_WeatherSystem::Init(int stage) {
//    if (bHasBeenCreated && pNetworkable && pPrecipitation && stage == 1 && c_config::get()->i["rain"] == 1) {
//        /* A1 ? ? ? ? 83 C1 F4 */
//        /* xref: 'Rain simulation: %du (%d tracers)' xref the callee of the func */
//        static auto fn = pattern::find(g_Csgo.m_client_dll, XOR("A1 ? ? ? ? 83 C1 F4")).as< void(__thiscall*)(void*)>();
//        fn(pPrecipitation + 12);
//    }

    static bool bHasBeenCreated = false;
    static Player* pPrecipitation = nullptr;
    static ClientClass* PrecipitationClientClass = nullptr;
    static IClientNetworkable* pNetworkable = nullptr;

    if (bHasBeenCreated && pPrecipitation && !c_config::get()->i["rain"]) 
        pPrecipitation->m_nPrecipType() = static_cast<PrecipitationType_t>(-1);
    
    if (!c_config::get()->i["rain"] || !g_Client.m_LocalPlayer || g_Csgo.m_ClientState->m_DeltaTick == -1 || g_skins.m_update || !g_Csgo.m_Engine->IsInGame() || !g_Csgo.m_Engine->IsConnected()) {
        pPrecipitation = nullptr;
        bHasBeenCreated = false;
        pNetworkable = nullptr;
        return;
    }

    if (bHasBeenCreated && pPrecipitation)
        pPrecipitation->m_nPrecipType() = static_cast<PrecipitationType_t>(c_config::get()->i["rain"] - PrecipitationType_t::PRECIPITATION_TYPE_SNOW);

    if (!PrecipitationClientClass)
        for (auto pclass = g_Csgo.m_Client->GetAllClasses(); pclass && !PrecipitationClientClass; pclass = pclass->m_pNext)
            if (pclass->m_ClassID == g_Netvars.GetClientID(HASH("CPrecipitation")))
                PrecipitationClientClass = pclass;

    if (!bHasBeenCreated && PrecipitationClientClass && PrecipitationClientClass->m_pCreate && stage == 5) {
        pNetworkable = reinterpret_cast<IClientNetworkable*>(PrecipitationClientClass->m_pCreate(
            g_Csgo.m_EntityList->GetHighestEntityIndex() + 1, g_Csgo.RandomInt(0, 4096 - 1)));

        if (pNetworkable) {
            pNetworkable->PreDataUpdate(DATA_UPDATE_CREATED);
            pNetworkable->OnPreDataChanged(DATA_UPDATE_CREATED);

            pPrecipitation = reinterpret_cast<Player*>(pNetworkable->GetIClientUnknown()->GetBaseEntity());

            pPrecipitation->m_nPrecipType() = static_cast<PrecipitationType_t>(c_config::get()->i["rain"] - PrecipitationType_t::PRECIPITATION_TYPE_SNOW);

            pPrecipitation->GetCollideable()->OBBMins() = Vector(-32768.0f, -32768.0f, -32768.0f);
            pPrecipitation->GetCollideable()->OBBMaxs() = Vector(32768.0f, 32768.0f, 32768.0f);

            pPrecipitation->GetClientNetworkable()->OnDataChanged(DATA_UPDATE_CREATED);
            pPrecipitation->GetClientNetworkable()->PostDataUpdate(DATA_UPDATE_CREATED);

            bHasBeenCreated = true;
        }
    }

    //if (bHasBeenCreated && pPrecipitation && pNetworkable && stage == 1)
    //    g_Csgo.CClient_Precipitation_Simulate(reinterpret_cast<Player*>(pPrecipitation), 0.f);
}

//void C_WeatherSystem::Init(int Stage)
//{
//    if (!c_config::get()->i["rain"] || g_Csgo.m_ClientState->m_DeltaTick == -1 || g_skins.m_update || !g_Csgo.m_Engine->IsInGame() || !g_Csgo.m_Engine->IsConnected()) {
//        Reset();
//        return;
//    }
//
//    if (!m_WeatherData.m_bHasBeenCreated) {
//        m_WeatherData.m_Networkable = CreateWeatherEntity();
//        return;
//    }
//
//    if (!m_WeatherData.m_Networkable)
//        return;
//
//    auto pPrecipitation = reinterpret_cast<Entity*>(m_WeatherData.m_Networkable->GetIClientUnknown()->GetBaseEntity());
//    if (!pPrecipitation)
//        return;
//
//    if (Stage == FRAME_RENDER_START)
//    {
//        g_Csgo.m_RainSideVel->SetValue(30.0f);
//        g_Csgo.m_RainAlpha->SetValue(1.0f);
//        g_Csgo.m_RainSpeed->SetValue(static_cast <float_t>(600.f));
//        g_Csgo.m_RainWidth->SetValue(static_cast <float_t>(50.f) / 100.0f);
//        g_Csgo.m_RainLength->SetValue(static_cast <float_t>(50.f) / 1000.0f);
//
//        pPrecipitation->GetClientNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
//        pPrecipitation->GetClientNetworkable()->OnPreDataChanged(DATA_UPDATE_CREATED);
//
//        pPrecipitation->m_nPrecipType() = static_cast<PrecipitationType_t>(c_config::get()->i["rain"] - PrecipitationType_t::PRECIPITATION_TYPE_SNOW);
//        pPrecipitation->GetCollideable()->OBBMins() = Vector(-MAX_POSITION_FLT, -MAX_POSITION_FLT, -MAX_POSITION_FLT);
//        pPrecipitation->GetCollideable()->OBBMaxs() = Vector(MAX_POSITION_FLT, MAX_POSITION_FLT, MAX_POSITION_FLT);
//
//        pPrecipitation->m_vecOrigin() = (pPrecipitation->GetCollideable()->OBBMins() + pPrecipitation->GetCollideable()->OBBMaxs()) * 0.5f;
//        pPrecipitation->SetAbsOrigin(pPrecipitation->m_vecOrigin());
//
//        pPrecipitation->GetClientNetworkable()->OnDataChanged(DATA_UPDATE_CREATED);
//        pPrecipitation->GetClientNetworkable()->PostDataUpdate(DATA_UPDATE_CREATED);
//    }
//
//    if (pPrecipitation && Stage == 1)
//        g_Csgo.CClient_Precipitation_Simulate(reinterpret_cast<Player*>(uintptr_t(pPrecipitation) - 0xC), 0.f);
//}

void C_WeatherSystem::Reset()
{
    m_WeatherData.m_bHasBeenCreated = false;
}

IClientNetworkable* C_WeatherSystem::CreateWeatherEntity()
{
    static ClientClass* PrecipitationClientClass = nullptr;

    for (auto Class = g_Csgo.m_Client->GetAllClasses(); Class && !PrecipitationClientClass; Class = Class->m_pNext) {
        if (Class->m_ClassID != g_Netvars.GetClientID(HASH("CPrecipitation")))
            continue;

        PrecipitationClientClass = Class;
    }

    if (!PrecipitationClientClass || !PrecipitationClientClass->m_pCreate)
        return nullptr;

    IClientNetworkable* Networkable = (IClientNetworkable*)PrecipitationClientClass->m_pCreate(g_Csgo.m_EntityList->GetHighestEntityIndex() + 1, g_Csgo.RandomInt(0, 4096 - 1));
    if (!Networkable || !Networkable->GetIClientUnknown()->GetBaseEntity())
        return nullptr;

    m_WeatherData.m_bHasBeenCreated = true;
    return Networkable;
}