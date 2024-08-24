#include "../../precompiled.h"

void AimPlayer::OnRoundStart(Player* pPlayer)
{
	m_Player = pPlayer;
	m_Records.clear();
}

void Aimbot::Init() {
	m_AutoStopMovement = false;

	if (!Enabled())
		return;

	bRevolver = false/*g_Client.m_WeaponItemDefinition == REVOLVER*/;

	if (!g_Client.CanFireWeapon(0, true, m_Config.m_AutoStopFlags[AUTOSTOP::AUTOSTOP_BETWEEN]))
		return RemoveAttack();

	m_PlayerData.clear();

	for (int nIndex{ 1 }; nIndex <= g_Csgo.m_Globals->m_MaxClients; ++nIndex)
	{
		AimPlayer* pData = &m_Players[nIndex - 1];
		if (pData == nullptr)
			continue;

		if (!IsValidTarget(pData->m_Player) || pData->m_Player->m_fFlags() & FL_FROZEN || pData->m_Player->GetClientNetworkable()->IsDormant())
			continue;

		m_PlayerData.emplace_back(pData->m_Player);
	}

	if (m_PlayerData.empty())
		return;

	if (m_Config.m_OptimizationConditions[0] && static_cast<size_t>(m_Config.m_DeterminePlayerAmount) > m_PlayerData.size() ||
		/*m_Config.m_OptimizationConditions[1] &&*/ m_Config.m_DetermineFramePerSecond > 0 && m_Config.m_DetermineFramePerSecond <= g_Client.m_FramePerSecond)
		m_Config.m_OptimizationLevel = OPTIMIZATION::OFF;

	m_OptimizedTargets.clear();

	size_t iDynamicEraser = m_Config.m_OptimizationLevel >= OPTIMIZATION::MAXIMUM ? 1 : static_cast<size_t>(m_Config.m_LimitTarget);

	// TODO@Monster: Think About it.
	if (m_Config.m_OptimizationLevel >= OPTIMIZATION::HIGH && m_PlayerData.size() > 1 && m_PlayerData.size() > iDynamicEraser)
	{
		if (m_Config.m_LimitTarget == 0)
			iDynamicEraser = m_PlayerData.size() / 2;

		std::sort(m_PlayerData.begin(), m_PlayerData.end(), [&](Player*& m_FirstTarget, Player*& m_LastTarget)
			{
				if (m_FirstTarget == nullptr || m_LastTarget == nullptr)
					return false;

				return g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_FirstTarget->m_vecOrigin()) < g_Client.m_LocalPlayer->m_vecOrigin().Delta(m_LastTarget->m_vecOrigin());
			});

		for (size_t i = 0u; i <= std::min(iDynamicEraser, m_PlayerData.size()); i++)
			m_OptimizedTargets.emplace_back(m_PlayerData.at(rand() % m_PlayerData.size()));
	}
	else m_OptimizedTargets = m_PlayerData;

	if (m_OptimizedTargets.empty())
		return;

	m_Data.clear();

	for (Player* const& pPlayer : m_OptimizedTargets)
	{
		if (pPlayer == nullptr)
			continue;

		AimPlayer* pData = &m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1];
		if (pData == nullptr)
			continue;

		LagRecord* FirstRecord = g_Resolver.GetIdealRecord(pData);
		if (FirstRecord == nullptr)
			continue;

		//LagRecord* AllRecord = g_Resolver.GetAllRecords(pData);
		//if (AllRecord == nullptr)
		//	continue;

		LagRecord* LastRecord = g_Resolver.GetLastRecord(pData);
		if (LastRecord == nullptr)
			continue;

		//if (AllRecord->m_Resolved && !AllRecord->m_Velocity.IsValid() && fabs(pData->m_WalkRecord.m_LowerbodyYaw) > 0.f)
		//	LastRecord = AllRecord;

		SetUpHitboxes(pPlayer, pData, false);

		AimData FrontPoint = GetAimPoint(pPlayer, FirstRecord);
		if (FrontPoint.IsValid(m_Config.m_MaximumFOV))
		{
			if (FrontPoint.m_Damage >= pPlayer->m_iHealth() && m_Config.m_OptimizationLevel >= OPTIMIZATION::MEDIUM || LastRecord->m_DuckAmount == FirstRecord->m_DuckAmount && LastRecord->m_EyeAngles.Delta(FirstRecord->m_EyeAngles) <= 5.f && LastRecord->m_Origin.Delta(FirstRecord->m_Origin) <= 10.f && m_Config.m_OptimizationLevel >= OPTIMIZATION::MEDIUM || g_LagCompensation.StartPrediction(pData))
			{
				m_Data.emplace_back(FrontPoint.m_Player, FrontPoint.m_Record, FrontPoint.m_AimPositionData, FrontPoint.m_Hitbox, FrontPoint.m_Damage, FrontPoint.m_FOV, FrontPoint.m_Distance, false);
				continue;
			}
		}

		if (m_Config.m_ManualHistoryHitboxes)
			SetUpHitboxes(pPlayer, pData, true);

		AimData LastPoint = GetAimPoint(pPlayer, LastRecord);
		if (!LastPoint.IsValid(m_Config.m_MaximumFOV))
		{
			if (!FrontPoint.IsValid(m_Config.m_MaximumFOV))
				continue;

			m_Data.emplace_back(FrontPoint.m_Player, FrontPoint.m_Record, FrontPoint.m_AimPositionData, FrontPoint.m_Hitbox, FrontPoint.m_Damage, FrontPoint.m_FOV, FrontPoint.m_Distance, false);
			continue;
		}

		AimData BestPoint = LastPoint.m_Damage >= FrontPoint.m_Damage ? LastPoint : FrontPoint;
		if (!BestPoint.IsValid(m_Config.m_MaximumFOV))
			continue;

		m_Data.emplace_back(BestPoint.m_Player, BestPoint.m_Record, BestPoint.m_AimPositionData, BestPoint.m_Hitbox, BestPoint.m_Damage, BestPoint.m_FOV, BestPoint.m_Distance, LastPoint.m_Damage >= FrontPoint.m_Damage);
	}

	if (m_Data.empty())
		return;

	std::sort(m_Data.begin(), m_Data.end(), [this](const AimData& m_FirstTarget, const AimData& m_LastTarget)
		{
			this->m_FirstTarget = m_FirstTarget; this->m_LastTarget = m_LastTarget;

			if (g_Client.m_WeaponType == WEAPONTYPE_KNIFE)
				return m_FirstTarget.m_Distance < m_LastTarget.m_Distance;

			else {
				switch (g_Config.i(g_Items.m_Aimbot.m_General.m_TargetSelection))
				{
				case 0:
					// TODO@Monster: Think about it and make it dynamic .
					//float flDamageDifference = m_FirstTarget.m_Damage - m_LastTarget.m_Damage;
					//float flHealthDifference = m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth();

					if ((m_FirstTarget.m_Damage >= m_FirstTarget.m_Player->m_iHealth()) != (m_LastTarget.m_Damage >= m_LastTarget.m_Player->m_iHealth()))
						return (m_FirstTarget.m_Damage >= m_FirstTarget.m_Player->m_iHealth()) >= (m_LastTarget.m_Damage >= m_LastTarget.m_Player->m_iHealth());

					else if (m_FirstTarget.m_Record->m_Legit != m_LastTarget.m_Record->m_Legit)
						return m_FirstTarget.m_Record->m_Legit >= m_LastTarget.m_Record->m_Legit;

					else if (m_FirstTarget.m_Record->m_Resolved != m_LastTarget.m_Record->m_Resolved)
						return m_FirstTarget.m_Record->m_Resolved >= m_LastTarget.m_Record->m_Resolved;

					else if (m_FirstTarget.m_Record->m_BrokeLagCompensation != m_LastTarget.m_Record->m_BrokeLagCompensation)
						return m_FirstTarget.m_Record->m_BrokeLagCompensation < m_LastTarget.m_Record->m_BrokeLagCompensation;

					else if (m_FirstTarget.m_Record->m_Shot != m_LastTarget.m_Record->m_Shot)
						return m_FirstTarget.m_Record->m_Shot >= m_LastTarget.m_Record->m_Shot;

					else if (m_FirstTarget.m_Record->m_FakeWalk != m_LastTarget.m_Record->m_FakeWalk)
						return m_FirstTarget.m_Record->m_FakeWalk < m_LastTarget.m_Record->m_FakeWalk;

					// TODO@Monster: Think about it and make it dynamic .
					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 75)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 75)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 50)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 50)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 25)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 25)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Distance != m_LastTarget.m_Distance)
						return m_FirstTarget.m_Distance < m_LastTarget.m_Distance;

					else return m_FirstTarget.m_FOV < m_LastTarget.m_FOV;

					//else if (abs(flHealthDifference - flDamageDifference) > 25)
					//	return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();
					//else 
					//	return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;
					break;
				case 1:
					if (m_FirstTarget.m_Player->m_iHealth() != m_LastTarget.m_Player->m_iHealth())
						return m_FirstTarget.m_Player->m_iHealth() >= m_LastTarget.m_Player->m_iHealth();

					else if ((m_FirstTarget.m_Damage >= m_FirstTarget.m_Player->m_iHealth()) != (m_LastTarget.m_Damage >= m_LastTarget.m_Player->m_iHealth()))
						return (m_FirstTarget.m_Damage >= m_FirstTarget.m_Player->m_iHealth()) >= (m_LastTarget.m_Damage >= m_LastTarget.m_Player->m_iHealth());

					else if (m_FirstTarget.m_Record->m_Legit != m_LastTarget.m_Record->m_Legit)
						return m_FirstTarget.m_Record->m_Legit >= m_LastTarget.m_Record->m_Legit;

					else if (m_FirstTarget.m_Record->m_Resolved != m_LastTarget.m_Record->m_Resolved)
						return m_FirstTarget.m_Record->m_Resolved >= m_LastTarget.m_Record->m_Resolved;

					else if (m_FirstTarget.m_Record->m_BrokeLagCompensation != m_LastTarget.m_Record->m_BrokeLagCompensation)
						return m_FirstTarget.m_Record->m_BrokeLagCompensation < m_LastTarget.m_Record->m_BrokeLagCompensation;

					else if (m_FirstTarget.m_Record->m_Shot != m_LastTarget.m_Record->m_Shot)
						return m_FirstTarget.m_Record->m_Shot >= m_LastTarget.m_Record->m_Shot;

					else if (m_FirstTarget.m_Record->m_FakeWalk != m_LastTarget.m_Record->m_FakeWalk)
						return m_FirstTarget.m_Record->m_FakeWalk < m_LastTarget.m_Record->m_FakeWalk;

					// TODO@Monster: Think about it and make it dynamic .
					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 75)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 75)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 50)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 50)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Player->m_iHealth() - m_LastTarget.m_Player->m_iHealth() > 25)
						return m_FirstTarget.m_Player->m_iHealth() < m_LastTarget.m_Player->m_iHealth();

					else if (m_FirstTarget.m_Damage - m_LastTarget.m_Damage > 25)
						return m_FirstTarget.m_Damage >= m_LastTarget.m_Damage;

					else if (m_FirstTarget.m_Distance != m_LastTarget.m_Distance)
						return m_FirstTarget.m_Distance < m_LastTarget.m_Distance;

					else return m_FirstTarget.m_FOV < m_LastTarget.m_FOV;
					break;
				default:
					if (m_FirstTarget.m_Record->m_BrokeLagCompensation != m_LastTarget.m_Record->m_BrokeLagCompensation)
						return m_FirstTarget.m_Record->m_BrokeLagCompensation < m_LastTarget.m_Record->m_BrokeLagCompensation;

					else return m_FirstTarget.m_Distance < m_LastTarget.m_Distance;
					break;
				}
			}
		});

	AimData FilledData{};
	for (AimData& pData : m_Data)
	{
		if (!pData.IsValid(m_Config.m_MaximumFOV))
			continue;

		bool bBreak{ true };

		if (FilledData.m_Damage >= pData.m_Damage)
			continue;

		FilledData = pData;

		if (bBreak)
			break;
	}

	if (!FilledData.IsValid(m_Config.m_MaximumFOV))
		return;

	QAngle angCalculatedAngle = QAngle(0.f, 0.f, 0.f);
	bool bAttack2 = false;

	ICollideable* pCollideable = FilledData.m_Player->GetCollideable();
	if (!pCollideable)
		return;

	CBoneCache* pBoneCache = &FilledData.m_Player->m_BoneCache();
	if (!pBoneCache)
		return;

	Backup_t pBackup;

	pBackup.m_AbsOrigin = FilledData.m_Player->GetAbsOrigin();
	pBackup.m_AbsAngles = FilledData.m_Player->GetAbsAngles();

	pBackup.m_OBBMaxs = pCollideable->OBBMaxs();
	pBackup.m_OBBMins = pCollideable->OBBMins();

	pBackup.m_CachedBones = pBoneCache->m_pCachedBones;
	pBackup.m_CachedBoneCount = pBoneCache->m_CachedBoneCount;

	auto Restore = [](AimData FilledData, ICollideable* pCollideable, CBoneCache* pBoneCache, Backup_t pBackup)
	{
		FilledData.m_Player->SetAbsOrigin(pBackup.m_AbsOrigin);
		FilledData.m_Player->SetAbsAngles(pBackup.m_AbsAngles);

		pCollideable->OBBMins() = pBackup.m_OBBMins;
		pCollideable->OBBMaxs() = pBackup.m_OBBMaxs;

		pBoneCache->m_CachedBoneCount = pBackup.m_CachedBoneCount;
		pBoneCache->m_pCachedBones = pBackup.m_CachedBones;
	};

	FilledData.m_Player->SetAbsAngles(FilledData.m_Record->m_AbsAngles);
	FilledData.m_Player->SetAbsOrigin(FilledData.m_Record->m_Origin);

	pCollideable->OBBMins() = FilledData.m_Record->m_OBBMins;
	pCollideable->OBBMaxs() = FilledData.m_Record->m_OBBMaxs;

	pBoneCache->m_CachedBoneCount = FilledData.m_Record->m_CachedBoneCount;
	pBoneCache->m_pCachedBones = FilledData.m_Record->m_CachedBones;

	int iAdjustTickCount = g_Aimbot.AdjustTickCount(FilledData, pCollideable);

	OptimizeMultipoint(FilledData);

	Math::VectorAngles(FilledData.m_AimPositionData.m_MultiPoints - g_Client.m_EyePosition, angCalculatedAngle);

	if (g_Client.m_WeaponType == WEAPONTYPE_KNIFE && g_Client.m_WeaponItemDefinition != ZEUS) {
		if (!CanKnife(FilledData.m_Player, angCalculatedAngle, bAttack2))
			return Restore(FilledData, pCollideable, pBoneCache, pBackup);
	}

	if (!IsPointAccurate(FilledData)) {
		if (!(g_Client.m_Buttons & IN_JUMP)) {
			if (!g_Client.m_LocalPlayer->m_bIsScoped() && g_Client.m_CanScope && m_Config.m_AutoScope) {
				g_Client.m_Command->m_Buttons |= IN_ATTACK2;
				return Restore(FilledData, pCollideable, pBoneCache, pBackup);
			}

			m_AutoStopMovement = !(m_Config.m_AutoStopConditions[0] && FilledData.m_Damage < FilledData.m_Player->m_iHealth());
		}

		return Restore(FilledData, pCollideable, pBoneCache, pBackup);
	}

	if (!g_Client.CanFireWeapon())
		return Restore(FilledData, pCollideable, pBoneCache, pBackup);

	if (m_Config.m_AutoFire && !IsAutoFiring(bAttack2))
		return Restore(FilledData, pCollideable, pBoneCache, pBackup);

	g_Client.m_Command->m_ViewAngles = angCalculatedAngle - (g_Client.m_LocalPlayer->m_aimPunchAngle() * g_Csgo.weapon_recoil_scale->GetFloat());

	if (!m_Config.m_Silent)
		g_Csgo.m_Engine->SetViewAngles(angCalculatedAngle);

	if (!(g_Client.m_Shot = g_Client.IsShooting()))
		return Restore(FilledData, pCollideable, pBoneCache, pBackup);

	g_Client.m_ShotCommandNumber = g_Client.GetModifableCommand()->m_CommandNumber;
	g_Client.m_Command->m_TickCount = iAdjustTickCount;

	// TODO@Monster: Improve.
	g_Client.GetModifablePacket() = !g_AntiAim.m_Config.m_HideShots;

	if (g_TickbaseManipulation.GetActiveExploit())
		g_Client.GetModifablePacket() = true;

	ShotRecord ShotData{ };

	ShotData.m_Player = FilledData.m_Player;
	ShotData.m_Record = FilledData.m_Record;
	ShotData.m_Damage = FilledData.m_Damage;
	ShotData.m_WantedHitbox = FilledData.m_Hitbox;
	ShotData.m_Latency = g_Client.m_LatencyOutgoing;
	ShotData.m_ShotTick = g_Client.m_Command->m_TickCount;
	ShotData.m_EndPosition = FilledData.m_AimPositionData.m_MultiPoints;
	ShotData.m_CurrentTime = g_Csgo.m_Globals->m_Curtime;
	ShotData.m_StartPosition = g_Client.m_EyePosition;
	ShotData.m_HasBeenRegistered = false;
	ShotData.m_HasBeenHurted = false;
	ShotData.m_HasBeenFired = false;

	if (!g_ShotSystem.m_Shots.empty())
		g_ShotSystem.m_Shots.clear();

	g_ShotSystem.m_Shots.emplace_back(ShotData);

	while (g_ShotSystem.m_Shots.size() > g_Client.m_TickRate)
		g_ShotSystem.m_Shots.pop_back();

	return Restore(FilledData, pCollideable, pBoneCache, pBackup);
}

