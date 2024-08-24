#include "../../../precompiled.h"

#define NET_FRAMES_BACKUP	128		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)

bool Hooks::SendNetMsg(INetMessage& pNetMessage, bool bForceReliable, bool bVoice)
{
	INetChannel* pNetChannelInfo = reinterpret_cast<INetChannel*>(this);
	if (!pNetChannelInfo)
		return g_Hooks.m_NetChannel.GetOldMethod< SendNetMsg_t >(INetChannel::SENDNETMSG)(this, pNetMessage, bForceReliable, bVoice);

	if (pNetMessage.GetType() == 14)
		return false;

	if (pNetMessage.GetGroup() == 9)
		bVoice = true;

	return g_Hooks.m_NetChannel.GetOldMethod< SendNetMsg_t >(INetChannel::SENDNETMSG)(this, pNetMessage, bForceReliable, bVoice);
}

int Hooks::SendDatagram(void* data) {
	INetChannel* pNetChannelInfo = reinterpret_cast<INetChannel*>(this);
	if (!pNetChannelInfo || !g_Config.b(g_Items.m_Misc.m_FakeLatency) || !g_KeyInput.Key(g_Items.m_Misc.m_FakeLatencyKey, g_Items.m_Misc.m_FakeLatencyMode))
		return g_Hooks.m_NetChannel.GetOldMethod< SendDatagram_t >(INetChannel::SENDDATAGRAM)(this, data);

	int iBackupSequence = pNetChannelInfo->m_InSequence;
	int iBackupRelState = pNetChannelInfo->m_InReliableState;

	if (!g_FakePing.m_Sequences.empty())
	{
		for (auto& pSequences : g_FakePing.m_Sequences)
		{
			float flCorrect = std::max(0.f, (g_Config.i(g_Items.m_Misc.m_FakeLatencyAmount) / 1000.f) + g_Client.m_Lerp - g_Client.m_LatencyOutgoing);

			if (g_Csgo.m_Globals->m_Curtime - pSequences.m_Curtime > flCorrect)
			{
				pNetChannelInfo->m_InSequence = pSequences.m_Sequence;
				pNetChannelInfo->m_InReliableState = pSequences.m_RelState;
				break;
			}
		}
	}

	int bOldMethod = g_Hooks.m_NetChannel.GetOldMethod< SendDatagram_t >(INetChannel::SENDDATAGRAM)(this, data);

	pNetChannelInfo->m_InSequence = iBackupSequence;
	pNetChannelInfo->m_InReliableState = iBackupRelState;

	return bOldMethod;
}

void Hooks::ProcessPacket( void* packet, bool bHasHeader ) {
	INetChannel* pNetChannelInfo = reinterpret_cast<INetChannel*>(this);
	if (!pNetChannelInfo)
		return g_Hooks.m_NetChannel.GetOldMethod< ProcessPacket_t >(INetChannel::PROCESSPACKET)(this, packet, bHasHeader);

	// call original.
	g_Hooks.m_NetChannel.GetOldMethod< ProcessPacket_t >( INetChannel::PROCESSPACKET )( this, packet, bHasHeader );

	// TODO@Monster: Use Hooks*this.
	g_FakePing.UpdateIncomingSequences( );

#ifdef LEGACY
	for( CEventInfo* it{ g_Csgo.m_ClientState->m_Events }; it != nullptr; it = it->m_Next ) {
		if( !it || it->m_ClassID == -1 )
			continue;

		it->m_FireDelay = 0.f;
	}
#endif
	g_Csgo.m_Engine->FireEvents( );
}

void Hooks::SetChoked() {
	INetChannel* pNetChannelInfo = reinterpret_cast<INetChannel*>(this);
	if (!pNetChannelInfo)
		return g_Hooks.m_NetChannel.GetOldMethod< SetChoked_t >(INetChannel::SETCHOKED)(this);

	if (!g_Client.m_Processing || g_TickbaseManipulation.m_Data.m_ShiftingTickbase)
		return g_Hooks.m_NetChannel.GetOldMethod< SetChoked_t >(INetChannel::SETCHOKED)(this);

	g_Network.m_SendPacket = false;
	g_Network.Finish(pNetChannelInfo);

	return g_Hooks.m_NetChannel.GetOldMethod< SetChoked_t >(INetChannel::SETCHOKED)(this);
}