#pragma once

class GrenadesWarning {
public:
	void Think(Player* entity, int index, const Vector& origin, const Vector& velocity, float throw_time, int offset);
	void PhysicsCheckWaterTransition();
	void Touch(Entity* pOther, trace_t& g_TouchTrace);
	void PhysicsImpact(Entity* other, trace_t& trace);
	void PhysicsPushEntity(const Vector& push, CGameTrace& pTrace);
	void ResolveFlyCollisionBounce(CGameTrace& trace);
	void Think_Detonate();
	void PhysicsAddGravityMove(Vector& move);
	void PhysicsToss(Player* entity);
	void Physics_TraceEntity(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CGameTrace& ptr);
	void PhysicsCheckSweep(const Vector& vecAbsStart, const Vector& vecAbsDelta, CGameTrace& pTrace);
	int  PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);

	void reset() {
		m_Player = nullptr;
		m_tickcount = 0;
		m_next_think_tick = 0;
		m_nWaterType = 0;
		m_detonate_time = 0;
		m_detonated = false;
		m_last_update_tick = 0;
		m_Origin.clear();
		m_correct_way.clear();
		m_bounces.clear();
	}

	void Detonate()
	{
		m_detonated = true;
		UpdatePath();
	}

	void UpdatePath()
	{
		m_correct_way.emplace_back(m_Origin);
		m_last_update_tick = m_tickcount;
	}

	__forceinline void set_next_think(float think_time) {
		m_next_think_tick = game::TIME_TO_TICKS(think_time);
	}

private:
	struct bounce_t {
		Vector point;
		Color  color;
	};

	using path_t = std::vector< Vector >;
	using bounces_t = std::vector< bounce_t >;
public:
	bool										paint_warning();
	bool										m_detonated;
	Player*										m_Player;
	Entity*										m_CollisionEntity;
	Vector										m_Origin, m_Velocity;
	Collision_Group_t							m_CollisionGroup;
	float										m_detonate_time, m_expire_time;
	int											m_id, m_tickcount, m_next_think_tick, m_last_update_tick;
	path_t										m_correct_way;
	bounces_t									m_bounces;
private:
	int GetWaterType() const
	{
		int out = 0;
		if (m_nWaterType & 1)
			out |= CONTENTS_WATER;
		if (m_nWaterType & 2)
			out |= CONTENTS_SLIME;
		return out;
	}

	void SetWaterType(int nType)
	{
		m_nWaterType = 0;
		if (nType & CONTENTS_WATER)
			m_nWaterType |= 1;
		if (nType & CONTENTS_SLIME)
			m_nWaterType |= 2;
	}

	void NormalizedToCollisionSpace(const Vector& in, Vector* pResult)
	{
		Vector m_vecMins(-2.f, -2.f, -2.f);
		Vector m_vecMaxs(2.f, 2.f, 2.f);

		pResult->x = Math::Lerp(m_vecMins.x, m_vecMaxs.x, in.x) + m_Origin.x;
		pResult->y = Math::Lerp(m_vecMins.y, m_vecMaxs.y, in.y) + m_Origin.y;
		pResult->z = Math::Lerp(m_vecMins.z, m_vecMaxs.z, in.z) + m_Origin.z;
	}

	int GetWaterLevel()
	{
		return m_nWaterLevel;
	}

	void SetWaterLevel(int nLevel)
	{
		m_nWaterLevel = nLevel;
	}

	bool PhysicsCheckWater()
	{
		return GetWaterLevel() > 1;
	}

	void UpdateWaterState()
	{
		Vector point;
		NormalizedToCollisionSpace(Vector(0.5f, 0.5f, 0.f), &point);

		SetWaterLevel(0);
		SetWaterType(CONTENTS_EMPTY);

		auto cont = g_Csgo.m_EngineTrace->GetPointContents_WorldOnly(point, MASK_WATER);

		if ((cont & MASK_WATER) == 0)
			return;

		SetWaterType(cont);
		SetWaterLevel(1);
	}

	inline void UTIL_TraceHull(const Vector& vecAbsStart, const Vector& vecAbsEnd, const Vector& hullMin, const Vector& hullMax, unsigned int mask, const Entity* ignore, int collisionGroup, trace_t& ptr)
	{
		CTraceFilterSimple_game traceFilter(ignore, collisionGroup);
		g_Csgo.m_EngineTrace->TraceRay(Ray(vecAbsStart, vecAbsEnd, hullMin, hullMax), mask, (ITraceFilter*)&traceFilter, &ptr);
	}

	inline void UTIL_TraceEntity(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, trace_t& ptr)
	{
		Vector mins(-2.f, -2.f, -2.f);
		Vector maxs(2.f, 2.f, 2.f);
		UTIL_TraceHull(vecAbsStart, vecAbsEnd, mins, maxs, mask, m_Player, m_CollisionGroup, ptr);
	}

	inline void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const Entity* ignore, int collisionGroup, trace_t& ptr)
	{
		CTraceFilterSimple_game traceFilter(ignore, collisionGroup);
		g_Csgo.m_EngineTrace->TraceRay(Ray(vecAbsStart, vecAbsEnd), mask, (ITraceFilter*)&traceFilter, &ptr);
	}
private:
	uint8_t m_nWaterType = 0;
	uint8_t m_nWaterLevel = 0;
};

inline GrenadesWarning g_grenades_warning;