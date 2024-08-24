#pragma once

class CSoundESP
{
public:
	struct SoundPlayer
	{
		float	m_RecieveTime = 0.0f;
		Vector	m_Origin = Vector(0, 0, 0);
		int		m_Flags = 0;
	};

	void Init();
	bool RecievedAdjustSound(Player* player);
	void SetupAdjust(Player* player, IEngineSound::SndInfo_t& sound);
	bool IsSoundValid(IEngineSound::SndInfo_t& sound);
	void Reset(Player* pPlayer);

	SoundPlayer m_SoundPlayer[65];

	CUtlVector <IEngineSound::SndInfo_t> m_SoundBuffer;
	CUtlVector <IEngineSound::SndInfo_t> m_CurrentSounds;
};

inline CSoundESP g_SoundESP;