void Aimbot::OptimizeMultipoint(AimData& Info)
{
	if (Info.m_AimPositionData.m_FinalRadius == 0.f || Info.m_AimPositionData.m_Radius == 0.f || Info.m_AimPositionData.m_CenterPoint == Info.m_AimPositionData.m_MultiPoints)
		return;

	auto vecOriginalPosition = Info.m_AimPositionData.m_MultiPoints;
	auto vecOptimalPosition = vecOriginalPosition;
	float flOptimalDamage = Info.m_Damage;

	static constexpr int nSteps = 6;
	for (auto i = 0; i <= nSteps; i++)
	{
		const float flScale = Info.m_AimPositionData.m_FinalRadius / Info.m_AimPositionData.m_Radius;
		if (flScale < .2f)
			break;

		vecOriginalPosition = Info.m_AimPositionData.m_CenterPoint + ((Info.m_AimPositionData.m_MultiPoints - Info.m_AimPositionData.m_CenterPoint) / Info.m_AimPositionData.m_FinalRadius) * Info.m_AimPositionData.m_Radius * ((flScale - .2f) / nSteps * (nSteps - i));

		Penetration::PenetrationInput_t Input;

		Input.m_PenetrationDamage = Input.m_Damage = Info.m_Damage;
		Input.m_Penetrate = m_Config.m_AutoWall;
		Input.m_Player = Info.m_Player;
		Input.m_From = g_Client.m_LocalPlayer;
		Input.m_Position = vecOriginalPosition;
		Input.m_EyePosition = g_Client.m_EyePosition;

		Penetration::PenetrationOutput_t Output;
		Penetration::Run(&Input, &Output);

		const auto flDamage = Output.m_Damage;
		if (flDamage > 0.f)
		{
			if (Info.m_Damage > Info.m_Player->m_iHealth()
				&& flDamage < Info.m_Player->m_iHealth() + 20.f)
				continue;

			if (flDamage + 20.f < Info.m_Damage)
				continue;

			vecOptimalPosition = vecOriginalPosition;
			flOptimalDamage = flDamage;
		}
	}

	Info.m_AimPositionData.m_MultiPoints = vecOptimalPosition;
	Info.m_Damage = flOptimalDamage;
}

