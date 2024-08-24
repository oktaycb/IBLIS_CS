#include "../../precompiled.h"

Grenades g_Grenades{};;

void Grenades::reset() {
	m_start = Vector{};
	m_move = Vector{};
	m_Velocity = Vector{};
	m_vel = 0.f;
	m_power = 0.f;

	m_path.clear();
	m_bounces.clear();
}

void Grenades::paint() {

	static CTraceFilterSimple_game filter{};
	CGameTrace	                   trace;
	std::pair< float, Player* >    target{ 0.f, nullptr };
	Color col = Color(c_config::get()->c["grenade_color"][0] * 255, c_config::get()->c["grenade_color"][1] * 255, c_config::get()->c["grenade_color"][2] * 255, c_config::get()->c["grenade_color"][3] * 255);

	if (!c_config::get()->b["grenade_simulation"])
		return;

	// dont process if dead.
	if (!g_Client.m_Processing)
		return;

	// dont draw if there isn't any point.
	if (m_path.size() < 2)
		return;

	// setup trace filter for later.
	filter.SetPassEntity(g_Client.m_LocalPlayer);

	// first point.
	Vector prev = m_path.front();
	Vector endpos = m_path[m_path.size() - 1];

	//if (m_id == SMOKE)
	//	Render::Polygon3D(endpos, 32, 130, col, false);

	//else if (m_id == MOLOTOV || m_id == FIREBOMB)
	//	Render::Polygon3D(endpos, 32, 140, col, false);

	//else if (m_id == HEGRENADE || m_id == DECOY || m_id == FLASHBANG)
	//	Render::Polygon3D(endpos, 32, 50, col, false);

	// iterate and draw path.
	for (const auto& cur : m_path) {
		Vector2D screen0, screen1;

		if (Math::WorldToScreen(prev, screen0) && Math::WorldToScreen(cur, screen1)) {
			new_render.Line(screen0, screen1, col);
			new_render.Line(screen0 + 1, screen1 + 1, col); // another line because wanted it thicker.
		}

		// store point for next iteration.
		prev = cur;
	}

	// iterate all entities.
	for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i) {
		Player* entity = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
		if (!g_Aimbot.IsValidTarget(entity))
			continue;

		// get center of mass for player.
		Vector center = entity->WorldSpaceCenter();

		// get Delta between center of mass and final nade pos.
		Vector Delta = center - prev;

		if (m_id == HEGRENADE) {
			// pGrenade->m_flDamage = 100;
			// pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;

			// is within damage radius?
			if (Delta.Length() > 350.f)
				continue;

			// check if our path was obstructed by anything using a trace.
			g_Csgo.m_EngineTrace->TraceRay(Ray(prev, center), MASK_SHOT, (ITraceFilter*)&filter, &trace);

			// something went wrong here.
			if (!trace.m_Player || trace.m_Player != entity)
				continue;

			// rather 'interesting' formula by valve to compute damage.
			float d = (Delta.Length() - 25.f) / 140.f;
			float damage = 105.f * std::exp(-d * d);

			// scale damage.
			damage = Penetration::scale(entity, damage, 1.f, static_cast<int>(Hitgroups_t::CHEST));

			// clip max damage.
			damage = std::min(damage, (entity->m_ArmorValue() > 0) ? 57.f : 98.f);

			// better target?
			if (damage > target.first) {
				target.first = damage;
				target.second = entity;
			}
		}
	}

	// we have a target for damage.
	if (target.second) {
		Vector2D screen;

		// replace the last bounce with green.
		if (!m_bounces.empty())
			m_bounces.back().color = { 0, 255, 0, 255 };

	/*	if (Render::WorldToScreen(prev, screen))
			Render::esp_grenades.string(screen.x, screen.y + 5, { 255, 255, 255, 0xb4 }, tfm::format(XOR("%i"), (int)target.first), Render::ALIGN_CENTER);*/
	}

	// Render bounces.
	for (const auto& b : m_bounces) {
		Vector2D screen;

		//if (Math::WorldToScreen(b.point, screen))
		//	Render::RectFilled(screen.x - 2, screen.y - 2, 4, 4, b.color);
	}
}

