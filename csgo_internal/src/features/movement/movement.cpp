#include "../../precompiled.h"

void Movement::JumpRelated() {
	if (g_Client.m_LocalPlayer->m_MoveType() == MOVETYPE_LADDER || g_Client.m_LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP || g_Client.m_LocalPlayer->m_MoveType() == MOVETYPE_OBSERVER)
		return;

	if (!(g_Client.m_Command->m_Buttons & IN_JUMP) || g_Client.m_Flags & FL_ONGROUND)
		return;

	if (g_Csgo.sv_autobunnyhopping->GetInt() == 1 && g_Csgo.sv_enablebunnyhopping->GetInt() == 1)
		return;

	if (g_Config.b(g_Items.m_Misc.m_AutomaticJump))
		g_Client.m_Command->m_Buttons &= ~IN_JUMP;
}

void Movement::Strafe() {
	float  Delta, abs_delta, velocity_delta, correct;

	// don't strafe while noclipping or on ladders.
	if (g_Client.m_LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP || g_Client.m_LocalPlayer->m_MoveType() == MOVETYPE_LADDER)
		return;

	// get networked velocity ( maybe absvelocity better here? ).
	// meh, should be predicted anyway? ill see.
	Vector velocity = g_Client.m_LocalPlayer->m_vecVelocity();

	// get the velocity len2d ( speed ).
	m_VelocityLenghtXY = velocity.Length2D();

	// compute the ideal strafe angle for our velocity.
	m_Ideal = (m_VelocityLenghtXY > 0.f) ? Math::RAD2DEG(std::asin(15.f / m_VelocityLenghtXY)) : 90.f;

	// some additional sanity.
	Math::clamp(m_Ideal, 0.f, 90.f);

	if (!g_Client.m_LocalPlayer->GetCollideable())
		return;

	// save entity bounds ( used much in circle-strafer ).
	m_OBBMins = g_Client.m_LocalPlayer->GetCollideable()->OBBMins();
	m_OBBMaxs = g_Client.m_LocalPlayer->GetCollideable()->OBBMaxs();

	// save our origin
	m_Origin = g_Client.m_LocalPlayer->m_vecOrigin();

	// disable strafing while pressing shift.
	if ((g_Client.m_Buttons & IN_SPEED) || (g_Client.m_Flags & FL_ONGROUND))
		return;

	// do ciclestrafer
	if (c_config::get()->b["circle"] && keyhandler::get()->auto_check("circle_key")) {
		// if no duck jump.
		if (!c_config::get()->b["crouch"])
			// crouch to fit into narrow areas.
			g_Client.m_Command->m_Buttons |= IN_DUCK;

		DoPrespeed();
		return;
	}

	// for changing direction.
	// we want to change strafe direction every call.
	m_switch_value *= -1.f;

	// do z strafer
	if (c_config::get()->b["zig"] && keyhandler::get()->auto_check("zig_key")) {
		float freq = (c_config::get()->i["z_freq"] * 0.2f) * g_Csgo.m_Globals->m_RealTime;

		// range [ 1, 100 ], aka grenerates a factor.
		float factor = c_config::get()->i["z_dist"] * 0.5f;

		g_Client.m_StrafeAngles.y += (factor * std::sin(freq));
	}

	if (!g_Config.b(g_Items.m_Misc.m_AutomaticStrafe))
		return;

	enum EDirections {
		FORWARDS = 0,
		BACKWARDS = 180,
		LEFT = 90,
		RIGHT = -90,
		BACK_LEFT = 135,
		BACK_RIGHT = -135
	};

	// move in the appropriate direction.
	if (g_Client.m_Buttons & IN_FORWARD) {
		//	forward left.
		if (g_Client.m_Buttons & IN_MOVELEFT)
			g_Client.m_StrafeAngles.y += Math::NormalizeYaw((EDirections::LEFT / 2));

		//	forward right.
		else if (g_Client.m_Buttons & IN_MOVERIGHT)
			g_Client.m_StrafeAngles.y += Math::NormalizeYaw((EDirections::RIGHT / 2));

		//	forward.
		else g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::FORWARDS);
	}

	else if (g_Client.m_Buttons & IN_BACK) {
		//	back left.
		if (g_Client.m_Buttons & IN_MOVELEFT)
			g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::BACK_LEFT);

		//	back right.
		else if (g_Client.m_Buttons & IN_MOVERIGHT)
			g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::BACK_RIGHT);

		//	back.
		else g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::BACKWARDS);
	}

	else if (g_Client.m_Buttons & IN_MOVELEFT)
		//	left.
		g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::LEFT);

	else if (g_Client.m_Buttons & IN_MOVERIGHT)
		//	right.
		g_Client.m_StrafeAngles.y += Math::NormalizeYaw(EDirections::RIGHT);

	// get our viewangle change.
	Delta = Math::NormalizedAngle(g_Client.m_StrafeAngles.y - m_old_yaw);

	// convert to absolute change.
	abs_delta = std::abs(Delta);

	// save old yaw for next call.
	m_circle_yaw = m_old_yaw = g_Client.m_StrafeAngles.y;

	// set strafe direction based on mouse direction change.
	g_Client.m_Command->m_SideMove = Delta < 0.f ? 450.f : -450.f;

	// we can accelerate more, because we strafed less then needed
	// or we got of track and need to be retracked.
	if (abs_delta <= m_Ideal || abs_delta >= 30.f) {
		// compute angle of the direction we are traveling in.
		QAngle velocity_angle;
		Math::VectorAngles(velocity, velocity_angle);

		// get the Delta between our direction and where we are looking at.
		velocity_delta = Math::NormalizeYaw(g_Client.m_StrafeAngles.y - velocity_angle.y);

		// correct our strafe amongst the path of a circle.
		correct = m_Ideal;

		if (velocity_delta <= correct || m_VelocityLenghtXY <= 15.f) {
			// not moving mouse, switch strafe every tick.
			if (-correct <= velocity_delta || m_VelocityLenghtXY <= 15.f) {
				g_Client.m_StrafeAngles.y += (m_Ideal * m_switch_value);
				g_Client.m_Command->m_SideMove = 450.f * m_switch_value;
			}

			else {
				g_Client.m_StrafeAngles.y = velocity_angle.y - correct;
				g_Client.m_Command->m_SideMove = 450.f;
			}
		}

		else {
			g_Client.m_StrafeAngles.y = velocity_angle.y + correct;
			g_Client.m_Command->m_SideMove = -450.f;
		}
	}

	// cancel out any forwardmove values.
	g_Client.m_Command->m_ForwardMove = 0.f;
}

