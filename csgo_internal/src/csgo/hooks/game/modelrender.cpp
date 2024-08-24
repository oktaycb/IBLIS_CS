#include "../../../precompiled.h"

void Hooks::DrawModelExecute(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix)
{
	if (g_Csgo.m_ModelRender->IsForcedMaterialOverride() &&
		!strstr(pInfo.m_model->m_Name, XOR("arms")) &&
		!strstr(pInfo.m_model->m_Name, XOR("weapons/v_")))
		return g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, pCtx, pState, pInfo, aMatrix);

	const Chams::ModelType_t iModelType = g_Chams.GetModelType(pInfo);
	if (iModelType == Chams::ModelType_t::Sleeves && g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_SLEEVES])
		return g_Hooks.m_ModelRender.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, pCtx, pState, pInfo, aMatrix);

	return g_Chams.OnModelRender(pCtx, pState, pInfo, aMatrix);
}