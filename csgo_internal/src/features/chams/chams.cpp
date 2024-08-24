#include "../../precompiled.h"

void Chams::OnModelRender(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix)
{
	bool bIsPlayer = strstr(pInfo.m_model->m_Name, XOR("models/player"));
	bool bIsArm = strstr(pInfo.m_model->m_Name, XOR("arms"));
	bool bIsWeapon = strstr(pInfo.m_model->m_Name, XOR("weapons/v_"));

	Player* pBaseEntity = reinterpret_cast <Player*> (g_Csgo.m_EntityList->GetClientEntity(pInfo.m_Index));
	if (pBaseEntity)
	{
		if (pBaseEntity->IsPlayer())
			bIsPlayer = true;
	}

	if (bIsPlayer)
	{
		Player* pPlayer = (Player*)(pBaseEntity);
		if (!pPlayer)
		{
			g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, aMatrix);
			return g_Csgo.m_ModelRender->ForcedMaterialOverride(nullptr);
		}

		bool bIsValid = pPlayer->IsValid();
		if (!bIsValid)
		{
			g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, aMatrix);
			return g_Csgo.m_ModelRender->ForcedMaterialOverride(nullptr);
		}

		bool aVisible = false;
		bool aInvisible = false;

		if (pPlayer->IsLocalPlayer()) 
		{
			if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagActivationType)[0] && g_Client.GetLocalPlayer()->m_vecVelocity().IsValid() && g_Chams.SetupMaterial(g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagVisibleColor).a(), Color(g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagVisibleColor).r(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagVisibleColor).g(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagVisibleColor).b()), false, g_Config.i(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagModelSelection))) {
				g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, g_Client.m_LagMatrix);
			}

			if (g_Config.b(g_Items.m_Visuals.m_Local.m_Chams.m_BlendWhenScoped) && pPlayer->m_bIsScoped())
			{
				g_Csgo.m_RenderView->SetBlend(0.2f);
				g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, aMatrix);
			}
			else 
			{
				aVisible = g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_ActivationType)[0];
				aInvisible = g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_ActivationType)[1];
			}
		}
		else if (pPlayer->teammate(g_Client.GetLocalPlayer()))
		{
			aVisible = g_Config.vb(g_Items.m_Visuals.m_Allies.m_Chams.m_ActivationType)[0];
			aInvisible = g_Config.vb(g_Items.m_Visuals.m_Allies.m_Chams.m_ActivationType)[0];
		}
		else
		{
			if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryActivationType)[0])
			{
				matrix3x4a_t pCachedBones[128];
				if (g_Chams.GenerateLerpedMatrix(pInfo.m_Index, pCachedBones)) 
				{
					if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryActivationType)[1])
						if (g_Chams.SetupMaterial(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryBehindWallColor).a(), Color(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryBehindWallColor).r(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryBehindWallColor).g(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryBehindWallColor).b()), false, g_Config.i(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryModelSelection)))
							g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, pCachedBones);

					if (g_Chams.SetupMaterial(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryVisibleColor).a(), Color(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryVisibleColor).r(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryVisibleColor).g(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryVisibleColor).b()), true, g_Config.i(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryModelSelection)))
						g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, pCachedBones);
				}
			}

			aVisible = g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_ActivationType)[0];
			aInvisible = g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_ActivationType)[0];
		}

		if (aInvisible)
			this->DrawModel(pPlayer, pCtx, pState, pInfo, aMatrix, true, true);

		if (aVisible)
			this->DrawModel(pPlayer, pCtx, pState, pInfo, aMatrix, false, false);
	}

	g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, aMatrix);
	return g_Csgo.m_ModelRender->ForcedMaterialOverride(nullptr);
}