void Movement::DoPrespeed() {
	float   mod, min, max, step, strafe, time, angle;
	Vector  plane;

	// min and max values are based on 128 ticks.
	mod = g_Csgo.m_Globals->m_Interval * 128.f;

	// scale min and max based on tickrate.
	min = 2.50f * mod;
	max = 5.f * mod;

	// compute ideal strafe angle for moving in a circle.
	strafe = m_Ideal * 2.f;

	// clamp ideal strafe circle value to min and max step.
	Math::clamp(strafe, min, max);

	// calculate time.
	time = g_Csgo.sv_maxspeed->GetFloat() / m_VelocityLenghtXY;

	// clamp time.
	Math::clamp(time, 0.35f, 1.f);

	// init step.
	step = strafe;

	while (true) {
		// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
		if (!WillCollide(time, step) || max <= step)
			break;

		// if we will collide with an object with the current strafe step then increment step to prevent a collision.
		step += 0.2f;
	}

	if (step > max) {
		// reset step.
		step = strafe;

		while (true) {
			// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
			if (!WillCollide(time, step) || step <= -min)
				break;

			// if we will collide with an object with the current strafe step decrement step to prevent a collision.
			step -= 0.2f;
		}

		if (step < -min) {
			if (GetClosestPlane(plane)) {
				// grab the closest object normal
				// compute the angle of the normal
				// and push us away from the object.
				angle = Math::RAD2DEG(std::atan2(plane.y, plane.x));
				step = -Math::NormalizedAngle(m_circle_yaw - angle) * 0.1f;
			}
		}

		else
			step -= 0.2f;
	}

	else
		step += 0.2f;

	// add the computed step to the steps of the previous circle iterations.
	m_circle_yaw = Math::NormalizedAngle(m_circle_yaw + step);

	// apply data to usercmd.
	g_Client.m_Command->m_ViewAngles.y = m_circle_yaw;
	g_Client.m_Command->m_SideMove = (step >= 0.f) ? -450.f : 450.f;
}

