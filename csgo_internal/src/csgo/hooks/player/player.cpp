#include "../../../precompiled.h"

Weapon* Hooks::GetActiveWeapon() {
	Stack stack;

	static Address ApplyIronSightScopeEffect = pattern::find(g_Csgo.m_client_dll, XOR("85 C0 74 1D 8B 88 ? ? ? ? 85 C9"));

	if (stack.ReturnAddress() == ApplyIronSightScopeEffect && g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SCOPE])
		return nullptr;

	return g_Hooks.m_GetActiveWeapon(this);
}

void Hooks::UpdateClientSideAnimation() 
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_UpdateClientSideAnimation(this);

	if (!g_Client.m_ClientAnimationUpdate)
		return;

	return g_Hooks.m_UpdateClientSideAnimation(this);
}

void Hooks::DoExtraBoneProcessing(int a2, int a3, int a4, int a5, int a6, int a7) {
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive() || !pPlayer->m_PlayerAnimStateCSGO())
		return g_Hooks.m_DoExtraBoneProcessing(this, a2, a3, a4, a5, a6, a7);

	bool bOnGround = pPlayer->m_PlayerAnimStateCSGO()->bOnGround;
	pPlayer->m_PlayerAnimStateCSGO()->bOnGround = false;

	if (g_Hooks.m_DoExtraBoneProcessing)
		g_Hooks.m_DoExtraBoneProcessing(this, a2, a3, a4, a5, a6, a7);

	pPlayer->m_PlayerAnimStateCSGO()->bOnGround = bOnGround;
}

void Hooks::StandardBlendingRules(int a2, int a3, int a4, int a5, int iBoneMask)
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_StandardBlendingRules(this, a2, a3, a4, a5, iBoneMask);

	auto fBackupEffects = pPlayer->m_fEffects();
	pPlayer->AddEffect(EF_NOINTERP);

	if (g_Hooks.m_StandardBlendingRules)
		g_Hooks.m_StandardBlendingRules(this, a2, a3, a4, a5, iBoneMask);

	pPlayer->m_fEffects() = fBackupEffects;
}

void Hooks::BuildTransformations(CStudioHdr* hdr, int a3, int a4, int a5, int a6, int a7) 
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_BuildTransformations(this, hdr, a3, a4, a5, a6, a7);

	auto backupBoneFlags = hdr->m_boneFlags;
	for (auto i = 0; i < hdr->m_boneFlags.Count(); i++)
		hdr->m_boneFlags.Element(i) &= ~BONE_ALWAYS_PROCEDURAL;

	g_Hooks.m_BuildTransformations(this, hdr, a3, a4, a5, a6, a7);
	hdr->m_boneFlags = backupBoneFlags;
}

Vector& Hooks::EyePosition(Vector& vecPos)
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_EyePosition(this, vecPos);

	vecPos = pPlayer->GetAbsOrigin() + pPlayer->m_vecViewOffset();
	return vecPos;
}

// TODO@Monster: Fix Client With This Hook.
Vector& Hooks::Weapon_ShootPosition(Vector& vecPos)
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_Weapon_ShootPosition(this, vecPos);

	pPlayer->EyePosition(vecPos);

	if (!pPlayer->m_bUseNewAnimstate() || !pPlayer->m_PlayerAnimStateCSGO())
		return vecPos;
	
	pPlayer->m_PlayerAnimStateCSGO()->ModifyEyePosition(vecPos);
	return vecPos;
}

void Hooks::CalcView(Vector& eyeOrigin, Vector& eyeAngles, float& zNear, float& zFar, float& flFov) 
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive() || !pPlayer->IsLocalPlayer())
		return g_Hooks.m_CalcView(this, eyeOrigin, eyeAngles, zNear, zFar, flFov);

	// Prevent CalcView from calling CCSGOPlayerAnimState::ModifyEyePosition( ... ) 
	const bool m_bUseNewAnimstate = pPlayer->m_bUseNewAnimstate();

	// don't let the server use client version of CCSGOPlayerAnimState::ModifyEyePosition.
	pPlayer->m_bUseNewAnimstate() = false;

	// call original.
	g_Hooks.m_CalcView(this, eyeOrigin, eyeAngles, zNear, zFar, flFov);

	// set it back after the original.
	pPlayer->m_bUseNewAnimstate() = m_bUseNewAnimstate;
}

