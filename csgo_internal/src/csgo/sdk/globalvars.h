#pragma once

class CSaveRestoreData;

class CGlobalVarsBase {
public:
	float			    m_RealTime;
	int				    m_FrameCount;
	float			    m_AbsFrametime;
	float			    m_AbsFrameStartTime;
	float			    m_Curtime;
	float		        m_FrameTime;
	int				    m_MaxClients;
	int				    m_TickCount;
	float		        m_Interval;
	float		        m_InterpolationAmount;
	int	                m_FrameSimulationTicks;
	int				    m_NetworkProtocol;
	CSaveRestoreData*   m_SaveData;
	bool			    m_Client;
	bool		        m_RemoteClient;
	int			        m_TimeStampNetworkingBase;
	int			        m_TimeStampRandomizeWindow;
};