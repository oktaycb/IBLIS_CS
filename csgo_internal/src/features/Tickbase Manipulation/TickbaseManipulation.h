#pragma once

enum EXPLOIT : int
{
	HIDESHOTS = 1,
	LAGPEEK,
	DOUBLETAP
};

enum MOD : int
{
	FIRE = 1,
};

enum MODE : int
{
	SHIFT_BUFFER,
	SHIFT_MOVE,
};

class CLCMove {
public:
	char PAD0[0x8];
public:
	int nBackupCommands;
	int nNewCommands;
public:
	std::string* pData;
public:
	ReadBF pDataIn;
	WriteBF pDataOut;
};

template < typename T > class CNetMessagePB : public CNetMessage, public T {};
using CLC_Move = CNetMessagePB< CLCMove >;

#define NUM_NEW_COMMAND_BITS 4
#define MAX_NEW_COMMANDS ((1 << NUM_NEW_COMMAND_BITS)-1)
#define MIN_NEW_COMMANDS 1

#define NUM_BACKUP_COMMAND_BITS	3
#define MAX_BACKUP_COMMANDS	((1 << NUM_BACKUP_COMMAND_BITS)-1)

class TickbaseManipulation
{
public:
	void PerformPackets();
	bool IsFinalTick();
	bool PerformCommand();
	bool IsAllowedToRun();
	void ManipulateTickbase();
	void BreakLagCompensation();
	void SetupCommand();
public:
	int32_t GetShiftedTickbase() { return m_Data.m_ShiftedTickbase; };
	int32_t GetChargedTickbase() { return m_Data.m_ChargedTickbase; };
	int32_t GetShiftAmount() { return m_Data.m_ShiftAmount; };
	int32_t GetForcedShiftAmount() { return m_Data.m_ShiftAmount; };
	int32_t GetShiftMode() { return m_Data.m_ShiftMode; };
	int32_t GetShiftCommand() { return m_Data.m_ShiftCommand; };
	int32_t GetActiveExploit() { return m_Data.m_Selected; };
	bool IsShiftingTickbase() { return m_Data.m_ShiftingTickbase; };
	int32_t GetChargeCommand() { return m_Data.m_ChargeCommand; };
	void ResetShiftAmount() { m_Data.m_ShiftAmount = 0; };
	bool CanSkipInterpolation() { return m_Data.m_bSkipInterpolation; };
public:
	struct
	{
		bool m_NeedsCharge = false;
		bool m_Manipulated = false;

		int32_t m_NextShift = 0;
		int32_t m_RemainToShift = 0;
		int32_t m_Selected = 0;
		int32_t m_TicksAllowedForProcessing = 0;
		int32_t m_ChargeTick = 0;

		int32_t m_ShiftCommand = 0;
		int32_t m_ChargeCommand = 0;

		int32_t m_ShiftAmount = 0;
		int32_t m_ChargeAmount = 0;

		int32_t m_ShiftedTickbase = 0;
		int32_t m_ChargedTickbase = 0;

		int32_t m_ShiftMode = 0;
		int32_t m_ChargeMode = 0;

		bool m_bSkipInterpolation = false;
		bool m_ShiftingTickbase = false;
		bool m_IsFinalTick = false;
	} m_Data;

	CMoveData m_MoveData = CMoveData();
};

inline TickbaseManipulation g_TickbaseManipulation;