#include "../../precompiled.h"

bool GrenadesWarning::paint_warning() {

	static CTraceFilterSimple_game	filter{};
	CGameTrace						trace;
	//float						    percent = ((m_expire_time - g_Csgo.m_Globals->m_Curtime) / game::TICKS_TO_TIME(m_Tick));
	int								alpha = 0;

	// dont draw if there isn't any point.
	if (!m_Player || m_correct_way.size() < 2 || g_Csgo.m_Globals->m_Curtime >= m_expire_time)
		return false;

	// distance for red warning. TODO: maybe make it with damage?
	float	    distance = g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_Origin) / 12.f;

	Vector		prev   = m_correct_way.front();
	Vector		endpos = m_correct_way[m_correct_way.size() - 1];
	Vector2D		dummy_vec;
	
	bool enemy		=  m_Player->enemy(g_Client.m_LocalPlayer) && true;
	bool teammate	= m_Player->teammate(g_Client.m_LocalPlayer) && c_config::get()->m["gw"][2];
	bool local		= m_Player == g_Client.m_LocalPlayer && true;

	if (!local && !teammate && !enemy)
		return false;

	Color col =
		enemy ? Color(c_config::get()->c["grenade_warning_enemy_color"][0] * 255, c_config::get()->c["grenade_warning_enemy_color"][1] * 255, c_config::get()->c["grenade_warning_enemy_color"][2] * 255, c_config::get()->c["grenade_warning_enemy_color"][3] * 255) :
		local ? Color(c_config::get()->c["grenade_warning_color"][0] * 255, c_config::get()->c["grenade_warning_color"][1] * 255, c_config::get()->c["grenade_warning_color"][2] * 255, c_config::get()->c["grenade_warning_color"][3] * 255) :
		Color(c_config::get()->c["grenade_warning_team_color"][0] * 255, c_config::get()->c["grenade_warning_team_color"][1] * 255, c_config::get()->c["grenade_warning_team_color"][2] * 255, c_config::get()->c["grenade_warning_team_color"][3] * 255);

	// iterate and draw path.
	for (const auto& cur : m_correct_way) {
		Vector2D screen0, screen1;

		// draw the lines
		if (Math::WorldToScreen(prev, screen0) && Math::WorldToScreen(cur, screen1)) {
			new_render.Line(screen0, screen1, { 255, 255, 255, 255 }, 2.f);
		}

		// store point for next iteration.
		prev = cur;
	}

	// get center of mass for player.
	Vector center = g_Client.m_LocalPlayer->WorldSpaceCenter();

	// check if our path was obstructed by anything using a trace.
	g_Csgo.m_EngineTrace->TraceRay(Ray(Vector(m_Origin.x, m_Origin.y, m_Origin.z + 5.f), center), MASK_SHOT, (ITraceFilter*)&filter, &trace);

	if (trace.m_Player == g_Client.m_LocalPlayer) {
		if (m_id == HEGRENADE && distance <= 20)
			alpha = 255 - 255 * (distance / 20);

		if ((m_id == MOLOTOV || m_id == FIREBOMB) && distance <= 15)
			alpha = 255 - 255 * (distance / 15);
	}

	Vector2D vec;
	if (Math::WorldToScreen(endpos, vec)) {

		new_render.CircleFilled(vec, 30, Color(10, 10, 10, 180));

		//Render::circle(vec.x, vec.y, 24, 30, Color(0, 0, 0, 180));
		//Render::circle(vec.x, vec.y, 23, 30, Color(255, 0, 0, alpha));
		//Render::cs_big.string(vec.x, vec.y - 20.f, colors::white, m_id == HEGRENADE ? "j" : "l", Render::ALIGN_CENTER);
	}

	return true;
}

void GrenadesWarning::PhysicsCheckWaterTransition()
{
	int oldcont = GetWaterType();
	UpdateWaterState();
	int cont = GetWaterType();

	if (cont & MASK_WATER)
	{
		if (oldcont == CONTENTS_EMPTY)
		{
			if (m_Player->m_iEFlags() != EFL_NO_WATER_VELOCITY_CHANGE)
			{
				UpdatePath();
				m_Velocity.z *= 0.5f;
			}
		}
	}
}

