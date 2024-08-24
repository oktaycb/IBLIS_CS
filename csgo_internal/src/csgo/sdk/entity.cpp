#include "../../precompiled.h"

void CBaseViewModel::UpdateAllViewmodelAddons()
{
	if (!this)
		return;

	using UpdateAllViewmodelAddons_t = void(__fastcall*)(void*);

	static auto UpdateAllViewmodelAddons = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 2C 53 8B D9 56 57 8B")).as< UpdateAllViewmodelAddons_t >();
	if (!UpdateAllViewmodelAddons)
		return;

	return UpdateAllViewmodelAddons(this);
}

CBaseViewModel* Player::GetViewModel(int nIndex)
{
	using GetViewmodelFn = CBaseViewModel * (__thiscall*)(void*, int);
	static GetViewmodelFn GetViewModel = nullptr;

	if (!GetViewModel)
		GetViewModel = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 8B 45 08 53 8B D9 56 8B")).as< GetViewmodelFn >();

	return GetViewModel(this, nIndex);
}

struct ClientAnimating_t
{
	C_BaseAnimating* m_Animating;
	unsigned int	m_Flags;
	ClientAnimating_t(C_BaseAnimating* _pAnim, unsigned int _flags) : m_Animating(_pAnim), m_Flags(_flags) {}
};

typedef CUtlVector< ClientAnimating_t > ClientSideAnimationList;
const unsigned int FCLIENTANIM_SEQUENCE_CYCLE = 0x00000001;

void Player::SetClientSideAnimation(bool bSequenceCycle)
{
	if (this == nullptr || !this->IsValid())
		return;

	static ClientSideAnimationList* g_ClientSideAnimationList = *pattern::find(g_Csgo.m_client_dll, XOR("A1 ? ? ? ? F6 44 F0 04 01 74 0B")).add(0x1).as< ClientSideAnimationList**>();

	int c = g_ClientSideAnimationList->Count();
	for (auto i = 0; i < c; ++i)
	{
		ClientAnimating_t& pAnimating = g_ClientSideAnimationList->Element(i);
		if (!pAnimating.m_Animating)
			continue;

		if (reinterpret_cast<Player*>(pAnimating.m_Animating) != this)
			continue;

		if (bSequenceCycle) 
		{
			if (!(pAnimating.m_Flags & FCLIENTANIM_SEQUENCE_CYCLE))
				pAnimating.m_Flags |= FCLIENTANIM_SEQUENCE_CYCLE;
		}
		else if (pAnimating.m_Flags & FCLIENTANIM_SEQUENCE_CYCLE)
			pAnimating.m_Flags &= ~FCLIENTANIM_SEQUENCE_CYCLE;
	}
}

void Player::SetLocalVelocity(const Vector& vecVelocity)
{
	// checks to see if the value has changed.
	if (m_vecVelocity() == vecVelocity)
		return;

	// the abs velocity won't be dirty since we're setting it here.
	//InvalidatePhysicsRecursive(VELOCITY_CHANGED);
	m_vecVelocity() = vecVelocity;
}

void Player::SetVelocity(const Vector& vecVelocity)
{
	// this code used to call SetAbsVelocity, which is a no-no since we are in networking and if in hieararchy, the parent velocity might not be set up yet, on top of that GetNetworkOrigin and GetOldOrigin are local coordinates, so we'll just set the local vel and avoid an Assert here.
	SetLocalVelocity(vecVelocity);

	// skip to call C_BaseEntity::CalcAbsoluteVelocity.
	// this is getting called inside InvalidatePhysicsRecursive.
	RemoveEFlags(EFL_DIRTY_ABSVELOCITY);

	// the abs velocity won't be dirty since we're setting it here.
	SetAbsVelocity(m_vecVelocity());
}

void Player::SetLocalOrigin(const Vector& vecOrigin)
{
	// checks to see if the value has changed.
	if (m_vecOrigin() == vecOrigin)
		return;

	// the abs velocity won't be dirty since we're setting it here.
	//InvalidatePhysicsRecursive(POSITION_CHANGED);
	m_vecOrigin() = vecOrigin;
}

void Player::SetOrigin(const Vector& vecOrigin)
{
	// calls CBaseEntity::InvalidatePhysicsRecursive.
	SetLocalOrigin(vecOrigin);

	// skip to call C_BaseEntity::CalcAbsolutePosition.
	// this is getting called inside InvalidatePhysicsRecursive.
	RemoveEFlags(EFL_DIRTY_ABSTRANSFORM);

	// the abs velocity won't be dirty since we're setting it here.
	SetAbsOrigin(m_vecOrigin());
}

void Player::AddEFlags(int nEFlagMask)
{
	m_iEFlags() |= nEFlagMask;
}

bool Player::IsEFlagSet(int nEFlagMask)
{
	return (m_iEFlags() & nEFlagMask) != 0;
}

void Player::RemoveEFlags(int nEFlagMask) {
	// removes the flags.
	m_iEFlags() &= ~nEFlagMask;
}

int Studio_LocalPoseParameter(const CStudioHdr* pStudioHdr, const float poseParameter[], mstudioseqdesc_t& seqdesc, int iSequence, int iLocalIndex, float& flSetting)
{
	int iPose = pStudioHdr->GetSharedPoseParameter(iSequence, seqdesc.paramindex[iLocalIndex]);

	if (iPose == -1)
	{
		flSetting = 0;
		return 0;
	}

	const mstudioposeparamdesc_t& Pose = ((CStudioHdr*)pStudioHdr)->pPoseParameter(iPose);

	float flValue = poseParameter[iPose];

	if (Pose.loop)
	{
		float wrap = (Pose.start + Pose.end) / 2.0 + Pose.loop / 2.0;
		float shift = Pose.loop - wrap;

		flValue = flValue - Pose.loop * floor((flValue + shift) / Pose.loop);
	}

	int nIndex = 0;
	if (seqdesc.posekeyindex == 0)
	{
		float flLocalStart = ((float)seqdesc.paramstart[iLocalIndex] - Pose.start) / (Pose.end - Pose.start);
		float flLocalEnd = ((float)seqdesc.paramend[iLocalIndex] - Pose.start) / (Pose.end - Pose.start);

		// convert into local range
		flSetting = (flValue - flLocalStart) / (flLocalEnd - flLocalStart);

		// clamp. This shouldn't ever need to happen if it's looping.
		if (flSetting < 0)
			flSetting = 0;
		if (flSetting > 1)
			flSetting = 1;

		nIndex = 0;
		if (seqdesc.groupsize[iLocalIndex] > 2)
		{
			// estimate index
			nIndex = (int)(flSetting * (seqdesc.groupsize[iLocalIndex] - 1));
			if (nIndex == seqdesc.groupsize[iLocalIndex] - 1)
			{
				nIndex = seqdesc.groupsize[iLocalIndex] - 2;
			}
			flSetting = flSetting * (seqdesc.groupsize[iLocalIndex] - 1) - nIndex;
		}
	}
	else
	{
		flValue = flValue * (Pose.end - Pose.start) + Pose.start;
		nIndex = 0;

		while (1)
		{
			flSetting = (flValue - seqdesc.poseKey(iLocalIndex, nIndex)) / (seqdesc.poseKey(iLocalIndex, nIndex + 1) - seqdesc.poseKey(iLocalIndex, nIndex));

			if (nIndex < seqdesc.groupsize[iLocalIndex] - 2 && flSetting > 1.0)
			{
				nIndex++;
				continue;
			}
			break;
		}

		// clamp.
		if (flSetting < 0.0f)
			flSetting = 0.0f;

		if (flSetting > 1.0f)
			flSetting = 1.0f;
	}
	return nIndex;
}

