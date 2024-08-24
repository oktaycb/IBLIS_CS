#include "../../../precompiled.h"

void Hooks::PacketStart(int iIncomingSequence, int iOutgoingAcknowledged) {
	if (!g_Client.m_Processing)
		return g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);
	
	for (auto m_Cmd = g_Client.m_aCommandList.begin(); m_Cmd != g_Client.m_aCommandList.end(); m_Cmd++)
		if (*m_Cmd == iOutgoingAcknowledged)
		{
			g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);
			break;
		}

	for (auto m_Cmd = g_Client.m_aCommandList.begin(); m_Cmd != g_Client.m_aCommandList.end();)
		if (*m_Cmd < iOutgoingAcknowledged)
			m_Cmd = g_Client.m_aCommandList.erase(m_Cmd);
		else
			++m_Cmd;
}

void Hooks::PacketEnd() {
	if (!g_Client.m_Processing)
		return g_Hooks.m_ClientState.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);

#ifdef LEGACY
	if (g_Csgo.m_ClientState->m_ServerTick != g_Csgo.m_ClientState->m_DeltaTick)
		return g_Hooks.m_ClientState.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);

#else
	if (g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick != g_Csgo.m_ClientState->m_DeltaTick)
		return g_Hooks.m_ClientState.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);
#endif // LEGACY

	return g_Hooks.m_ClientState.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);
}

bool Hooks::SVCMsg_TempEntities(void* msg) {
	if (!this || !g_Client.m_Processing) {
		return g_Hooks.m_ClientState.GetOldMethod< TempEntities_t >(CClientState::TEMPENTITIES)(this, msg);
	}

	const bool ret = g_Hooks.m_ClientState.GetOldMethod< TempEntities_t >(CClientState::TEMPENTITIES)(this, msg);

#ifdef LEGACY
	CEventInfo* ei = g_Csgo.m_ClientState->m_Events;
	CEventInfo* next = nullptr;

	if (!ei) {
		return ret;
	}

	do {
		next = ei->m_Next;

		uint16_t classID = ei->m_ClassID - 1;

		auto m_pCreateEventFn = ei->m_client_class->m_pCreateEvent;
		if (!m_pCreateEventFn) {
			continue;
		}

		void* pCE = m_pCreateEventFn();
		if (!pCE) {
			continue;
		}

		if (classID == g_Netvars.GetClientID(HASH("CTEFireBullets"))) 
			ei->m_FireDelay = 0.0f;

		ei = next;
	} while (next != nullptr);
#endif
	return ret;
}

//for (auto m_Cmd = g_Client.m_aCommandList.begin(); m_Cmd != g_Client.m_aCommandList.end(); m_Cmd++)
//{
//	if (*m_Cmd != iOutgoingAcknowledged)
//		continue;
//
//	g_Client.m_aCommandList.erase(m_Cmd);
//	return g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);
//}

//if (g_Client.m_TransmissionCommands.empty())
//	return g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);

//for (auto m_Cmd = g_Client.m_TransmissionCommands.rbegin(); m_Cmd != g_Client.m_TransmissionCommands.rend(); ++m_Cmd)
//{
//	if (!m_Cmd->m_IsOutgoing)
//		continue;

//	if (m_Cmd->m_CommandNumber == iOutgoingAcknowledged || iOutgoingAcknowledged > m_Cmd->m_CommandNumber && (!m_Cmd->m_IsUsed || m_Cmd->m_PreviousCommandNumber == iOutgoingAcknowledged))
//	{
//		m_Cmd->m_PreviousCommandNumber = iOutgoingAcknowledged;
//		m_Cmd->m_IsUsed = true;
//		g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);
//		break;
//	}
//}

//auto bResult = false;

//for (auto m_Cmd = g_Client.m_TransmissionCommands.begin(); m_Cmd != g_Client.m_TransmissionCommands.end();)
//{
//	if (iOutgoingAcknowledged == m_Cmd->m_CommandNumber || iOutgoingAcknowledged == m_Cmd->m_PreviousCommandNumber)
//		bResult = true;

//	if (iOutgoingAcknowledged > m_Cmd->m_CommandNumber && iOutgoingAcknowledged > m_Cmd->m_PreviousCommandNumber)
//		m_Cmd = g_Client.m_TransmissionCommands.erase(m_Cmd);
//	else
//		++m_Cmd;
//}

//if (bResult)
//	return;

//for (auto m_Cmd = g_Client.m_aCommandList.begin(); m_Cmd != g_Client.m_aCommandList.end(); m_Cmd++)
//	if (*m_Cmd == iOutgoingAcknowledged)
//	{
//		g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);
//		break;
//	}

//for (auto m_Cmd = g_Client.m_aCommandList.begin(); m_Cmd != g_Client.m_aCommandList.end(); m_Cmd++)
//	if (*m_Cmd < iOutgoingAcknowledged)
//		g_Client.m_aCommandList.erase(m_Cmd);

//return g_Hooks.m_ClientState.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, iIncomingSequence, iOutgoingAcknowledged);