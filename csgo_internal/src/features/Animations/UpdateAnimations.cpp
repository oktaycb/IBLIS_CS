#include "../../precompiled.h"

void AimPlayer::OnNetUpdate(Player* pPlayer)
{
	if (m_Player != pPlayer) 
		m_Records.clear();

	m_Player = pPlayer;
	if (!m_Player)
		return;

	if (m_Player->GetClientNetworkable()->IsDormant())
	{
		if (m_Records.empty() || !m_Records.front().get()->m_Dormant) {
			m_Records.emplace_front(std::make_shared< LagRecord >(m_Player));
			m_Records.front().get()->m_Dormant = true;
		}

		g_Resolver.Reset(this, false);
		return;
	}

	if (m_Records.empty() || CanUpdateAnimation()) {
		m_Records.emplace_front(std::make_shared< LagRecord >(m_Player));

		if (m_Player->teammate(g_Client.GetLocalPlayer()))
			UpdateAlliesAnimations();
		else UpdateEnemiesAnimations();

		UpdateInterpolation();
	}

	while (m_Records.size() > g_Client.m_TickRate)
		m_Records.pop_back();
}

bool AimPlayer::CanUpdateAnimation()
{
	if (m_Player->m_flSimulationTime() > m_Player->m_flOldSimulationTime())
		return true;

	if (m_Player->m_vecOrigin() != m_Player->m_vecOldOrigin())
		return true;

	if (m_Records.empty())
		return false;

	LagRecord* pRecord = m_Records.front().get();
	if (!pRecord)
		return false;

	for (int i = 0; i < ANIMATION_LAYER_COUNT; i++)
		if (m_Player->m_AnimOverlay()[i].GetCycle() != pRecord->m_Layers[i].GetCycle() ||
			m_Player->m_AnimOverlay()[i].GetPlaybackRate() != pRecord->m_Layers[i].GetPlaybackRate() ||
			m_Player->m_AnimOverlay()[i].GetWeight() != pRecord->m_Layers[i].GetWeight() ||
			m_Player->m_AnimOverlay()[i].GetSequence() != pRecord->m_Layers[i].GetSequence())
			return true;

	return false;
}

void AimPlayer::UpdateInterpolation()
{
	int m_ChokedCommands = game::TIME_TO_TICKS(m_Player->m_flSimulationTime() - m_Player->m_flOldSimulationTime());
	if (m_ChokedCommands < 1)
		return;

	bool m_ShouldInterpolate = m_Player->ShouldInterpolate();
	if (!m_ShouldInterpolate || game::IsFakePlayer(m_Player->GetClientNetworkable()->GetIndex()))
		return;

	for (int i = 0; i < m_Player->m_VarMap().m_Entries.Count(); i++)
	{
		VarMapEntry_t* pEntries = &m_Player->m_VarMap().m_Entries[i];
		if (!pEntries || !pEntries->m_Watcher)
			continue;

		pEntries->m_Watcher->SetInterpolationAmount(game::TICKS_TO_TIME(m_ChokedCommands));
	}
}

void AimPlayer::reset() {
	m_ResolverMode = AimPlayer::RESOLVER::LEGIT;
	m_Player = nullptr;
	m_Records.clear();
	m_SpawnTime = 0.f;
	m_FakeFlickMissedShots = 0;
	g_Resolver.Reset(this, false);
}