bool Aimbot::CanKnife(Player* pPlayer, QAngle vecAngles, bool& Stab)
{
	Vector vecForward;
	Math::AngleVectors(vecAngles, &vecForward);

	CGameTrace pTrace;
	KnifeTrace(vecForward, false, &pTrace);

	if (!pTrace.m_Player || pTrace.m_Player != pPlayer)
		return false;

	const bool bArmor = pPlayer->m_ArmorValue() > 0;
	const bool bFirst = g_Client.m_Weapon->m_flNextPrimaryAttack() + 0.4f < g_Csgo.m_Globals->m_Curtime;
	const bool bBack = KnifeIsBehind(pPlayer);

	const int iStabDmg = m_KnifeDamages.Stab[bArmor][bBack];
	const int iSlashDmg = m_KnifeDamages.Swing[bFirst][bArmor][bBack];
	const int iSwingDmg = m_KnifeDamages.Swing[false][bArmor][bBack];

	const int iHealth = pPlayer->m_iHealth();
	if (iHealth <= iSlashDmg)
		Stab = false;

	else if (iHealth <= iStabDmg)
		Stab = true;

	else if (iHealth > (iSlashDmg + iSwingDmg + iStabDmg))
		Stab = true;

	else Stab = false;

	if (Stab && !KnifeTrace(vecForward, true, &pTrace))
		return false;

	return true;
}

bool Aimbot::KnifeTrace(Vector dir, bool Stab, CGameTrace* pTrace)
{
	const float range = Stab ? 32.f : 48.f;

	const Vector start = g_Client.m_EyePosition;
	const Vector end = start + (dir * range);

	CTraceFilterSimple filter; filter.SetPassEntity(g_Client.m_LocalPlayer);
	g_Csgo.m_EngineTrace->TraceRay(Ray(start, end), MASK_SOLID, &filter, pTrace);

	if (pTrace->m_Fraction >= 1.f) {
		g_Csgo.m_EngineTrace->TraceRay(Ray(start, end, { -16.f, -16.f, -18.f }, { 16.f, 16.f, 18.f }), MASK_SOLID, &filter, pTrace);
		return pTrace->m_Fraction < 1.f;
	}

	return true;
}

bool Aimbot::KnifeIsBehind(Player* pPlayer)
{
	Vector delta{ pPlayer->GetAbsOrigin() - g_Client.m_EyePosition };
	delta.z = 0.f;
	delta.Normalize();

	Vector target;
	Math::AngleVectors(pPlayer->GetAbsAngles(), &target);
	target.z = 0.f;

	return delta.Dot(target) > 0.475f;
}

int Aimbot::TickLatency()
{
	return game::TICKS_TO_TIME(g_Client.m_LatencyTicks - 1);
}

Vector Aimbot::Extrapolation(AimData FilledData)
{
	return FilledData.m_AimPositionData.m_MultiPoints + (FilledData.m_Record->m_Velocity * g_Aimbot.TickLatency());
}

#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
//#define INFORMATION

int Aimbot::AdjustTickCount(AimData FilledData, ICollideable* pCollideable)
{
#ifdef INFORMATION
	if (g_Client.m_Command->m_Buttons & IN_ATTACK)
	{
		g_notify.add(XOR("Client Tick Count: ") + std::to_string(g_Client.m_Command->m_TickCount) + "\n");
		g_notify.add(XOR("Predicted Tick Count: ") + std::to_string(game::TIME_TO_TICKS(FilledData.m_Record->m_SimulationTime + g_Client.m_Lerp)) + "\n");
		g_notify.add(XOR("Difference: ") + std::to_string(abs(g_Client.m_Command->m_TickCount - game::TIME_TO_TICKS(FilledData.m_Record->m_SimulationTime + g_Client.m_Lerp))) + "\n");
	}
#endif // INFORMATION

	int iFutureCommand = 0;

	//if (!FilledData.m_LagCompensation)
	//	iFutureCommand = g_TickbaseManipulation.GetShiftAmount();

	if (iFutureCommand > 0) {
#ifdef INFORMATION
		if (g_Client.m_Command->m_Buttons & IN_ATTACK)
			g_notify.add(XOR("Client Tick Count: ") + std::to_string(g_Client.m_Command->m_TickCount) + "\n");
#endif // INFORMATION
		return g_Client.m_Command->m_TickCount;
	}

	if (!FilledData.m_Record) {
#ifdef INFORMATION
		if (g_Client.m_Command->m_Buttons & IN_ATTACK)
			g_notify.add(XOR("Last Tick Count: ") + std::to_string(FilledData.m_Player->m_flSimulationTime() + g_Client.m_Lerp) + "\n");
#endif // INFORMATION
		return game::TIME_TO_TICKS(FilledData.m_Player->m_flSimulationTime() + g_Client.m_Lerp);
	}

	if (FilledData.m_Record->m_BrokeLagCompensation) {
#ifdef INFORMATION
		if (g_Client.m_Command->m_Buttons & IN_ATTACK)
			g_notify.add(XOR("Client Tick Count: ") + std::to_string(g_Client.m_Command->m_TickCount) + "\n");
#endif // INFORMATION
		return g_Client.m_Command->m_TickCount;
	}

	// Todo@Monster: Make This Work By Working With Lerping, 0.1f.
	const float flLerpRemainder = std::fmodf(g_Client.m_Lerp, g_Csgo.m_Globals->m_Interval);
	if (flLerpRemainder <= 0.f) {
#ifdef INFORMATION
		if (g_Client.m_Command->m_Buttons & IN_ATTACK)
			g_notify.add(XOR("Manipulated Tick Count: ") + std::to_string(FilledData.m_Record->m_SimulationTime + g_Client.m_Lerp) + "\n");
#endif // INFORMATION
		return game::TIME_TO_TICKS(FilledData.m_Record->m_SimulationTime + g_Client.m_Lerp);
	}

	float flTargetTime = FilledData.m_Record->m_SimulationTime;
	flTargetTime += g_Csgo.m_Globals->m_Interval - flLerpRemainder;

	int iAdjustTickCount = game::TIME_TO_TICKS(flTargetTime + g_Client.m_Lerp);

#ifdef INFORMATION
	if (g_Client.m_Command->m_Buttons & IN_ATTACK)
		g_notify.add(XOR("Adjust Tick Count: ") + std::to_string(iAdjustTickCount) + "\n");
#endif // INFORMATION

	AimPlayer* pLagCompensation = &m_Players[FilledData.m_Player->GetClientNetworkable()->GetIndex() - 1];
	if (!pLagCompensation || pLagCompensation->m_Records.size() <= 1)
		return iAdjustTickCount;

	LagRecord* pPrevious = pLagCompensation->m_Records[1].get();
	if (!pPrevious || pPrevious->m_Dormant)
		return iAdjustTickCount;

	// NOTE: Server Has Started To Add Interpolation To The Origin, So We Should.
	if (FilledData.m_Record->m_SimulationTime > flTargetTime || FilledData.m_Record->m_SimulationTime > pPrevious->m_SimulationTime)
		return iAdjustTickCount;

	float flFraction = (flTargetTime - FilledData.m_Record->m_SimulationTime) /
		g_Csgo.m_Globals->m_Interval;

	QAngle angAbsAngles = Math::Lerp(flFraction, FilledData.m_Record->m_AbsAngles, pPrevious->m_AbsAngles);
	Vector vecOrigin = Math::Lerp(flFraction, FilledData.m_Record->m_Origin, pPrevious->m_Origin);
	Vector vecMins = Math::Lerp(flFraction, FilledData.m_Record->m_OBBMins, pPrevious->m_OBBMins);
	Vector vecMaxs = Math::Lerp(flFraction, FilledData.m_Record->m_OBBMaxs, pPrevious->m_OBBMaxs);

	if ((FilledData.m_Record->m_AbsAngles - angAbsAngles).LengthSqr() > LAG_COMPENSATION_EPS_SQR)
		FilledData.m_Player->SetAbsAngles(angAbsAngles);

	if ((FilledData.m_Record->m_Origin - vecOrigin).LengthSqr() > LAG_COMPENSATION_EPS_SQR)
		FilledData.m_Player->SetAbsOrigin(vecOrigin);

	if (vecMins != FilledData.m_Record->m_OBBMins || vecMaxs != FilledData.m_Record->m_OBBMaxs)
	{
		pCollideable->OBBMins() = vecMins;
		pCollideable->OBBMaxs() = vecMaxs;
	}

	return iAdjustTickCount;
}

bool Aimbot::IsAutoFiring(bool bAttack2)
{
	bAttack2 ? g_Client.m_Command->m_Buttons |= IN_ATTACK2 : g_Client.m_Command->m_Buttons |= IN_ATTACK;

	m_AccuracyData.m_HasFired = true;

	return g_Client.m_Command->m_Buttons & IN_ATTACK || g_Client.m_Command->m_Buttons & IN_ATTACK2;
}

void Aimbot::RemoveAttack()
{
	bRevolver ? g_Client.m_Command->m_Buttons &= ~IN_ATTACK2 : g_Client.m_Command->m_Buttons &= ~IN_ATTACK;
}

void Aimbot::AddAttack()
{
	bRevolver ? g_Client.m_Command->m_Buttons |= IN_ATTACK2 : g_Client.m_Command->m_Buttons |= IN_ATTACK;
}

void Aimbot::AddMultipoints(Vector vecPoint, Vector vecCenter, int& iMultiPointAmount, float flHeadFinalRadius, float flRadius)
{
	++iMultiPointAmount;

	m_Multipoints.emplace_back(vecPoint, vecCenter, flHeadFinalRadius, flRadius);
}