void Grenades::think() {

	// reset some data.
	reset();

	if (!c_config::get()->b["grenade_simulation"])
		return;

	if (!g_Client.m_Processing || !g_Client.m_Weapon || !g_Client.m_WeaponInfo)
		return;

	// validate nade.
	if (g_Client.m_WeaponType != WEAPONTYPE_GRENADE)
		return;

	m_id = g_Client.m_WeaponItemDefinition;
	m_power = g_Client.m_Weapon->m_flThrowStrength();
	m_vel = g_Client.m_WeaponInfo->m_throw_velocity;

	simulate();
}

void Grenades::think_warning(Entity* entity) {

	// backup stuff
	static auto last_tick = g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick;

	// clear.
	if (g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick != last_tick) {
		g_grenades_warning.reset();
		last_tick = g_Csgo.m_ClientState->m_ClockDriftMgr.m_ServerTick;
	}

	// should never happen
	if (!entity || entity->GetClientNetworkable()->IsDormant())
		return;

	// there is no projectile.
	if (!entity->is(HASH("CMolotovProjectile")) && !entity->is(HASH("CBaseCSGrenadeProjectile")))
		return;

	// there is no frag-grenade.
	if (entity->is(HASH("CBaseCSGrenadeProjectile")) && (!entity->GetClientRenderable()->GetModel() || std::string_view(entity->GetClientRenderable()->GetModel()->m_Name).find("fraggrenade") == std::string::npos))
		return;

	// erase if theres no grenade.
	if (entity->m_fEffects() & EF_NODRAW) {
		g_grenades_warning.reset();
		return;
	}

	const auto m_throw_time = entity->m_flSpawnTime_Grenade();
	const auto offset = game::TIME_TO_TICKS(reinterpret_cast<Player*>(entity)->m_flSimulationTime() - entity->m_flSpawnTime_Grenade());
	const auto thrower = reinterpret_cast<Player*>(g_Csgo.m_EntityList->GetClientEntityFromHandle(entity->m_hOwnerEntity()));

	// work.
	g_grenades_warning.Think(thrower, entity->is(HASH("CMolotovProjectile")) ? MOLOTOV : HEGRENADE, entity->m_vecOrigin(), entity->m_vecVelocity(), m_throw_time, offset);
}

void Grenades::simulate() {
	// init member variables
	// that will be used during the simulation.
	setup();

	// log positions 20 times per second.
	size_t step = (size_t)game::TIME_TO_TICKS(0.05f), timer{ 0u };

	// iterate until the container is full, should never happen.
	for (size_t i{ 0u }; i < 4096u; ++i) {

		// the timer was reset, insert new point.
		if (!timer)
			m_path.push_back(m_start);

		// advance object to this frame.
		size_t flags = advance(i);

		// if we detonated, we are done.
		// our path is complete.
		if ((flags & DETONATE))
			break;

		// reset or bounced.
		// add a new point when bounced, and one every step.
		if ((flags & BOUNCE) || timer >= step)
			timer = 0;

		// increment timer.
		else
			++timer;

		if (m_Velocity == Vector{})
			break;
	}

	// fire grenades can extend to the ground.
	// this happens if their endpoint is within range of the floor.
	// 131 units to be exact.
	if (m_id == MOLOTOV || m_id == FIREBOMB) {
		CGameTrace trace;
		PhysicsPushEntity(m_start, { 0.f, 0.f, -131.f }, trace, g_Client.m_LocalPlayer);

		if (trace.m_Fraction < 0.9f)
			m_start = trace.m_EndPosition;
	}

	// store final point.
	// likely the point of detonation.
	m_path.push_back(m_start);
	m_bounces.push_back({ m_start, colors::red });
}