void Studio_SeqAnims(const CStudioHdr* pStudioHdr, mstudioseqdesc_t& seqdesc, int iSequence, const float poseParameter[], mstudioanimdesc_t* panim[4], float* weight)
{
	if (!pStudioHdr || iSequence >= pStudioHdr->GetNumSeq())
	{
		weight[0] = weight[1] = weight[2] = weight[3] = 0.0;
		return;
	}

	float s0 = 0, s1 = 0;

	int i0 = Studio_LocalPoseParameter(pStudioHdr, poseParameter, seqdesc, iSequence, 0, s0);
	int i1 = Studio_LocalPoseParameter(pStudioHdr, poseParameter, seqdesc, iSequence, 1, s1);

	panim[0] = &((CStudioHdr*)pStudioHdr)->pAnimdesc(((CStudioHdr*)pStudioHdr)->iRelativeAnim(iSequence, seqdesc.anim(i0, i1)));
	weight[0] = (1 - s0) * (1 - s1);

	panim[1] = &((CStudioHdr*)pStudioHdr)->pAnimdesc(((CStudioHdr*)pStudioHdr)->iRelativeAnim(iSequence, seqdesc.anim(i0 + 1, i1)));
	weight[1] = (s0) * (1 - s1);

	panim[2] = &((CStudioHdr*)pStudioHdr)->pAnimdesc(((CStudioHdr*)pStudioHdr)->iRelativeAnim(iSequence, seqdesc.anim(i0, i1 + 1)));
	weight[2] = (1 - s0) * (s1);

	panim[3] = &((CStudioHdr*)pStudioHdr)->pAnimdesc(((CStudioHdr*)pStudioHdr)->iRelativeAnim(iSequence, seqdesc.anim(i0 + 1, i1 + 1)));
	weight[3] = (s0) * (s1);

	Assert(weight[0] >= 0.0f && weight[1] >= 0.0f && weight[2] >= 0.0f && weight[3] >= 0.0f);
}

bool Studio_AnimPosition(mstudioanimdesc_t* panim, float flCycle, Vector& vecPos, Vector& vecAngle)
{
	float	prevframe = 0;
	vecPos.Init();
	vecAngle.Init();

	if (panim->nummovements == 0)
		return false;

	int iLoops = 0;
	if (flCycle > 1.0)
	{
		iLoops = (int)flCycle;
	}
	else if (flCycle < 0.0)
	{
		iLoops = (int)flCycle - 1;
	}
	flCycle = flCycle - iLoops;

	float	flFrame = flCycle * (panim->numframes - 1);

	for (int i = 0; i < panim->nummovements; i++)
	{
		mstudiomovement_t* pmove = panim->pMovement(i);

		if (pmove->endframe >= flFrame)
		{
			float f = (flFrame - prevframe) / (pmove->endframe - prevframe);

			float d = pmove->v0 * f + 0.5 * (pmove->v1 - pmove->v0) * f * f;

			vecPos = vecPos + d * pmove->vector;
			vecAngle.y = vecAngle.y * (1 - f) + pmove->angle * f;
			if (iLoops != 0)
			{
				mstudiomovement_t* pmove = panim->pMovement(panim->nummovements - 1);
				vecPos = vecPos + iLoops * pmove->position;
				vecAngle.y = vecAngle.y + iLoops * pmove->angle;
			}
			return true;
		}
		else
		{
			prevframe = pmove->endframe;
			vecPos = pmove->position;
			vecAngle.y = pmove->angle;
		}
	}

	return false;
}

bool Studio_AnimMovement(mstudioanimdesc_t* panim, float flCycleFrom, float flCycleTo, Vector& deltaPos, Vector& deltaAngle)
{
	if (panim->nummovements == 0)
		return false;

	Vector startPos;
	Vector startA;
	Studio_AnimPosition(panim, flCycleFrom, startPos, startA);

	Vector endPos;
	Vector endA;
	Studio_AnimPosition(panim, flCycleTo, endPos, endA);

	Vector tmp = endPos - startPos;
	deltaAngle.y = endA.y - startA.y;
	Math::VectorYawRotate(tmp, -startA.y, deltaPos);

	return true;
}

struct animtaglookup_t
{
	int nIndex;
	const char* szName;
};

#define REGISTER_ANIMTAG( _n ) { _n, #_n },
const animtaglookup_t g_AnimTagLookupTable[ANIMTAG_COUNT] =
{
	REGISTER_ANIMTAG(ANIMTAG_UNINITIALIZED)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_N)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_NE)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_E)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_SE)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_S)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_SW)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_W)
	REGISTER_ANIMTAG(ANIMTAG_STARTCYCLE_NW)

	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMIN_IDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMAX_IDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMIN_WALK)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMAX_WALK)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMIN_RUN)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMAX_RUN)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMIN_CROUCHIDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMAX_CROUCHIDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMIN_CROUCHWALK)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_YAWMAX_CROUCHWALK)

	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMIN_IDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMAX_IDLE)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMIN_WALKRUN)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMAX_WALKRUN)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMIN_CROUCH)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMAX_CROUCH)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMIN_CROUCHWALK)
	REGISTER_ANIMTAG(ANIMTAG_AIMLIMIT_PITCHMAX_CROUCHWALK)

	REGISTER_ANIMTAG(ANIMTAG_FLASHBANG_PASSABLE)

	REGISTER_ANIMTAG(ANIMTAG_WEAPON_POSTLAYER)
};

int IndexFromAnimTagName(const char* szName) {
	int i;
	for (i = 1; i < ANIMTAG_COUNT; i++)
	{
		const animtaglookup_t* pAnimTag = &g_AnimTagLookupTable[i];
		if (!strcmp(szName, pAnimTag->szName))
		{
			return pAnimTag->nIndex;
		}
	}
	return ANIMTAG_INVALID;
}

