#include "../../precompiled.h"

float Penetration::scale(Player* player, float fCurrentDamage, float armor_ratio, int hitgroup) {
	bool  has_heavy_armor;
	int   armor;
	float heavy_ratio, bonus_ratio, ratio, new_damage;

	static auto is_armored = [](Player* player, int armor, int hitgroup) {
		// the player has no armor.
		if (armor <= 0)
			return false;

		// if the hitgroup is head and the player has a helment, return true.
		// otherwise only return true if the hitgroup is not generic / legs / gear.
		if (hitgroup == static_cast<int>(Hitgroups_t::HEAD) && player->m_bHasHelmet())
			return true;

		else if (hitgroup >= static_cast<int>(Hitgroups_t::CHEST) && hitgroup <= static_cast<int>(Hitgroups_t::RIGHTARM))
			return true;

		return false;
	};

	// check if the player has heavy armor, this is only really used in operation stuff.
	has_heavy_armor = player->m_bHasHeavyArmor();

	// scale fCurrentDamage based on hitgroup.
	switch (hitgroup) {
	case static_cast<int>(Hitgroups_t::HEAD):
		fCurrentDamage *= has_heavy_armor ? 2.f : 4.f;
		break;

	case static_cast<int>(Hitgroups_t::STOMACH):
		fCurrentDamage *= 1.25f;
		break;

	case static_cast<int>(Hitgroups_t::LEFTLEG):
	case static_cast<int>(Hitgroups_t::RIGHTLEG):
		fCurrentDamage *= 0.75f;
		break;

	case static_cast<int>(Hitgroups_t::LEFTFOOT):
	case static_cast<int>(Hitgroups_t::RIGHTFOOT):
		fCurrentDamage *= 0.75f;
		break;
	default:
		break;
	}

	// grab amount of player armor.
	armor = player->m_ArmorValue();

	// check if the ent is armored and scale fCurrentDamage based on armor.
	if (is_armored(player, armor, hitgroup)) {
		heavy_ratio = 1.f;
		bonus_ratio = 0.5f;
		ratio = armor_ratio * 0.5f;

		// player has heavy armor.
		if (has_heavy_armor) {
			// calculate ratio values.
			bonus_ratio = 0.33f;
			ratio = armor_ratio * 0.25f;
			heavy_ratio = 0.33f;

			// calculate new fCurrentDamage.
			new_damage = (fCurrentDamage * ratio) * 0.85f;
		}

		// no heavy armor, do normal fCurrentDamage calculation.
		else
			new_damage = fCurrentDamage * ratio;

		if (((fCurrentDamage - new_damage) * (heavy_ratio * bonus_ratio)) > armor)
			new_damage = fCurrentDamage - (armor / bonus_ratio);

		fCurrentDamage = new_damage;
	}

	// might be wrong not sure, std::round, std:floor.
	return fCurrentDamage;
}

bool Penetration::TraceToExit(const Vector& vecSrc, const Vector& vecDir, Vector& out, CGameTrace* enter_trace, CGameTrace* exitTr) {
	static CTraceFilterSimple_game pFilter{};

	float  dist{};
	Vector new_end;
	int    contents, first_contents{};

	// max nPenetrationCount distance is 90 units.
	while (dist <= 90.f) {
		// step forward a bit.
		dist += 4.f;

		// set out pos.
		out = vecSrc + (vecDir * dist);

		if (!first_contents)
			first_contents = g_Csgo.m_EngineTrace->GetPointContents(out, MASK_SHOT, nullptr);

		contents = g_Csgo.m_EngineTrace->GetPointContents(out, MASK_SHOT, nullptr);

		if ((contents & MASK_SHOT_HULL) && (!(contents & CONTENTS_HITBOX) || contents == first_contents))
			continue;

		// move vecEnd pos a bit for tracing.
		new_end = out - (vecDir * 4.f);

		// do first Trace.
		g_Csgo.m_EngineTrace->TraceRay(Ray(out, new_end), MASK_SHOT, nullptr, exitTr);

		// note - dex; this is some new stuff added sometime around late 2017 ( 10.31.2017 update? ).
		if (g_Csgo.sv_clip_penetration_traces_to_players->GetInt())
			game::UTIL_ClipTraceToPlayers(out, new_end, MASK_SHOT, nullptr, exitTr, -60.f);

		// we hit an ent's Hitbox, do another Trace.
		if (exitTr->m_StartSolid && (exitTr->m_Surface.m_Flags & SURF_HITBOX)) {
			pFilter.SetPassEntity(exitTr->m_Player);

			g_Csgo.m_EngineTrace->TraceRay(Ray(out, vecSrc), MASK_SHOT_HULL, (ITraceFilter*)&pFilter, exitTr);

			if (exitTr->hit() && !exitTr->m_StartSolid) {
				out = exitTr->m_EndPosition;
				return true;
			}

			continue;
		}

		if (!exitTr->hit() || exitTr->m_StartSolid) {
			if (game::IsBreakable(enter_trace->m_Player)) {
				*exitTr = *enter_trace;
				exitTr->m_EndPosition = vecSrc + vecDir;
				return true;
			}

			continue;
		}

		if ((exitTr->m_Surface.m_Flags & SURF_NODRAW)) {
			// note - dex; ok, when this happens the game seems to not ignore world?
			if (game::IsBreakable(exitTr->m_Player) && game::IsBreakable(enter_trace->m_Player)) {
				out = exitTr->m_EndPosition;
				return true;
			}

			if (!(enter_trace->m_Surface.m_Flags & SURF_NODRAW))
				continue;
		}

		if (exitTr->m_Plane.m_Normal.Dot(vecDir) <= 1.f) {
			out -= (vecDir * (exitTr->m_Fraction * 4.f));
			return true;
		}
	}

	return false;
}

