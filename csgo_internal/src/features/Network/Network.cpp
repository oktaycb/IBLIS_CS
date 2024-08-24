#include "../../precompiled.h"

void Network::Start(INetChannel* pNetChannelInfo)
{
	if (!pNetChannelInfo)
		return;

	m_Sequence = pNetChannelInfo->m_OutSequenceNr;
}

void Network::Finish(INetChannel* pNetChannelInfo)
{
	if (!g_Client.m_Processing)
		return;

	if (!pNetChannelInfo)
		return;

	if (g_Network.m_Sequence > 0)
	{
		int iChokedPackets = pNetChannelInfo->m_ChokedPackets;
		int iSequenceNumber = pNetChannelInfo->m_OutSequenceNr;

		pNetChannelInfo->m_OutSequenceNr = g_Network.m_Sequence;
		pNetChannelInfo->m_ChokedPackets = 0;

		pNetChannelInfo->SendDatagram();

		pNetChannelInfo->m_OutSequenceNr = iSequenceNumber;
		pNetChannelInfo->m_ChokedPackets = iChokedPackets;
		return;
	}

	int iChokedPackets = pNetChannelInfo->m_ChokedPackets;

	pNetChannelInfo->m_ChokedPackets = 0;
	pNetChannelInfo->SendDatagram();

	--pNetChannelInfo->m_OutSequenceNr;
	pNetChannelInfo->m_ChokedPackets = iChokedPackets;
}

float Network::GetLatency()
{
	return g_Client.m_LatencyOutgoing + g_Client.m_LatencyIncoming;
}