// Purpose: Applies gravity to falling objects
void GrenadesWarning::PhysicsAddGravityMove(Vector& vecMove)
{
	float flGravity = g_Csgo.m_Gravity->GetFloat() * 0.4f;

	// move one tick using current velocity.
	vecMove.x = m_Velocity.x * g_Csgo.m_Globals->m_Interval;
	vecMove.y = m_Velocity.y * g_Csgo.m_Globals->m_Interval;

	// apply linear acceleration due to gravity.
	// calculate new z velocity.
	float newZVelocity = m_Velocity.z - flGravity * g_Csgo.m_Globals->m_Interval;

	// apply velocity to move, the average of the new and the old.
	vecMove.z = ((m_Velocity.z + newZVelocity) / 2.f) * g_Csgo.m_Globals->m_Interval;

	// write back new gravity corrected z-velocity.
	m_Velocity.z = newZVelocity;
}

void GrenadesWarning::PhysicsToss(Player* entity)  // PhysicsSimulate -> PhysicsToss
{
	trace_t	trace;
	Vector	move;

	if (m_detonated)
		return;

	PhysicsCheckWater();

	if (!m_Player->PhysicsRunThink())
		return;

	PhysicsAddGravityMove(move);
	PhysicsPushEntity(move, trace);

	//m_Player->PhysicsCheckVelocity();

	if (trace.m_Fraction != 1.f) {
		UpdatePath();
		ResolveFlyCollisionBounce(trace);
	}

	PhysicsCheckWaterTransition();
}

void GrenadesWarning::Physics_TraceEntity(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CGameTrace& ptr)
{
	UTIL_TraceEntity(vecAbsStart, vecAbsEnd, mask, ptr);

	if (ptr.m_StartSolid && ptr.m_Contents & CONTENTS_CURRENT_90)
	{
		ptr.m_clear();
		UTIL_TraceEntity(vecAbsStart, vecAbsEnd, mask & ~CONTENTS_CURRENT_90, ptr);
	}

	if (ptr.m_Fraction < 1.f || ptr.m_AllSolid || ptr.m_StartSolid)
	{
		if (ptr.m_Player && ptr.m_Player->IsPlayer())
		{
			ptr.m_clear();
			UTIL_TraceLine(vecAbsStart, vecAbsEnd, mask, m_Player, m_CollisionGroup, ptr);
		}
	}
}