void Aimbot::Multipoints(Player* pPlayer, LagRecord* pRecord, Hitboxes_t index)
{
	const model_t* pModel = pPlayer->GetClientRenderable()->GetModel();
	if (!pModel)
		return;

	studiohdr_t* pStudioModel = g_Csgo.m_ModelInfo->GetStudioModel(pModel);
	if (!pStudioModel)
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->pHitboxSet(pPlayer->m_nHitboxSet());
	if (!pHitboxSet)
		return;

	mstudiobbox_t* pHitbox = pHitboxSet->Hitbox(index);
	if (!pHitbox)
		return;

	ICollideable* pCollideable = pPlayer->GetCollideable();
	if (!pCollideable)
		return;

	CBoneCache* pBoneCache = &pPlayer->m_BoneCache();
	if (!pBoneCache)
		return;

	pPlayer->SetAbsAngles(pRecord->m_AbsAngles);
	pPlayer->SetAbsOrigin(pRecord->m_Origin);

	pCollideable->OBBMins() = pRecord->m_OBBMins;
	pCollideable->OBBMaxs() = pRecord->m_OBBMaxs;

	pBoneCache->m_CachedBoneCount = pRecord->m_CachedBoneCount;
	pBoneCache->m_pCachedBones = pRecord->m_CachedBones;

	const float scale = m_Config.m_MultipointHeadScale / 100.f;
	const float bscale = m_Config.m_MultipointBodyScale / 100.f;

	if (pHitbox->flCapsuleRadius <= 0.f) {
		matrix3x4a_t rot_matrix;
		g_Csgo.AngleMatrix(pHitbox->angOffsetOrientation, rot_matrix);

		matrix3x4a_t pCachedBones;
		Math::ConcatTransforms(pBoneCache->m_pCachedBones[pHitbox->bone], rot_matrix, pCachedBones);

		const Vector vecCenter = (pHitbox->vecMin + pHitbox->vecMax) / 2.f;

		int iMultiPointsCount = 0;

		AddMultipoints(vecCenter, vecCenter, iMultiPointsCount = 1);

		if (index == Hitboxes_t::R_FOOT || index == Hitboxes_t::L_FOOT) {
			float d1 = (pHitbox->vecMin.z - vecCenter.z) * 0.875f;

			if (index == Hitboxes_t::L_FOOT)
				d1 *= -1.f;

			AddMultipoints(Vector{ vecCenter.x, vecCenter.y, vecCenter.z + d1 }, vecCenter, iMultiPointsCount);

			if (m_Config.m_Multipoint[Multipoint_t::FEET] && bscale > 0.f)
			{
				AddMultipoints(Vector{ vecCenter.x + (pHitbox->vecMin.x - vecCenter.x) * bscale, vecCenter.y, vecCenter.z }, vecCenter, iMultiPointsCount);
				AddMultipoints(Vector{ vecCenter.x + (pHitbox->vecMax.x - vecCenter.x) * bscale, vecCenter.y, vecCenter.z }, vecCenter, iMultiPointsCount);
			}
		}

		if (m_Multipoints.empty())
			return;

		for (auto& p : m_Multipoints)
		{
			p.m_MultiPoints = { p.m_MultiPoints.Dot(pCachedBones[0]), p.m_MultiPoints.Dot(pCachedBones[1]), p.m_MultiPoints.Dot(pCachedBones[2]) };
			p.m_MultiPoints += pCachedBones.GetOrigin();

			p.m_CenterPoint = { p.m_CenterPoint.Dot(pCachedBones[0]), p.m_CenterPoint.Dot(pCachedBones[1]), p.m_CenterPoint.Dot(pCachedBones[2]) };
			p.m_CenterPoint += pCachedBones.GetOrigin();
		}
	}
	else {
		const float flRadius = pHitbox->flCapsuleRadius;
		const Vector vecCenter = (pHitbox->vecMin + pHitbox->vecMax) / 2.f;

		if (index == Hitboxes_t::HEAD) {
			const float flHeadFinalRadius = flRadius * scale;

			int iMultiPointsCount = 1;

			AddMultipoints(vecCenter, vecCenter, ++iMultiPointsCount, flHeadFinalRadius, flRadius);

			if (scale > 0.f && m_Config.m_Multipoint[Multipoint_t::HEAD]) {
				constexpr float rotation = 0.70710678f;

				static int iStaticMultiPointAmount = 0;
				
				if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 0)
					AddMultipoints(Vector{ pHitbox->vecMax.x + (rotation * flHeadFinalRadius), pHitbox->vecMax.y + (-rotation * flHeadFinalRadius), pHitbox->vecMax.z }, vecCenter, iMultiPointsCount, flHeadFinalRadius, flRadius);

				if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 1)
					AddMultipoints(Vector{ pHitbox->vecMax.x, pHitbox->vecMax.y, pHitbox->vecMax.z + flHeadFinalRadius }, vecCenter, iMultiPointsCount, flHeadFinalRadius, flRadius);

				if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 2)
					AddMultipoints(Vector{ pHitbox->vecMax.x, pHitbox->vecMax.y, pHitbox->vecMax.z - flHeadFinalRadius }, vecCenter, iMultiPointsCount, flHeadFinalRadius, flRadius);

				if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 3)
					AddMultipoints(Vector{ pHitbox->vecMax.x, pHitbox->vecMax.y - flHeadFinalRadius, pHitbox->vecMax.z }, vecCenter, iMultiPointsCount, flHeadFinalRadius, flRadius);

				if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 4)
					if (pRecord->m_Player->m_vecVelocity().Length() <= 0.1f && pRecord->m_EyeAngles.x <= 89.f)
						AddMultipoints(Vector{ pHitbox->vecMax.x - flHeadFinalRadius, pHitbox->vecMax.y, pHitbox->vecMax.z }, vecCenter, iMultiPointsCount, flHeadFinalRadius, flRadius);

				iStaticMultiPointAmount = iMultiPointsCount;
			}
		}
		else {
			float flScaleIncreaser = 1.0f;

			if (index == Hitboxes_t::BODY || index == Hitboxes_t::PELVIS || index == Hitboxes_t::UPPER_CHEST || index == Hitboxes_t::THORAX || index == Hitboxes_t::CHEST)
				flScaleIncreaser = 1.2f;

			const float flBodyFinalRadius = flRadius * bscale * flScaleIncreaser;

			int iMultiPointsCount = 1;

			AddMultipoints(vecCenter, vecCenter, ++iMultiPointsCount, flBodyFinalRadius, flRadius);

			static int iStaticMultiPointAmount = 0;

			if (index == Hitboxes_t::BODY) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::LOWER])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 0)
						AddMultipoints(Vector{ vecCenter.x, pHitbox->vecMax.y - flBodyFinalRadius, vecCenter.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			else if (index == Hitboxes_t::PELVIS) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::LOWER])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 1)
						AddMultipoints(Vector{ vecCenter.x, pHitbox->vecMax.y - flBodyFinalRadius, vecCenter.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			else if (index == Hitboxes_t::UPPER_CHEST) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::UPPER])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 1)
						AddMultipoints(Vector{ vecCenter.x, pHitbox->vecMax.y - flBodyFinalRadius, vecCenter.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			else if (index == Hitboxes_t::THORAX || index == Hitboxes_t::CHEST) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::CHEST])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 2)
						AddMultipoints(Vector{ vecCenter.x, pHitbox->vecMax.y - flBodyFinalRadius, vecCenter.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			else if (index == Hitboxes_t::R_CALF || index == Hitboxes_t::L_CALF) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::LEGS])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 3)
						AddMultipoints(Vector{ pHitbox->vecMax.x - (flRadius / 2.f), pHitbox->vecMax.y, pHitbox->vecMax.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			else if (index == Hitboxes_t::R_UPPER_ARM || index == Hitboxes_t::L_UPPER_ARM) {
				if (bscale > 0.f && m_Config.m_Multipoint[Multipoint_t::ARMS])
					if (m_Config.m_OptimizationLevel < OPTIMIZATION::MEDIUM || !iStaticMultiPointAmount || g_Csgo.m_Globals->m_TickCount % iStaticMultiPointAmount == 4)
						AddMultipoints(Vector{ pHitbox->vecMax.x + flRadius, vecCenter.y, vecCenter.z }, vecCenter, iMultiPointsCount, flBodyFinalRadius, flRadius);
			}

			iStaticMultiPointAmount = iMultiPointsCount;
		}

		if (m_Multipoints.empty())
			return;

		for (auto& p : m_Multipoints) {
			Math::VectorTransform(p.m_MultiPoints, pBoneCache->m_pCachedBones[pHitbox->bone], p.m_MultiPoints);
			Math::VectorTransform(p.m_CenterPoint, pBoneCache->m_pCachedBones[pHitbox->bone], p.m_CenterPoint);
		}
	}
}

