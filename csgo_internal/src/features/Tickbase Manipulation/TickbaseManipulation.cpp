#include "../../precompiled.h"

void TickbaseManipulation::ManipulateTickbase()
{
	if (!m_Data.m_Selected)
		return;

	BreakLagCompensation();

	if (!g_Client.GetModifablePacket())
		return;

	if (!(g_Client.GetModifableCommand()->m_Buttons & IN_ATTACK))
		return;

	if (!g_Client.CanFireWeapon())
		return;

	if (!g_Config.vb(g_Items.m_Aimbot.m_General.m_TickbaseManipulationSelection)[0])
		return;

	if (m_Data.m_ChargeTick) 
	{
		m_Data.m_ChargeTick = game::TIME_TO_TICKS(g_Csgo.m_Globals->m_RealTime + 1.f);
		return;
	}

	if (m_Data.m_Selected == EXPLOIT::DOUBLETAP)
		m_Data.m_ShiftMode = MODE::SHIFT_MOVE;
	else m_Data.m_ShiftMode = MODE::SHIFT_BUFFER;
	
	m_Data.m_ShiftAmount = m_Data.m_NextShift;
	m_Data.m_ShiftedTickbase = m_Data.m_ShiftAmount;
	m_Data.m_ShiftCommand = g_Client.GetModifableCommand()->m_CommandNumber;
}

void TickbaseManipulation::BreakLagCompensation()
{
	if (!g_Config.vb(g_Items.m_Aimbot.m_General.m_TickbaseManipulationSelection)[1])
		return;

	if (m_Data.m_Selected != EXPLOIT::DOUBLETAP || m_Data.m_ChargeTick || !g_Client.GetModifablePacket())
		return;

	if (g_Client.CanFireWeapon() && (g_Client.GetModifableCommand()->m_Buttons & IN_ATTACK))
		return;

	m_Data.m_ShiftMode = MODE::SHIFT_BUFFER;
	m_Data.m_ShiftAmount = g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2;
}

void TickbaseManipulation::SetupCommand()
{
	if (!g_Client.m_Processing)
	{
		m_Data.m_Selected = 0;
		m_Data.m_TicksAllowedForProcessing = 0;
		return;
	}

	if (g_Client.m_LocalPlayer->m_fFlags() & FL_FROZEN || g_Csgo.m_GameRules->m_bFreezePeriod())
	{
		m_Data.m_Selected = 0;
		m_Data.m_TicksAllowedForProcessing = 0;
		return;
	}

	Weapon* pCombatWeapon = g_Client.m_Weapon;
	if (!pCombatWeapon)
		return;

	if (c_config::get()->b["fakewalk"] && keyhandler::get()->auto_check("fakewalk_key"))
	{
		m_Data.m_TicksAllowedForProcessing = 0;
		m_Data.m_Selected = 0;
		return;
	}

	bool bIsDoubleTapActive = g_Aimbot.m_Config.m_TickbaseManipulation;
	bool bIsHideShotsActive = false;

	int m_OldSelected = m_Data.m_Selected;

	m_Data.m_Selected = 0;

	if (bIsDoubleTapActive) m_Data.m_Selected = EXPLOIT::DOUBLETAP;
	else if (bIsHideShotsActive) m_Data.m_Selected = EXPLOIT::HIDESHOTS;

	if (!m_OldSelected && m_Data.m_Selected)
	{
		m_Data.m_ChargeCommand = g_Client.GetModifableCommand()->m_CommandNumber;
		m_Data.m_ChargeAmount = 17 - m_Data.m_TicksAllowedForProcessing;
		m_Data.m_ChargedTickbase = m_Data.m_ChargeAmount;
	}

	if (m_OldSelected == EXPLOIT::DOUBLETAP || m_OldSelected == EXPLOIT::HIDESHOTS)
	{
		if (!m_Data.m_Selected)
		{
			m_Data.m_NextShift = m_Data.m_TicksAllowedForProcessing;
			m_Data.m_ShiftMode = MODE::SHIFT_MOVE;
			m_Data.m_ShiftAmount = m_Data.m_NextShift;
			m_Data.m_ShiftCommand = g_Client.GetModifableCommand()->m_CommandNumber;
			m_Data.m_ShiftedTickbase = m_Data.m_ShiftAmount;
		}
	}

	m_Data.m_NextShift = 0;
	switch (m_Data.m_Selected)
	{
	case EXPLOIT::DOUBLETAP: m_Data.m_NextShift = g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2; break;
	case EXPLOIT::HIDESHOTS: m_Data.m_NextShift = 9; break;
	}

	if (m_Data.m_Selected)
	{
		g_Client.m_MaxLagAmount = 1;
		if (m_Data.m_ChargeTick)
		{
			if (game::TIME_TO_TICKS(g_Csgo.m_Globals->m_RealTime) == m_Data.m_ChargeTick)
			{
				m_Data.m_ChargeCommand = g_Client.GetModifableCommand()->m_CommandNumber;
				m_Data.m_ChargeAmount = 17 - m_Data.m_TicksAllowedForProcessing;
				m_Data.m_ChargedTickbase = m_Data.m_ChargeAmount;
			}

			m_Data.m_NextShift = 0;
		}
	}
}