float GetAnySequenceAnimTag(CStudioHdr* pstudiohdr, int sequence, int nDesiredTag, float flDefault)
{
	if (!pstudiohdr || sequence >= pstudiohdr->GetNumSeq())
		return flDefault;

	mstudioseqdesc_t& seqdesc = pstudiohdr->pSeqdesc(sequence);
	if (seqdesc.numanimtags == 0)
		return flDefault;

	mstudioanimtag_t* panimtag = NULL;

	int index;
	for (index = 0; index < (int)seqdesc.numanimtags; index++)
	{
		panimtag = seqdesc.pAnimTag(index);

		if (panimtag->tag == ANIMTAG_INVALID)
			continue;

		if (panimtag->tag == ANIMTAG_UNINITIALIZED)
		{
			panimtag->tag = IndexFromAnimTagName(panimtag->pszTagName());
		}

		if (panimtag->tag == nDesiredTag)
		{
			return panimtag->cycle;
		}
	}

	return flDefault;
}

float GetFirstSequenceAnimTag(CStudioHdr* pstudiohdr, int sequence, int nDesiredTag, float flStart, float flEnd)
{
	if (!pstudiohdr || sequence >= pstudiohdr->GetNumSeq())
		return flStart;

	mstudioseqdesc_t& seqdesc = pstudiohdr->pSeqdesc(sequence);
	if (seqdesc.numanimtags == 0)
		return flStart;

	mstudioanimtag_t* panimtag = NULL;

	int index;
	for (index = 0; index < (int)seqdesc.numanimtags; index++)
	{
		panimtag = seqdesc.pAnimTag(index);

		if (panimtag->tag == ANIMTAG_INVALID)
			continue;

		if (panimtag->tag == ANIMTAG_UNINITIALIZED)
		{
			panimtag->tag = IndexFromAnimTagName(panimtag->pszTagName());
		}

		if (panimtag->tag == nDesiredTag && panimtag->cycle >= flStart && panimtag->cycle < flEnd)
		{
			return panimtag->cycle;
		}
	}

	return flStart;
}

bool Studio_SeqMovement(const CStudioHdr* pStudioHdr, int iSequence, float flCycleFrom, float flCycleTo, const float poseParameter[], Vector& deltaPos, Vector& deltaAngles)
{
	mstudioanimdesc_t* panim[4];
	float	weight[4];

	mstudioseqdesc_t& seqdesc = ((CStudioHdr*)pStudioHdr)->pSeqdesc(iSequence);

	Studio_SeqAnims(pStudioHdr, seqdesc, iSequence, poseParameter, panim, weight);

	deltaPos.Init();
	deltaAngles.Init();

	bool found = false;

	for (int i = 0; i < 4; i++)
	{
		if (weight[i])
		{
			Vector localPos;
			Vector localAngles;

			localPos.Init();
			localAngles.Init();

			if (Studio_AnimMovement(panim[i], flCycleFrom, flCycleTo, localPos, localAngles))
			{
				found = true;
				deltaPos = deltaPos + localPos * weight[i];
				// FIXME: this makes no sense
				deltaAngles = deltaAngles + localAngles * weight[i];
			}
			else if (!(panim[i]->flags & STUDIO_DELTA) && panim[i]->nummovements == 0 && seqdesc.weight(0) > 0.0)
			{
				found = true;
			}
		}
	}

	// FIXME: add support for more than just start 0 and end 0 pose param layers (currently no cycle handling or angular Delta)
	for (int j = 0; j < seqdesc.numautolayers; j++)
	{
		mstudioautolayer_t* pLayer = seqdesc.pAutolayer(j);

		if (pLayer->flags & STUDIO_AL_LOCAL)
			continue;

		float layerWeight = 0;

		int iSequenceLocal = pStudioHdr->iRelativeSeq(iSequence, pLayer->iSequence);

		if (pLayer->start == 0 && pLayer->end == 0 && (pLayer->flags & STUDIO_AL_POSE))
		{
			int iPose = pStudioHdr->GetSharedPoseParameter(iSequenceLocal, pLayer->iPose);
			if (iPose == -1)
				continue;

			const mstudioposeparamdesc_t& Pose = ((CStudioHdr*)pStudioHdr)->pPoseParameter(iPose);
			float s = poseParameter[iPose] * (Pose.end - Pose.start) + Pose.start;

			Assert((pLayer->tail - pLayer->peak) != 0);

			s = Math::clampf((s - pLayer->peak) / (pLayer->tail - pLayer->peak), 0, 1);

			if (pLayer->flags & STUDIO_AL_SPLINE)
			{
				s = Math::SimpleSpline(s);
			}

			layerWeight = seqdesc.weight(0) * s;
		}

		if (layerWeight)
		{
			Vector layerPos;
			layerPos.Init();

			bool bLayerFound = false;

			mstudioseqdesc_t& seqdescLocal = ((CStudioHdr*)pStudioHdr)->pSeqdesc(iSequenceLocal);
			Studio_SeqAnims(pStudioHdr, seqdescLocal, iSequenceLocal, poseParameter, panim, weight);

			for (int i = 0; i < 4; i++)
			{
				if (weight[i])
				{
					Vector localPos;
					Vector localAngles;

					localPos.Init();
					//localAngles.Init();

					if (Studio_AnimMovement(panim[i], flCycleFrom, flCycleTo, localPos, localAngles))
					{
						bLayerFound = true;
						layerPos = layerPos + localPos * weight[i];
					}
				}
			}

			if (bLayerFound)
			{
				deltaPos = Math::Lerp(layerWeight, deltaPos, layerPos);
			}
		}
	}

	return found;
}

float Studio_GetPoseParameter(const CStudioHdr* pStudioHdr, int iParameter, float ctlValue)
{
	if (iParameter < 0 || iParameter >= pStudioHdr->GetNumPoseParameters())
	{
		return 0;
	}

	const mstudioposeparamdesc_t& PoseParam = ((CStudioHdr*)pStudioHdr)->pPoseParameter(iParameter);

	return ctlValue * (PoseParam.end - PoseParam.start) + PoseParam.start;
}

int Studio_BoneIndexByName(const CStudioHdr* pStudioHdr, const char* pName)
{
	// binary search for the bone matching pName
	int iStart = 0, iEnd = pStudioHdr->numbones() - 1;
	const byte* pBoneTable = pStudioHdr->GetBoneTableSortedByName();
	const mstudiobone_t* pBones = pStudioHdr->pBone(0);

	while (iStart <= iEnd)
	{
		int iMid = (iStart + iEnd) >> 1;
		int iCmp = _stricmp(pBones[pBoneTable[iMid]].pszName(), pName);

		if (iCmp < 0) iStart = iMid + 1;
		else if (iCmp > 0) iEnd = iMid - 1;
		else return pBoneTable[iMid];
	}

	return -1;
}