void Grenades::setup() {
	// get the last CreateMove angles.
	QAngle angle = g_Client.m_ViewAngles;

	// grab the pitch from these angles.
	float pitch = angle.x;

	// correct the pitch.
	if (pitch < -90.f)
		pitch += 360.f;

	else if (pitch > 90.f)
		pitch -= 360.f;

	// a rather 'interesting' approach at the approximation of some angle.
	// lets keep it on a pitch 'correction'.
	angle.x = pitch - (90.f - std::abs(pitch)) * 10.f / 90.f;

	// get ThrowVelocity from weapon files.
	float vel = m_vel * 0.9f;

	// clipped to [ 15, 750 ]
	Math::clamp(vel, 15.f, 750.f);

	// apply throw power to velocity.
	// this is set depending on mouse states:
	// m1=1  m1+m2=0.5  m2=0
	vel *= ((m_power * 0.7f) + 0.3f);

	// convert throw angle into forward direction.
	Vector forward;
	Math::AngleVectors(angle, &forward);

	// set start point to our shoot position.
	m_start = g_Client.m_EyePosition;

	// adjust starting point based on throw power.
	m_start.z += (m_power * 12.f) - 12.f;

	// create end point from start point.
	// and move it 22 units along the forward axis.
	Vector end = m_start + (forward * 22.f);

	CGameTrace trace;
	TraceHull(m_start, end, trace, g_Client.m_LocalPlayer);

	// we now have 'endpoint', set in our gametrace object.

	// move back start point 6 units along forward axis.
	m_start = trace.m_EndPosition - (forward * 6.f);

	// finally, calculate the velocity where we will start off with.
	// weird formula, valve..
	m_Velocity = g_Client.m_LocalPlayer->m_vecVelocity();
	m_Velocity *= 1.25f;
	m_Velocity += (forward * vel);
}

size_t Grenades::advance(size_t tick) {
	size_t     flags{ NONE };
	CGameTrace trace;

	// apply gravity.
	PhysicsAddGravityMove(m_move);

	// move object.
	PhysicsPushEntity(m_start, m_move, trace, g_Client.m_LocalPlayer);

	// check if the object would detonate at this point.
	// if so stop simulating further and endthe path here.
	if (detonate(tick, trace))
		flags |= DETONATE;

	// fix collisions/bounces.
	if (trace.m_Fraction != 1.f) {
		// mark as bounced.
		flags |= BOUNCE;

		// adjust velocity.
		ResolveFlyCollisionBounce(trace);
	}

	// take new start point.
	m_start = trace.m_EndPosition;

	return flags;
}

bool Grenades::detonate(size_t tick, CGameTrace& trace) {
	// convert current simulation tick to time.
	float time = game::TICKS_TO_TIME(tick);

	// CSmokeGrenadeProjectile::Think_Detonate
	// speed <= 0.1
	// checked every 0.2s

	// CDecoyProjectile::Think_Detonate
	// speed <= 0.2
	// checked every 0.2s

	// CBaseCSGrenadeProjectile::SetDetonateTimerLength
	// auto detonate at 1.5s
	// checked every 0.2s

	switch (m_id) {
	case FLASHBANG:
	case HEGRENADE:
		return time >= 1.5f && !(tick % game::TIME_TO_TICKS(0.2f));

	case SMOKE:
		return m_Velocity.Length() <= 0.1f && !(tick % game::TIME_TO_TICKS(0.2f));

	case DECOY:
		return m_Velocity.Length() <= 0.2f && !(tick % game::TIME_TO_TICKS(0.2f));

	case MOLOTOV:
	case FIREBOMB:
		// detonate when hitting the floor.
		if (trace.m_Fraction != 1.f && (std::cos(Math::DEG2RAD(g_Csgo.weapon_molotov_maxdetonateslope->GetFloat())) <= trace.m_Plane.m_Normal.z))
			return true;

		// detonate if we have traveled for too long.
		// checked every 0.1s
		return time >= g_Csgo.molotov_throw_detonate_time->GetFloat() && !(tick % game::TIME_TO_TICKS(0.1f));

	default:
		return false;
	}

	return false;
}