bool TickbaseManipulation::IsFinalTick()
{
	return m_Data.m_Selected && m_Data.m_ShiftAmount;
}

bool TickbaseManipulation::PerformCommand()
{
	SetupCommand();

	if (m_Data.m_ChargeTick || !m_Data.m_ShiftingTickbase || m_Data.m_ShiftMode != MODE::SHIFT_MOVE)
		return true;

	g_Client.GetModifableCommand()->m_Buttons &= ~(IN_ATTACK | IN_ATTACK2);

	if (g_Aimbot.CanAutoStop())
		g_Aimbot.AutomaticStop();

	g_Movement.AutoPeek();
	g_Movement.FastStop();
	g_Movement.FakeWalk();

	if (g_Client.GetModifableCommand()->m_SideMove > 5.0f)
		g_Client.GetModifableCommand()->m_SideMove = 450.0f;
	else if (g_Client.GetModifableCommand()->m_SideMove < -5.0f)
		g_Client.GetModifableCommand()->m_SideMove = -450.0f;

	if (g_Client.GetModifableCommand()->m_ForwardMove > 5.0f)
		g_Client.GetModifableCommand()->m_ForwardMove = 450.0f;
	else if (g_Client.GetModifableCommand()->m_ForwardMove < -5.0f)
		g_Client.GetModifableCommand()->m_ForwardMove = -450.0f;

	if (!m_Data.m_IsFinalTick)
		g_Client.GetModifablePacket() = false;

	g_InputPrediction.Run();

	if (g_Client.GetModifablePacket())
		g_Client.m_aCommandList.emplace_back(g_Client.m_Command->m_CommandNumber);

	return false;
}

void TickbaseManipulation::PerformPackets()
{
	if (!m_Data.m_ShiftAmount || m_Data.m_ShiftMode != MODE::SHIFT_MOVE)
		return;

	for (int32_t iShiftTime = 0; iShiftTime < m_Data.m_ShiftAmount; iShiftTime++)
	{
		m_Data.m_RemainToShift = m_Data.m_ShiftAmount - iShiftTime;
		m_Data.m_IsFinalTick = m_Data.m_RemainToShift == 1;
		m_Data.m_ShiftingTickbase = true;

		g_Detours.m_CL_Move(0.0f, m_Data.m_IsFinalTick);
	}

	m_Data.m_ShiftingTickbase = false;
	m_Data.m_TicksAllowedForProcessing = 0;
	m_Data.m_ChargeTick = game::TIME_TO_TICKS(g_Csgo.m_Globals->m_RealTime + 1.f);
	m_Data.m_ShiftAmount = 0;
	m_Data.m_NextShift = 0;
	m_Data.m_IsFinalTick = false;
}

bool TickbaseManipulation::IsAllowedToRun()
{
	g_Network.m_SendPacket = true;
	g_Network.Start(g_Csgo.m_NetChannelInfo);

	if (!m_Data.m_Selected)
		return true;

	if (m_Data.m_ChargeTick)
	{
		if (game::TIME_TO_TICKS(g_Csgo.m_Globals->m_RealTime) <= m_Data.m_ChargeTick)
			return true;
	}

	m_Data.m_ChargeTick = 0;
	if (m_Data.m_TicksAllowedForProcessing >= g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2)
		return true;

	m_Data.m_TicksAllowedForProcessing++;
	return false;
}