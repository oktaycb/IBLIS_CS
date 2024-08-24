#include "../../precompiled.h"

inline bool IsVectorValid(Vector vecOriginal, Vector vecCurrent)
{
	Vector vecDelta = vecOriginal - vecCurrent;
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(vecDelta[i]) > 0.03125f)
			return false;
	}

	return true;
}

inline bool IsAngleValid(QAngle vecOriginal, QAngle vecCurrent)
{
	QAngle vecDelta = vecOriginal - vecCurrent;
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(vecDelta[i]) > 0.03125f)
			return false;
	}

	return true;
}

inline bool IsFloatValid(float_t flOriginal, float_t flCurrent, bool bDatamap = false)
{
	const float flDifference = bDatamap ? 0.00625f : 0.03125f;

	return fabsf(flOriginal - flCurrent) <= flDifference;
}

void NetData::Store(Player* pPlayer, int nCommand)
{
	if (!g_Client.m_Processing)
		return;

	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_nTickbase = pPlayer->m_nTickBase();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flDuckAmount = pPlayer->m_flDuckAmount();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flDuckSpeed = pPlayer->m_flDuckSpeed();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_vecOrigin = pPlayer->m_vecOrigin();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_vecVelocity = pPlayer->m_vecVelocity();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_vecBaseVelocity = pPlayer->m_vecBaseVelocity();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flFallVelocity = pPlayer->m_flFallVelocity();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_vecViewOffset = pPlayer->m_vecViewOffset();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_angAimPunchAngle = pPlayer->m_aimPunchAngle();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_vecAimPunchAngleVel = pPlayer->m_aimPunchAngleVel();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_angViewPunchAngle = pPlayer->m_viewPunchAngle();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flVelocityModifier = pPlayer->m_flVelocityModifier();

	Weapon* pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
	{
		m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flRecoilIndex = 0.0f;
		m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flAccuracyPenalty = 0.0f;
		m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flPostponeFireReadyTime = 0.0f;
		return;
	}

	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flRecoilIndex = pWeapon->m_flRecoilIndex();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flAccuracyPenalty = pWeapon->m_fAccuracyPenalty();
	m_aCompressData[nCommand % MULTIPLAYER_BACKUP].m_flPostponeFireReadyTime = pWeapon->m_flPostponeFireReadyTime();

	if (pPlayer->m_vphysicsCollisionState() != VPHYS_WALK)
		pPlayer->m_vphysicsCollisionState() = VPHYS_WALK;
}

// TODO@Monster: Add More Stuff Here.
// TODO@Monster: Add SetVelocity(), SetViewOffset(), etc.
void NetData::Apply(Player* pPlayer, int nCommand)
{
	if (!g_Client.m_Processing)
		return;

	volatile auto aNetVars = &m_aCompressData[nCommand % MULTIPLAYER_BACKUP];
	if (aNetVars->m_nTickbase != pPlayer->m_nTickBase())
		return;

	g_Client.m_PredictionError = pPlayer->m_flVelocityModifier() < 1.0f && g_Client.m_ChokedCommands > 0 && pPlayer->m_vecVelocity().IsValid() && (pPlayer->m_vecOrigin() - pPlayer->m_vecOldOrigin()).Length() > 1.f;
	
	if (IsVectorValid(aNetVars->m_vecVelocity, pPlayer->m_vecVelocity()))
		pPlayer->m_vecVelocity() = aNetVars->m_vecVelocity;
	else g_Client.m_PredictionError = true;

	if (IsVectorValid(aNetVars->m_vecBaseVelocity, pPlayer->m_vecBaseVelocity()))
		pPlayer->m_vecBaseVelocity() = aNetVars->m_vecBaseVelocity;
	else g_Client.m_PredictionError = true;

	if (IsVectorValid(aNetVars->m_vecViewOffset, pPlayer->m_vecViewOffset()))
		pPlayer->m_vecViewOffset() = aNetVars->m_vecViewOffset;
	else g_Client.m_PredictionError = true;

	if (IsAngleValid(aNetVars->m_angAimPunchAngle, pPlayer->m_aimPunchAngle()))
		pPlayer->m_aimPunchAngle() = aNetVars->m_angAimPunchAngle;
	else g_Client.m_PredictionError = true;

	if (IsVectorValid(aNetVars->m_vecAimPunchAngleVel, pPlayer->m_aimPunchAngleVel()))
		pPlayer->m_aimPunchAngleVel() = aNetVars->m_vecAimPunchAngleVel;
	else g_Client.m_PredictionError = true;

	if (IsAngleValid(aNetVars->m_angViewPunchAngle, pPlayer->m_viewPunchAngle()))
		pPlayer->m_viewPunchAngle() = aNetVars->m_angViewPunchAngle;
	else g_Client.m_PredictionError = true;

	if (IsFloatValid(aNetVars->m_flFallVelocity, pPlayer->m_flFallVelocity()))
		pPlayer->m_flFallVelocity() = aNetVars->m_flFallVelocity;
	else g_Client.m_PredictionError = true;

	if (IsFloatValid(aNetVars->m_flDuckAmount, pPlayer->m_flDuckAmount()))
		pPlayer->m_flDuckAmount() = aNetVars->m_flDuckAmount;
	else g_Client.m_PredictionError = true;

	if (IsFloatValid(aNetVars->m_flDuckSpeed, pPlayer->m_flDuckSpeed()))
		pPlayer->m_flDuckSpeed() = aNetVars->m_flDuckSpeed;
	else g_Client.m_PredictionError = true;

	if (pPlayer->GetActiveWeapon())
	{
		if (IsFloatValid(aNetVars->m_flAccuracyPenalty, pPlayer->GetActiveWeapon()->m_fAccuracyPenalty()))
			pPlayer->GetActiveWeapon()->m_fAccuracyPenalty() = aNetVars->m_flAccuracyPenalty;
		else g_Client.m_PredictionError = true;

		if (IsFloatValid(aNetVars->m_flRecoilIndex, pPlayer->GetActiveWeapon()->m_flRecoilIndex()))
			pPlayer->GetActiveWeapon()->m_flRecoilIndex() = aNetVars->m_flRecoilIndex;
		else g_Client.m_PredictionError = true;
	}

	if (IsFloatValid(aNetVars->m_flVelocityModifier, pPlayer->m_flVelocityModifier(), true))
		pPlayer->m_flVelocityModifier() = aNetVars->m_flVelocityModifier;
	else g_Client.m_PredictionError = true;

	// TODO@Monster: FIX THAT SHIT.
	//if (pPlayer->m_vecViewOffset().z > 64.f)
	//	pPlayer->m_vecViewOffset().z = 64.f;
	//else if (pPlayer->m_vecViewOffset().z < 46.f)
	//	pPlayer->m_vecViewOffset().z = 46.f;

	if (pPlayer->m_fFlags() & FL_ONGROUND)
		pPlayer->m_flFallVelocity() = 0.f;
}

void NetData::Reset() {
	m_aCompressData.fill(CNetvarData());
}