void Penetration::ClipTraceToPlayer(const Vector& vecSrc, const Vector& vecEnd, uint32_t mask, CGameTrace* tr, ITraceFilter* pFilter, Player* player, float min)
{
	CGameTrace new_trace;
	float frac = tr->m_Fraction;
	float range = Math::DistanceToRay(player->WorldSpaceCenter(), vecSrc, vecEnd);

	if (pFilter && !pFilter->ShouldHitEntity(player, mask))
		return;

	// within range.
	if (range > min && range <= 60.0f) {
		// clip to player.
		g_Csgo.m_EngineTrace->ClipRayToEntity(Ray(vecSrc, vecEnd), mask | CONTENTS_HITBOX, player, &new_trace);

		// within ray bounds.
		if (frac > new_trace.m_Fraction) {
			// update.
			frac = new_trace.m_Fraction;
			*tr = new_trace;
		}
	}
}

bool Penetration::Run(PenetrationInput_t* in, PenetrationOutput_t* out) {
	static CTraceFilterSkipTwoEntities_game pFilter{};

	int			  nPenetrationCount{ 4 }, iEnterMaterial, iExitMaterial;
	float		  fCurrentDamage{}, flPenetrationPower{}, flPenetrationDistance{}, flScaledDamage{}, flCurrentDistance{}, flPenetrationModifier{}, flDamageModifier{}, flDamLostPercent{}, flPenWepMod{}, flTraceDistance{}, flTotalLostDamage{};
	surfacedata_t* pEnterSurfaceData, * pExitSurfaceData;
	bool		  bIsNodraw, hitGrate;
	Vector		  vecSrc, vecDir, vecEnd, vecPenEnd;
	CGameTrace	  pTrace, exitTr;
	Weapon* pWeapon;
	WeaponInfo* pWeaponInfo;

	// if we are tracing from our local player perspective.
	if (in->m_From->IsLocalPlayer()) {
		pWeapon = g_Client.m_Weapon;
		if (!pWeapon)
			return false;

		pWeaponInfo = g_Client.m_WeaponInfo;
		if (!pWeaponInfo)
			return false;

		vecSrc = in->m_EyePosition;
	}

	// not local player.
	else {
		pWeapon = in->m_From->GetActiveWeapon();
		if (!pWeapon)
			return false;

		// get pWeapon info.
		pWeaponInfo = pWeapon->GetWpnData();
		if (!pWeaponInfo)
			return false;

		// set pTrace vecSrc.
		vecSrc = in->m_EyePosition;
	}

	// damage of the bullet at it's current trajectory.
	fCurrentDamage = (float)pWeaponInfo->m_Damage;

	// thickness of a wall that this bullet can penetrate, distance at which the bullet is capable of penetrating a wall
	in->m_From->GetBulletTypeParameters(pWeapon->GetBulletType(), flPenetrationPower, flPenetrationDistance);

	// we use the max penetrations on this gun to figure out how much Penetration it's capable of
	if (g_Csgo.sv_penetration_type->GetInt() == 1)
		flPenetrationPower = pWeaponInfo->m_Penetration;

	// get direction to vecEnd point.
	vecDir = (in->m_Position - vecSrc).Normalized();

	// setup pTrace pFilter for later.
	pFilter.SetPassEntity(in->m_From);
	pFilter.SetPassEntity2(nullptr);

	while (fCurrentDamage > 0.f) {
		// set pTrace vecEnd.
		vecEnd = vecSrc + (vecDir * (pWeaponInfo->m_WeaponRange - flCurrentDistance));

		// setup ray and pTrace.
		// TODO; use UTIL_TraceLineIgnoreTwoEntities?
		g_Csgo.m_EngineTrace->TraceRay(Ray(vecSrc, vecEnd), MASK_SHOT, (ITraceFilter*)&pFilter, &pTrace);

		// check for player hitboxes extending outside their collision bounds.
		// if no target is passed we clip the pTrace to a specific player, otherwise we clip the pTrace to any player.
		if (in->m_Player)
			ClipTraceToPlayer(vecSrc, vecEnd + (vecDir * 40.f), MASK_SHOT, &pTrace, (ITraceFilter*)&pFilter, in->m_Player);

		else
			game::UTIL_ClipTraceToPlayers(vecSrc, vecEnd + (vecDir * 40.f), MASK_SHOT, (ITraceFilter*)&pFilter, &pTrace);

		// we didn't hit anything.
		if (pTrace.m_Fraction == 1.f)
			break;

		// calculate fCurrentDamage based on the distance the bullet traveled.
		flCurrentDistance += pTrace.m_Fraction * (pWeaponInfo->m_WeaponRange - flCurrentDistance);
		fCurrentDamage *= powf(pWeaponInfo->m_RangeModifier, flCurrentDistance / 500.f);

		// if a target was passed.
		if (in->m_Player) {

			// validate that we hit the target we aimed for.
			if (pTrace.m_Player && pTrace.m_Player == in->m_Player && game::IsValidHitgroup(pTrace.m_HitGroup)) {
				int group = pTrace.m_HitGroup;

				// scale fCurrentDamage based on the hitgroup we hit.
				flScaledDamage = scale(in->m_Player, fCurrentDamage, pWeaponInfo->m_ArmorRatio, group);

				// set result data for when we hit a player.
				out->m_Penetration = nPenetrationCount != 4;
				out->m_HitGroup = group;
				out->m_Damage = flScaledDamage;
				out->m_Player = in->m_Player;

				// non-penetrate fCurrentDamage.
				if (nPenetrationCount == 4)
					return flScaledDamage >= in->m_Damage;

				// Penetration fCurrentDamage.
				return flScaledDamage >= in->m_PenetrationDamage;
			}
		}

		// no target was passed, check for any player hit or just get final fCurrentDamage done.
		else {
			out->m_Penetration = nPenetrationCount != 4;

			// todo - dex; team checks / other checks / etc.
			if (pTrace.m_Player && pTrace.m_Player->IsPlayer() && game::IsValidHitgroup(pTrace.m_HitGroup)) {
				int group = pTrace.m_HitGroup;

				flScaledDamage = scale(pTrace.m_Player->as< Player* >(), fCurrentDamage, pWeaponInfo->m_ArmorRatio, group);

				// set result data for when we hit a player.
				out->m_HitGroup = group;
				out->m_Damage = flScaledDamage;
				out->m_Player = pTrace.m_Player->as< Player* >();

				// non-penetrate fCurrentDamage.
				if (nPenetrationCount == 4)
					return flScaledDamage >= in->m_Damage;

				// Penetration fCurrentDamage.
				return flScaledDamage >= in->m_PenetrationDamage;
			}

			// if we've reached here then we didn't hit a player yet, set fCurrentDamage and hitgroup.
			out->m_Damage = fCurrentDamage;
		}

		if (!in->m_Penetrate)
			break;

		// get surface at entry point.
		pEnterSurfaceData = g_Csgo.m_PhysProps->GetSurfaceData(pTrace.m_Surface.m_SurfaceProps);
		if (!pEnterSurfaceData)
			break;

		// this happens when we're too far away from a surface and can penetrate walls or the surface's nPenetrationCount flPenWepMod is too low.
		if ((flCurrentDistance > flPenetrationDistance && pWeaponInfo->m_Penetration > 0.f) || pEnterSurfaceData->m_Game.m_PenetrationModifier < 0.1f)
			nPenetrationCount = 0;

		// store data about surface flags / contents.
		bIsNodraw = (pTrace.m_Surface.m_Flags & SURF_NODRAW);
		hitGrate = (pTrace.m_Contents & CONTENTS_GRATE);

		// get material at entry point.
		iEnterMaterial = pEnterSurfaceData->m_Game.m_Material;

		// note - dex; some extra stuff the game does.
		if (!nPenetrationCount && !hitGrate && !bIsNodraw && iEnterMaterial != CHAR_TEX_GRATE && iEnterMaterial != CHAR_TEX_GLASS)
			break;

		if (pWeaponInfo->m_Penetration <= 0.f || nPenetrationCount <= 0)
			break;

		if (!TraceToExit(pTrace.m_EndPosition, vecDir, vecPenEnd, &pTrace, &exitTr) && !(g_Csgo.m_EngineTrace->GetPointContents(pTrace.m_EndPosition, MASK_SHOT_HULL) & MASK_SHOT_HULL))
			break;

		// get surface / material at exit point.
		pExitSurfaceData = g_Csgo.m_PhysProps->GetSurfaceData(exitTr.m_Surface.m_SurfaceProps);
		iExitMaterial = pExitSurfaceData->m_Game.m_Material;

		if (g_Csgo.sv_penetration_type->GetInt() != 1)
		{
			if (hitGrate || bIsNodraw)
			{
				flPenetrationModifier = 1.0f;
				flDamageModifier = 0.99f;
			}
			else
			{
				flPenetrationModifier = fminf(flPenetrationModifier, pExitSurfaceData->m_Game.m_PenetrationModifier);
				flDamageModifier = fminf(flDamageModifier, pExitSurfaceData->m_Game.m_DamageModifier);
			}

			if (iEnterMaterial == iExitMaterial && (iExitMaterial == CHAR_TEX_WOOD || iExitMaterial == CHAR_TEX_METAL))
				flPenetrationModifier *= 2.f;

			float flTraceDistance = (exitTr.m_EndPosition - pTrace.m_EndPosition).Length();
			if (flTraceDistance > (flPenetrationPower * flPenetrationModifier))
				break;

			fCurrentDamage *= flDamageModifier;
			flPenetrationPower -= flTraceDistance / flPenetrationModifier;
		}
		else
		{
			flDamLostPercent = 0.16f;
			flPenetrationModifier = (pEnterSurfaceData->m_Game.m_PenetrationModifier + pExitSurfaceData->m_Game.m_PenetrationModifier) / 2.f;

			if (hitGrate || bIsNodraw || iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
			{
				if (iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
				{
					flPenetrationModifier = 3.0f;
					flDamLostPercent = 0.05f;
				}
				else flPenetrationModifier = 1.0f;
			}
			else if (iEnterMaterial == CHAR_TEX_FLESH && g_Csgo.ff_damage_reduction_bullets->GetFloat() == 0.f && pTrace.m_Player && pTrace.m_Player->IsPlayer() && pTrace.m_Player->m_iTeamNum() == in->m_From->m_iTeamNum())
			{
				if (g_Csgo.ff_damage_bullet_penetration->GetFloat() == 0)
				{
					flPenetrationModifier = 0.f;
					break;
				}

				flPenetrationModifier = g_Csgo.ff_damage_bullet_penetration->GetFloat();
			}

			if (iEnterMaterial == iExitMaterial) {
				if (iExitMaterial == CHAR_TEX_CARDBOARD || iExitMaterial == CHAR_TEX_WOOD)
					flPenetrationModifier = 3.f;

				else if (iExitMaterial == CHAR_TEX_PLASTIC)
					flPenetrationModifier = 2.f;
			}

			flTraceDistance = (exitTr.m_EndPosition - pTrace.m_EndPosition).Length();
			flPenWepMod = fmax(0.f, 1.f / flPenetrationModifier);
			flTotalLostDamage = fmax(0.f, ((flPenWepMod * 3.f) * std::max(0.f, (3.f / flPenetrationPower) * 1.25f) + (fCurrentDamage * flDamLostPercent)) + (((flTraceDistance * flTraceDistance) * flPenWepMod) / 24.f));

			fCurrentDamage -= flTotalLostDamage;
			if (fCurrentDamage < 1.f)
				break;
		}

		vecSrc = exitTr.m_EndPosition;
		nPenetrationCount--;
	}

	return false;
}