void Hooks::PhysicsSimulate() 
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive() || !pPlayer->IsLocalPlayer())
		return g_Hooks.m_PhysicsSimulate(this);

	int32_t iSimulationTick = pPlayer->m_nSimulationTick();
	if (iSimulationTick == g_Csgo.m_Globals->m_TickCount)
		return;

	if (int numUsrCmdProcessTicksMax = g_Csgo.m_MaxUserCommandProcessTicks->GetInt()) {
		g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining += g_Csgo.m_Globals->m_Interval;
		if (g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining > numUsrCmdProcessTicksMax * g_Csgo.m_Globals->m_Interval)
			g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining = numUsrCmdProcessTicksMax * g_Csgo.m_Globals->m_Interval;
	} else g_InputPrediction.m_MovementTimeForUserCmdProcessingRemaining = FLT_MAX;
	
	C_CommandContext* pCommandContext = reinterpret_cast <C_CommandContext*>(reinterpret_cast<uintptr_t>(pPlayer) + 0x34D0);
	if (!pCommandContext || !pCommandContext->m_bNeedsProcessing)
		return;

	// TODO@Fix this and make the cheat perfect, using events to rebuilt the server messages and getting the correct variable from command number.
	int m_CommandNumber = pCommandContext->m_nCommandNumber - 1;
	if (m_CommandNumber == g_Csgo.m_ClientState->m_LastCommandAck)
	{
		if (pPlayer->m_fFlags() & FL_ONGROUND)
		{
			const float kVelocityRecoveryRate = 1.f / 2.5f;
			if (g_InputPrediction.m_flVelocityModifier < 1.0f)
				g_InputPrediction.m_flVelocityModifier = std::clamp(g_InputPrediction.m_flVelocityModifier + std::min(m_CommandNumber - g_InputPrediction.m_iLastCommand, 0) * g_Csgo.m_Globals->m_Interval * kVelocityRecoveryRate, 0.0f, 1.0f);

			if (pPlayer->m_flVelocityModifier() != g_InputPrediction.m_flVelocityModifier)
				pPlayer->m_flVelocityModifier() = g_InputPrediction.m_flVelocityModifier;
		}
	}

	if (!g_Config.vb(g_Items.m_Aimbot.m_General.m_TickbaseManipulationSelection)[1])
	{
		m_CommandNumber = pCommandContext->m_nCommandNumber;
		if (m_CommandNumber == g_TickbaseManipulation.GetShiftCommand())
			pPlayer->m_nTickBase() -= g_TickbaseManipulation.GetShiftedTickbase();

		auto pWeapon = pPlayer->GetActiveWeapon();
		if (pWeapon)
		{
			if (m_CommandNumber == g_TickbaseManipulation.GetShiftCommand())
				pWeapon->m_fLastShotTime() += g_Csgo.m_Globals->m_Interval;
		}

		m_CommandNumber = pCommandContext->m_nCommandNumber - 1;
		if (m_CommandNumber == g_TickbaseManipulation.GetChargeCommand())
			pPlayer->m_nTickBase() += g_TickbaseManipulation.GetChargedTickbase();
	}

	g_Netdata.Apply(pPlayer, m_CommandNumber);
	g_Hooks.m_PhysicsSimulate(this);

	m_CommandNumber = pCommandContext->m_nCommandNumber;
	return g_Netdata.Store(pPlayer, m_CommandNumber);
}