void Chams::DrawModel(Player* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ)
{
	Color cColor = Color(0.f, 0.f, 0.f);
	float flAlpha = 0.f;
	int iModelSelection = 0;

	if (pPlayer->IsLocalPlayer())
	{
		if (!bXQZ) 
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_VisibleColor).r(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_VisibleColor).g(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_VisibleColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_VisibleColor).a();
		}
		else 
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_BehindWallColor).r(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_BehindWallColor).g(), g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_BehindWallColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_BehindWallColor).a();
		}

		iModelSelection = g_Config.i(g_Items.m_Visuals.m_Local.m_Chams.m_ModelSelection);
	}
	else if (pPlayer->teammate(g_Client.GetLocalPlayer()))
	{
		if (!bXQZ)
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_VisibleColor).r(), g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_VisibleColor).g(), g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_VisibleColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_VisibleColor).a();
		}
		else 
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_BehindWallColor).r(), g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_BehindWallColor).g(), g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_BehindWallColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_BehindWallColor).a();
		}

		iModelSelection = g_Config.i(g_Items.m_Visuals.m_Allies.m_Chams.m_ModelSelection);
	}
	else
	{
		if (!bXQZ)
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_VisibleColor).r(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_VisibleColor).g(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_VisibleColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_VisibleColor).a();
		}
		else
		{
			cColor = Color(g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_BehindWallColor).r(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_BehindWallColor).g(), g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_BehindWallColor).b());
			flAlpha = g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_BehindWallColor).a();
		}

		iModelSelection = g_Config.i(g_Items.m_Visuals.m_Enemies.m_Chams.m_ModelSelection);
	}


	if (g_Chams.SetupMaterial(flAlpha, cColor, bXQZ, iModelSelection))
		g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_Csgo.m_ModelRender, pCtx, pState, pInfo, aMatrix);

	if (!bForceNull)
		return;

	const float aWhite[3]
		=
	{
		1.0f,
		1.0f,
		1.0f
	};

	g_Csgo.m_RenderView->SetBlend(1.f);
	g_Csgo.m_RenderView->SetColorModulation({ 255, 255, 255 });

	return g_Csgo.m_ModelRender->ForcedMaterialOverride(nullptr);
}

void Chams::SetColor(float flAlpha, Color cColor, IMaterial* pMaterial)
{
	if (!pMaterial)
		return;

	bool bFound = false;
	IMaterialVar* pVar = pMaterial->FindVar(XOR("$envmaptint"), &bFound, true);

	if (pVar && bFound)
		pVar->SetVecValue(cColor.r() / 255.f, cColor.g() / 255.f, cColor.b() / 255.f);

	g_Csgo.m_RenderView->SetBlend(flAlpha);
	g_Csgo.m_RenderView->SetColorModulation(cColor);
}

bool Chams::SetupMaterial(float flAlpha, Color cColor, bool bFlag, int iPlayerChamType)
{
	IMaterial* pMaterial = nullptr;

	switch (iPlayerChamType)
	{
	case 0: pMaterial = m_RegularMaterial; break;
	case 1: pMaterial = m_FlatMaterial; break;
	case 2: pMaterial = m_GhostMaterial; break;
	case 3: pMaterial = m_GlassMaterial; break;
	case 4: pMaterial = m_PulseMaterial; break;
	}

	if (pMaterial == nullptr || pMaterial->IsErrorMaterial())
		return false;

	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, bFlag);
	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, bFlag);
	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, bFlag);

	SetColor(flAlpha / 255.f, cColor, pMaterial);

	g_Csgo.m_ModelRender->ForcedMaterialOverride(pMaterial);
	return true;
}

