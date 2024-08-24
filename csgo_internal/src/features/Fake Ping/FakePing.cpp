#include "../../precompiled.h"

void CFakePing::UpdateIncomingSequences() {
	if (m_Sequences.empty() || g_Csgo.m_NetChannelInfo->m_InSequence > m_Sequences.front().m_Sequence)
		m_Sequences.emplace_front(g_Csgo.m_Globals->m_Curtime, g_Csgo.m_NetChannelInfo->m_InReliableState, g_Csgo.m_NetChannelInfo->m_InSequence);

	while (m_Sequences.size() > 2048)
		m_Sequences.pop_back();
}