void Grenades::ResolveFlyCollisionBounce(CGameTrace& trace) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1341

	// assume all surfaces have the same elasticity
	float surface = 1.f;

	if (trace.m_Player) {
		if (game::IsBreakable(trace.m_Player)) {
			if (!trace.m_Player->is(HASH("CFuncBrush")) &&
				!trace.m_Player->is(HASH("CBaseDoor")) &&
				!trace.m_Player->is(HASH("CCSPlayer")) &&
				!trace.m_Player->is(HASH("CBaseEntity"))) {

				// move object.
				PhysicsPushEntity(m_start, m_move, trace, trace.m_Player);

				// deduct velocity penalty.
				m_Velocity *= 0.4f;
				return;
			}
		}
	}

	// combine elasticities together.
	float elasticity = 0.45f * surface;

	// clipped to [ 0, 0.9 ]
	Math::clamp(elasticity, 0.f, 0.9f);

	Vector velocity;
	PhysicsClipVelocity(m_Velocity, trace.m_Plane.m_Normal, velocity, 2.f);
	velocity *= elasticity;

	if (trace.m_Plane.m_Normal.z > 0.7f) {
		float speed = velocity.LengthSqr();

		// hit surface with insane speed.
		if (speed > 96000.f) {

			// weird formula to slow down by normal angle?
			float len = velocity.Normalized().Dot(trace.m_Plane.m_Normal);
			if (len > 0.5f)
				velocity *= 1.5f - len;
		}

		// are we going too slow?
		// just stop completely.
		if (speed < 400.f)
			m_Velocity = Vector{};

		else {
			// set velocity.
			m_Velocity = velocity;

			// compute friction left.
			float left = 1.f - trace.m_Fraction;

			// advance forward.
			PhysicsPushEntity(trace.m_EndPosition, velocity * (left * g_Csgo.m_Globals->m_Interval), trace, g_Client.m_LocalPlayer);
		}
	}

	else {
		// set velocity.
		m_Velocity = velocity;

		// compute friction left.
		float left = 1.f - trace.m_Fraction;

		// advance forward.
		PhysicsPushEntity(trace.m_EndPosition, velocity * (left * g_Csgo.m_Globals->m_Interval), trace, g_Client.m_LocalPlayer);
	}

	m_bounces.push_back({ trace.m_EndPosition, colors::white });
}

void Grenades::PhysicsPushEntity(Vector& start, const Vector& move, CGameTrace& trace, Entity* ent) {
	// compute end point.
	Vector end = start + move;

	// trace through world.
	TraceHull(start, end, trace, ent);
}

void Grenades::TraceHull(const Vector& start, const Vector& end, CGameTrace& trace, Entity* ent) {
	// create trace filter.
	static CTraceFilterSimple_game filter{};

	filter.SetPassEntity(ent);

	g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }), MASK_SOLID, (ITraceFilter*)&filter, &trace);
}

void Grenades::PhysicsAddGravityMove(Vector& move) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1264

	// gravity for grenades.
	float gravity = g_Csgo.m_Gravity->GetFloat() * 0.4f;

	// move one tick using current velocity.
	move.x = m_Velocity.x * g_Csgo.m_Globals->m_Interval;
	move.y = m_Velocity.y * g_Csgo.m_Globals->m_Interval;

	// apply linear acceleration due to gravity.
	// calculate new z velocity.
	float z = m_Velocity.z - (gravity * g_Csgo.m_Globals->m_Interval);

	// apply velocity to move, the average of the new and the old.
	move.z = ((m_Velocity.z + z) / 2.f) * g_Csgo.m_Globals->m_Interval;

	// write back new gravity corrected z-velocity.
	m_Velocity.z = z;
}

void Grenades::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1294
	constexpr float STOP_EPSILON = 0.1f;

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1303

	float backoff = in.Dot(normal) * overbounce;

	for (int i{}; i < 3; ++i) {
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}