bool Chams::GenerateLerpedMatrix(int index, matrix3x4a_t* out)
{
	LagRecord* current_record;
	AimPlayer* data;

	Player* ent = g_Csgo.m_EntityList->GetClientEntity< Player* >(index);
	if (!ent)
		return false;

	if (!g_Aimbot.IsValidTarget(ent))
		return false;

	data = &g_Aimbot.m_Players[index - 1];
	if (!data || data->m_Records.empty())
		return false;

	if (data->m_Records.size() < 2)
		return false;

	auto* channel_info = g_Csgo.m_Engine->GetNetChannelInfo();
	if (!channel_info)
		return false;

	const float max_unlag = g_Csgo.sv_maxunlag ? g_Csgo.sv_maxunlag->GetFloat() : 0.2f;

	for (auto it = data->m_Records.rbegin(); it != data->m_Records.rend(); it++) {
		current_record = it->get();

		bool end = it + 1 == data->m_Records.rend();

		if (current_record && current_record->IsValidTime(g_TickbaseManipulation.GetShiftAmount()) && (!end && ((it + 1)->get()))) {
			if (!current_record->m_Resolved && current_record->m_Origin.Delta(ent->GetAbsOrigin()) < 1.f)
				return false;

			matrix3x4_t ret[128];

			std::memcpy(ret,
				current_record->m_CachedBones,
				sizeof(ret));

			if (!ent->ShouldInterpolate())
			{
				Vector next = end ? ent->GetAbsOrigin() : (it + 1)->get()->m_Origin;
				float  time_next = end ? ent->m_flSimulationTime() : (it + 1)->get()->m_SimulationTime;

				float total_latency = channel_info->GetAvgLatency(0) + channel_info->GetAvgLatency(1);
				total_latency = std::clamp(total_latency, 0.f, max_unlag);

				float correct = total_latency + g_Client.m_Lerp;
				float time_delta = time_next - current_record->m_SimulationTime;
				float add = end ? 1.f : time_delta;
				float deadtime = current_record->m_SimulationTime + correct + add;

				float curtime = game::TICKS_TO_TIME(g_Client.m_LocalPlayer->m_nTickBase() - g_TickbaseManipulation.GetShiftAmount());
				float Delta = deadtime - curtime;

				float mul = 1.f / add;
				Vector lerp = Math::Interpolate(next, current_record->m_Origin, std::clamp(Delta * mul, 0.f, 1.f));

				for (size_t i{ }; i < 128; ++i) {
					Vector matrix_delta = current_record->m_CachedBones[i].GetOrigin() - current_record->m_Origin;
					ret[i].SetOrigin(matrix_delta + lerp);
				}
			}

			std::memcpy(out,
				ret,
				sizeof(ret));

			return true;
		}
	}

	return false;
}

void Chams::GenerateMaterials()
{
	m_RegularMaterial = g_Csgo.m_MaterialSystem->FindMaterial(XOR("debug/debugambientcube"), nullptr);
	m_RegularMaterial->IncrementReferenceCount();

	m_FlatMaterial = g_Csgo.m_MaterialSystem->FindMaterial(XOR("debug/debugdrawflat"), nullptr);
	m_FlatMaterial->IncrementReferenceCount();

	m_GhostMaterial = g_Csgo.m_MaterialSystem->FindMaterial(XOR("dev/glow_armsrace"), nullptr);
	m_GhostMaterial->IncrementReferenceCount();

	m_GlassMaterial = g_Csgo.m_MaterialSystem->FindMaterial(XOR("models/inventory_items/trophy_majors/gloss"), nullptr);
	m_GlassMaterial->IncrementReferenceCount();
	
	m_PulseMaterial = g_Csgo.m_MaterialSystem->FindMaterial(XOR("models/inventory_items/dogtags/dogtags_outline"), nullptr);
	m_PulseMaterial->IncrementReferenceCount();
}

Chams::ModelType_t Chams::GetModelType(const ModelRenderInfo_t& pInfo)
{
	std::string mdl{ pInfo.m_model->m_Name };

	if (mdl.find(XOR("player")) != std::string::npos && pInfo.m_Index > 0 && pInfo.m_Index <= 64)
		return ModelType_t::Player;

	if (mdl.find(XOR("shadow")) != std::string::npos)
		return ModelType_t::Shadow;

	if (mdl.find(XOR("sleeve")) != std::string::npos)
		return ModelType_t::Sleeves;

	if (mdl.find(XOR("arms")) != std::string::npos)
		return ModelType_t::Arms;

	if (mdl.find(XOR("weapons/v_")) != std::string::npos)
		return ModelType_t::Weapon;

	return ModelType_t::Invalid;
}