void GrenadesWarning::PhysicsCheckSweep(const Vector& vecAbsStart, const Vector& vecAbsDelta, CGameTrace& pTrace)
{
	unsigned int mask; // = PhysicsSolidMaskForEntity();
	if (m_CollisionGroup == COLLISION_GROUP_DEBRIS)
		mask = (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER;
	else
		mask = MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CURRENT_90 | CONTENTS_HITBOX;

	Physics_TraceEntity(vecAbsStart, vecAbsStart + vecAbsDelta, mask, pTrace);
}

void GrenadesWarning::Touch(Entity* pOther, trace_t& pTrace)
{
	if (m_id == MOLOTOV || m_id == DECOY)
	{
		if (!pOther->IsPlayer() && pTrace.m_Plane.m_Normal.z >= std::cos(Math::DEG2RAD(g_Csgo.weapon_molotov_maxdetonateslope->GetFloat())))
			Detonate();
	}
	else if (m_id == TAG_GRENADE)
	{
		if (!pOther->IsPlayer())
			Detonate();
	}
}

void GrenadesWarning::PhysicsImpact(Entity* other, trace_t& trace)
{
	Touch(other, trace);
}

void GrenadesWarning::PhysicsPushEntity(const Vector& push, CGameTrace& pTrace)
{
	PhysicsCheckSweep(m_Origin, push, pTrace);

	if (pTrace.m_StartSolid) 
	{
		m_CollisionGroup = COLLISION_GROUP_INTERACTIVE_DEBRIS;

		UTIL_TraceLine(m_Origin - push, m_Origin + push, (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER, m_Player, m_CollisionGroup, pTrace);
	}

	if (pTrace.m_Fraction)
	{
		m_Origin = pTrace.m_EndPosition;
	}

	if (pTrace.m_Player)
	{
		PhysicsImpact(pTrace.m_Player, pTrace);
	}
}

#define	STOP_EPSILON	0.1
int GrenadesWarning::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;		// floor
	}
	if (!angle)
	{
		blocked |= 2;		// step
	}

	backoff = Math::DotProduct(in, normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}

void GrenadesWarning::ResolveFlyCollisionBounce(CGameTrace& trace) 
{
	// Get the impact surface's elasticity.
	float flSurfaceElasticity = 1.f;

	if (trace.m_Player)
	{
		if (game::IsBreakable(trace.m_Player))
		{
			if (trace.m_Player->m_iHealth() <= 0)
			{
				m_Velocity *= 0.4f;

				return;
			}
		}

		if (trace.m_Player->IsPlayer())
			flSurfaceElasticity = 0.3f;

		if (trace.m_Player->GetClientNetworkable()->GetIndex())
		{
			if (m_CollisionEntity == trace.m_Player)
			{
				if (trace.m_Player->IsPlayer())
				{
					m_CollisionGroup = COLLISION_GROUP_DEBRIS;
					return;
				}
			}

			m_CollisionEntity = trace.m_Player;
		}
	}

	// combine elasticities together.
	float flTotalElasticity = 0.45f * flSurfaceElasticity;

	if (flTotalElasticity >= 0.0f)
		flTotalElasticity = fminf(flTotalElasticity, 0.9f);
	else
		flTotalElasticity = 0.0f;

	// NOTE: A backoff of 2.0f is a reflection
	Vector vecAbsVelocity;
	PhysicsClipVelocity(m_Velocity, trace.m_Plane.m_Normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Get the total velocity (player + conveyors, etc.)
	const float flSpeedSqr = vecAbsVelocity.LengthSqr();

	// compute friction left.
	const float left = 1.f - trace.m_Fraction;

	// Stop if on ground.
	if (trace.m_Plane.m_Normal.z > 0.7f) // Floor
	{
		const float flSpeedSqr = vecAbsVelocity.Dot(vecAbsVelocity);
		if (flSpeedSqr > 96000.f)
		{
			auto l = vecAbsVelocity.Normalized().Dot(trace.m_Plane.m_Normal);
			if (l > 0.5f)
				vecAbsVelocity *= 1.f - l + 0.5f;
		}

		if (flSpeedSqr < 20.f * 20.f)
			m_Velocity = Vector(0, 0, 0);
		else
		{
			m_Velocity = vecAbsVelocity;
			PhysicsPushEntity(vecAbsVelocity * (left * g_Csgo.m_Globals->m_Interval), trace);
		}
	}
	else
	{
		m_Velocity = vecAbsVelocity;
		PhysicsPushEntity(vecAbsVelocity * (left * g_Csgo.m_Globals->m_Interval), trace);
	}
}

void GrenadesWarning::Think_Detonate() {
	
	if (m_next_think_tick > m_tickcount)
		return;

	switch (m_id)
	{
	case SMOKE:
		if (m_Velocity.LengthSqr() <= 0.1f * 0.1f)
			Detonate();

		break;
	case DECOY:
		if (m_Velocity.LengthSqr() <= 0.2f * 0.2f)
			Detonate();

		break;
	case FLASHBANG:
	case HEGRENADE:
	case MOLOTOV:
	case FIREBOMB:
		if (game::TICKS_TO_TIME(m_tickcount) > m_detonate_time)
			Detonate();

		if (m_nWaterType)
			m_Velocity *= 0.5f;

		break;
	default:
		break;
	}

	m_next_think_tick = m_tickcount + game::TIME_TO_TICKS(0.2f);
}

void GrenadesWarning::Think(Player* entity, int index, const Vector& origin, const Vector& velocity, float throw_time, int offset) {
	// reset data.
	reset();

	// set data.
	m_Player = entity;
	m_id = index;
	m_Origin = origin;
	m_Velocity = velocity;
	m_CollisionGroup = COLLISION_GROUP_PROJECTILE;
	m_detonated = false;

	const auto sample_tick = game::TIME_TO_TICKS(1.f / 30.f);
	m_last_update_tick = -sample_tick;

	switch (m_id) {
	case SMOKE:
		set_next_think(1.5f);
		break;
	case DECOY:
		set_next_think(2.f);
		break;
	case FLASHBANG:
	case HEGRENADE:
		m_detonate_time = 1.5f;
		set_next_think(0.02f);
		break;
	case MOLOTOV:
	case FIREBOMB:
		m_detonate_time = g_Csgo.molotov_throw_detonate_time->GetFloat();
		set_next_think(0.02f);
		break;
	}

	for (; m_tickcount < game::TIME_TO_TICKS(60.f); ++m_tickcount) {
		
		Think_Detonate();

		if (m_tickcount < offset)
			continue;

		PhysicsToss(entity);

		if (m_last_update_tick + sample_tick <= m_tickcount)
			UpdatePath();

		if (m_detonated)
			break;
	}

	if (m_last_update_tick + sample_tick <= m_tickcount) {
		UpdatePath();
	}

	m_expire_time = throw_time + game::TICKS_TO_TIME(m_tickcount);

	// don't erase if we are drawing.
	if (!g_grenades_warning.paint_warning()) {

		// reset.
		reset();
	}
}