bool Movement::GetClosestPlane(Vector& plane) {
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;
	Vector                start{ m_Origin };
	float                 smallest{ 1.f };
	const float		      dist{ 75.f };

	// trace around us in a circle
	for (float step{ }; step <= Math::pi_2; step += (Math::pi / 10.f)) {
		// extend endpoint x units.
		Vector end = start;
		end.x += std::cos(step) * dist;
		end.y += std::sin(step) * dist;

		g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, m_OBBMins, m_OBBMaxs), CONTENTS_SOLID, &filter, &trace);

		// we found an object closer, then the previouly found object.
		if (trace.m_Fraction < smallest) {
			// save the normal of the object.
			plane = trace.m_Plane.m_Normal;
			smallest = trace.m_Fraction;
		}
	}

	// did we find any valid object?
	return smallest != 1.f && plane.z < 0.1f;
}

bool Movement::WillCollide(float time, float change) {
	struct PredictionData_t {
		Vector start;
		Vector end;
		Vector velocity;
		float  direction;
		bool   ground;
		float  predicted;
	};

	PredictionData_t      data;
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;

	// set base data.
	data.ground = g_Client.m_Flags & FL_ONGROUND;
	data.start = m_Origin;
	data.end = m_Origin;
	data.velocity = g_Client.m_LocalPlayer->m_vecVelocity();
	data.direction = Math::RAD2DEG(std::atan2(data.velocity.y, data.velocity.x));

	for (data.predicted = 0.f; data.predicted < time; data.predicted += g_Csgo.m_Globals->m_Interval) {
		// predict movement direction by adding the direction change.
		// make sure to Normalize it, in case we go over the -180/180 turning point.
		data.direction = Math::NormalizedAngle(data.direction + change);

		// pythagoras.
		float hyp = data.velocity.Length2D();

		// adjust velocity for new direction.
		data.velocity.x = std::cos(Math::DEG2RAD(data.direction)) * hyp;
		data.velocity.y = std::sin(Math::DEG2RAD(data.direction)) * hyp;

		// assume we bhop, set upwards impulse.
		if (data.ground)
			data.velocity.z = g_Csgo.sv_jump_impulse->GetFloat();

		else
			data.velocity.z -= g_Csgo.m_Gravity->GetFloat() * g_Csgo.m_Globals->m_Interval;

		// we adjusted the velocity for our new direction.
		// see if we can move in this direction, predict our new origin if we were to travel at this velocity.
		data.end += (data.velocity * g_Csgo.m_Globals->m_Interval);

		// trace
		g_Csgo.m_EngineTrace->TraceRay(Ray(data.start, data.end, m_OBBMins, m_OBBMaxs), MASK_PLAYERSOLID, &filter, &trace);

		// check if we hit any objects.
		if (trace.m_Fraction != 1.f && trace.m_Plane.m_Normal.z <= 0.9f || trace.m_StartSolid || trace.m_AllSolid)
			return true;

		// adjust start and end point.
		data.start = data.end = trace.m_EndPosition;

		// move endpoint 2 units down, and re-trace.
		// do this to check if we are on th floor.
		g_Csgo.m_EngineTrace->TraceRay(Ray(data.start, data.end - Vector{ 0.f, 0.f, 2.f }, m_OBBMins, m_OBBMaxs), MASK_PLAYERSOLID, &filter, &trace);

		// see if we moved the player into the ground for the next iteration.
		data.ground = trace.hit() && trace.m_Plane.m_Normal.z > 0.7f;
	}

	// the entire loop has ran
	// we did not hit shit.
	return false;
}

