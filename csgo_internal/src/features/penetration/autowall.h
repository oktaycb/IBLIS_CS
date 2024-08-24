#pragma once

namespace Penetration {
	struct PenetrationInput_t {
		Player* m_From;
		Player* m_Player;
		Vector  m_Position;
		Vector  m_EyePosition;
		float	m_Damage;
		float   m_PenetrationDamage;
		bool	m_Penetrate;
	};

	struct PenetrationOutput_t {
		Player* m_Player;
		float   m_Damage;
		int     m_HitGroup;
		bool    m_Penetration;

        __forceinline PenetrationOutput_t() : m_Player{ nullptr }, m_Damage{ 0.f }, m_HitGroup{ -1 }, m_Penetration{ false } {}
	};

    float scale( Player* player, float damage, float armor_ratio, int hitgroup);
	bool  TraceToExit(const Vector& start, const Vector& dir, Vector& out, CGameTrace* enter_trace, CGameTrace* exit_trace);
	void  ClipTraceToPlayer(const Vector& start, const Vector& end, uint32_t mask, CGameTrace* tr, ITraceFilter* filter, Player* player, float min = 0);
	bool  Run( PenetrationInput_t* in, PenetrationOutput_t* out );
}