float Studio_SetPoseParameter(const CStudioHdr* pStudioHdr, int iParameter, float flValue, float& ctlValue)
{
	if (iParameter < 0 || iParameter >= pStudioHdr->GetNumPoseParameters())
		return 0;

	const mstudioposeparamdesc_t& PoseParam = ((CStudioHdr*)pStudioHdr)->pPoseParameter(iParameter);

	if (PoseParam.loop)
		flValue = flValue - PoseParam.loop * floor((flValue + (PoseParam.loop - (PoseParam.start + PoseParam.end) / 2.0 + PoseParam.loop / 2.0)) / PoseParam.loop);

	ctlValue = (flValue - PoseParam.start) / (PoseParam.end - PoseParam.start);

	if (ctlValue < 0)
		ctlValue = 0;

	if (ctlValue > 1)
		ctlValue = 1;

	return ctlValue * (PoseParam.end - PoseParam.start) + PoseParam.start;
}

void GetSequenceLinearMotion(CStudioHdr* pstudiohdr, int iSequence, const float poseParameter[], Vector* pVec)
{
	if (!pstudiohdr || !pstudiohdr->SequencesAvailable())
		return;

	if (iSequence < 0 || iSequence >= pstudiohdr->GetNumSeq())
	{
		pVec->Init();
		return;
	}

	Vector vecAngles;
	Studio_SeqMovement(pstudiohdr, iSequence, 0.f, 1.0f, poseParameter, (*pVec), vecAngles);
}

int Player::SelectWeightedSequenceFromModifiers(Activity activity, CUtlSymbol* pActivityModifiers, int iModifierCount)
{
	if (!this || !GetModelPtr())
		return 0;

	return GetModelPtr()->SelectWeightedSequenceFromModifiers(activity, pActivityModifiers, iModifierCount, this);
}

void Player::EstimateAbsVelocity(Vector& vel)
{
	return util::get_method< void(__thiscall*)(decltype(this), Vector&) >(this, ESTIMATEABSVELOCITY)(this, vel);
}

float Player::SetPoseParameter(CStudioHdr* pStudioHdr, int iParameter, float flValue)
{
	if (!pStudioHdr)
		return flValue;

	if (iParameter >= 0)
	{
		float flNewValue;
		flValue = Studio_SetPoseParameter(pStudioHdr, iParameter, flValue, flNewValue);
		*(float*)((DWORD)this + g_Offsets.m_flPoseParameter + (sizeof(float) * iParameter)) = flNewValue;
	}

	return flValue;
}

float Player::SetPoseParameter(int iParameter, float flValue) {
	return SetPoseParameter(GetModelPtr(), iParameter, flValue);
}

float Player::GetPoseParameter(int iParameter)
{
	CStudioHdr* pStudioHdr = GetModelPtr();

	if (!pStudioHdr)
		return 0.0f;

	if (iParameter >= 0)
		return Studio_GetPoseParameter(pStudioHdr, iParameter, *(float*)((DWORD)this + g_Offsets.m_flPoseParameter + (sizeof(float) * iParameter)));

	return 0.0f;
}

int Player::LookupPoseParameter(CStudioHdr* pStudioHdr, const char* szName)
{
	if (!pStudioHdr || !pStudioHdr->SequencesAvailable())
		return 0;

	for (int i = 0; i < pStudioHdr->GetNumPoseParameters(); i++) {
		if (_stricmp(pStudioHdr->pPoseParameter(i).pszName(), szName) == 0)
			return i;
	}

	return -1;
}

int Player::LookupPoseParameter(const char* szName) {
	return LookupPoseParameter(GetModelPtr(), szName);
}

bool animstate_pose_param_cache_t::Init(Player* pPlayer, const char* szPoseParamName)
{
	m_szName = szPoseParamName;
	m_nIndex = pPlayer->LookupPoseParameter(szPoseParamName);
	if (m_nIndex != -1)
		m_bInitialized = true;
	return m_bInitialized;
}

float animstate_pose_param_cache_t::GetValue(Player* pPlayer)
{
	if (!m_bInitialized)
		Init(pPlayer, m_szName);

	if (m_bInitialized && pPlayer)
		return pPlayer->GetPoseParameter(m_nIndex);

	return 0;
}

void animstate_pose_param_cache_t::SetValue(Player* pPlayer, float flValue)
{
	if (!m_bInitialized)
		Init(pPlayer, m_szName);

	if (m_bInitialized && pPlayer)
		pPlayer->SetPoseParameter(m_nIndex, flValue);
}

float Player::GetFirstSequenceAnimTag(int sequence, int nDesiredTag, float flStart, float flEnd)
{
	return ::GetFirstSequenceAnimTag(GetModelPtr(), sequence, nDesiredTag, flStart, flEnd);
}

float Player::GetAnySequenceAnimTag(int sequence, int nDesiredTag, float flDefault)
{
	return ::GetAnySequenceAnimTag(GetModelPtr(), sequence, nDesiredTag, flDefault);
}

int Player::LookupBone(const char* szName)
{
	if (!GetModelPtr())
		return -1;

	int iReturn = Studio_BoneIndexByName(GetModelPtr(), szName);
	if (iReturn == -1)
	{
		if (strstr(szName, "weapon_bone")) iReturn = Studio_BoneIndexByName(GetModelPtr(), "hand_R");
		else if (strstr(szName, "Head")) iReturn = Studio_BoneIndexByName(GetModelPtr(), "head_0");
		else if (strstr(szName, "L_Hand")) iReturn = Studio_BoneIndexByName(GetModelPtr(), "hand_L");
		else if (strstr(szName, "R_Hand")) iReturn = Studio_BoneIndexByName(GetModelPtr(), "hand_R");
	}

	return iReturn;
}

matrix3x4_t& Player::GetCoordinateFrame()
{
	return get< matrix3x4_t >(g_Offsets.m_rgflCoordinateFrame);
}

void Player::CalcAbsolutePosition() {
	using CalcAbsolutePositionFn = void(__thiscall*)(void*);
	static CalcAbsolutePositionFn CalcAbsolutePosition = nullptr;

	if (!CalcAbsolutePosition)
		CalcAbsolutePosition = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F0 83 EC 68 80 3D")).as< CalcAbsolutePositionFn >();

	return CalcAbsolutePosition(this);
}