AimData Aimbot::GetAimPoint(Player* pPlayer, LagRecord* pRecord)
{
	if (pRecord == nullptr || pPlayer == nullptr || pPlayer->m_iHealth() <= 0)
		return AimData();

	ICollideable* pCollideable = pPlayer->GetCollideable();
	if (!pCollideable)
		return AimData();

	CBoneCache* pBoneCache = &pPlayer->m_BoneCache();
	if (!pBoneCache)
		return AimData();

	AimPlayer* pData = &m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1];
	if (pData == nullptr)
		return AimData();

	if (m_Hitboxes.empty())
		return AimData();

	Backup_t pBackup;

	pBackup.m_AbsOrigin = pPlayer->GetAbsOrigin();
	pBackup.m_AbsAngles = pPlayer->GetAbsAngles();

	pBackup.m_OBBMaxs = pCollideable->OBBMaxs();
	pBackup.m_OBBMins = pCollideable->OBBMins();

	pBackup.m_CachedBones = pBoneCache->m_pCachedBones;
	pBackup.m_CachedBoneCount = pBoneCache->m_CachedBoneCount;

	AimData BestPoint = AimData();

	for (const HitscanBox& HitscanData : m_Hitboxes)
	{
		const float flDistance = g_Client.m_LocalPlayer->m_vecOrigin().Delta(pPlayer->m_vecOrigin());
		if (flDistance > g_Client.m_WeaponInfo->m_WeaponRange)
			continue;

		if (HitscanData.m_HitboxIndex < Hitboxes_t::HEAD || HitscanData.m_HitboxIndex >= Hitboxes_t::MAX)
			continue;

		const float flNextUpdateDifference = pData->m_NextLowerbodyYawUpdate - pRecord->m_SimulationTime;
		const bool bCanBreakLowerbodyYaw = pData->m_NextLowerbodyYawUpdate > 0.f && pData->m_ServerUpdatedLowerbodyYaw && pRecord->m_ResolverMode == Resolver::Modes::RESOLVE_STAND && fabs(pData->m_WalkRecord.m_SimulationTime) > 0.f;
		const bool bResolved = pRecord->m_Resolved || pData->m_ResolverMode == AimPlayer::RESOLVER::RESOLVED || pData->m_ResolverMode == AimPlayer::RESOLVER::LEGIT;

		if (m_Config.m_WaitForResolved && !flNextUpdateDifference && bCanBreakLowerbodyYaw && !bResolved && g_Client.m_Weapon->IsGun())
			continue;

		m_Multipoints.clear();

		Multipoints(pPlayer, pRecord, HitscanData.m_HitboxIndex);

		if (m_Multipoints.empty())
			continue;

		bool bBreak = false;

		for (const AimPosition& AimPositionData : m_Multipoints)
		{
			const float flFOV = Math::GetFOV(g_Client.m_ViewAngles, Math::CalcAngle(g_Client.m_EyePosition, AimPositionData.m_MultiPoints));
			if (flFOV >= m_Config.m_MaximumFOV)
				continue;

			if (c_config::get()->b["aimpoint_debug"])
				g_Csgo.m_DebugOverlay->AddBoxOverlay(AimPositionData.m_MultiPoints, Vector(-.6f, -.6f, -.6f), Vector(.6f, .6f, .6f), QAngle(0.f, 0.f, 0.f), 255, 0, 0, 127, 0.03f);

			Penetration::PenetrationInput_t Input;

			//const QAngle angCalculatedAngle = Math::CalcAngle(g_Client.m_EyePosition, AimPositionData.m_MultiPoints);

			//g_Client.SetupShootPosition(angCalculatedAngle - (g_Client.m_LocalPlayer->m_aimPunchAngle() * g_Csgo.weapon_recoil_scale->GetFloat()));

			Input.m_PenetrationDamage = Input.m_Damage = GetMinimumDamage(pPlayer, game::HitboxToHitgroup(HitscanData.m_HitboxIndex));
			Input.m_Penetrate = m_Config.m_AutoWall;
			Input.m_Player = pPlayer;
			Input.m_From = g_Client.m_LocalPlayer;
			Input.m_Position = AimPositionData.m_MultiPoints;
			Input.m_EyePosition = g_Client.m_EyePosition;

			Penetration::PenetrationOutput_t Output;

			if (!Penetration::Run(&Input, &Output))
				continue;

			if (HitscanData.m_HitboxIndex == Hitboxes_t::HEAD && Output.m_HitGroup != static_cast<int>(Hitgroups_t::HEAD))
				continue;

			if (m_Config.m_PredictiveDamage && HitscanData.m_HitboxIndex == Hitboxes_t::HEAD && Output.m_Damage * 2.f > pPlayer->m_iHealth() && Output.m_Damage < pPlayer->m_iHealth())
				continue;

			if (!m_Config.m_WaitForResolved && Output.m_Damage > pPlayer->m_iHealth())
			{
				if (bCanBreakLowerbodyYaw && flNextUpdateDifference <= 0.2f && !bResolved && g_Client.m_Weapon->IsGun())
					continue;
			}

			const bool bDisableBody =
				m_Config.m_PreferHeadAim[0] && pRecord->m_Resolved
				|| m_Config.m_PreferHeadAim[1] && pRecord->m_Shot
				|| m_Config.m_PreferHeadAim[2] && pRecord->m_Legit;

			if (!bDisableBody 
				&& (HitscanData.m_HitscanMode == HitscanMode::PREFER ||
					HitscanData.m_HitscanMode == HitscanMode::LETHAL2 && (Output.m_Damage >= pPlayer->m_iHealth() || Output.m_Damage * 2.f >= pPlayer->m_iHealth() && !bResolved) ||
					HitscanData.m_HitscanMode == HitscanMode::LETHAL && Output.m_Damage >= pPlayer->m_iHealth()))
				bBreak = true;

			else if (HitscanData.m_HitscanMode == HitscanMode::NORMAL) {
				bBreak = m_Config.m_OptimizationLevel >= OPTIMIZATION::MEDIUM;

				if (BestPoint.m_Damage >= Output.m_Damage)
					continue;

				BestPoint = AimData(pPlayer, pRecord, AimPositionData, HitscanData.m_HitboxIndex, Output.m_Damage, flFOV, flDistance);

				if (AimPositionData.m_MultiPoints == m_Multipoints.front().m_MultiPoints && Output.m_Damage >= pPlayer->m_iHealth())
					break;
			}

			if (bBreak) {
				BestPoint = AimData(pPlayer, pRecord, AimPositionData, HitscanData.m_HitboxIndex, Output.m_Damage, flFOV, flDistance);
				break;
			}
		}

		if (bBreak)
			break;
	}

	pPlayer->SetAbsOrigin(pBackup.m_AbsOrigin);
	pPlayer->SetAbsAngles(pBackup.m_AbsAngles);

	pCollideable->OBBMins() = pBackup.m_OBBMins;
	pCollideable->OBBMaxs() = pBackup.m_OBBMaxs;

	pBoneCache->m_CachedBoneCount = pBackup.m_CachedBoneCount;
	pBoneCache->m_pCachedBones = pBackup.m_CachedBones;

	return BestPoint.IsValid(m_Config.m_MaximumFOV) ? BestPoint : AimData();
}

void Aimbot::SetUpHitboxes(Player* pPlayer, AimPlayer* pData, bool bHistory) {
	m_Hitboxes.clear();

	if (g_Client.m_WeaponInfo == nullptr || pPlayer == nullptr || pData == nullptr)
		return;

	if (g_Client.m_WeaponType == WEAPONTYPE_KNIFE && g_Client.m_WeaponItemDefinition != ZEUS)
	{
		m_Hitboxes.emplace_back(Hitboxes_t::UPPER_CHEST, HitscanMode::NORMAL);
		m_Hitboxes.emplace_back(Hitboxes_t::CHEST, HitscanMode::NORMAL);
		m_Hitboxes.emplace_back(Hitboxes_t::PELVIS, HitscanMode::NORMAL);
		m_Hitboxes.emplace_back(Hitboxes_t::HEAD, HitscanMode::NORMAL);
		return;
	}

	const auto SetUpBodyAim = [&](std::unordered_map<int, bool> m_BodyHitbox, HitscanMode Mode)
	{
		if (m_BodyHitbox.empty())
		{
			m_Hitboxes.emplace_back(Hitboxes_t::BODY, Mode);
			m_Hitboxes.emplace_back(Hitboxes_t::UPPER_CHEST, Mode);
			return;
		}

		if (m_BodyHitbox[0])
			m_Hitboxes.emplace_back(Hitboxes_t::PELVIS, Mode);

		if (m_BodyHitbox[1])
			m_Hitboxes.emplace_back(Hitboxes_t::BODY, Mode);

		if (m_BodyHitbox[2])
			m_Hitboxes.emplace_back(Hitboxes_t::THORAX, Mode);

		if (m_BodyHitbox[3])
			m_Hitboxes.emplace_back(Hitboxes_t::CHEST, Mode);

		if (m_BodyHitbox[4])
			m_Hitboxes.emplace_back(Hitboxes_t::UPPER_CHEST, Mode);
	};

	auto bForceBodyAim = m_Config.m_ForceBodyAimKey
		|| m_Config.m_ForceBodyAim[0] && !(pPlayer->m_fFlags() & FL_ONGROUND)
		|| m_Config.m_ForceBodyAim[1] && pData->m_MissedShots >= m_Config.m_ForceBodyAfterXMisses;

	auto bPreferBodyAim = m_Config.m_PreferBodyAim[2] && !(pPlayer->m_fFlags() & FL_ONGROUND)
		|| m_Config.m_PreferBodyAim[3] && pData->m_MissedShots >= m_Config.m_BodyAfterXMisses
		|| m_Config.m_PreferBodyAim[4] && pPlayer->m_iHealth() <= m_Config.m_BodyOnXHealth;

	auto m_BodyHitbox = bHistory ? m_Config.m_BodyAimHistoryHitbox : m_Config.m_BodyAimHitbox;

	if (bForceBodyAim)
	{
		SetUpBodyAim(m_BodyHitbox, HitscanMode::NORMAL);
		return;
	}

	if (bPreferBodyAim)
		SetUpBodyAim(m_BodyHitbox, HitscanMode::PREFER);

	else if (m_Config.m_PreferBodyAim[1])
		SetUpBodyAim(m_BodyHitbox, HitscanMode::LETHAL2);

	else if (m_Config.m_PreferBodyAim[0])
		SetUpBodyAim(m_BodyHitbox, HitscanMode::LETHAL);
	
	auto m_Hitbox = m_Config.m_Hitbox;
	if (m_Hitbox.empty()) 
	{
		m_Hitboxes.emplace_back(Hitboxes_t::HEAD, HitscanMode::NORMAL);
		return;
	}

	for (int i{ static_cast<int>(Hitboxes_t::HEAD) }; i < static_cast<int>(Hitboxes_t::MAX); i++)
	{
		const Hitboxes_t Hitboxes = static_cast<Hitboxes_t>(i);
		if (Hitboxes == Hitboxes_t::LOWER_NECK && m_Config.m_OptimizationLevel >= OPTIMIZATION::LOW)
			continue;

		const int nGroup = game::HitboxToHitgroup(Hitboxes);
		if (nGroup == static_cast<int>(Hitgroups_t::GENERIC))
			continue;

		const int m_Damage = GetMinimumDamage(pPlayer, nGroup);
		if (m_Damage >= Penetration::scale(pPlayer, g_Client.m_WeaponInfo->m_Damage, g_Client.m_WeaponInfo->m_ArmorRatio, nGroup) && m_Config.m_Optimization[2])
			continue;

		if (!m_Hitbox[0] && nGroup == static_cast<int>(Hitgroups_t::HEAD) ||
			!m_Hitbox[3] && nGroup == static_cast<int>(Hitgroups_t::STOMACH) ||
			!m_Hitbox[1] && nGroup == static_cast<int>(Hitgroups_t::NECK) ||
			!m_Hitbox[2] && nGroup == static_cast<int>(Hitgroups_t::CHEST) ||
			!m_Hitbox[4] && (nGroup == static_cast<int>(Hitgroups_t::LEFTARM) || nGroup == static_cast<int>(Hitgroups_t::RIGHTARM)) ||
			!m_Hitbox[5] && (nGroup == static_cast<int>(Hitgroups_t::LEFTLEG) || nGroup == static_cast<int>(Hitgroups_t::RIGHTLEG)) ||
			!m_Hitbox[6] && (nGroup == static_cast<int>(Hitgroups_t::LEFTFOOT) || nGroup == static_cast<int>(Hitgroups_t::RIGHTFOOT)))
			continue;

		m_Hitboxes.emplace_back(Hitboxes, HitscanMode::NORMAL);
	}
}

