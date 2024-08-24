#include "../../precompiled.h"

class Network
{
public:
	void ProcessInterpolation(bool bPostFrame);
	void Start(INetChannel* pNetChannel);
	void Finish(INetChannel* pNetChannel);
public:
	float GetLatency();
public:
	int m_Sequence = 0;
	bool m_SendPacket = false;
	float m_Interpolation = 0.0f;
	int32_t m_FinalPredictedTick = 0;
};

inline Network g_Network;