const matrix3x4_t& Player::GetEntityToWorldTransform() const
{
	const_cast<Player*>(this)->CalcAbsolutePosition();
	return const_cast<Player*>(this)->GetCoordinateFrame();
}

void Player::GetCachedBoneMatrix(int boneIndex, matrix3x4_t& out)
{
	Math::MatrixCopy(m_BoneAccessor().GetBone(boneIndex), out);
}

void Player::GetBoneTransform(int iBone, matrix3x4_t& pBoneToWorld)
{
	CStudioHdr* pModelPtr = GetModelPtr();

	if (pModelPtr && iBone >= 0 && iBone < pModelPtr->numbones())
	{
		if (m_iMostRecentModelBoneCounter() != g_Csgo.ModelBoneCounter)
			SetupBones(NULL, -1, BONE_USED_BY_ANYTHING, g_Csgo.m_Globals->m_Curtime);
		GetCachedBoneMatrix(iBone, pBoneToWorld);
	}
	else Math::MatrixCopy(GetEntityToWorldTransform(), pBoneToWorld);
}

void Player::GetBonePosition(int iBone, Vector& origin, QAngle& angles)
{
	matrix3x4_t bonetoworld;
	GetBoneTransform(iBone, bonetoworld);
	Math::MatrixAngles(bonetoworld, angles, origin);
}

// todo: rebuild from server.
float Player::GetSequenceCycleRate(CStudioHdr* studio, int sequence)
{
	using GetSequenceCycleRate = float(__thiscall*)(void*, CStudioHdr*, int);
	return util::get_method< GetSequenceCycleRate >(this, GETSEQUENCECYCLERATE)(this, studio, sequence);
}

float Player::GetLayerSequenceCycleRate(C_AnimationLayer* layer, int sequence)
{
	using GetLayerSequenceCycleRate = float(__thiscall*)(void*, C_AnimationLayer*, int);
	return util::get_method< GetLayerSequenceCycleRate >(this, GETLAYERSEQUENCECYCLERATE)(this, layer, sequence);
}

float Player::GetSequenceMoveDist(CStudioHdr* pStudioHdr, int nSequence)
{
	Vector vReturn;
	GetSequenceLinearMotion(pStudioHdr, nSequence, (float*)((DWORD)this + g_Offsets.m_flPoseParameter), &vReturn);
	return vReturn.Length();
}

void Player::SetNumAnimOverlays(int num)
{
}

int Player::LookupSequence(const char* name) {
	using LookupSequenceFn = int(__thiscall*)(void*, const char*);
	static LookupSequenceFn LookupSequence = nullptr;

	if (!LookupSequence)
		LookupSequence = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 56 8B F1 83 BE 3C 29 00 00 00 75 14 8B 46 04 8D 4E 04 FF 50 ? 85 C0 74 07 8B CE E8 ? ? ? ? 8B B6 3C 29 00 00 85 F6 74 48")).as< LookupSequenceFn >();
	return LookupSequence(this, name);
}

void Player::SelectItem(const char* string, int sub_type)
{
	return util::get_method< void(__thiscall*)(void*, const char*, int) >(this, SELECTITEM)(this, string, sub_type);
}

Player* Player::m_hOwner(void)
{
	return get< Player* >(g_Offsets.m_hOwner);
}

#define THROWGRENADE_COUNTER_BITS 3

void Player::DoAnimationEvent(PlayerAnimEvent_t event, int nData)
{
	if (m_bUseNewAnimstate())
	{
		// run the event on the server
		m_PlayerAnimStateCSGO()->DoAnimationEvent(event);
		return;
	}

	if (event == PLAYERANIMEVENT_THROW_GRENADE)
	{
		m_iThrowGrenadeCounter() = (m_iThrowGrenadeCounter() + 1) % (1 << THROWGRENADE_COUNTER_BITS);
	}
	else
	{
		m_PlayerAnimState()->DoAnimationEvent(event);
		//TE_PlayerAnimEvent(this, event, nData);
	}
}

// These are the names of the ammo types that go in the CAmmoDefs and that the 
// weapon script files reference.
#define BULLET_PLAYER_50AE		"BULLET_PLAYER_50AE"
#define BULLET_PLAYER_762MM		"BULLET_PLAYER_762MM"
#define BULLET_PLAYER_556MM		"BULLET_PLAYER_556MM"
#define BULLET_PLAYER_556MM_SMALL "BULLET_PLAYER_556MM_SMALL"
#define BULLET_PLAYER_556MM_BOX	"BULLET_PLAYER_556MM_BOX"
#define BULLET_PLAYER_338MAG	"BULLET_PLAYER_338MAG"
#define BULLET_PLAYER_9MM		"BULLET_PLAYER_9MM"
#define BULLET_PLAYER_BUCKSHOT	"BULLET_PLAYER_BUCKSHOT"
#define BULLET_PLAYER_45ACP		"BULLET_PLAYER_45ACP"
#define BULLET_PLAYER_357SIG	"BULLET_PLAYER_357SIG"
#define BULLET_PLAYER_357SIG_P250	"BULLET_PLAYER_357SIG_P250"
#define BULLET_PLAYER_357SIG_SMALL	"BULLET_PLAYER_357SIG_SMALL"
#define BULLET_PLAYER_357SIG_MIN	"BULLET_PLAYER_357SIG_MIN"
#define BULLET_PLAYER_57MM		"BULLET_PLAYER_57MM"
#define AMMO_TYPE_HEGRENADE		"AMMO_TYPE_HEGRENADE"
#define AMMO_TYPE_FLASHBANG		"AMMO_TYPE_FLASHBANG"
#define AMMO_TYPE_SMOKEGRENADE	"AMMO_TYPE_SMOKEGRENADE"
#define AMMO_TYPE_DECOY			"AMMO_TYPE_DECOY"
#define AMMO_TYPE_MOLOTOV		"AMMO_TYPE_MOLOTOV"
#define AMMO_TYPE_TASERCHARGE	"AMMO_TYPE_TASERCHARGE"
#define AMMO_TYPE_HEALTHSHOT	"AMMO_TYPE_HEALTHSHOT"
#define AMMO_TYPE_TAGRENADE	"AMMO_TYPE_TAGRENADE"

