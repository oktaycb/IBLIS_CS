#include "../../precompiled.h"

void Pitch_proxy( CRecvProxyData *pData, Address pPtr, Address pOut ) {
	Math::NormalizeAngle( pData->m_Value.m_Float );
	Math::clamp( pData->m_Value.m_Float, -90.f, 90.f );

	return g_Hooks.m_Pitch( pData, pPtr, pOut );
}

void Force_proxy( CRecvProxyData *pData, Address pPtr, Address pOut ) {
	Ragdoll *ragdoll = pPtr.as< Ragdoll * >( );

	Player *pPlayer = ragdoll->GetPlayer( );

	if (c_config::get()->b["ragdoll_force"] && g_Client.m_LocalPlayer && pPlayer && pPlayer->enemy(g_Client.m_LocalPlayer)) {
		Vector vel = { pData->m_Value.m_Vector[ 0 ], pData->m_Value.m_Vector[ 1 ], pData->m_Value.m_Vector[ 2 ] };

		vel *= 10000.f;

		if ( vel.z <= 1.f )
			vel.z = 2.f;

		vel.z *= 5.f;

		Math::clamp( vel.x, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
		Math::clamp( vel.y, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
		Math::clamp( vel.z, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );

		pData->m_Value.m_Vector[ 0 ] = vel.x;
		pData->m_Value.m_Vector[ 1 ] = vel.y;
		pData->m_Value.m_Vector[ 2 ] = vel.z;
	}

	return g_Hooks.m_Force( pData, pPtr, pOut );
}

void VelocityModifier_proxy(CRecvProxyData* pData, Address pPtr, Address pOut) {
	g_InputPrediction.m_flVelocityModifier = 1.f;
	if (g_Hooks.m_VelocityModifier)
		g_Hooks.m_VelocityModifier(pData, pPtr, pOut);

	Player* pPlayer = reinterpret_cast<Player*>(pPtr.as< Player* >()->GetBaseEntity());

	if (pPlayer && pPlayer->IsPlayer() && pPlayer->IsLocalPlayer())
	{
		if (g_InputPrediction.m_flVelocityModifier != pData->m_Value.m_Float)
			g_InputPrediction.m_iLastCommand = g_Csgo.m_ClientState->m_LastCommandAck;
		g_InputPrediction.m_flVelocityModifier = pData->m_Value.m_Float;
	}
}

void ClientSideAnimation_proxy(CRecvProxyData* pData, Address pPtr, Address pOut) {
	Player* pPlayer = reinterpret_cast<Player*>(pPtr.as< Player* >()->GetBaseEntity());
	if (pPlayer && pPlayer->IsPlayer())
		if (!g_Client.m_ClientAnimationUpdate /*&& !g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation)*/)
			pData->m_Value.m_Int = 0;

	return g_Hooks.m_ClientSideAnimation(pData, pPtr, pOut);
}

void ReceivedFlashDuration_proxy(CRecvProxyData* pData, Address pPtr, Address pOut) {
	Player* pPlayer = reinterpret_cast<Player*>(pPtr.as< Player* >());
	if (!pPlayer || !pPlayer->IsPlayer())
		return;

	AimPlayer* data = &g_Aimbot.m_Players[pPlayer->GetClientNetworkable()->GetIndex() - 1];
	if (data)
		data->m_AwaitingFlashedResult = true;

	return g_Hooks.m_ReceivedFlashDuration(pData, pPtr, pOut);
}

void Hooks::Init() {
	m_Device.Init(g_Csgo.m_Device);
	m_Device.Add(17, util::force_cast(&Hooks::Present));
	m_Device.Add(16, util::force_cast(&Hooks::Reset));

	m_Panel.Init(g_Csgo.m_Panel);
	m_Panel.Add(IPanel::PAINTTRAVERSE, util::force_cast(&Hooks::PaintTraverse));
#ifdef LEGACY
	m_EngineTrace.Init(g_Csgo.m_EngineTrace);
	m_EngineTrace.Add(IEngineTrace::CLIPRAYTOCOLLIDEABLE, util::force_cast(&Hooks::ClipRayToCollideable));
	m_EngineTrace.Add(IEngineTrace::TRACERAY, util::force_cast(&Hooks::TraceRay));

	m_Client.Init(g_Csgo.m_Client);
	m_Client.Add(CHLClient::LEVELINITPREENTITY, util::force_cast(&Hooks::LevelInitPreEntity));
	m_Client.Add(CHLClient::LEVELINITPOSTENTITY, util::force_cast(&Hooks::LevelInitPostEntity));
	m_Client.Add(CHLClient::LEVELSHUTDOWN, util::force_cast(&Hooks::LevelShutdown));
	m_Client.Add(CHLClient::CREATEMOVE, util::force_cast(&Hooks::CreateMove_Proxy));
	m_Client.Add(CHLClient::FRAMESTAGENOTIFY, util::force_cast(&Hooks::FrameStageNotify));
	m_Client.Add(CHLClient::WRITEUSERCMDDELTATOBUFFER, util::force_cast(&Hooks::WriteUsercmdDeltaToBuffer));
#endif
#ifdef LEGACY
	m_Engine.Init(g_Csgo.m_Engine);
	m_Engine.Add(IVEngineClient::FIREEVENTS, util::force_cast(&Hooks::FireEvents));
	m_Engine.Add(IVEngineClient::ISCONNECTED, util::force_cast(&Hooks::IsConnected));
	m_Engine.Add(IVEngineClient::ISHLTV, util::force_cast(&Hooks::IsHLTV));
	m_Engine.Add(IVEngineClient::ISPAUSED, util::force_cast(&Hooks::IsPaused));
	m_Engine.Add(IVEngineClient::ISPLAYINGTIMEDEMO, util::force_cast(&Hooks::IsPlayingTimeDemo));

	m_ClientState.Init(g_Csgo.m_HookableClientState);
	m_ClientState.Add(CClientState::PACKETSTART, util::force_cast(&Hooks::PacketStart));
	m_ClientState.Add(CClientState::PACKETEND, util::force_cast(&Hooks::PacketEnd));
	m_ClientState.Add(CClientState::TEMPENTITIES, util::force_cast(&Hooks::SVCMsg_TempEntities));

	m_Prediction.Init(g_Csgo.m_Prediction);
	m_Prediction.Add(CPrediction::INPREDICTION, util::force_cast(&Hooks::InPrediction));
	m_Prediction.Add(CPrediction::RUNCOMMAND, util::force_cast(&Hooks::RunCommand));
	m_Prediction.Add(CPrediction::SETUPMOVE, util::force_cast(&Hooks::SetupMove));
	m_Prediction.Add(CPrediction::FINISHMOVE, util::force_cast(&Hooks::FinishMove));
	m_Prediction.Add(CPrediction::UPDATE, util::force_cast(&Hooks::Update));

	m_GameMovement.Init(g_Csgo.m_GameMovement);
	m_GameMovement.Add(CGameMovement::PROCESSMOVEMENT, util::force_cast(&Hooks::ProcessMovement));
	m_GameMovement.Add(CGameMovement::ONJUMP, util::force_cast(&Hooks::OnJump));

	m_ClientMode.Init(g_Csgo.m_ClientMode);
	m_ClientMode.Add(IClientMode::SHOULDDRAWFOG, util::force_cast(&Hooks::ShouldDrawFog));
	m_ClientMode.Add(IClientMode::OVERRIDEVIEW, util::force_cast(&Hooks::OverrideView));
	m_ClientMode.Add(IClientMode::DOPOSTSPACESCREENEFFECTS, util::force_cast(&Hooks::DoPostScreenSpaceEffects));

	m_BSP.Init(g_Csgo.m_Engine->GetBSPTreeQuery());
	m_BSP.Add(ISpatialQuery::LISTLEAVESINBOX, util::force_cast(&Hooks::ListLeavesInBox));

	m_ModelRender.Init(g_Csgo.m_ModelRender);
	m_ModelRender.Add(IVModelRender::DRAWMODELEXECUTE, util::force_cast(&Hooks::DrawModelExecute));

	m_ClientShadowMgr.Init(g_Csgo.m_ClientShadowMgr);
	m_ClientShadowMgr.Add(IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES, util::force_cast(&Hooks::ComputeShadowDepthTextures));

	m_ViewRender.Init(g_Csgo.m_ViewRender);
	m_ViewRender.Add(CViewRender::ONRENDERSTART, util::force_cast(&Hooks::OnRenderStart));
	m_ViewRender.Add(CViewRender::RENDER2DEFFECTSPOSTHUD, util::force_cast(&Hooks::Render2DEffectsPostHUD));
	m_ViewRender.Add(CViewRender::RENDERSMOKEOVERLAY, util::force_cast(&Hooks::RenderSmokeOverlay));

	m_MaterialSystem.Init(g_Csgo.m_MaterialSystem);
	m_MaterialSystem.Add(IMaterialSystem::OVERRIDECONFIG, util::force_cast(&Hooks::OverrideConfig));

	m_EngineSound.Init(g_Csgo.m_EngineSound);
	m_EngineSound.Add(IEngineSound::EMITSOUND, util::force_cast(&Hooks::EmitSound));

	m_net_show_fragments.Init(g_Csgo.net_showfragments);
	m_net_show_fragments.Add(ConVar::GETINT, util::force_cast(&Hooks::NetShowFragmentsGetBool));

	m_cl_csm_shadows.Init(g_Csgo.cl_csm_shadows);
	m_cl_csm_shadows.Add(ConVar::GETINT, util::force_cast(&Hooks::CsmShadowGetInt));

	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_angEyeAngles[0]"), Pitch_proxy, m_Pitch);
	g_Netvars.SetProxy(HASH("DT_CSRagdoll"), HASH("m_vecForce"), Force_proxy, m_Force);
	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_flVelocityModifier"), VelocityModifier_proxy, m_VelocityModifier);
	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_bClientSideAnimation"), ClientSideAnimation_proxy, m_ClientSideAnimation);
	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_flFlashDuration"), ReceivedFlashDuration_proxy, m_ReceivedFlashDuration);
#endif
}

void Hooks::RemoveHook()
{
	m_Panel.Reset();
	m_Panel.Remove(IPanel::PAINTTRAVERSE);

	m_EngineTrace.Reset();
	m_EngineTrace.Remove(IEngineTrace::CLIPRAYTOCOLLIDEABLE);
	m_EngineTrace.Remove(IEngineTrace::TRACERAY);

	m_Client.Reset();
	m_Client.Remove(CHLClient::LEVELINITPREENTITY);
	m_Client.Remove(CHLClient::LEVELINITPOSTENTITY);
	m_Client.Remove(CHLClient::LEVELSHUTDOWN);
	m_Client.Remove(CHLClient::CREATEMOVE);
	m_Client.Remove(CHLClient::FRAMESTAGENOTIFY);

	m_Engine.Reset();
	m_Engine.Remove(IVEngineClient::FIREEVENTS);
	m_Engine.Remove(IVEngineClient::ISCONNECTED);
	m_Engine.Remove(IVEngineClient::ISHLTV);
	m_Engine.Remove(IVEngineClient::ISPAUSED);

	m_Prediction.Reset();
	m_Prediction.Remove(CPrediction::INPREDICTION);
	m_Prediction.Remove(CPrediction::RUNCOMMAND);

	m_GameMovement.Reset();
	m_GameMovement.Remove(CGameMovement::PROCESSMOVEMENT);
	m_GameMovement.Remove(CGameMovement::ONJUMP);

	m_ClientMode.Reset();
	m_ClientMode.Remove(IClientMode::SHOULDDRAWFOG);
	m_ClientMode.Remove(IClientMode::OVERRIDEVIEW);
	m_ClientMode.Remove(IClientMode::DOPOSTSPACESCREENEFFECTS);

	m_Surface.Reset();
	m_Surface.Remove(ISurface::ONSCREENSIZECHANGED);

	m_ModelRender.Reset();
	m_ModelRender.Remove(IVModelRender::DRAWMODELEXECUTE);

	m_ClientShadowMgr.Reset();
	m_ClientShadowMgr.Remove(IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES);

	m_ViewRender.Reset();
	m_ViewRender.Remove(CViewRender::ONRENDERSTART);
	m_ViewRender.Remove(CViewRender::RENDER2DEFFECTSPOSTHUD);
	m_ViewRender.Remove(CViewRender::RENDERSMOKEOVERLAY);

	m_MaterialSystem.Reset();
	m_MaterialSystem.Remove(IMaterialSystem::OVERRIDECONFIG);

	m_EngineSound.Reset();
	m_EngineSound.Remove(IEngineSound::EMITSOUND);

	m_Device.Reset();
	m_Device.Remove(17);
	m_Device.Remove(16);

	m_cl_csm_shadows.Reset();
	m_cl_csm_shadows.Remove(ConVar::GETINT);

	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_angEyeAngles[0]"), m_Pitch, m_Pitch);
	g_Netvars.SetProxy(HASH("DT_CSRagdoll"), HASH("m_vecForce"), m_Force, m_Force);
	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_flVelocityModifier"), m_VelocityModifier, m_VelocityModifier);
	g_Netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_bClientSideAnimation"), m_ClientSideAnimation, m_ClientSideAnimation);
}