void Hooks::AccumulateLayers(void* boneSetup, Vector pos[], Quaternion q[], float currentTime) {
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_AccumulateLayers(this, boneSetup, pos, q, currentTime);

	// backup to restore so we dont fuck animstate.
	const bool bFirstRunSinceInit = pPlayer->m_PlayerAnimStateCSGO()->bFirstRunSinceInit;

	// stop running client CCSGOPlayerAnimState::OnClientWeaponChange( ... ).
	pPlayer->m_PlayerAnimStateCSGO()->bFirstRunSinceInit = true;

	// call original we have removed CCSGOPlayerAnimState::OnClientWeaponChange( ... ).
	g_Hooks.m_AccumulateLayers(this, boneSetup, pos, q, currentTime);

	// restore so we dont fuck up animations.
	pPlayer->m_PlayerAnimStateCSGO()->bFirstRunSinceInit = bFirstRunSinceInit;
}

// Notes: No need to reset m_nLastNonSkippedFrame since we hook ShouldSkipAnimationFrame.
bool Hooks::SetupBones(matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) {
	Player* pPlayer = reinterpret_cast<Player*>(this) - 0x4;
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_SetupBones(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	
	//if (!g_Client.m_SetupBones && pPlayer->IsLocalPlayer() && pBoneToWorldOut)
	//{
	//	std::memcpy(pBoneToWorldOut, g_Client.m_Matrix, sizeof(matrix3x4_t) * pPlayer->m_BoneCache().m_CachedBoneCount);
	//	return true;
	//}

	// backup originals.
	const auto nComputedLODframe = pPlayer->m_nComputedLODframe();
	const auto nAnimLODflags = pPlayer->m_nAnimLODflags();
	const auto nCustomBlendingRuleMask = pPlayer->m_nCustomBlendingRuleMask();
	const auto EntClientFlags = pPlayer->m_EntClientFlags();
	const auto pIk = pPlayer->m_pIk();

	// attachment helper called if (!prevmask & 0x200) && (mask & 0x200)
	// seems to be required to fix right arm t-posing.
	pPlayer->m_iPrevBoneMask() = 0;
	boneMask |= BONE_USED_BY_ATTACHMENT;

	// remove lods.
	pPlayer->m_nComputedLODframe() = 0;
	pPlayer->m_nAnimLODflags() = 0;
	pPlayer->m_nCustomBlendingRuleMask() = -1;

	// clear initiks function.
	pPlayer->AddClientEffect(ENTCLIENTFLAG_DONTUSEIK);
	pPlayer->m_pIk() = nullptr;

	// no interp, forces the game to clear iks before the initiks function.
	const int m_fEffects = pPlayer->m_fEffects();
	pPlayer->AddEffect(EF_NOINTERP);
	const bool bSetuped = g_Hooks.m_SetupBones(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	pPlayer->m_fEffects() = m_fEffects;

	// restore to original.
	pPlayer->m_nComputedLODframe() = nComputedLODframe;
	pPlayer->m_nAnimLODflags() = nAnimLODflags;
	pPlayer->m_nCustomBlendingRuleMask() = nCustomBlendingRuleMask;
	pPlayer->m_EntClientFlags() = EntClientFlags;
	pPlayer->m_pIk() = pIk;

	// call changed original.
	return bSetuped;
}

bool Hooks::IsFollowingEntity()
{
	Stack stack;

	static Address Interpolation = pattern::find(g_Csgo.m_client_dll, XOR("84 C0 0F 85 ? ? ? ? 38 05 ? ? ? ? 0F 84 ? ? ? ? 53"));

	if (!g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation) && stack.ReturnAddress() == Interpolation)
		return true;

	return g_Hooks.m_IsFollowingEntity(this);
}

bool Hooks::ShouldInterpolate()
{
	Player* pPlayer = reinterpret_cast<Player*>(this);
	if (!pPlayer || !pPlayer->IsAlive())
		return g_Hooks.m_ShouldInterpolate(this);

	return g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation);
}