void Player::GetBulletTypeParameters(
	const char* iBulletType,
	float& fPenetrationPower,
	float& flPenetrationDistance)
{
	if (g_Csgo.sv_penetration_type->GetInt() == 1)
	{
		fPenetrationPower = 35;
		flPenetrationDistance = 3000.0;
		return;
	}

	// make ammo types come from a script file.
	if (!strcmp(iBulletType, BULLET_PLAYER_50AE))
	{
		fPenetrationPower = 30;
		flPenetrationDistance = 1000.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_762MM))
	{
		fPenetrationPower = 39;
		flPenetrationDistance = 5000.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_556MM) ||
		!strcmp(iBulletType, BULLET_PLAYER_556MM_SMALL) ||
		!strcmp(iBulletType, BULLET_PLAYER_556MM_BOX))
	{
		fPenetrationPower = 35;
		flPenetrationDistance = 4000.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_338MAG))
	{
		fPenetrationPower = 45;
		flPenetrationDistance = 8000.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_9MM))
	{
		fPenetrationPower = 21;
		flPenetrationDistance = 800.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_BUCKSHOT))
	{
		fPenetrationPower = 0;
		flPenetrationDistance = 0.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_45ACP))
	{
		fPenetrationPower = 15;
		flPenetrationDistance = 500.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_357SIG) ||
		!strcmp(iBulletType, BULLET_PLAYER_357SIG_SMALL) ||
		!strcmp(iBulletType, BULLET_PLAYER_357SIG_P250) ||
		!strcmp(iBulletType, BULLET_PLAYER_357SIG_MIN))
	{
		fPenetrationPower = 25;
		flPenetrationDistance = 800.0;
	}
	else if (!strcmp(iBulletType, BULLET_PLAYER_57MM))
	{
		fPenetrationPower = 30;
		flPenetrationDistance = 2000.0;
	}
	else if (!strcmp(iBulletType, AMMO_TYPE_TASERCHARGE))
	{
		fPenetrationPower = 0;
		flPenetrationDistance = 0.0;
	}
	else
	{
		// What kind of ammo is this?
		Assert(false);
		fPenetrationPower = 0;
		flPenetrationDistance = 0.0;
	}

	//using GetBulletTypeParametersFn = void(__thiscall*)(void*, int , float& , float& );
	//static GetBulletTypeParametersFn GetBulletTypeParameters = nullptr;

	//if (!GetBulletTypeParameters)
	//	GetBulletTypeParameters = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 0C A1 ? ? ? ? 35 ? ? ? ? EB 05 8B 01 FF 50 34 83 F8 01")).as< GetBulletTypeParametersFn >();
	//return GetBulletTypeParameters(this, iBulletType, fPenetrationPower, flPenetrationDistance);
}

const char* Weapon::GetBulletType()
{
	auto weaponId = m_iItemDefinitionIndex();

	switch (weaponId) {
	case AWP: return "BULLET_PLAYER_338MAG";

	case AK47: return "BULLET_PLAYER_762MM";
	case AUG: return "BULLET_PLAYER_762MM";
	case G3SG1: return "BULLET_PLAYER_762MM";
	case SCAR20: return "BULLET_PLAYER_762MM";
	case SSG08: return "BULLET_PLAYER_762MM";

	case FAMAS: return "BULLET_PLAYER_556MM";
	case GALIL: return "BULLET_PLAYER_556MM";
	case M4A4: return "BULLET_PLAYER_556MM";
	case SG553: return "BULLET_PLAYER_556MM";
	case M249: return "BULLET_PLAYER_556MM_BOX";
	case NEGEV: return "BULLET_PLAYER_556MM_BOX";
	case M4A1S: return "BULLET_PLAYER_556MM_SMALL";

	case FIVESEVEN: return "BULLET_PLAYER_57MM";
	case P90: return "BULLET_PLAYER_57MM";

	case DEAGLE: return "BULLET_PLAYER_50AE";
	case REVOLVER: return "BULLET_PLAYER_50AE";

	case P2000: return "BULLET_PLAYER_357SIG";
	case P250: return "BULLET_PLAYER_357SIG_P250";
	case CZ75A: return "BULLET_PLAYER_357SIG_MIN";
	case USPS: return "BULLET_PLAYER_357SIG_SMALL";

	case ELITE: return "BULLET_PLAYER_9MM";
	case GLOCK: return "BULLET_PLAYER_9MM";
	case TEC9: return "BULLET_PLAYER_9MM";
	case BIZON: return "BULLET_PLAYER_9MM";
	case MP7: return "BULLET_PLAYER_9MM";
	case MP5SD: return "BULLET_PLAYER_9MM";
	case MP9: return "BULLET_PLAYER_9MM";

	case MAC10: return "BULLET_PLAYER_45ACP";
	case UMP45: return "BULLET_PLAYER_45ACP";

	case MAG7: return "BULLET_PLAYER_BUCKSHOT";
	case NOVA: return "BULLET_PLAYER_BUCKSHOT";
	case SAWEDOFF: return "BULLET_PLAYER_BUCKSHOT";
	case XM1014: return "BULLET_PLAYER_BUCKSHOT";

	default: return "Not a gun";
	}
}

QAngle& Player::EyeAngles()
{
	return util::get_method< QAngle& (__thiscall*)(void*) >(this, EYEANGLES)(this);
}

float Player::FrameAdvance(float interval)
{
	return util::get_method< float(__thiscall*)(void*, float) >(this, FRAMEADVANCE)(this, interval);
}

void Player::OnLatchInterpolatedVariables(int flags)
{
	return util::get_method< void(__thiscall*)(void*, int) >(this, ONLATCHINTERPOLATEDVARIABLES)(this, flags);
}

bool& Player::m_bKilledByTaser()
{
	return *(bool*)((DWORD)this + g_Offsets.m_bKilledByTaser);
}

void Player::HandleTaserAnimation() {
	using HandleTaserAnimationFn = void(__thiscall*)(void*);
	static HandleTaserAnimationFn HandleTaserAnimation = nullptr;

	if (!HandleTaserAnimation)
		HandleTaserAnimation = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 EC 08 56 8B F1 80 BE 75")).as< HandleTaserAnimationFn >();

	return HandleTaserAnimation(this);
}

void Player::UpdateViewmodels()
{
	using GetViewmodelFn = CBaseViewModel* (__thiscall*)(void*, bool);
	static GetViewmodelFn GetViewmodel = nullptr;

	if (!GetViewmodel)
		GetViewmodel = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 8B 45 08 53 8B D9 56 8B")).as< GetViewmodelFn >();

	auto pWeapon = GetActiveWeapon();
	if (pWeapon)
	{
		int ViewModelIndex = util::get_method<int(__thiscall*)(void*)>(this, 262)(this);
		if (ViewModelIndex)
		{
			CBaseViewModel* pViewModel = GetViewmodel(this, *(bool*)(ViewModelIndex + 0x31D4));
			if (pViewModel)
			{
				pViewModel->UpdateAllViewmodelAddons();
			}
		}
	}
}

#define LATCH_ANIMATION_VAR (1<<0)		// use AnimTime as sample basis
#define LATCH_SIMULATION_VAR (1<<1)		// use SimulationTime as sample basis