bool Aimbot::CanHit(Vector start, Vector end, Player* pPlayer, Hitboxes_t box, LagRecord* pRecord)
{
	const model_t* pModel = pPlayer->GetClientRenderable()->GetModel();
	if (!pModel)
		return false;

	studiohdr_t* pStudioModel = g_Csgo.m_ModelInfo->GetStudioModel(pModel);
	if (!pStudioModel)
		return false;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->pHitboxSet(pPlayer->m_nHitboxSet());
	if (!pHitboxSet)
		return false;

	mstudiobbox_t* pHitbox = pHitboxSet->Hitbox(box);
	if (!pHitbox)
		return false;

	auto pCollideable = pPlayer->GetCollideable();
	if (!pCollideable)
		return false;

	CBoneCache* pBoneCache = &pPlayer->m_BoneCache();
	if (!pBoneCache)
		return false;

	if (!pRecord)
		return false;

	Backup_t pBackup;
	pBackup.m_AbsOrigin = pPlayer->GetAbsOrigin();
	pBackup.m_AbsAngles = pPlayer->GetAbsAngles();
	pBackup.m_OBBMaxs = pCollideable->OBBMaxs();
	pBackup.m_OBBMins = pCollideable->OBBMins();
	pBackup.m_CachedBones = pBoneCache->m_pCachedBones;
	pBackup.m_CachedBoneCount = pBoneCache->m_CachedBoneCount;

	// WTF IS THIS FOR, WE JUST NEED THE BONES!
	pPlayer->SetAbsOrigin(pRecord->m_Origin);
	pPlayer->SetAbsAngles(pRecord->m_AbsAngles);
	pCollideable->OBBMins() = pRecord->m_OBBMins;
	pCollideable->OBBMaxs() = pRecord->m_OBBMaxs;
	pBoneCache->m_CachedBoneCount = pRecord->m_CachedBoneCount;
	pBoneCache->m_pCachedBones = pRecord->m_CachedBones;

	matrix3x4a_t* pCachedBones = pBoneCache->m_pCachedBones;

	pPlayer->SetAbsOrigin(pBackup.m_AbsOrigin);
	pPlayer->SetAbsAngles(pBackup.m_AbsAngles);
	pCollideable->OBBMins() = pBackup.m_OBBMins;
	pCollideable->OBBMaxs() = pBackup.m_OBBMaxs;
	pBoneCache->m_CachedBoneCount = pBackup.m_CachedBoneCount;
	pBoneCache->m_pCachedBones = pBackup.m_CachedBones;

	if (!pCachedBones)
		return false;

	Vector min, max;
	if (pHitbox->flCapsuleRadius != -1.f) {
		Math::VectorTransform(pHitbox->vecMin, pCachedBones[pHitbox->bone], min);
		Math::VectorTransform(pHitbox->vecMax, pCachedBones[pHitbox->bone], max);

		if (Math::SegmentToSegment(start, end, min, max) < pHitbox->flCapsuleRadius)
			return true;
	}
	else {
		Math::VectorTransform(Math::VectorRotate(pHitbox->vecMin, pHitbox->angOffsetOrientation), pCachedBones[pHitbox->bone], min);
		Math::VectorTransform(Math::VectorRotate(pHitbox->vecMax, pHitbox->angOffsetOrientation), pCachedBones[pHitbox->bone], max);

		Math::VectorITransform(start, pCachedBones[pHitbox->bone], min);
		Math::VectorITransform(end, pCachedBones[pHitbox->bone], max);

		if (Math::IntersectLineWithBB(min, max, pHitbox->vecMin, pHitbox->vecMax))
			return true;
	}

	return false;
}

int Aimbot::GetMinimumDamage(Player* pPlayer, int nGroup)
{
	if (!g_Client.m_WeaponInfo)
		return -1;

	if (nGroup == static_cast<int>(Hitgroups_t::GENERIC))
		return -1;

	int iConfigDamage = m_Config.m_MinimumDamage;

	if (m_Config.m_HitgroupDamage)
	{
		if (m_Config.m_HitgroupMinimumDamage[0] && nGroup == static_cast<int>(Hitgroups_t::HEAD))
			iConfigDamage = m_Config.m_HeadMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[1] && nGroup == static_cast<int>(Hitgroups_t::NECK))
			iConfigDamage = m_Config.m_NeckMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[2] && nGroup == static_cast<int>(Hitgroups_t::CHEST))
			iConfigDamage = m_Config.m_ChestMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[3] && nGroup == static_cast<int>(Hitgroups_t::STOMACH))
			iConfigDamage = m_Config.m_StomachMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[4] && (nGroup == static_cast<int>(Hitgroups_t::LEFTARM) || nGroup == static_cast<int>(Hitgroups_t::RIGHTARM)))
			iConfigDamage = m_Config.m_ArmsMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[5] && (nGroup == static_cast<int>(Hitgroups_t::LEFTLEG) || nGroup == static_cast<int>(Hitgroups_t::RIGHTLEG)))
			iConfigDamage = m_Config.m_LegsMinimumDamage;

		else if (m_Config.m_HitgroupMinimumDamage[6] && (nGroup == static_cast<int>(Hitgroups_t::LEFTFOOT) || nGroup == static_cast<int>(Hitgroups_t::RIGHTFOOT)))
			iConfigDamage = m_Config.m_FeetMinimumDamage;
	}

	int iMinimumDamage = iConfigDamage;
	if (iMinimumDamage < 1) {
		float flCycle = std::max(1.f, (g_Client.m_WeaponInfo->m_cycletime * std::max(10, pPlayer->m_iHealth() + 1)) * 1.25f);
		iMinimumDamage = flCycle;

		int iCalcDamage = 1;

		if (g_Client.m_WeaponItemDefinition != AWP || flCycle <= 105)
			iCalcDamage = flCycle;
		else {
			iMinimumDamage = 105;
			iCalcDamage = 105;
		}

		if (g_Client.m_WeaponItemDefinition == SSG08 && iCalcDamage > 70) {
			iMinimumDamage = 70;
			iCalcDamage = 70;
		}

		int iHealthAbove = iMinimumDamage + 5;
		if (iCalcDamage > iHealthAbove) {
			iMinimumDamage = iHealthAbove;
			iCalcDamage = iHealthAbove;
		}

		if (iCalcDamage >= (g_Client.m_WeaponInfo->m_Damage - 5))
			iCalcDamage = g_Client.m_WeaponInfo->m_Damage - 5;

		iMinimumDamage = iCalcDamage;
	}

	if (m_Config.m_OverrideDamage)
		iMinimumDamage = m_Config.m_OverrideDamageAmount;

	if (m_Config.m_ScaleDamage)
	{
		const int iHealth = pPlayer->m_iHealth() + 1/*m_Config.m_AddScaleDamage*/;

		if (iMinimumDamage > 100)
			iMinimumDamage = iHealth + (iMinimumDamage - 100);
		else iMinimumDamage = std::min(iMinimumDamage, iHealth);
	}

	return iMinimumDamage;
}

int Aimbot::GetHitChance()
{
	return m_Config.m_OverrideHitChance ? m_Config.m_OverrideHitChanceAmount : m_Config.m_HitChanceAmount;
}

bool Aimbot::IsPointAccurate(AimData pData)
{
	int iHitChanceAmount = GetHitChance();
	if (!iHitChanceAmount || g_Csgo.weapon_accuracy_nospread->GetBool())
		return true;

	const Vector vecShootPosition = g_Client.m_EyePosition;
	QAngle angViewAngles = Math::CalcAngle(vecShootPosition, pData.m_AimPositionData.m_MultiPoints);
	Math::Normalize(angViewAngles);

	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors(angViewAngles, &vecForward, &vecRight, &vecUp);

	if (vecShootPosition.Delta(pData.m_AimPositionData.m_MultiPoints) > g_Client.m_WeaponInfo->m_WeaponRange)
		return false;

	int iHitchanceValidSeeds = 0;

	constexpr float flMaxChance = 100.f;
	constexpr auto iMaxSeed = 255u;

	const float flAccuracy{ (g_Client.m_Weapon->GetInaccuracy() + g_Client.m_Weapon->GetSpread()) * 500.f };
	const float flLowestAccuracy = m_AccuracyData.m_LowestAccuracy + m_AccuracyData.m_AddAccuracy;
	const bool bHighestAccuracy = m_AccuracyData.m_LowestAccuracy > 0.f && flAccuracy <= flLowestAccuracy;

	if (m_Config.m_HighestAccuracyCheck && bHighestAccuracy)
		return true;

	for (auto iSeed = 0u; iSeed <= iMaxSeed; iSeed++) {
		g_Csgo.RandomSeed(iSeed);

		m_AccuracyData.m_aInaccuracyMultiValues[iSeed] = g_Csgo.RandomFloat(0.f, 1.f);
		m_AccuracyData.m_aInaccuracy[iSeed] = g_Csgo.RandomFloat(0.f, Math::pi_2);

		if (g_Client.m_WeaponType == WEAPONTYPE_SHOTGUN && g_Csgo.weapon_accuracy_shotgun_spread_patterns->GetInt() > 0)
			g_Csgo.GetShotgunSpread(g_Client.m_WeaponItemDefinition, 0, g_Client.m_WeaponInfo->m_bullets * g_Client.m_Weapon->m_flRecoilIndex(), &m_AccuracyData.m_aSpread[iSeed], &m_AccuracyData.m_aSpreadMultiValues[iSeed]);

		else {
			m_AccuracyData.m_aSpreadMultiValues[iSeed] = g_Csgo.RandomFloat(0.f, 1.f);
			m_AccuracyData.m_aSpread[iSeed] = g_Csgo.RandomFloat(0.f, Math::pi_2);
		}

		if (g_Client.m_WeaponItemDefinition == REVOLVER && g_Client.m_Command->m_Buttons & IN_ATTACK2) {
			m_AccuracyData.m_aInaccuracyMultiValues[iSeed] = 1.f - (m_AccuracyData.m_aInaccuracyMultiValues[iSeed] * m_AccuracyData.m_aInaccuracyMultiValues[iSeed]);
			m_AccuracyData.m_aSpreadMultiValues[iSeed] = 1.f - (m_AccuracyData.m_aSpreadMultiValues[iSeed] * m_AccuracyData.m_aSpreadMultiValues[iSeed]);
		}

		else if (g_Client.m_WeaponItemDefinition == NEGEV && g_Client.m_Weapon->m_flRecoilIndex() < 3.f) {
			for (int i{ 3 }; i > g_Client.m_Weapon->m_flRecoilIndex(); --i) {
				m_AccuracyData.m_aInaccuracyMultiValues[iSeed] *= m_AccuracyData.m_aInaccuracyMultiValues[iSeed];
				m_AccuracyData.m_aSpreadMultiValues[iSeed] *= m_AccuracyData.m_aSpreadMultiValues[iSeed];
			}

			m_AccuracyData.m_aInaccuracyMultiValues[iSeed] = 1.f - m_AccuracyData.m_aInaccuracyMultiValues[iSeed];
			m_AccuracyData.m_aSpreadMultiValues[iSeed] = 1.f - m_AccuracyData.m_aSpreadMultiValues[iSeed];
		}

		const Vector vecSpreadView
		(
			(cos(m_AccuracyData.m_aInaccuracy[iSeed]) * m_AccuracyData.m_aInaccuracyMultiValues[iSeed] * g_Client.m_Weapon->GetInaccuracy()) + (cos(m_AccuracyData.m_aSpread[iSeed]) * m_AccuracyData.m_aSpreadMultiValues[iSeed] * g_Client.m_Weapon->GetSpread()),
			(sin(m_AccuracyData.m_aInaccuracy[iSeed]) * m_AccuracyData.m_aInaccuracyMultiValues[iSeed] * g_Client.m_Weapon->GetInaccuracy()) + (sin(m_AccuracyData.m_aSpread[iSeed]) * m_AccuracyData.m_aSpreadMultiValues[iSeed] * g_Client.m_Weapon->GetSpread()),
			0.0f
		);

		Vector vecDirection = Vector(0.0f, 0.0f, 0.0f);
		vecDirection.x = vecForward.x + (vecSpreadView.x * vecRight.x) + (vecSpreadView.y * vecUp.x);
		vecDirection.y = vecForward.y + (vecSpreadView.x * vecRight.y) + (vecSpreadView.y * vecUp.y);
		vecDirection.z = vecForward.z + (vecSpreadView.x * vecRight.z) + (vecSpreadView.y * vecUp.z);
		vecDirection.Normalized();

		QAngle angSpreadView;
		Math::VectorAngles(vecDirection, angSpreadView, &vecUp);
		Math::Normalize(angSpreadView);

		Vector vecForwardView;
		Math::AngleVectors(angSpreadView, &vecForwardView);
		vecForwardView.NormalizeInPlace();

		vecForwardView = vecShootPosition + vecForwardView * g_Client.m_WeaponInfo->m_WeaponRange;

		if (CanHit(vecShootPosition, vecForwardView, pData.m_Player, pData.m_Hitbox, pData.m_Record))
			++iHitchanceValidSeeds;

		if (iHitchanceValidSeeds >= iMaxSeed * (iHitChanceAmount / flMaxChance))
			return true;

		if ((iMaxSeed - iSeed + iHitchanceValidSeeds) < iMaxSeed * (iHitChanceAmount / flMaxChance))
			return false;
	}

	return false;
}