void Movement::FixMove(QAngle wish_angles) {
	Vector  m_vMove, m_vDirection;
	float   flDelta, flLen;
	QAngle   pMoveAngle;

	// infinite duck.
	if (!g_Csgo.m_GameRules->m_bIsValveDS())
		g_Client.m_Command->m_Buttons |= IN_BULLRUSH;

	// convert movement to vector.
	m_vMove = { g_Client.m_Command->m_ForwardMove, g_Client.m_Command->m_SideMove, 0.f };

	// get m_vMove Length and ensure we're using a unit vector ( vector with Length of 1 ).
	flLen = m_vMove.Normalize();
	if (!flLen)
		return;

	// convert m_vMove to an angle.
	Math::VectorAngles(m_vMove, pMoveAngle);

	// calculate yaw flDelta.
	flDelta = (g_Client.m_Command->m_ViewAngles.y - wish_angles.y);

	// accumulate yaw flDelta.
	pMoveAngle.y += flDelta;

	// calculate our new m_vMove direction.
	// m_vDirection = move_angle_forward * move_length
	Math::AngleVectors(pMoveAngle, &m_vDirection);

	// scale to og movement.
	m_vDirection *= flLen;

	// the move type
	const int iMoveType = g_Client.m_LocalPlayer->m_MoveType();

	// fix ladder and noclip.
	if (iMoveType == MoveType_t::MOVETYPE_LADDER || iMoveType == MoveType_t::MOVETYPE_NOCLIP) {
		// strip old flags.
		g_Client.m_Command->m_Buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT);

		// invert directon for up and down.
		if (g_Client.m_Command->m_ViewAngles.x >= 45 && wish_angles.x < 45 && abs(flDelta) <= 65)
			m_vDirection.x = -m_vDirection.x;

		// write to movement.
		g_Client.m_Command->m_ForwardMove = m_vDirection.x;
		g_Client.m_Command->m_SideMove = m_vDirection.y;

		// set new button flags.
		if (g_Client.m_Command->m_ForwardMove > 200)
			g_Client.m_Command->m_Buttons |= IN_FORWARD;

		else if (g_Client.m_Command->m_ForwardMove < -200)
			g_Client.m_Command->m_Buttons |= IN_BACK;

		if (g_Client.m_Command->m_SideMove > 200)
			g_Client.m_Command->m_Buttons |= IN_MOVERIGHT;

		else if (g_Client.m_Command->m_SideMove < -200)
			g_Client.m_Command->m_Buttons |= IN_MOVELEFT;
	}
	// we are moving normally.
	else {
		// we must do this for pitch angles that are out of bounds.
		if (g_Client.m_Command->m_ViewAngles.x < -90 || g_Client.m_Command->m_ViewAngles.x > 90)
			m_vDirection.x = -m_vDirection.x;

		// set m_vMove.
		g_Client.m_Command->m_ForwardMove = m_vDirection.x;
		g_Client.m_Command->m_SideMove = m_vDirection.y;

		// strip old flags.
		g_Client.m_Command->m_Buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT);
	}
}

void Movement::FastStop()
{
	if (g_Client.m_PressingButtons || g_Client.m_Command->m_Buttons & IN_JUMP || !g_Client.m_LocalPlayer->m_vecVelocity().Length2D() || !(g_Client.m_LocalPlayer->m_fFlags() & FL_ONGROUND) || g_Client.m_LocalPlayer->m_fFlags() & FL_INWATER || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_LADDER || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_NOCLIP || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	Movement::StopMovement();
}

void Movement::AutoPeek() {
	if (!g_Config.b(g_Items.m_Misc.m_AutomaticPeek) || !g_KeyInput.Key(g_Items.m_Misc.m_AutomaticPeekKeyBinds, g_Items.m_Misc.m_AutomaticPeekKeyBindsMode)) {
		m_AutomaticPeek.Reset();
		return;
	}

	if (m_AutomaticPeek.m_Origin == Vector(0.f, 0.f, 0.f))
		m_AutomaticPeek.Reset(g_Client.m_LocalPlayer->m_vecOrigin());
	else {
		if (g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_AutomaticPeek.m_Origin) < 512.f)
			m_AutomaticPeek.m_CanRetrack = true;

		if (g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_AutomaticPeek.m_Origin) > 8.0f && m_AutomaticPeek.m_CanRetrack) {
			if (m_AutomaticPeek.m_ShouldRetrack) {
				g_Client.m_Command->m_Buttons &= ~IN_DUCK;

				QAngle vecAngleToPos;
				Math::VectorAngles(m_AutomaticPeek.m_Origin - g_Client.m_LocalPlayer->m_vecOrigin(), vecAngleToPos);

				m_AutomaticPeek.m_SideMove = fmaxf(fminf(450.0f, (float)(sin(Math::DEG2RAD((g_Client.m_ViewAngles.y - vecAngleToPos.y))) * cos(vecAngleToPos.x * 0.017453292)) * 450.0f), -450.0f);
				m_AutomaticPeek.m_ForwardMove = fmaxf(fminf(450.0f, cos(Math::DEG2RAD((g_Client.m_ViewAngles.y - vecAngleToPos.y))) * cos(vecAngleToPos.x * 0.017453292) * 450.0f), -450.0f);

				g_Client.m_Command->m_SideMove = m_AutomaticPeek.m_SideMove;
				g_Client.m_Command->m_ForwardMove = m_AutomaticPeek.m_ForwardMove;
			}
		}
		else m_AutomaticPeek.m_ShouldRetrack = false;

		bool bStop = g_Client.m_Weapon && g_Client.m_Weapon->IsGun() && !g_Client.CanFireWeapon();

		if (m_AutomaticPeek.m_AutoPeekingDone && bStop)
			Movement::StopMovement();
	}

	if (!m_AutomaticPeek.m_ShouldRetrack && m_AutomaticPeek.m_AutoPeeking)
		m_AutomaticPeek.m_AutoPeekingDone = g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_AutomaticPeek.m_Origin) < 8.0f;

	if (g_Client.IsShooting(false)) {
		m_AutomaticPeek.m_ShouldRetrack = true;
		m_AutomaticPeek.m_AutoPeeking = true;
	}
}

