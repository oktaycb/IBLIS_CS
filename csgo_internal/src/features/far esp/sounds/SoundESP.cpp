#include "../../../precompiled.h"

void CSoundESP::Init()
{
	m_CurrentSounds.RemoveAll();

	g_Csgo.m_EngineSound->GetActiveSounds(m_CurrentSounds);

	//if (m_CurrentSounds.IsEmpty())
	//	return;

	for (auto i = 0; i < m_CurrentSounds.Count(); i++)
	{
		auto& pSound = m_CurrentSounds[i];
		if (pSound.m_nSoundSource < 1 || pSound.m_nSoundSource > 64)
			continue;

		if (pSound.m_pOrigin->IsZero())
			continue;

		if (!IsSoundValid(pSound))
			continue;

		auto pPlayer = static_cast<Player*>(g_Csgo.m_EntityList->GetClientEntity(pSound.m_nSoundSource));
		if (!pPlayer || !pPlayer->IsAlive())
			continue;

		SetupAdjust(pPlayer, pSound);

		m_SoundPlayer[pSound.m_nSoundSource].m_RecieveTime = g_Csgo.m_Globals->m_RealTime;
		m_SoundPlayer[pSound.m_nSoundSource].m_Origin = *pSound.m_pOrigin;
	}

	m_SoundBuffer = m_CurrentSounds;
}

void CSoundESP::SetupAdjust(Player* pPlayer, IEngineSound::SndInfo_t& pSound)
{
	Vector src3D, dst3D;
	trace_t pGameTrace;

	src3D = *pSound.m_pOrigin + Vector(0.0f, 0.0f, 1.0f);
	dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

	static CTraceFilterSkipTwoEntities_game pFilter{};
	pFilter.SetPassEntity(pPlayer);

	g_Csgo.m_EngineTrace->TraceRay(Ray(src3D, dst3D), MASK_PLAYERSOLID, (ITraceFilter*)&pFilter, &pGameTrace);

	if (pGameTrace.m_AllSolid)
		m_SoundPlayer[pSound.m_nSoundSource].m_RecieveTime = -1;

	*pSound.m_pOrigin = pGameTrace.m_Fraction <= 0.97f ? pGameTrace.m_EndPosition : *pSound.m_pOrigin;

	m_SoundPlayer[pSound.m_nSoundSource].m_Flags = pPlayer->m_fFlags();
	m_SoundPlayer[pSound.m_nSoundSource].m_Flags |= (pGameTrace.m_Fraction < 0.50f ? FL_DUCKING : 0) | (pGameTrace.m_Fraction < 1.0f ? FL_ONGROUND : 0);
	m_SoundPlayer[pSound.m_nSoundSource].m_Flags &= (pGameTrace.m_Fraction >= 0.50f ? ~FL_DUCKING : 0) | (pGameTrace.m_Fraction >= 1.0f ? ~FL_ONGROUND : 0);
}

void CSoundESP::Reset(Player* pPlayer)
{
	m_CurrentSounds.RemoveAll();
	m_SoundBuffer.RemoveAll();

	CSoundESP::SoundPlayer* pSoundPlayer = &m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()];
	if (!pSoundPlayer)
		return;

	m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_RecieveTime = -1.f;
	m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_Origin = Vector(0.f, 0.f, 0.f);
	m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_Flags = -1;
}

bool CSoundESP::RecievedAdjustSound(Player* pPlayer)
{
	CSoundESP::SoundPlayer pSoundPlayer = m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()];

	if (fabs(g_Csgo.m_Globals->m_RealTime - m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_RecieveTime) > 10.0f || m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_RecieveTime <= 0.0f || m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_Origin.IsZero())
		return false;

	pPlayer->m_bSpotted() = true;
	pPlayer->m_fFlags() = m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_Flags;
	pPlayer->SetOrigin(m_SoundPlayer[pPlayer->GetClientNetworkable()->GetIndex()].m_Origin);

	return true;
}

bool CSoundESP::IsSoundValid(IEngineSound::SndInfo_t& pSound)
{
	for (auto i = 0; i < m_SoundBuffer.Count(); i++)
		if (m_SoundBuffer[i].m_nGuid == pSound.m_nGuid)
			return false;

	return true;
}