void Aimbot::CalculateMaxAccuracy()
{
	if (g_Client.m_LocalPlayer == nullptr || g_Client.m_Weapon == nullptr || g_Client.m_WeaponInfo == nullptr || g_Client.m_WeaponType == WEAPONTYPE_UNKNOWN || g_Client.m_WeaponType == WEAPONTYPE_GRENADE || g_Client.m_WeaponType == WEAPONTYPE_C4 || g_Client.m_WeaponItemDefinition == UNKOWN)
		return;

	g_Client.m_MaxPlayerSpeed = g_Client.m_LocalPlayer->m_bIsScoped() ? g_Client.m_WeaponInfo->m_MaxPlayerSpeedAlt : g_Client.m_WeaponInfo->m_MaxPlayerSpeed;
	g_Client.m_AccurateSpeed = g_Client.m_MaxPlayerSpeed * 0.34f;

	if (g_Client.m_LocalPlayer->m_bDucking()) {
		if (g_Client.m_LocalPlayer->m_bIsScoped())
			m_AccuracyData.m_flMinInaccuracy = g_Client.m_WeaponInfo->m_inaccuracy_crouch_alt;
		else m_AccuracyData.m_flMinInaccuracy = g_Client.m_WeaponInfo->m_inaccuracy_crouch;
	}
	else {
		if (g_Client.m_LocalPlayer->m_bIsScoped())
			m_AccuracyData.m_flMinInaccuracy = g_Client.m_WeaponInfo->m_inaccuracy_stand_alt;
		else m_AccuracyData.m_flMinInaccuracy = g_Client.m_WeaponInfo->m_inaccuracy_stand;
	}

	m_AccuracyData.m_AddAccuracy = 0.f;

	if (g_Aimbot.IsMaxAccuracy()) {
		if (!m_AccuracyData.bCalculateAccuracy)
			m_AccuracyData.m_LowestAccuracy = (g_Client.m_Weapon->GetInaccuracy() + g_Client.m_Weapon->GetSpread() * 500.f);
		m_AccuracyData.bCalculateAccuracy = true;
	}
	else m_AccuracyData.bCalculateAccuracy = false;

	if (g_Client.m_LocalPlayer->m_vecVelocity().Length2D() > g_Client.m_AccurateSpeed + 5.f)
		m_AccuracyData.m_HasFired = false;

	if (m_AccuracyData.m_HasFired) {
		if (g_Client.m_LocalPlayer->m_bIsScoped())
			m_AccuracyData.m_AddAccuracy = fabsf(5.f * m_Config.m_SlideOnStop) / 100.f;
		else m_AccuracyData.m_AddAccuracy = fabsf(10.f * m_Config.m_SlideOnStop) / 100.f;
	}
	else m_AccuracyData.m_AddAccuracy = fabsf(1.f * m_Config.m_SlideOnStop) / 100.f;
}

bool Aimbot::IsMaxAccuracy()
{
	if (g_Client.m_LocalPlayer == nullptr || g_Client.m_Weapon == nullptr || g_Client.m_WeaponInfo == nullptr || g_Client.m_WeaponType == WEAPONTYPE_UNKNOWN || g_Client.m_WeaponType == WEAPONTYPE_GRENADE || g_Client.m_WeaponType == WEAPONTYPE_C4 || g_Client.m_WeaponItemDefinition == UNKOWN)
		return false;

	if (g_Client.m_PredictionError || g_Client.m_CanScope && !g_Client.m_LocalPlayer->m_bIsScoped())
		return false;

	if (g_Client.m_LocalPlayer->m_vecVelocity().Length2D() > g_Client.m_AccurateSpeed)
		return false;

	return g_Client.m_Weapon->GetInaccuracy() - m_AccuracyData.m_flMinInaccuracy <= 0.0001f;
}