void CustomEntityListener::OnEntityCreated(Entity* ent) {
	if (!g_Csgo.m_Engine->IsConnected())
		return;

	Player* pPlayer = ent->as< Player* >();
	if (!pPlayer || !pPlayer->IsPlayer())
		return;

	// access out pPlayer data stucture and reset pPlayer data.
	AimPlayer* data = &g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1];
	if (data)
		data->reset();

	// get ptr to vmt instance and reset tables.
	VMT* vmt = &g_Hooks.m_Player[pPlayer->GetClientNetworkable()->GetIndex() - 1];
	if (vmt) {
		// init vtable with new ptr.
		vmt->Reset();
		vmt->Init(pPlayer);

		g_Hooks.m_UpdateClientSideAnimation = vmt->Add< Hooks::UpdateClientSideAnimation_t >(Player::UPDATECLIENTSIDEANIMATION, util::force_cast(&Hooks::UpdateClientSideAnimation));
		g_Hooks.m_DoExtraBoneProcessing = vmt->Add< Hooks::DoExtraBoneProcessing_t >(Player::DOEXTRABONEPROCESSING, util::force_cast(&Hooks::DoExtraBoneProcessing));
		g_Hooks.m_StandardBlendingRules = vmt->Add< Hooks::StandardBlendingRules_t >(Player::STANDARDBLENDINGRULES, util::force_cast(&Hooks::StandardBlendingRules));
		g_Hooks.m_BuildTransformations = vmt->Add< Hooks::BuildTransformations_t >(Player::BUILDTRANSFORMATIONS, util::force_cast(&Hooks::BuildTransformations));
		g_Hooks.m_EyePosition = vmt->Add< Hooks::EyePosition_t >(Player::GETEYEPOS, util::force_cast(&Hooks::EyePosition));
		g_Hooks.m_ShouldInterpolate = vmt->Add< Hooks::ShouldInterpolate_t >(Player::SHOULDINTERPOLATE, util::force_cast(&Hooks::ShouldInterpolate));
		g_Hooks.m_IsFollowingEntity = vmt->Add< Hooks::IsFollowingEntity_t >(Player::ISFOLLOWINGENTITY, util::force_cast(&Hooks::IsFollowingEntity));
		g_Hooks.m_AccumulateLayers = vmt->Add< Hooks::AccumulateLayers_t >(Player::ACCUMULATELAYERS, util::force_cast(&Hooks::AccumulateLayers));
		g_Hooks.m_PhysicsSimulate = vmt->Add< Hooks::PhysicsSimulate_t >(Player::PHYSICSSIMULATE, util::force_cast(&Hooks::PhysicsSimulate));
		g_Hooks.m_CalcView = vmt->Add< Hooks::CalcView_t >(Player::CALCVIEW, util::force_cast(&Hooks::CalcView));

		if (pPlayer->GetClientNetworkable()->GetIndex() == g_Csgo.m_Engine->GetLocalPlayer()) 
			g_Hooks.m_GetActiveWeapon = vmt->Add< Hooks::GetActiveWeapon_t >(Player::GETACTIVEWEAPON, util::force_cast(&Hooks::GetActiveWeapon));
	}

	VMT* renderable = &g_Hooks.m_Renderable[pPlayer->GetClientNetworkable()->GetIndex() - 1];
	if (renderable) {
		// init vtable with new ptr.
		renderable->Reset();
		renderable->Init(pPlayer->GetClientRenderable());

		// local gets special treatment.
		// hook the setup bones.
		g_Hooks.m_SetupBones = renderable->Add< Hooks::SetupBones_t >(Player::SETUPBONES, util::force_cast(&Hooks::SetupBones));
	}
}

void CustomEntityListener::OnEntityDeleted(Entity* ent) {
	Player* pPlayer = ent->as< Player* >();

	// note; IsPlayer doesn't work here because the ent class is CBaseEntity.
	if (!pPlayer || pPlayer->GetClientNetworkable()->GetIndex() < 1 || pPlayer->GetClientNetworkable()->GetIndex() > 64)
		return;

	// access out pPlayer data stucture and reset pPlayer data.
	g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1].reset();

	// get ptr to vmt instance and reset tables.
	g_Hooks.m_Player[pPlayer->GetClientNetworkable()->GetIndex() - 1].Reset();

	// get ptr to vmt instance and reset tables.
	g_Hooks.m_Renderable[pPlayer->GetClientNetworkable()->GetIndex() - 1].Reset();
}		