#define MAX_VIEWMODELS			2
//void Player::UpdateClientSideAnimation()
//{
//	if (!Rebuilt) {
//		util::get_method< void(__thiscall*)(decltype(this)) >(this, UPDATECLIENTSIDEANIMATION)(this);
//		return;
//	}
//
//	if (m_bUseNewAnimstate())
//		m_PlayerAnimStateCSGO()->Update(EyeAngles());
//	else
//	{
//		if (m_nSequence() != -1)
//			FrameAdvance(0.0f);
//		
//
//		if (IsLocalPlayer())
//			m_PlayerAnimState()->Update(EyeAngles());
//		else m_PlayerAnimState()->Update(m_angEyeAngles());
//	}
//
//	if (m_nSequence() != -1)
//		OnLatchInterpolatedVariables(LATCH_ANIMATION_VAR);
//
//	if (m_bKilledByTaser())
//		HandleTaserAnimation();
//
//	// we only update the view model for the local player.
//	if (IsLocalPlayer())	
//		UpdateViewmodels();	
//}

// todo: reverse that part.
//pViewModel->RemoveViewmodelStatTrak();
//pViewModel->RemoveViewmodelStickers();

void Player::UpdateClientSideAnimation() 
{
	g_Client.m_ClientAnimationUpdate = true;
	util::get_method< void(__thiscall*)(decltype(this)) >(this, UPDATECLIENTSIDEANIMATION)(this);
	g_Client.m_ClientAnimationUpdate = false;
}

bool Player::GetPredictable(void)
{
	return get< bool >(g_Offsets.m_bPredictable);
}

inline bool Player::IsEffectActive(int nEffects)
{
	return (m_fEffects() & nEffects) != 0;
}

bool Player::Teleported()
{
	using TeleportedFn = bool(__thiscall*)(void*);
	static TeleportedFn Teleported = nullptr;

	if (!Teleported)
		Teleported = pattern::find(g_Csgo.m_client_dll, XOR("8B 91 ? ? ? ? 83 FA FF 74 17 0F B7 C2 C1 E0 04 05 ? ? ? ? C1 EA 10 39 50 04 75 04 8B 00 EB 02 33 C0 56")).as< TeleportedFn >();

	return Teleported(this);
}

void Player::MarkForThreadedBoneSetup()
{
	using MarkForThreadedBoneSetupFn = void(__thiscall*)(void*);
	static MarkForThreadedBoneSetupFn MarkForThreadedBoneSetup = nullptr;

	if (!MarkForThreadedBoneSetup)
		MarkForThreadedBoneSetup = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 51 80 3D ? ? ? ? 00 56 8B F1 0F 84")).as< MarkForThreadedBoneSetupFn >();

	return MarkForThreadedBoneSetup(this);
}

CThreadFastMutex& Player::m_BoneSetupLock() {
	return get< CThreadFastMutex >(g_Csgo.BoneSetupLock);
}

bool Player::SetupBonesServer(matrix3x4a_t* out, int max, int iBoneMask, float time) {
	g_Client.m_SetupBones = true;
	bool bSetuped = GetClientRenderable()->SetupBones(out, max, iBoneMask, time);
	g_Client.m_SetupBones = false;
	return bSetuped;
}

//#define MEMALL_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new MEMALL_DEBUG_NEW

void CIKContext::ClearTargets(void)
{
	int i;
	for (i = 0; i < m_target.Count(); i++)
	{
		m_target[i].latched.iFramecounter = -9999;
	}
}

void CIKContext::Init(const CStudioHdr* pStudioHdr, const QAngle& angles, const Vector& pos, float flTime, int iFramecounter, int boneMask)
{
	m_pStudioHdr = pStudioHdr;
	m_ikChainRule.RemoveAll(); // m_numikrules = 0;
	if (pStudioHdr->numikchains())
	{
		m_ikChainRule.SetSize(pStudioHdr->numikchains());

		// FIXME: Brutal hackery to prevent a crash
		if (m_target.Count() == 0)
		{
			m_target.SetSize(12);
			memset(m_target.Base(), 0, sizeof(m_target[0]) * m_target.Count());
			ClearTargets();
		}

	}
	else
	{
		m_target.SetSize(0);
	}
	Math::AngleMatrix(angles, pos, m_rootxform);
	m_iFramecounter = iFramecounter;
	m_flTime = flTime;
	m_boneMask = boneMask;
}

void CIKContext::UpdateTargets(Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed)
{
	using UpdateTargetsFn = void(__thiscall*)(void*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&);
	static UpdateTargetsFn UpdateTargets = nullptr;

	if (!UpdateTargets)
		UpdateTargets = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 33 D2")).as< UpdateTargetsFn >();

	return UpdateTargets(this, pos, q, boneToWorld, boneComputed);
}

void CIKContext::SolveDependencies(Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed)
{
	using SolveDependenciesFn = void(__thiscall*)(void*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&);
	static SolveDependenciesFn SolveDependencies = nullptr;

	if (!SolveDependencies)
		SolveDependencies = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 8B 81 ? ? ? ?")).as< SolveDependenciesFn >();

	return SolveDependencies(this, pos, q, boneToWorld, boneComputed);
}

void* Player::m_pRagdoll()
{
	return get< void* >(g_Offsets.m_pRagdoll);
}

void* Player::m_bClientSideRagdoll()
{
	return get< void* >(g_Offsets.m_bClientSideRagdoll);
}

bool Player::IsRagdoll()
{
	return m_pRagdoll() && m_bClientSideRagdoll();
}

void Player::UpdateIKLocks(float currentTime)
{
	using UpdateIKLocksFn = void(__thiscall*)(void*, float);
	static UpdateIKLocksFn UpdateIKLocks = nullptr;

	if (!UpdateIKLocks)
		UpdateIKLocks = pattern::find(g_Csgo.m_client_dll, XOR("8B 01 F3 0F ? ? ? ? 51 F3 ? ? ? ? FF 90 ? ? ? ? 8B 8F ? ? ? ? 8D")).as< UpdateIKLocksFn >();

	return UpdateIKLocks(this, currentTime);
}

void Player::CalculateIKLocks(float currentTime)
{
	using CalculateIKLocksFn = void(__thiscall*)(void*, float);
	static CalculateIKLocksFn CalculateIKLocks = nullptr;

	if (!CalculateIKLocks)
		CalculateIKLocks = pattern::find(g_Csgo.m_client_dll, XOR("8D 4F FC F3 ? ? ? ? ? 51 F3 ? ? ? ? FF 90 ? ? ? ? 8B 8F ? ? ? ? 8D")).as< CalculateIKLocksFn >();

	return CalculateIKLocks(this, currentTime);
}