bool Aimbot::CanAutoStop()
{
	if (g_Client.m_LocalPlayer == nullptr || g_Client.m_Weapon == nullptr || g_Client.m_WeaponInfo == nullptr || g_Client.m_WeaponType == WEAPONTYPE_UNKNOWN || g_Client.m_WeaponType == WEAPONTYPE_GRENADE || g_Client.m_WeaponType == WEAPONTYPE_C4 || g_Client.m_WeaponItemDefinition == UNKOWN)
		return false;

	if (!(g_Client.m_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return false;

	if (g_Client.m_LocalPlayer->m_vecVelocity().Length2D() <= 0.f)
		return false;

	return m_Config.m_AutoStop && g_Aimbot.m_AutoStopMovement;
}

void Aimbot::AutomaticStop()
{
	if (!m_Config.m_AutoStopFlags[AUTOSTOP::AUTOSTOP_FORCE_ACCURACY])
	{
		if (g_Client.m_LocalPlayer->m_vecVelocity().Length2D() <= g_Client.m_AccurateSpeed)
			return g_Movement.SlowWalk();
	}

	return g_Movement.StopMovement();
}

bool Aimbot::Enabled() {
	if (g_Client.m_LocalPlayer == nullptr || g_Client.m_Weapon == nullptr || g_Client.m_WeaponInfo == nullptr || g_Client.m_WeaponType == WEAPONTYPE_UNKNOWN || g_Client.m_WeaponType == WEAPONTYPE_GRENADE || g_Client.m_WeaponType == WEAPONTYPE_C4 || g_Client.m_WeaponItemDefinition == UNKOWN)
		return false;

	m_Config.m_Enabled = g_Config.b(g_Items.m_Aimbot.m_General.m_Enabled);
	if (!m_Config.m_Enabled) {
		m_Config.m_Enabled					= false;
		m_Config.m_Silent					= false;
		m_Config.m_AutoFire					= false;
		m_Config.m_AutoWall					= false;
		m_Config.m_AutoScope				= false;
		m_Config.m_AutoStop					= false;
		m_Config.m_ForceBodyAimKey			= false;
		m_Config.m_ScaleDamage				= false;
		m_Config.m_PredictiveDamage			= false;
		m_Config.m_WaitForResolved			= false;
		m_Config.m_TickbaseManipulation		= false;

		m_Config.m_MultipointHeadScale		= 0.f;
		m_Config.m_MultipointBodyScale		= 0.f;
		m_Config.m_HitChanceAmount			= 0;
		m_Config.m_MinimumDamage			= 1;
		m_Config.m_OverrideDamageAmount		= 1;
		m_Config.m_MaximumFOV				= 0;
		m_Config.m_DetermineFramePerSecond	= 0;
		m_Config.m_DetermineFramePerSecond	= 0;

		m_Config.m_Hitbox.clear();
		m_Config.m_Multipoint.clear();
		m_Config.m_PreferBodyAim.clear();
		m_Config.m_AutoStopFlags.clear();
		m_Config.m_AutoStopConditions.clear();
		return false;
	}

	m_Config.m_TickbaseManipulation		= g_Config.b(g_Items.m_Aimbot.m_General.m_TickbaseManipulation) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_TickbaseManipulationKey, g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode);
	m_Config.m_WaitForResolved			= g_Config.b(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdate) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateKey, g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode);
	
	m_Config.m_OverrideHitChance		= g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideHitChance) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideHitChanceKey, g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode);
	m_Config.m_OverrideDamage			= g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideDamage) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideDamageKey, g_Items.m_Aimbot.m_General.m_OverrideDamageMode);
	m_Config.m_ForceBodyAimKey			= g_Config.b(g_Items.m_Aimbot.m_General.m_ForceBody) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_ForceBodyKey, g_Items.m_Aimbot.m_General.m_ForceBodyMode);

	m_Config.m_OptimizationLevel		= g_Config.i(g_Items.m_Aimbot.m_General.m_OptimizationLevel);
	m_Config.m_DetermineFramePerSecond	= g_Config.i(g_Items.m_Aimbot.m_General.m_DetermineFPS);

	const auto defaultSet = [&]() -> bool
	{
		m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_General.m_Enabled);
		m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_General.m_Silent);
		m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_General.m_AutoFire);
		m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_General.m_AutoWall);
		m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_General.m_AutoScope);
		m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_General.m_AutoStop);
		m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_General.m_ScaleDamage);
		m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_General.m_PredictiveDamage);

		m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_General.m_MultipointHeadScale);
		m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_General.m_MultipointBodyScale);
		m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_General.m_HitChanceAmount);
		m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_General.m_MinimumDamage);
		m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideHitChanceAmount);
		m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideDamageAmount);
		m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_General.m_MaximumFOV);

		m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_General.m_Hitbox);
		m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_General.m_Multipoint);
		m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_General.m_PreferBodyAim);
		m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_General.m_AutoStopFlags);
		m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_General.m_AutoStopConditions);
		return true;
	};

	if (g_Client.m_WeaponItemDefinition == ZEUS)
	{
		m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_Taser.m_Enabled);
		m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_Taser.m_Silent);
		m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_Taser.m_AutoFire);
		m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_Taser.m_AutoWall);
		m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_Taser.m_AutoScope);
		m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_Taser.m_AutoStop);
		m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_Taser.m_ScaleDamage);
		m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_Taser.m_PredictiveDamage);

		m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_Taser.m_MultipointHeadScale);
		m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_Taser.m_MultipointBodyScale);
		m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_Taser.m_HitChanceAmount);
		m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_Taser.m_MinimumDamage);
		m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_Taser.m_OverrideHitChanceAmount);
		m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_Taser.m_OverrideDamageAmount);
		m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_Taser.m_MaximumFOV);

		m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_Taser.m_Hitbox);
		m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_Taser.m_Multipoint);
		m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_Taser.m_PreferBodyAim);
		m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_Taser.m_AutoStopFlags);
		m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_Taser.m_AutoStopConditions);
	}
	else {
		switch (g_Client.m_WeaponType) {
		case WEAPONTYPE_KNIFE:
			m_Config.m_Enabled					= true;
			m_Config.m_Silent					= true;
			m_Config.m_AutoFire					= true;
			m_Config.m_AutoWall					= false;
			m_Config.m_AutoScope				= false;
			m_Config.m_AutoStop					= false;
			m_Config.m_ScaleDamage				= false;
			m_Config.m_PredictiveDamage			= false;

			m_Config.m_MultipointHeadScale		= 90.f;
			m_Config.m_MultipointBodyScale		= 90.f;
			m_Config.m_HitChanceAmount			= 0;
			m_Config.m_MinimumDamage			= 1;
			m_Config.m_OverrideHitChanceAmount	= 0;
			m_Config.m_OverrideDamageAmount		= 1;
			m_Config.m_MaximumFOV				= 360;

			m_Config.m_Hitbox.clear();
			m_Config.m_Multipoint = g_Config.vb(g_Items.m_Aimbot.m_General.m_Multipoint);
			m_Config.m_PreferBodyAim.clear();
			m_Config.m_AutoStopFlags.clear();
			m_Config.m_AutoStopConditions.clear();
			break;
		case WEAPONTYPE_PISTOL:
			if (g_Client.m_WeaponItemDefinition == DEAGLE || g_Client.m_WeaponItemDefinition == REVOLVER)
			{
				m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_Enabled);
				m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_Silent);
				m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_AutoFire);
				m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_AutoWall);
				m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_AutoScope);
				m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_AutoStop);
				m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_ScaleDamage);
				m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_HeavyPistol.m_PredictiveDamage);

				m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_MultipointHeadScale);
				m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_MultipointBodyScale);
				m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_HitChanceAmount);
				m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_MinimumDamage);
				m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_OverrideHitChanceAmount);
				m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_OverrideDamageAmount);
				m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_HeavyPistol.m_MaximumFOV);

				m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_HeavyPistol.m_Hitbox);
				m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_HeavyPistol.m_Multipoint);
				m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_HeavyPistol.m_PreferBodyAim);
				m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_HeavyPistol.m_AutoStopFlags);
				m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_HeavyPistol.m_AutoStopConditions);
			}
			else
			{
				m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_Enabled);
				m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_Silent);
				m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_AutoFire);
				m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_AutoWall);
				m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_AutoScope);
				m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_AutoStop);
				m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_ScaleDamage);
				m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_Pistol.m_PredictiveDamage);

				m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_MultipointHeadScale);
				m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_MultipointBodyScale);
				m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_HitChanceAmount);
				m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_MinimumDamage);
				m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_OverrideHitChanceAmount);
				m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_OverrideDamageAmount);
				m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_Pistol.m_MaximumFOV);

				m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_Pistol.m_Hitbox);
				m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_Pistol.m_Multipoint);
				m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_Pistol.m_PreferBodyAim);
				m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_Pistol.m_AutoStopFlags);
				m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_Pistol.m_AutoStopConditions);
			}
			break;
		case WEAPONTYPE_SNIPER_RIFLE:
			if (g_Client.m_Weapon->IsAutoSniper())
			{
				m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_Auto.m_Enabled);
				m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_Auto.m_Silent);
				m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_Auto.m_AutoFire);
				m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_Auto.m_AutoWall);
				m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_Auto.m_AutoScope);
				m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_Auto.m_AutoStop);
				m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_Auto.m_ScaleDamage);
				m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_Auto.m_PredictiveDamage);

				m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_Auto.m_MultipointHeadScale);
				m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_Auto.m_MultipointBodyScale);
				m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_Auto.m_HitChanceAmount);
				m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_Auto.m_MinimumDamage);
				m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_Auto.m_OverrideHitChanceAmount);
				m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_Auto.m_OverrideDamageAmount);
				m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_Auto.m_MaximumFOV);

				m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_Auto.m_Hitbox);
				m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_Auto.m_Multipoint);
				m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_Auto.m_PreferBodyAim);
				m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_Auto.m_AutoStopFlags);
				m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_Auto.m_AutoStopConditions);
			}
			else if (g_Client.m_WeaponItemDefinition == AWP)
			{
				m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_AWP.m_Enabled);
				m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_AWP.m_Silent);
				m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_AWP.m_AutoFire);
				m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_AWP.m_AutoWall);
				m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_AWP.m_AutoScope);
				m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_AWP.m_AutoStop);
				m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_AWP.m_ScaleDamage);
				m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_AWP.m_PredictiveDamage);

				m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_AWP.m_MultipointHeadScale);
				m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_AWP.m_MultipointBodyScale);
				m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_AWP.m_HitChanceAmount);
				m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_AWP.m_MinimumDamage);
				m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_AWP.m_OverrideHitChanceAmount);
				m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_AWP.m_OverrideDamageAmount);
				m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_AWP.m_MaximumFOV);

				m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_AWP.m_Hitbox);
				m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_AWP.m_Multipoint);
				m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_AWP.m_PreferBodyAim);
				m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_AWP.m_AutoStopFlags);
				m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_AWP.m_AutoStopConditions);
			}
			else if (g_Client.m_WeaponItemDefinition == SSG08) 
			{
				m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_Scout.m_Enabled);
				m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_Scout.m_Silent);
				m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_Scout.m_AutoFire);
				m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_Scout.m_AutoWall);
				m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_Scout.m_AutoScope);
				m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_Scout.m_AutoStop);
				m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_Auto.m_ScaleDamage);
				m_Config.m_PredictiveDamage			= g_Config.b(g_Items.m_Aimbot.m_Auto.m_PredictiveDamage);

				m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_Scout.m_MultipointHeadScale);
				m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_Scout.m_MultipointBodyScale);
				m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_Scout.m_HitChanceAmount);
				m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_Scout.m_MinimumDamage);
				m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_Scout.m_OverrideHitChanceAmount);
				m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_Scout.m_OverrideDamageAmount);
				m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_Scout.m_MaximumFOV);

				m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_Scout.m_Hitbox);
				m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_Scout.m_Multipoint);
				m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_Scout.m_PreferBodyAim);
				m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_Scout.m_AutoStopFlags);
				m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_Scout.m_AutoStopConditions);
			}
			break;
		case WEAPONTYPE_RIFLE:
		case WEAPONTYPE_MACHINEGUN:
		case WEAPONTYPE_SUBMACHINEGUN:
		case WEAPONTYPE_SHOTGUN:
			m_Config.m_Enabled					= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_Enabled);
			m_Config.m_Silent					= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_Silent);
			m_Config.m_AutoFire					= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_AutoFire);
			m_Config.m_AutoWall					= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_AutoWall);
			m_Config.m_AutoScope				= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_AutoScope);
			m_Config.m_AutoStop					= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_AutoStop);
			m_Config.m_ScaleDamage				= g_Config.b(g_Items.m_Aimbot.m_Rifle.m_ScaleDamage);

			m_Config.m_MultipointHeadScale		= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_MultipointHeadScale);
			m_Config.m_MultipointBodyScale		= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_MultipointBodyScale);
			m_Config.m_HitChanceAmount			= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_HitChanceAmount);
			m_Config.m_MinimumDamage			= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_MinimumDamage);
			m_Config.m_OverrideHitChanceAmount	= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_OverrideHitChanceAmount);
			m_Config.m_OverrideDamageAmount		= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_OverrideDamageAmount);
			m_Config.m_MaximumFOV				= g_Config.i(g_Items.m_Aimbot.m_Rifle.m_MaximumFOV);

			m_Config.m_Hitbox					= g_Config.vb(g_Items.m_Aimbot.m_Rifle.m_Hitbox);
			m_Config.m_Multipoint				= g_Config.vb(g_Items.m_Aimbot.m_Rifle.m_Multipoint);
			m_Config.m_PreferBodyAim			= g_Config.vb(g_Items.m_Aimbot.m_Rifle.m_PreferBodyAim);
			m_Config.m_AutoStopFlags			= g_Config.vb(g_Items.m_Aimbot.m_Rifle.m_AutoStopFlags);
			m_Config.m_AutoStopConditions		= g_Config.vb(g_Items.m_Aimbot.m_Rifle.m_AutoStopConditions);
			break;
		}
	}

	if (!m_Config.m_Enabled)
		return defaultSet();

	return m_Config.m_Enabled && m_Config.m_MaximumFOV > 0;
}