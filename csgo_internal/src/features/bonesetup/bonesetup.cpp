#include "../../precompiled.h"

bool Bones::SetupBones(Player* pPlayer, LagRecord* pRecord)
{
	if (!pRecord->m_Setup) {
		if (!BuildBones(pPlayer, pRecord, BONE_USED_BY_ANYTHING & ~BONE_USED_BY_BONE_MERGE))
			return false;

		pRecord->m_Setup = true;
	}

	return true;
}

//pPlayer->SetupBones(pRecord->m_CachedBones, MAXSTUDIOBONES, iBoneMask, pRecord->m_SimulationTime);

bool Bones::BuildBones(Player* pPlayer, LagRecord* pRecord, int iBoneMask)
{
	if (pPlayer->m_nSequence() == -1)
		return false;

	CStudioHdr* pHdr = pPlayer->GetModelPtr();
	if (!pHdr || !pHdr->SequencesAvailable()) 
	{
		pPlayer->m_BoneSetupLock().Unlock();
		return false;
	}

	CBoneAccessor* pAccessor = &pPlayer->m_BoneAccessor();
	if (!pAccessor)
		return false;

	bool m_InPrediction = g_Csgo.m_Prediction->m_InPrediction;
	g_Csgo.m_Prediction->m_InPrediction = false;

	const auto nComputedLODframe = pPlayer->m_nComputedLODframe();
	const auto nAnimLODflags = pPlayer->m_nAnimLODflags();
	const auto nCustomBlendingRuleMask = pPlayer->m_nCustomBlendingRuleMask();
	const auto EntClientFlags = pPlayer->m_EntClientFlags();
	const auto pIk = pPlayer->m_pIk();

	pPlayer->m_iPrevBoneMask() = 0;
	iBoneMask |= BONE_USED_BY_ATTACHMENT;

	pPlayer->m_nComputedLODframe() = 0;
	pPlayer->m_nAnimLODflags() = 0;
	pPlayer->m_nCustomBlendingRuleMask() = -1;

	pPlayer->AddClientEffect(ENTCLIENTFLAG_DONTUSEIK);
	pPlayer->m_pIk() = nullptr;

	DWORD EntityPlus4 = (DWORD)((DWORD)pPlayer + 0x4);
	Vector* pEntPos = (Vector*)((DWORD)EntityPlus4 + 0xA68);
	QuaternionAligned* pEntQ = (QuaternionAligned*)((DWORD)EntityPlus4 + 0x166C);

	Backup_t pBackup;
	pBackup.m_Origin = pPlayer->GetAbsOrigin();
	pBackup.m_AbsAngles = pPlayer->GetAbsAngles();
	pBackup.m_Effects = pPlayer->m_fEffects();
	pBackup.m_CachedBones = pAccessor->m_pBones;
	pPlayer->GetPoseParameters(pBackup.m_Poses);
	pPlayer->GetAnimLayers(pBackup.m_Layers);

	pPlayer->AddEffect(EF_NOINTERP);
	pPlayer->SetAbsOrigin(pRecord->m_Origin);
	pPlayer->SetAbsAngles(pRecord->m_AbsAngles);
	pPlayer->SetPoseParameters(pRecord->m_Poses);
	pPlayer->SetAnimLayers(pRecord->m_Layers);

	pAccessor->m_pBones = pRecord->m_CachedBones;

	if (iBoneMask == -1)
		iBoneMask = pPlayer->m_iPrevBoneMask();

	iBoneMask |= BONE_ALWAYS_SETUP;

	if (g_Csgo.cl_SetupAllBones->GetInt())
		iBoneMask |= BONE_USED_BY_ANYTHING;

	if (pPlayer->m_bIsToolRecording())
		iBoneMask |= BONE_USED_BY_ANYTHING;

	int nLOD = 0;
	int nMask = BONE_USED_BY_VERTEX_LOD0;
	for (; nLOD < MAX_NUM_LODS; ++nLOD, nMask <<= 1)
		if (iBoneMask & nMask)
			break;

	for (; nLOD < MAX_NUM_LODS; ++nLOD, nMask <<= 1)
		iBoneMask |= nMask;

	float currentTime = pRecord->m_SimulationTime;

	if (pPlayer->GetPredictable() && g_Csgo.m_Prediction->InPrediction())
		currentTime = g_Csgo.m_Prediction->GetSavedTime();

//	if (pPlayer->m_iMostRecentModelBoneCounter() != g_Csgo.ModelBoneCounter)
//	{
//		if (FLT_MAX >= pPlayer->m_flLastBoneSetupTime() || currentTime < pPlayer->m_flLastBoneSetupTime())
//		{
//			pAccessor->SetReadableBones(0);
//			pAccessor->SetWritableBones(0);
//			pPlayer->m_flLastBoneSetupTime() = currentTime;
//		}
//
//		pPlayer->m_iPrevBoneMask() = pPlayer->m_iAccumulatedBoneMask();
//		pPlayer->m_iAccumulatedBoneMask() = 0;
//
//#ifdef STUDIO_ENABLE_PERF_COUNTERS
//		pHdr->ClearPerfCounters();
//#endif
//	}

	pPlayer->MarkForThreadedBoneSetup();

	pPlayer->m_iAccumulatedBoneMask() |= iBoneMask;
	//pPlayer->m_iMostRecentModelBoneCounter() = g_Csgo.ModelBoneCounter;

	//if ((pAccessor->GetReadableBones() & iBoneMask) != iBoneMask)
	{
		ALIGN16 matrix3x4_t parentTransform;
		Math::AngleMatrix(pPlayer->GetClientRenderable()->GetRenderAngles(), pPlayer->GetClientRenderable()->GetRenderOrigin(), parentTransform);

		iBoneMask |= pPlayer->m_iPrevBoneMask();

		int oldReadableBones = pAccessor->GetReadableBones();
		pAccessor->SetWritableBones(pAccessor->GetReadableBones() | iBoneMask);
		pAccessor->SetReadableBones(pAccessor->GetWritableBones());

		if (!(pHdr->flags() & STUDIOHDR_FLAGS_STATIC_PROP)) {
			if (!pPlayer->m_pIk() && pHdr->numikchains() > 0 && !(pPlayer->m_EntClientFlags() & ENTCLIENTFLAG_DONTUSEIK))
				pPlayer->m_pIk() = new CIKContext;

			Vector vecPosition[MAXSTUDIOBONES];
			QuaternionAligned Quaternion[MAXSTUDIOBONES];

			if (pPlayer->m_pIk()) {
				if (pPlayer->Teleported() || pPlayer->IsEffectActive(EF_NOINTERP))
					pPlayer->m_pIk()->ClearTargets();

				pPlayer->m_pIk()->Init(pHdr, pPlayer->GetClientRenderable()->GetRenderAngles(), pPlayer->GetClientRenderable()->GetRenderOrigin(), currentTime, g_Csgo.m_Globals->m_FrameCount, iBoneMask);
			}

			CBoneBitList boneComputed;

			bool bSkipAnimationFrame = false;
			if (!bSkipAnimationFrame) {
				int newBoneMask = iBoneMask;
				int nTempMask = newBoneMask;

				if (!pPlayer->IsPlayer()) {
					if (pPlayer->m_nCustomBlendingRuleMask() != -1)
						newBoneMask = iBoneMask & pPlayer->m_nCustomBlendingRuleMask();

					newBoneMask |= BONE_ALWAYS_SETUP;
					nTempMask = newBoneMask;
				} else nTempMask = BONE_USED_BY_HITBOX | BONE_USED_BY_VERTEX_MASK;
				
				pPlayer->AddEFlags(EFL_SETTING_UP_BONES);
				pPlayer->StandardBlendingRules(pHdr, vecPosition, Quaternion, currentTime, nTempMask);

				if (pPlayer->IsPlayer() && nTempMask != iBoneMask) {
					for (int i = 0; i < pHdr->numbones(); ++i) {
						if ((pHdr->boneFlags(i) & (BONE_USED_BY_ATTACHMENT | BONE_USED_BY_HITBOX | BONE_ALWAYS_SETUP)) == 0) {
							vecPosition[i] = pEntPos[i];
							Quaternion[i] = pEntQ[i];
						}
					}
				}

				pPlayer->m_nLastNonSkippedFrame() = g_Csgo.m_Globals->m_FrameCount;
			} 
			else {
				memcpy(vecPosition, pEntPos, sizeof(Vector) * pHdr->numbones());
				memcpy(Quaternion, pEntQ, sizeof(QuaternionAligned) * pHdr->numbones());

				boneComputed.ClearAll();
				iBoneMask = pAccessor->GetWritableBones();
			}

			if (pPlayer->m_pIk() && !pPlayer->IsRagdoll()) {
				pPlayer->UpdateIKLocks(currentTime);
				pPlayer->m_pIk()->UpdateTargets(vecPosition, Quaternion, pAccessor->m_pBones, boneComputed);

				pPlayer->CalculateIKLocks(currentTime);
				pPlayer->m_pIk()->SolveDependencies(vecPosition, Quaternion, pAccessor->m_pBones, boneComputed);

				if (pPlayer->IsPlayer() && ((BONE_USED_BY_VERTEX_LOD0 & iBoneMask) == BONE_USED_BY_VERTEX_LOD0))
					pPlayer->DoExtraBoneProcessing(pHdr, vecPosition, Quaternion, pAccessor->m_pBones, boneComputed, pPlayer->m_pIk());
			}

			pPlayer->BuildTransformations(pHdr, vecPosition, Quaternion, parentTransform, iBoneMask, boneComputed);

			pPlayer->RemoveEFlags(EFL_SETTING_UP_BONES);
			pPlayer->ControlMouth(pHdr);

			if (!bSkipAnimationFrame) {
				memcpy(pEntPos, vecPosition, sizeof(Vector) * pHdr->numbones());
				memcpy(pEntQ, Quaternion, sizeof(QuaternionAligned) * pHdr->numbones());
			}
		}
		else Math::MatrixCopy(parentTransform, pAccessor->GetBoneForWrite(0));

		pPlayer->InvalidatePhysicsRecursive(POSITION_CHANGED | ANGLES_CHANGED | ANIMATION_CHANGED);
		pPlayer->SetupBones_AttachmentHelper(pHdr);
	}

	pAccessor->m_pBones = pBackup.m_CachedBones;

	pPlayer->m_nComputedLODframe() = nComputedLODframe;
	pPlayer->m_nAnimLODflags() = nAnimLODflags;
	pPlayer->m_nCustomBlendingRuleMask() = nCustomBlendingRuleMask;
	pPlayer->m_EntClientFlags() = EntClientFlags;
	pPlayer->m_pIk() = pIk;

	pPlayer->SetEffect(pBackup.m_Effects);
	pPlayer->SetAbsOrigin(pBackup.m_Origin);
	pPlayer->SetAbsAngles(pBackup.m_AbsAngles);
	pPlayer->SetPoseParameters(pBackup.m_Poses);
	pPlayer->SetAnimLayers(pBackup.m_Layers);

	g_Csgo.m_Prediction->m_InPrediction = m_InPrediction;

	return true;
}