void Movement::SlowWalk(bool bForcedWalk)
{
	float flMaxSpeed = g_Client.m_MaxPlayerSpeed;

	if (m_WalkSwitch)
		flMaxSpeed *= 0.033999f;
	else flMaxSpeed *= 0.022999f;

	m_WalkSwitch = !m_WalkSwitch;

	if (g_Client.m_SendPacket)
	{
		if (m_MoveSwitch)
			g_Client.m_Command->m_Buttons |= IN_SPEED;
		else g_Client.m_Command->m_Buttons &= ~IN_SPEED;

		m_MoveSwitch = !m_MoveSwitch;
	}

	const float flDistanceToMinimalSpeed = g_Client.m_LocalPlayer->m_vecVelocity().Length2D() / flMaxSpeed;
	if (flDistanceToMinimalSpeed <= 0.0f)
		return;

	g_Client.m_Command->m_ForwardMove /= flDistanceToMinimalSpeed;
	g_Client.m_Command->m_SideMove /= flDistanceToMinimalSpeed;
}

void Movement::StopMovement(bool bSlideMovement)
{
	if (g_Client.m_Command->m_Buttons & IN_JUMP || !(g_Client.m_LocalPlayer->m_fFlags() & FL_ONGROUND) || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_LADDER || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_NOCLIP || g_Client.m_LocalPlayer->m_MoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	g_Client.m_Command->m_SideMove = 0.0f;
	g_Client.m_Command->m_ForwardMove = g_Client.m_LocalPlayer->m_vecVelocity().Length2D() > 20.0f ? 450.0f : 0.0f;

	QAngle viewangles; g_Csgo.m_Engine->GetViewAngles(viewangles);

	float yaw = Math::CalcAngle(Vector(0.0f, 0.0f, 0.0f), g_Client.m_LocalPlayer->m_vecVelocity()).y + 180.0f;
	float rotation = Math::DEG2RAD(viewangles.y - yaw);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = cos_rot * g_Client.m_Command->m_ForwardMove - sin_rot * g_Client.m_Command->m_SideMove;
	float new_sidemove = sin_rot * g_Client.m_Command->m_ForwardMove + cos_rot * g_Client.m_Command->m_SideMove;

	g_Client.m_Command->m_ForwardMove = new_forwardmove;
	g_Client.m_Command->m_SideMove = new_sidemove;
}

void Movement::FakeWalk() {
	Vector velocity{ g_Client.m_LocalPlayer->m_vecVelocity() };
	int    ticks{ }, max{ g_Csgo.m_MaxUserCommandProcessTicks->GetInt()};

	if (!g_Config.b(g_Items.m_Misc.m_FakeWalk) || !g_KeyInput.Key(g_Items.m_Misc.m_FakeWalkKeyBinds, g_Items.m_Misc.m_FakeWalkBindsMode))
		return;

	if (!g_Client.m_LocalPlayer->GetGroundEntity())
		return;

	// calculate friction.
	float friction = g_Csgo.sv_friction->GetFloat() * g_Client.m_LocalPlayer->m_surfaceFriction();

	for (; ticks < g_Client.m_MaxLagAmount; ++ticks) {
		// calculate speed.
		float speed = velocity.Length();

		// if too slow return.
		if (speed < 0.1f)
			break;

		// bleed off some speed, but if we have less than the bleed, threshold, bleed the threshold amount.
		float control = std::max(speed, g_Csgo.sv_stopspeed->GetFloat());

		// calculate the drop amount.
		float drop = control * friction * g_Csgo.m_Globals->m_Interval;

		// scale the velocity.
		float newspeed = std::max(0.f, speed - drop);

		if (newspeed != speed) {
			// determine proportion of old speed we are using.
			newspeed /= speed;

			// adjust velocity according to proportion.
			velocity *= newspeed;
		}
	}

	// zero forwardmove and sidemove.
	if (ticks > ((max - 1) - g_Client.m_ChokedCommands) || !g_Client.m_ChokedCommands)
		Movement::StopMovement();
}