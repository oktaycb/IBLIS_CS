#pragma once

class Grenades {
private:
	enum GrenadeFlags : size_t {
		NONE = 0,
		DETONATE,
		BOUNCE,
	};

	struct bounce_t {
		Vector point;
		Color  color;
	};

	using path_t    = std::vector< Vector >;
	using bounces_t = std::vector< bounce_t >;

private:
	Player*   m_Player;
	int       m_id;
	path_t    m_path;
	bounces_t m_bounces;
	float     m_vel, m_power;
	Vector    m_start, m_Velocity, m_move;
public:
	void   reset();
	void   paint();
	void   think();
	void   think_warning(Entity* player);
	void   simulate();
	void   setup();
	size_t advance(size_t tick);
	bool   detonate(size_t tick, CGameTrace & trace);
	void   ResolveFlyCollisionBounce(CGameTrace & trace);
	void   PhysicsPushEntity(Vector & start, const Vector & move, CGameTrace & trace, Entity * ent);
	void   TraceHull(const Vector & start, const Vector & end, CGameTrace & trace, Entity * ent);
	void   PhysicsAddGravityMove(Vector & move);
	void   PhysicsClipVelocity(const Vector & in, const Vector & normal, Vector & out, float overbounce);
};

extern Grenades g_Grenades;