CIKContext::CIKContext()
{
	m_target.EnsureCapacity(12); // FIXME: this sucks, shouldn't it be grown?
	m_iFramecounter = -1;
	m_pStudioHdr = NULL;
	m_flTime = -1.0f;
	m_target.SetSize(0);
}

//static bool InThreadedBoneSetup = pattern::find(g_Csgo.m_client_dll, XOR("80 3D ? ? ? ? 00 8D 8F")).add(0x2).as< bool >();

bool Player::SetupBonesRebuilt(matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int iBoneMask, float currentTime) {

	CStudioHdr* pModelPtr = GetModelPtr();
	if (!pModelPtr || !pModelPtr->SequencesAvailable())	
		return false;

	if (m_nSequence() == -1)
		return false;

	if (iBoneMask == -1)
		iBoneMask = m_iPrevBoneMask();

	iBoneMask |= BONE_ALWAYS_SETUP;

	if (g_Csgo.cl_SetupAllBones->GetInt())
		iBoneMask |= BONE_USED_BY_ANYTHING;

	if (m_bIsToolRecording())
		iBoneMask |= BONE_USED_BY_ANYTHING;

	int nLOD = 0;
	int nMask = BONE_USED_BY_VERTEX_LOD0;
	for (; nLOD < MAX_NUM_LODS; ++nLOD, nMask <<= 1)
		if (iBoneMask & nMask)
			break;

	for (; nLOD < MAX_NUM_LODS; ++nLOD, nMask <<= 1)
		iBoneMask |= nMask;

	if (GetPredictable() &&
		g_Csgo.m_Prediction->InPrediction())
		currentTime = g_Csgo.m_Prediction->GetSavedTime();

	MarkForThreadedBoneSetup();
	m_iAccumulatedBoneMask() |= iBoneMask;

	ALIGN16 matrix3x4_t parentTransform;
	Math::AngleMatrix(GetClientRenderable()->GetRenderAngles(), GetClientRenderable()->GetRenderOrigin(), parentTransform);

	iBoneMask |= m_iPrevBoneMask();

	int oldReadableBones = m_BoneAccessor().GetReadableBones();
	m_BoneAccessor().SetWritableBones(m_BoneAccessor().GetReadableBones() | iBoneMask);
	m_BoneAccessor().SetReadableBones(m_BoneAccessor().GetWritableBones());

	if (pModelPtr->flags() & STUDIOHDR_FLAGS_STATIC_PROP)
	{
		Math::MatrixCopy(parentTransform, m_BoneAccessor().GetBoneForWrite(0));
	}
	else {
		AddEFlags(EFL_SETTING_UP_BONES);

		if (!m_pIk() && pModelPtr->numikchains() > 0 && !(m_EntClientFlags() & ENTCLIENTFLAG_DONTUSEIK))
			m_pIk() = new CIKContext;

		if (m_pIk())
		{
			if (Teleported() || IsEffectActive(EF_NOINTERP))
			{
				m_pIk()->ClearTargets();
			}

			m_pIk()->Init(pModelPtr, GetClientRenderable()->GetRenderAngles(), GetClientRenderable()->GetRenderOrigin(), currentTime, g_Csgo.m_Globals->m_FrameCount, iBoneMask);
		}

		DWORD EntityPlus4 = (DWORD)((DWORD)this + 0x4);
		Vector* pEntPos = (Vector*)((DWORD)EntityPlus4 + 0xA68);
		QuaternionAligned* pEntQ = (QuaternionAligned*)((DWORD)EntityPlus4 + 0x166C);

		Vector vecPosition[MAXSTUDIOBONES];
		QuaternionAligned Quaternion[MAXSTUDIOBONES];

		CBoneBitList boneComputed;

		if (!false)
		{
			int nTempMask = iBoneMask;

			if (m_nCustomBlendingRuleMask() != -1)
				nTempMask &= m_nCustomBlendingRuleMask();

			nTempMask |= BONE_ALWAYS_SETUP;

			StandardBlendingRules(pModelPtr, vecPosition, Quaternion, currentTime, nTempMask);

			if (IsPlayer() && nTempMask != iBoneMask)
			{
				for (int i = 0; i < pModelPtr->numbones(); ++i)
				{
					if ((pModelPtr->boneFlags(i) & (BONE_USED_BY_ATTACHMENT | BONE_USED_BY_HITBOX | BONE_ALWAYS_SETUP)) == 0)
					{
						vecPosition[i] = pEntPos[i];
						Quaternion[i] = pEntQ[i];
					}
				}
			}

			m_nLastNonSkippedFrame() = g_Csgo.m_Globals->m_FrameCount;
		}
		else
		{
			memcpy(vecPosition, pEntPos, sizeof(Vector) * pModelPtr->numbones());
			memcpy(Quaternion, pEntQ, sizeof(QuaternionAligned) * pModelPtr->numbones());

			boneComputed.ClearAll();
			iBoneMask = m_BoneAccessor().GetWritableBones();
		}

		if (m_pIk() && !IsRagdoll())
		{
			UpdateIKLocks(currentTime);

			m_pIk()->UpdateTargets(vecPosition, Quaternion, m_BoneAccessor().m_pBones, boneComputed);

			CalculateIKLocks(currentTime);
			m_pIk()->SolveDependencies(vecPosition, Quaternion, m_BoneAccessor().m_pBones, boneComputed);

			if (IsPlayer() && ((BONE_USED_BY_VERTEX_LOD0 & iBoneMask) == BONE_USED_BY_VERTEX_LOD0))
				DoExtraBoneProcessing(pModelPtr, vecPosition, Quaternion, m_BoneAccessor().m_pBones, boneComputed, m_pIk());
		}

		BuildTransformations(pModelPtr, vecPosition, Quaternion, parentTransform, iBoneMask, boneComputed);
		RemoveEFlags(EFL_SETTING_UP_BONES);
		ControlMouth(pModelPtr);

		if (!false)
		{
			memcpy(pEntPos, vecPosition, sizeof(Vector) * pModelPtr->numbones());
			memcpy(pEntQ, Quaternion, sizeof(QuaternionAligned) * pModelPtr->numbones());
		}
	}

	//if (!(oldReadableBones & BONE_USED_BY_ATTACHMENT) && (iBoneMask & BONE_USED_BY_ATTACHMENT))
	SetupBones_AttachmentHelper(pModelPtr);
	
	return true;
}

bool Player::SetupBones(matrix3x4a_t* out, int max, int iBoneMask, float time) {
	g_Client.m_SetupBones = true;
	bool bSetuped = GetClientRenderable()->SetupBones(out, max, iBoneMask, time);
	g_Client.m_SetupBones = false;
	return bSetuped;
}