#include "../../precompiled.h"

const studiohdr_t* studiohdr_t::FindModel(void** cache, char const* modelname) const
{
	return g_Csgo.m_ModelInfo->FindModel(this, cache, modelname);
}

virtualmodel_t* studiohdr_t::GetVirtualModel(void) const
{
	if (numincludemodels == 0)
		return nullptr;
	return g_Csgo.m_ModelInfo->GetVirtualModel(this);
}

const studiohdr_t* virtualgroup_t::GetStudioHdr() const
{
	return g_Csgo.m_ModelInfo->FindModel(this->cache);
}

byte* studiohdr_t::GetAnimBlock(int iBlock, bool preloadIfMissing) const
{
	return g_Csgo.m_ModelInfo->GetAnimBlock(this, iBlock, preloadIfMissing);
}

bool studiohdr_t::hasAnimBlockBeenPreloaded(int i) const
{
	return g_Csgo.m_ModelInfo->HasAnimBlockBeenPreloaded(this, i);
}

int	studiohdr_t::GetAutoplayList(unsigned short** pOut) const
{
	return g_Csgo.m_ModelInfo->GetAutoplayList(this, pOut);
}

mstudioanimdesc_t& studiohdr_t::pAnimdesc_Internal(int i) const
{
	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_anim[i].group];
	const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
	Assert(pStudioHdr);

	return *pStudioHdr->pLocalAnimdesc(pVModel->m_anim[i].index);
}

byte* mstudioanimdesc_t::pAnimBlock(int block, int index, bool preloadIfMissing) const
{
	if (block == -1)
	{
		return (byte*)NULL;
	}
	if (block == 0)
	{
		return (((byte*)this) + index);
	}

	byte* pAnimBlock = pStudiohdr()->GetAnimBlock(block, preloadIfMissing);
	if (pAnimBlock)
	{
		return pAnimBlock + index;
	}

	return (byte*)NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Indicates if the block has been preloaded already.
//			Returns true if the block is in memory, or is asynchronously loading.
//-----------------------------------------------------------------------------
bool mstudioanimdesc_t::hasAnimBlockBeenPreloaded(int block) const
{
	return pStudiohdr()->hasAnimBlockBeenPreloaded(block);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

byte* mstudioanimdesc_t::pAnim(int* piFrame) const
{
	float flStall = 0;
	return pAnim(piFrame, flStall);
}

byte* mstudioanimdesc_t::pAnim(int* piFrame, float& flStall) const
{
	byte* panim = NULL;

	int block = animblock;
	int index = animindex;
	int section = 0;

	if (sectionframes != 0)
	{
		if (numframes > sectionframes && *piFrame == numframes - 1)
		{
			// last frame on long anims is stored separately
			*piFrame = 0;
			section = (numframes / sectionframes) + 1;
		}
		else
		{
			section = *piFrame / sectionframes;
			*piFrame -= section * sectionframes;
		}

		block = pSection(section)->animblock;
		index = pSection(section)->animindex;
	}

	if (block == -1)
	{
		// model needs to be recompiled
		return NULL;
	}

	panim = pAnimBlock(block, index);

	// force a preload of future animations
	if (sectionframes != 0)
	{
		// calc how many sections ahead to try looking
		// todo: monster fix this float.
		int maxSection = std::min(section + (g_Csgo.m_CVar->FindVar(HASH("mod_load_preload"))->GetInt() * (int)fps / sectionframes), (numframes / sectionframes)) + 1;
		int prevBlock = block;

		for (int i = section + 1; i <= maxSection; i++)
		{
			// if this is a new block, preload it
			if (pSection(i)->animblock != prevBlock)
			{
				bool preloaded = hasAnimBlockBeenPreloaded(pSection(i)->animblock);
				if (preloaded == false)
				{
					// This will preload the block
					pAnimBlock(pSection(i)->animblock, pSection(i)->animindex, true);
					// Msg( "[%8.3f] precaching %s:%s:%d:%d.\n",  Plat_FloatTime(), pStudiohdr()->pszName(), pszName(), i, pSection( i )->animblock );
				}

				prevBlock = pSection(i)->animblock;
			}
		}
	}

	if (panim == NULL)
	{
		// back up until a previously loaded block is found
		while (--section >= 0)
		{
			block = pSection(section)->animblock;
			index = pSection(section)->animindex;
			panim = pAnimBlock(block, index, false);
			if (panim)
			{
				// set it to the last frame in the last valid section
				*piFrame = sectionframes - 1;
				break;
			}
		}
	}

	// try to guess a valid stall time interval (tuned for the X360)
	flStall = 0.0f;
	if (panim == NULL && section <= 0)
	{
		zeroframestalltime = 0.f;
		flStall = 1.0f;
	}
	else if (panim != NULL && zeroframestalltime != 0.0f)
	{
		float dt = 0.f - zeroframestalltime;
		if (dt >= 0.0)
		{
			flStall = Math::SimpleSpline(Math::clampf((0.200f - dt) * 5.0, 0.0f, 1.0f));
		}

		if (flStall == 0.0f)
		{
			// disable stalltime
			zeroframestalltime = 0.0f;
		}
	}

	return panim;
}

mstudioikrule_t* mstudioanimdesc_t::pIKRule(int i) const
{
	if (numikrules)
	{
		if (ikruleindex)
		{
			return (mstudioikrule_t*)(((byte*)this) + ikruleindex) + i;
		}
		else
		{
			if (animblock == 0)
			{
				return  (mstudioikrule_t*)(((byte*)this) + animblockikruleindex) + i;
			}
			else
			{
				byte* pAnimBlock = pStudiohdr()->GetAnimBlock(animblock);

				if (pAnimBlock)
				{
					return (mstudioikrule_t*)(pAnimBlock + animblockikruleindex) + i;
				}
			}
		}
	}

	return NULL;
}


mstudiolocalhierarchy_t* mstudioanimdesc_t::pHierarchy(int i) const
{
	if (localhierarchyindex)
	{
		if (animblock == 0)
		{
			return  (mstudiolocalhierarchy_t*)(((byte*)this) + localhierarchyindex) + i;
		}
		else
		{
			byte* pAnimBlock = pStudiohdr()->GetAnimBlock(animblock);

			if (pAnimBlock)
			{
				return (mstudiolocalhierarchy_t*)(pAnimBlock + localhierarchyindex) + i;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

bool studiohdr_t::SequencesAvailable() const
{
	if (numincludemodels == 0)
	{
		return true;
	}

	return (GetVirtualModel() != NULL);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int studiohdr_t::GetNumSeq_Internal(void) const
{
	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);
	return pVModel->m_Sequence.Count();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

mstudioseqdesc_t& studiohdr_t::pSeqdesc_Internal(int i) const
{
	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	if (!pVModel)
	{
		return *pLocalSeqdesc(i);
	}

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[i].group];
	const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
	Assert(pStudioHdr);

	return *pStudioHdr->pLocalSeqdesc(pVModel->m_Sequence[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int studiohdr_t::iRelativeAnim_Internal(int baseseq, int relanim) const
{
	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[baseseq].group];

	return pGroup->masterAnim[relanim];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int studiohdr_t::iRelativeSeq_Internal(int baseseq, int relseq) const
{
	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[baseseq].group];

	return pGroup->masterSeq[relseq];
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int	studiohdr_t::GetNumPoseParameters(void) const
{
	if (numincludemodels == 0)
	{
		return numlocalposeparameters;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	return pVModel->m_pose.Count();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const mstudioposeparamdesc_t& studiohdr_t::pPoseParameter(int i)
{
	if (numincludemodels == 0)
	{
		return *pLocalPoseParameter(i);
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	if (pVModel->m_pose[i].group == 0)
		return *pLocalPoseParameter(pVModel->m_pose[i].index);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_pose[i].group];

	const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
	Assert(pStudioHdr);

	return *pStudioHdr->pLocalPoseParameter(pVModel->m_pose[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int studiohdr_t::GetSharedPoseParameter(int iSequence, int iLocalPose) const
{
	if (numincludemodels == 0)
	{
		return iLocalPose;
	}

	if (iLocalPose == -1)
		return iLocalPose;

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[iSequence].group];

	return pGroup->masterPose[iLocalPose];
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int studiohdr_t::EntryNode(int iSequence)
{
	mstudioseqdesc_t& seqdesc = pSeqdesc(iSequence);

	if (numincludemodels == 0 || seqdesc.localentrynode == 0)
	{
		return seqdesc.localentrynode;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[iSequence].group];

	return pGroup->masterNode[seqdesc.localentrynode - 1] + 1;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------


int studiohdr_t::ExitNode(int iSequence)
{
	mstudioseqdesc_t& seqdesc = pSeqdesc(iSequence);

	if (numincludemodels == 0 || seqdesc.localexitnode == 0)
	{
		return seqdesc.localexitnode;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_Sequence[iSequence].group];

	return pGroup->masterNode[seqdesc.localexitnode - 1] + 1;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	studiohdr_t::GetNumAttachments(void) const
{
	if (numincludemodels == 0)
	{
		return numlocalattachments;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	return pVModel->m_attachment.Count();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

const mstudioattachment_t& studiohdr_t::pAttachment(int i) const
{
	if (numincludemodels == 0)
	{
		return *pLocalAttachment(i);
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_attachment[i].group];
	const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
	Assert(pStudioHdr);

	return *pStudioHdr->pLocalAttachment(pVModel->m_attachment[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	studiohdr_t::GetAttachmentBone(int i)
{
	const mstudioattachment_t& attachment = pAttachment(i);

	// remap bone
	virtualmodel_t* pVModel = GetVirtualModel();
	if (pVModel)
	{
		virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_attachment[i].group];
		int iBone = pGroup->masterBone[attachment.localbone];
		if (iBone == -1)
			return 0;
		return iBone;
	}
	return attachment.localbone;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

void studiohdr_t::SetAttachmentBone(int iAttachment, int iBone)
{
	mstudioattachment_t& attachment = (mstudioattachment_t&)pAttachment(iAttachment);

	// remap bone
	virtualmodel_t* pVModel = GetVirtualModel();
	if (pVModel)
	{
		virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_attachment[iAttachment].group];
		iBone = pGroup->boneMap[iBone];
	}
	attachment.localbone = iBone;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

char* studiohdr_t::pszNodeName(int iNode)
{
	if (numincludemodels == 0)
	{
		return pszLocalNodeName(iNode);
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	if (pVModel->m_node.Count() <= iNode - 1)
		return 0; //"Invalid node";

	return pVModel->m_group[pVModel->m_node[iNode - 1].group].GetStudioHdr()->pszLocalNodeName(pVModel->m_node[iNode - 1].index);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int studiohdr_t::GetTransition(int iFrom, int iTo) const
{
	if (numincludemodels == 0)
	{
		return *pLocalTransition((iFrom - 1) * numlocalnodes + (iTo - 1));
	}

	return iTo;
	/*
	FIXME: not connected
	virtualmodel_t *pVModel = (virtualmodel_t *)GetVirtualModel();
	Assert( pVModel );
	return pVModel->m_transition.Element( iFrom ).Element( iTo );
	*/
}


int	studiohdr_t::GetActivityListVersion(void)
{
	// shit.
	if (numincludemodels == 0)
	{
		return activitylistversion;
	}
	return 0;
}

void studiohdr_t::SetActivityListVersion(int version) const
{
	activitylistversion = version;

	if (numincludemodels == 0)
	{
		return;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	int i;
	for (i = 1; i < pVModel->m_group.Count(); i++)
	{
		virtualgroup_t* pGroup = &pVModel->m_group[i];
		const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();

		Assert(pStudioHdr);

		pStudioHdr->SetActivityListVersion(version);
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------


int studiohdr_t::GetNumIKAutoplayLocks(void) const
{
	if (numincludemodels == 0)
	{
		return numlocalikautoplaylocks;
	}

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	return pVModel->m_iklock.Count();
}

const mstudioiklock_t& studiohdr_t::pIKAutoplayLock(int i)
{
	// shit.
	if (numincludemodels == 0)
		return *pLocalIKAutoplayLock(i);

	virtualmodel_t* pVModel = (virtualmodel_t*)GetVirtualModel();
	Assert(pVModel);

	virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_iklock[i].group];
	const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
	Assert(pStudioHdr);

	return *pStudioHdr->pLocalIKAutoplayLock(pVModel->m_iklock[i].index);
}

int	studiohdr_t::CountAutoplaySequences() const
{
	int count = 0;
	for (int i = 0; i < GetNumSeq(); i++)
	{
		mstudioseqdesc_t& seqdesc = pSeqdesc(i);
		if (seqdesc.flags & STUDIO_AUTOPLAY)
		{
			count++;
		}
	}
	return count;
}

int	studiohdr_t::CopyAutoplaySequences(unsigned short* pOut, int outCount) const
{
	int outIndex = 0;
	for (int i = 0; i < GetNumSeq() && outIndex < outCount; i++)
	{
		mstudioseqdesc_t& seqdesc = pSeqdesc(i);
		if (seqdesc.flags & STUDIO_AUTOPLAY)
		{
			pOut[outIndex] = i;
			outIndex++;
		}
	}
	return outIndex;
}

//-----------------------------------------------------------------------------
// Purpose:	maps local sequence bone to global bone
//-----------------------------------------------------------------------------

int	CStudioHdr::RemapSeqBone(int iSequence, int iLocalBone) const
{
	// remap bone
	if (m_pVModel)
	{
		const virtualgroup_t* pSeqGroup = m_pVModel->pSeqGroup(iSequence);
		return pSeqGroup->masterBone[iLocalBone];
	}
	return iLocalBone;
}

int	CStudioHdr::RemapAnimBone(int iAnim, int iLocalBone) const
{
	// remap bone
	if (m_pVModel)
	{
		const virtualgroup_t* pAnimGroup = m_pVModel->pAnimGroup(iAnim);
		return pAnimGroup->masterBone[iLocalBone];
	}
	return iLocalBone;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

CStudioHdr::CStudioHdr(void)
{
	// set pointer to bogus value
	m_nFrameUnlockCounter = 0;
	m_pFrameUnlockCounter = &m_nFrameUnlockCounter;
	Init(NULL);
}

CStudioHdr::CStudioHdr(const studiohdr_t* pStudioHdr, IMDLCache* mdlcache)
{
	// preset pointer to bogus value (it may be overwritten with legitimate data later)
	m_nFrameUnlockCounter = 0;
	m_pFrameUnlockCounter = &m_nFrameUnlockCounter;
	Init(pStudioHdr, mdlcache);
}

void CStudioHdr::Init(const studiohdr_t* pStudioHdr, IMDLCache* mdlcache)
{
	m_pStudioHdr = pStudioHdr;
	m_pVModel = NULL;
	m_pSoftbody = NULL;
	m_pStudioHdrCache.RemoveAll();

	if (m_pStudioHdr == NULL)
	{
		return;
	}

	if (mdlcache)
	{
		m_pFrameUnlockCounter = mdlcache->GetFrameUnlockCounterPtr(MDLCACHE_STUDIOHDR);
		m_nFrameUnlockCounter = *m_pFrameUnlockCounter - 1;
	}

	if (m_pStudioHdr->numincludemodels != 0)	
		ResetVModel(m_pStudioHdr->GetVirtualModel());
	
	for (int i = 0; i < numbones(); i++)
	{
		m_boneFlags[i] = pBone(i)->flags;
		m_boneParent[i] = pBone(i)->parent;
	}

	m_pActivityToSequence = NULL;
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

bool CStudioHdr::SequencesAvailable() const
{
	if (m_pStudioHdr->numincludemodels == 0)
	{
		return true;
	}

	if (m_pVModel == NULL)
	{
		// repoll m_pVModel
		return (ResetVModel(m_pStudioHdr->GetVirtualModel()) != NULL);
	}
	else
		return true;
}


const virtualmodel_t* CStudioHdr::ResetVModel(const virtualmodel_t* pVModel) const
{
	if (pVModel != NULL)
	{
		m_pVModel = (virtualmodel_t*)pVModel;
#if !defined( POSIX )
		Assert(!pVModel->m_Lock.GetOwnerId());
#endif
		int i;
		for (i = 0; i < m_pStudioHdrCache.Count(); i++)
		{
			m_pStudioHdrCache[i] = NULL;
		}

		return const_cast<virtualmodel_t*>(pVModel);
	}
	else
	{
		m_pVModel = NULL;
		return NULL;
	}
}

const studiohdr_t* CStudioHdr::GroupStudioHdr(int i)
{
	auto pStudioHdr = m_pStudioHdrCache[i];
	Assert(pStudioHdr);
	return pStudioHdr;
}


const studiohdr_t* CStudioHdr::pSeqStudioHdr(int sequence)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr;
	}

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_Sequence[sequence].group);

	return pStudioHdr;
}


const studiohdr_t* CStudioHdr::pAnimStudioHdr(int animation)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr;
	}

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_anim[animation].group);

	return pStudioHdr;
}



mstudioanimdesc_t& CStudioHdr::pAnimdesc_Internal(int i)
{
	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_anim[i].group);

	return *pStudioHdr->pLocalAnimdesc(m_pVModel->m_anim[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::GetNumSeq_Internal(void) const
{
	return m_pVModel->m_Sequence.Count();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

mstudioseqdesc_t& CStudioHdr::pSeqdesc_Internal(int i)
{
	Assert(i >= 0 && i < GetNumSeq());
	if (i < 0 || i >= GetNumSeq())
	{
		// Avoid reading random memory.
		i = 0;
	}

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_Sequence[i].group);

	return *pStudioHdr->pLocalSeqdesc(m_pVModel->m_Sequence[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::iRelativeAnim_Internal(int baseseq, int relanim) const
{
	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_Sequence[baseseq].group];

	return pGroup->masterAnim[relanim];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::iRelativeSeq(int baseseq, int relseq) const
{
	if (m_pVModel == NULL)
	{
		return relseq;
	}

	Assert(m_pVModel);

	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_Sequence[baseseq].group];

	return pGroup->masterSeq[relseq];
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	CStudioHdr::GetNumPoseParameters(void) const
{
	if (m_pVModel == NULL)
	{
		if (m_pStudioHdr)
			return m_pStudioHdr->numlocalposeparameters;
		else
			return 0;
	}

	Assert(m_pVModel);

	return m_pVModel->m_pose.Count();
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

const mstudioposeparamdesc_t& CStudioHdr::pPoseParameter(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalPoseParameter(i);
	}

	if (m_pVModel->m_pose[i].group == 0)
		return *m_pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_pose[i].group);

	return *pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::GetSharedPoseParameter(int iSequence, int iLocalPose) const
{
	if (m_pVModel == NULL)
	{
		return iLocalPose;
	}

	if (iLocalPose == -1)
		return iLocalPose;

	Assert(m_pVModel);

	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_Sequence[iSequence].group];

	return pGroup->masterPose[iLocalPose];
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::EntryNode(int iSequence)
{
	mstudioseqdesc_t& seqdesc = pSeqdesc(iSequence);

	if (m_pVModel == NULL || seqdesc.localentrynode == 0)
	{
		return seqdesc.localentrynode;
	}

	Assert(m_pVModel);

	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_Sequence[iSequence].group];

	return pGroup->masterNode[seqdesc.localentrynode - 1] + 1;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------


int CStudioHdr::ExitNode(int iSequence)
{
	mstudioseqdesc_t& seqdesc = pSeqdesc(iSequence);

	if (m_pVModel == NULL || seqdesc.localexitnode == 0)
	{
		return seqdesc.localexitnode;
	}

	Assert(m_pVModel);

	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_Sequence[iSequence].group];

	return pGroup->masterNode[seqdesc.localexitnode - 1] + 1;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	CStudioHdr::GetNumAttachments(void) const
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->numlocalattachments;
	}

	Assert(m_pVModel);

	return m_pVModel->m_attachment.Count();
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

const mstudioattachment_t& CStudioHdr::pAttachment(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalAttachment(i);
	}

	Assert(m_pVModel);

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_attachment[i].group);

	return *pStudioHdr->pLocalAttachment(m_pVModel->m_attachment[i].index);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	CStudioHdr::GetAttachmentBone(int i)
{
	if (m_pVModel == 0)
	{
		return m_pStudioHdr->pLocalAttachment(i)->localbone;
	}

	virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_attachment[i].group];
	const mstudioattachment_t& attachment = pAttachment(i);
	int iBone = pGroup->masterBone[attachment.localbone];
	if (iBone == -1)
		return 0;
	return iBone;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

void CStudioHdr::SetAttachmentBone(int iAttachment, int iBone)
{
	mstudioattachment_t& attachment = (mstudioattachment_t&)m_pStudioHdr->pAttachment(iAttachment);

	// remap bone
	if (m_pVModel)
	{
		virtualgroup_t* pGroup = &m_pVModel->m_group[m_pVModel->m_attachment[iAttachment].group];
		iBone = pGroup->boneMap[iBone];
	}
	attachment.localbone = iBone;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

char* CStudioHdr::pszNodeName(int iNode)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->pszLocalNodeName(iNode);
	}

	if (m_pVModel->m_node.Count() <= iNode - 1)
		return 0; //"Invalid node";

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_node[iNode - 1].group);

	return pStudioHdr->pszLocalNodeName(m_pVModel->m_node[iNode - 1].index);
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int CStudioHdr::GetTransition(int iFrom, int iTo) const
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalTransition((iFrom - 1) * m_pStudioHdr->numlocalnodes + (iTo - 1));
	}

	return iTo;
	/*
	FIXME: not connected
	virtualmodel_t *pVModel = (virtualmodel_t *)GetVirtualModel();
	Assert( pVModel );
	return pVModel->m_transition.Element( iFrom ).Element( iTo );
	*/
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	CStudioHdr::GetActivityListVersion(void)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->activitylistversion;
	}

	int version = m_pStudioHdr->activitylistversion;

	int i;
	for (i = 1; i < m_pVModel->m_group.Count(); i++)
	{
		const studiohdr_t* pStudioHdr = GroupStudioHdr(i);
		Assert(pStudioHdr);
		version = std::min(version, pStudioHdr->activitylistversion);
	}

	return version;
}

void CStudioHdr::SetActivityListVersion(int version)
{
	m_pStudioHdr->activitylistversion = version;

	if (m_pVModel == NULL)
	{
		return;
	}

	int i;
	for (i = 1; i < m_pVModel->m_group.Count(); i++)
	{
		const studiohdr_t* pStudioHdr = GroupStudioHdr(i);
		Assert(pStudioHdr);
		pStudioHdr->SetActivityListVersion(version);
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

int	CStudioHdr::GetEventListVersion(void)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->eventsindexed;
	}

	int version = m_pStudioHdr->eventsindexed;

	int i;
	for (i = 1; i < m_pVModel->m_group.Count(); i++)
	{
		const studiohdr_t* pStudioHdr = GroupStudioHdr(i);
		Assert(pStudioHdr);
		version = std::min(version, pStudioHdr->eventsindexed);
	}

	return version;
}

void CStudioHdr::SetEventListVersion(int version)
{
	m_pStudioHdr->eventsindexed = version;

	if (m_pVModel == NULL)
	{
		return;
	}

	int i;
	for (i = 1; i < m_pVModel->m_group.Count(); i++)
	{
		const studiohdr_t* pStudioHdr = GroupStudioHdr(i);
		Assert(pStudioHdr);
		pStudioHdr->eventsindexed = version;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------


int CStudioHdr::GetNumIKAutoplayLocks(void) const
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->numlocalikautoplaylocks;
	}

	return m_pVModel->m_iklock.Count();
}

const mstudioiklock_t& CStudioHdr::pIKAutoplayLock(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalIKAutoplayLock(i);
	}

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_iklock[i].group);
	Assert(pStudioHdr);
	return *pStudioHdr->pLocalIKAutoplayLock(m_pVModel->m_iklock[i].index);
}

#if 0
int	CStudioHdr::CountAutoplaySequences() const
{
	int count = 0;
	for (int i = 0; i < GetNumSeq(); i++)
	{
		mstudioseqdesc_t& seqdesc = pSeqdesc(i);
		if (seqdesc.flags & STUDIO_AUTOPLAY)
		{
			count++;
		}
	}
	return count;
}

int	CStudioHdr::CopyAutoplaySequences(unsigned short* pOut, int outCount) const
{
	int outIndex = 0;
	for (int i = 0; i < GetNumSeq() && outIndex < outCount; i++)
	{
		mstudioseqdesc_t& seqdesc = pSeqdesc(i);
		if (seqdesc.flags & STUDIO_AUTOPLAY)
		{
			pOut[outIndex] = i;
			outIndex++;
		}
	}
	return outIndex;
}

#endif

//-----------------------------------------------------------------------------
// Purpose:	maps local sequence bone to global bone
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Run the interpreted FAC's expressions, converting flex_controller 
//			values into FAC weights
//-----------------------------------------------------------------------------
void CStudioHdr::RunFlexRulesOld(const float* src, float* dest)
{
	int i, j;

	// FIXME: this shouldn't be needed, flex without rules should be stripped in studiomdl
	for (i = 0; i < numflexdesc(); i++)
	{
		dest[i] = 0;
	}

	for (i = 0; i < numflexrules(); i++)
	{
		float stack[32] = { 0.0 };
		int k = 0;
		mstudioflexrule_t* prule = pFlexRule(i);

		mstudioflexop_t* pops = prule->iFlexOp(0);

		// debugoverlay->AddTextOverlay( GetAbsOrigin() + Vector( 0, 0, 64 ), i + 1, 0, "%2d:%d\n", i, prule->flex );

		for (j = 0; j < prule->numops; j++)
		{
			switch (pops->op)
			{
			case STUDIO_ADD: stack[k - 2] = stack[k - 2] + stack[k - 1]; k--; break;
			case STUDIO_SUB: stack[k - 2] = stack[k - 2] - stack[k - 1]; k--; break;
			case STUDIO_MUL: stack[k - 2] = stack[k - 2] * stack[k - 1]; k--; break;
			case STUDIO_DIV:
				if (stack[k - 1] > 0.0001)
				{
					stack[k - 2] = stack[k - 2] / stack[k - 1];
				}
				else
				{
					stack[k - 2] = 0;
				}
				k--;
				break;
			case STUDIO_NEG: stack[k - 1] = -stack[k - 1]; break;
			case STUDIO_MAX: stack[k - 2] = std::max(stack[k - 2], stack[k - 1]); k--; break;
			case STUDIO_MIN: stack[k - 2] = std::min(stack[k - 2], stack[k - 1]); k--; break;
			case STUDIO_CONST: stack[k] = pops->d.value; k++; break;
			case STUDIO_FETCH1:
			{
				int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
				stack[k] = src[m];
				k++;
				break;
			}
			case STUDIO_FETCH2:
			{
				stack[k] = dest[pops->d.index]; k++; break;
			}
			case STUDIO_COMBO:
			{
				int m = pops->d.index;
				int km = k - m;
				for (int i = km + 1; i < k; ++i)
				{
					stack[km] *= stack[i];
				}
				k = k - m + 1;
			}
			break;
			case STUDIO_DOMINATE:
			{
				int m = pops->d.index;
				int km = k - m;
				float dv = stack[km];
				for (int i = km + 1; i < k; ++i)
				{
					dv *= stack[i];
				}
				stack[km - 1] *= 1.0f - dv;
				k -= m;
			}
			break;
			case STUDIO_2WAY_0:
			{
				int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
				stack[k] = Math::RemapValClamped(src[m], -1.0f, 0.0f, 1.0f, 0.0f);
				k++;
			}
			break;
			case STUDIO_2WAY_1:
			{
				int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
				stack[k] = Math::RemapValClamped(src[m], 0.0f, 1.0f, 0.0f, 1.0f);
				k++;
			}
			break;
			case STUDIO_NWAY:
			{
				LocalFlexController_t valueControllerIndex = static_cast<LocalFlexController_t>((int)stack[k - 1]);
				int m = pFlexcontroller(valueControllerIndex)->localToGlobal;
				float flValue = src[m];
				int v = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;

				// shit.
				/*const vec filterRamp(stack[k - 5], stack[k - 4], stack[k - 3], stack[k - 2]);

				// Apply multicontrol remapping
				if (flValue <= filterRamp.x || flValue >= filterRamp.w)
				{
					flValue = 0.0f;
				}
				else if (flValue < filterRamp.y)
				{
					flValue = RemapValClamped(flValue, filterRamp.x, filterRamp.y, 0.0f, 1.0f);
				}
				else if (flValue > filterRamp.z)
				{
					flValue = RemapValClamped(flValue, filterRamp.z, filterRamp.w, 1.0f, 0.0f);
				}
				else
				{
					flValue = 1.0f;
				}*/

				stack[k - 5] = flValue * src[v];

				k -= 4;
			}
			break;
			case STUDIO_DME_LOWER_EYELID:
			{
				const mstudioflexcontroller_t* const pCloseLidV = pFlexcontroller((LocalFlexController_t)pops->d.index);
				const float flCloseLidV = Math::RemapValClamped(src[pCloseLidV->localToGlobal], pCloseLidV->min, pCloseLidV->max, 0.0f, 1.0f);

				const mstudioflexcontroller_t* const pCloseLid = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 1]));
				const float flCloseLid = Math::RemapValClamped(src[pCloseLid->localToGlobal], pCloseLid->min, pCloseLid->max, 0.0f, 1.0f);

				int nBlinkIndex = static_cast<int>(stack[k - 2]);
				float flBlink = 0.0f;
				if (nBlinkIndex >= 0)
				{
					const mstudioflexcontroller_t* const pBlink = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 2]));
					flBlink = Math::RemapValClamped(src[pBlink->localToGlobal], pBlink->min, pBlink->max, 0.0f, 1.0f);
				}

				int nEyeUpDownIndex = static_cast<int>(stack[k - 3]);
				float flEyeUpDown = 0.0f;
				if (nEyeUpDownIndex >= 0)
				{
					const mstudioflexcontroller_t* const pEyeUpDown = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 3]));
					flEyeUpDown = Math::RemapValClamped(src[pEyeUpDown->localToGlobal], pEyeUpDown->min, pEyeUpDown->max, -1.0f, 1.0f);
				}

				if (flEyeUpDown > 0.0)
				{
					stack[k - 3] = (1.0f - flEyeUpDown) * (1.0f - flCloseLidV) * flCloseLid;
				}
				else
				{
					stack[k - 3] = (1.0f - flCloseLidV) * flCloseLid;
				}
				k -= 2;
			}
			break;
			case STUDIO_DME_UPPER_EYELID:
			{
				const mstudioflexcontroller_t* const pCloseLidV = pFlexcontroller((LocalFlexController_t)pops->d.index);
				const float flCloseLidV = Math::RemapValClamped(src[pCloseLidV->localToGlobal], pCloseLidV->min, pCloseLidV->max, 0.0f, 1.0f);

				const mstudioflexcontroller_t* const pCloseLid = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 1]));
				const float flCloseLid = Math::RemapValClamped(src[pCloseLid->localToGlobal], pCloseLid->min, pCloseLid->max, 0.0f, 1.0f);

				int nBlinkIndex = static_cast<int>(stack[k - 2]);
				float flBlink = 0.0f;
				if (nBlinkIndex >= 0)
				{
					const mstudioflexcontroller_t* const pBlink = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 2]));
					flBlink = Math::RemapValClamped(src[pBlink->localToGlobal], pBlink->min, pBlink->max, 0.0f, 1.0f);
				}

				int nEyeUpDownIndex = static_cast<int>(stack[k - 3]);
				float flEyeUpDown = 0.0f;
				if (nEyeUpDownIndex >= 0)
				{
					const mstudioflexcontroller_t* const pEyeUpDown = pFlexcontroller(static_cast<LocalFlexController_t>((int)stack[k - 3]));
					flEyeUpDown = Math::RemapValClamped(src[pEyeUpDown->localToGlobal], pEyeUpDown->min, pEyeUpDown->max, -1.0f, 1.0f);
				}

				if (flEyeUpDown < 0.0f)
				{
					stack[k - 3] = (1.0f + flEyeUpDown) * flCloseLidV * flCloseLid;
				}
				else
				{
					stack[k - 3] = flCloseLidV * flCloseLid;
				}
				k -= 2;
			}
			break;
			}

			pops++;
		}

		dest[prule->flex] = stack[0];
	}
}

void CStudioHdr::RunFlexRulesNew(const float* src, float* dest)
{
	// FIXME: this shouldn't be needed, flex without rules should be stripped in studiomdl
	memset(dest, 0, sizeof(dest[0]) * numflexdesc());

	for (int i = 0; i < numflexrules(); i++)
	{
		float stack[32];
		float* pSP = stack + ARRAYSIZE(stack);
		mstudioflexrule_t* prule = pFlexRule(i);

		mstudioflexop_t* pops = prule->iFlexOp(0);

		int nOps = prule->numops;
		float flTOS = 0.;
		if (nOps)
			do
			{
				switch (pops->op)
				{
				case STUDIO_ADD:
					flTOS += *(pSP++);
					break;

				case STUDIO_SUB:
					flTOS = *(pSP++) - flTOS;
					break;

				case STUDIO_MUL:
					flTOS *= *(pSP++);
					break;
				case STUDIO_DIV:
					if (flTOS > 0.0001)
					{
						flTOS = *(pSP) / flTOS;
					}
					else
					{
						flTOS = 0.;
					}
					pSP++;
					break;

				case STUDIO_NEG:
					flTOS = -flTOS;
					break;

				case STUDIO_MAX:
				{
					float flNos = *(pSP++);
					flTOS = std::max(flTOS, flNos);
					break;
				}

				case STUDIO_MIN:
				{
					float flNos = *(pSP++);
					flTOS = std::min(flTOS, flNos);
					break;
				}
				case STUDIO_CONST:
					*(--pSP) = flTOS;
					flTOS = pops->d.value;
					break;

				case STUDIO_FETCH1:
				{
					*(--pSP) = flTOS;
					int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
					flTOS = src[m];
					break;
				}

				case STUDIO_FETCH2:
				{
					*(--pSP) = flTOS;
					flTOS = dest[pops->d.index];
					break;
				}
				case STUDIO_COMBO:
				{
					// tos = prod( top m elements on stack)
					int m = pops->d.index;
					while (--m)
					{
						flTOS *= *(pSP++);
					}
					break;
				}
				break;

				case STUDIO_DOMINATE:
				{
					// tos *= 1-prod( next top m elements on stack)
					int m = pops->d.index;
					float dv = *(pSP++);
					while (--m)
					{
						dv *= *(pSP++);
					}
					flTOS *= 1.0 - dv;
					break;
				}
				break;
				case STUDIO_2WAY_0:
				{
					int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
					*(--pSP) = flTOS;
					flTOS = Math::RemapValClamped(src[m], -1.0f, 0.0f, 1.0f, 0.0f);
				}
				break;

				case STUDIO_2WAY_1:
				{
					int m = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;
					*(--pSP) = flTOS;
					flTOS = Math::RemapValClamped(src[m], 0.0f, 1.0f, 0.0f, 1.0f);
				}
				break;

				case STUDIO_NWAY:
				{
					LocalFlexController_t valueControllerIndex = static_cast<LocalFlexController_t>((int)flTOS);
					int m = pFlexcontroller(valueControllerIndex)->localToGlobal;
					float flValue = src[m];
					int v = pFlexcontroller((LocalFlexController_t)pops->d.index)->localToGlobal;

					/*const Vector4D filterRamp(pSP[3], pSP[2], pSP[1], pSP[0]);

					// Apply multicontrol remapping
					if (flValue <= filterRamp.x || flValue >= filterRamp.w)
					{
						flValue = 0.0f;
					}
					else if (flValue < filterRamp.y)
					{
						flValue = RemapValClamped(flValue, filterRamp.x, filterRamp.y, 0.0f, 1.0f);
					}
					else if (flValue > filterRamp.z)
					{
						flValue = RemapValClamped(flValue, filterRamp.z, filterRamp.w, 1.0f, 0.0f);
					}
					else
					{
						flValue = 1.0f;
					}*/

					pSP += 4;
					flTOS = flValue * src[v];
				}
				break;

				case STUDIO_DME_LOWER_EYELID:
				{
					const mstudioflexcontroller_t* const pCloseLidV =
						pFlexcontroller((LocalFlexController_t)pops->d.index);
					const float flCloseLidV =
						Math::RemapValClamped(src[pCloseLidV->localToGlobal], pCloseLidV->min, pCloseLidV->max, 0.0f, 1.0f);

					const mstudioflexcontroller_t* const pCloseLid =
						pFlexcontroller(static_cast<LocalFlexController_t>((int)flTOS));
					const float flCloseLid =
						Math::RemapValClamped(src[pCloseLid->localToGlobal], pCloseLid->min, pCloseLid->max, 0.0f, 1.0f);

					int nBlinkIndex = static_cast<int>(pSP[0]);
					float flBlink = 0.0f;
					if (nBlinkIndex >= 0)
					{
						const mstudioflexcontroller_t* const pBlink =
							pFlexcontroller(static_cast<LocalFlexController_t>(nBlinkIndex));
						flBlink = Math::RemapValClamped(src[pBlink->localToGlobal], pBlink->min, pBlink->max, 0.0f, 1.0f);
					}

					int nEyeUpDownIndex = static_cast<int>(pSP[1]);
					float flEyeUpDown = 0.0f;
					if (nEyeUpDownIndex >= 0)
					{
						const mstudioflexcontroller_t* const pEyeUpDown =
							pFlexcontroller(static_cast<LocalFlexController_t>(nEyeUpDownIndex));
						flEyeUpDown = Math::RemapValClamped(src[pEyeUpDown->localToGlobal], pEyeUpDown->min, pEyeUpDown->max, -1.0f, 1.0f);
					}

					if (flEyeUpDown > 0.0)
					{
						flTOS = (1.0f - flEyeUpDown) * (1.0f - flCloseLidV) * flCloseLid;
					}
					else
					{
						flTOS = (1.0f - flCloseLidV) * flCloseLid;
					}
					pSP += 2;
				}
				break;

				case STUDIO_DME_UPPER_EYELID:
				{
					const mstudioflexcontroller_t* const pCloseLidV = pFlexcontroller((LocalFlexController_t)pops->d.index);
					const float flCloseLidV = Math::RemapValClamped(src[pCloseLidV->localToGlobal], pCloseLidV->min, pCloseLidV->max, 0.0f, 1.0f);

					const mstudioflexcontroller_t* const pCloseLid = pFlexcontroller(static_cast<LocalFlexController_t>((int)flTOS));
					const float flCloseLid = Math::RemapValClamped(src[pCloseLid->localToGlobal], pCloseLid->min, pCloseLid->max, 0.0f, 1.0f);

					int nBlinkIndex = static_cast<int>(pSP[0]);
					float flBlink = 0.0f;
					if (nBlinkIndex >= 0)
					{
						const mstudioflexcontroller_t* const pBlink = pFlexcontroller(static_cast<LocalFlexController_t>(nBlinkIndex));
						flBlink = Math::RemapValClamped(src[pBlink->localToGlobal], pBlink->min, pBlink->max, 0.0f, 1.0f);
					}

					int nEyeUpDownIndex = static_cast<int>(pSP[1]);
					float flEyeUpDown = 0.0f;
					if (nEyeUpDownIndex >= 0)
					{
						const mstudioflexcontroller_t* const pEyeUpDown = pFlexcontroller(static_cast<LocalFlexController_t>(nEyeUpDownIndex));
						flEyeUpDown = Math::RemapValClamped(src[pEyeUpDown->localToGlobal], pEyeUpDown->min, pEyeUpDown->max, -1.0f, 1.0f);
					}

					if (flEyeUpDown < 0.0f)
					{
						flTOS = (1.0f + flEyeUpDown) * flCloseLidV * flCloseLid;
					}
					else
					{
						flTOS = flCloseLidV * flCloseLid;
					}
					pSP += 2;
				}
				break;
				}

				pops++;
			} while (--nOps);
			dest[prule->flex] = flTOS;
	}
}

#define USE_OLD_FLEX_RULES_INTERPRETER

void CStudioHdr::RunFlexRules(const float* src, float* dest)
{
#ifndef USE_OLD_FLEX_RULES_INTERPRETER
	RunFlexRulesNew(src, dest);
#else
	RunFlexRulesOld(src, dest);
#endif

#if defined(_DEBUG) && !defined(USE_OLD_FLEX_RULES_INTERPRETER)
	float d1[MAXSTUDIOFLEXDESC];
	RunFlexRulesOld(src, d1);

	for (int i = 0; i < numflexdesc(); i++)
	{
		if (fabs(d1[i] - dest[i]) > 0.001)
		{
			Warning("bad %d old =%f new=%f\n", i, dest[i], d1[i]);
		}
	}
#endif // _DEBUG
}


//-----------------------------------------------------------------------------
//	propagate flags all the way down
//-----------------------------------------------------------------------------

void CStudioHdr::setBoneFlags(int iBone, int flags)
{
	((mstudiobone_t*)pBone(iBone))->flags |= flags;
	mstudiolinearbone_t* pLinear = pLinearBones();
	if (pLinear)
	{
		*(pLinear->pflags(iBone)) |= flags;
	}

	m_boneFlags[iBone] |= flags;
}

void CStudioHdr::clearBoneFlags(int iBone, int flags)
{
	((mstudiobone_t*)pBone(iBone))->flags &= ~flags;
	mstudiolinearbone_t* pLinear = pLinearBones();
	if (pLinear)
	{
		*(pLinear->pflags(iBone)) &= ~flags;
	}

	m_boneFlags[iBone] &= ~flags;
}


//-----------------------------------------------------------------------------
//	CODE PERTAINING TO ACTIVITY->SEQUENCE MAPPING SUBCLASS
//-----------------------------------------------------------------------------
#define iabs(i) (( (i) >= 0 ) ? (i) : -(i) )

//-----------------------------------------------------------------------------
//	
//-----------------------------------------------------------------------------

int CStudioHdr::LookupSequence(const char* pszName)
{
	return 0;

}

CSoftbody* CStudioHdr::InitSoftbody(CSoftbodyEnvironment* pSoftbodyEnvironment)
{
//#if defined( CLIENT_DLL ) || defined( ENABLE_STUDIO_SOFTBODY )
//
//	if (m_pSoftbody)
//	{
//		ExecuteOnce(Warning("InitSoftbody is called with Softbody already existing. This pattern is suspicious and will not work if skeleton has changed, please show Sergiy a repro.\n"));
//		return m_pSoftbody;
//	}
//
//	if (m_pStudioHdr->studiohdr2index)
//	{
//		const studiohdr2_t* pHdr2 = m_pStudioHdr->pStudioHdr2();
//		if (const PhysFeModelDesc_t* pFeModelDesc = pHdr2->m_pFeModel.GetPtr())
//		{
//			uint numCtrlNames = pFeModelDesc->m_CtrlName.Count();
//			CBufferStrider feModelData(MemAlloc_AllocAligned(sizeof(CFeModel) + sizeof(CSoftbody) + sizeof(char*) * numCtrlNames, 16));
//			CFeModel* pFeModel = feModelData.Stride<CFeModel>();
//			m_pSoftbody = feModelData.Stride< CSoftbody >();
//			Construct(m_pSoftbody);
//#ifdef _DEBUG
//			if (softbody_debug.GetInt())
//			{
//				const char* pSubstr = softbody_debug_substr.GetString();
//				Msg("InitSoftbody%s %s %p\n", (*pSubstr && V_stristr(name(), pSubstr)) ? "!" : "", name(), m_pSoftbody);
//			}
//#endif
//			char** pCtrlNames = feModelData.Stride< char* >(numCtrlNames);
//			Clone(pFeModelDesc, 0, pCtrlNames, pFeModel);
//			int numModelBones = numbones();
//
//			m_pSoftbody->Init(pSoftbodyEnvironment, pFeModel, numModelBones);
//			for (uint nCtrl = 0; nCtrl < pFeModel->m_nCtrlCount; ++nCtrl)
//			{
//				int nModelBone = Studio_BoneIndexByName(this, pFeModel->m_pCtrlName[nCtrl]);
//				m_pSoftbody->BindModelBoneToCtrl(nModelBone, nCtrl);
//				//m_pStudioHdr->setBoneFlags( nModelBone, BONE_ALWAYS_PROCEDURAL );
//				//const_cast< mstudiobone_t* >( m_pStudioHdr->pBone( nModelBone ) )->proctype = STUDIO_PROC_SOFTBODY;						
//			}
//		}
//	}
//#endif
	return m_pSoftbody;
}

void CStudioHdr::FreeSoftbody()
{
//#if defined( CLIENT_DLL ) || defined( ENABLE_STUDIO_SOFTBODY )
//	if (m_pSoftbody)
//	{
//#ifdef _DEBUG
//		if (softbody_debug.GetInt())
//			Msg("FreeSoftbody %s %p\n", name(), m_pSoftbody);
//#endif
//		CFeModel* pFeModel = m_pSoftbody->GetFeModel();
//		m_pSoftbody->Shutdown();
//		Assert(pFeModel);
//		MemAlloc_FreeAligned(pFeModel);
//		m_pSoftbody = NULL;
//	}
//#endif
}

void CStudioHdr::CActivityToSequenceMapping::Reinitialize(CStudioHdr* pstudiohdr)
{
	using Reinitialize_t = void(__thiscall*)(CStudioHdr::CActivityToSequenceMapping*, CStudioHdr*);
	return g_Csgo.Reinitialize.as< Reinitialize_t >()(this, pstudiohdr);
}

void IndexModelSequences(CStudioHdr* pstudiohdr)
{
	using IndexModelSequences_t = void(__thiscall*)(CStudioHdr*);
	return g_Csgo.IndexModelSequences.as< IndexModelSequences_t >()(pstudiohdr);
}

void VerifySequenceIndex(CStudioHdr* pstudiohdr)
{
	if (!pstudiohdr)
		return;
	
	if (pstudiohdr->GetActivityListVersion() < *g_Csgo.ActivityListVersion.as<int*>()) // sometimes the server's numbers can get ahead of the client's if we're sharing memory between the two, so it's only necessary to reindex if a model is lagging the version number.
	{
		// this model's sequences have not yet been indexed by activity
		IndexModelSequences(pstudiohdr);
	}
}

CStudioHdr::CActivityToSequenceMapping* CStudioHdr::CActivityToSequenceMapping::FindMapping(CStudioHdr* pstudiohdr)
{
	// todo: find this shit.
	static DWORD adr = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57 8B F9 8B 17 83 BA ? ? ? ? ? 74 34 83 7F 04 00 75 2E 83 BA ? ? ? ? ? 75 04 33 C0 EB 0C 8B 0D ? ? ? ? 52 8B 01 FF 50 68"));

	return ((CStudioHdr::CActivityToSequenceMapping*(__thiscall*)(CStudioHdr*))adr)(pstudiohdr);
}

CStudioHdr::CActivityToSequenceMapping* getEmptyMapping()
{
	// todo: find this shit.
	static DWORD adr = pattern::find(g_Csgo.m_client_dll, XOR("87 0A 5F 5E 5B 8B E5 5D C3 B8 ? ? ? ? 5F 5E 5B 8B E5 5D C3"));
	static DWORD offset = NULL;
	if (!offset)
		offset = *(DWORD*)(adr + 10);
	
	return (CStudioHdr::CActivityToSequenceMapping*)offset;
}

bool CStudioHdr::CActivityToSequenceMapping::ValidateAgainst(const CStudioHdr* __restrict pstudiohdr) __restrict
{
	return (this == getEmptyMapping() || (m_pStudioHdr == pstudiohdr->m_pStudioHdr && m_expectedVModel == pstudiohdr->GetVirtualModel()));
}

int CStudioHdr::CActivityToSequenceMapping::SelectWeightedSequenceFromModifiers(CStudioHdr* pstudiohdr, int activity, CUtlSymbol* pActivityModifiers, int iModifierCount, Player* pParent)
{
	if (!pstudiohdr->SequencesAvailable())
	{
		return -1;
	}

	VerifySequenceIndex(pstudiohdr);

	if (pstudiohdr->GetNumSeq() == 1)
	{
		return (pParent->GetSequenceActivity(0) == activity) ? 0 : -1;
	}

	if (!ValidateAgainst(pstudiohdr))
	{
		Reinitialize(pstudiohdr);
	}

	// a null m_pSequenceTuples just means that this studio header has no activities.
	if (!m_pSequenceTuples)
		return -1;

	// get the data for the given activity
	HashValueType dummy(activity, 0, 0, 0);
	UtlHashHandle_t handle = m_ActToSeqHash.Find(dummy);
	if (!m_ActToSeqHash.IsValidHandle(handle))
	{
		return -1;
	}
	const HashValueType* __restrict actData = &m_ActToSeqHash[handle];

	// go through each sequence and give actmod matches preference. LACK of an actmod is a detriment
	int top_score = -1;
	CUtlVector<int> topScoring;

	for (int i = 0; i < actData->count; i++)
	{
		SequenceTuple* __restrict sequenceInfo = m_pSequenceTuples + actData->startingIdx + i;
		int score = 0;

		int num_seq_modifiers = sequenceInfo->iNumActivityModifiers;
		for (int k = 0; k < num_seq_modifiers; k++)
		{
			bool bFoundMod = false;

			for (int m = 0; m < iModifierCount; m++)
			{
				if (sequenceInfo->pActivityModifiers[k] == pActivityModifiers[m])
				{
					bFoundMod = true;
				}
			}

			if (bFoundMod)
			{
				score += 2;
			}
			else
			{
				score--;
			}

		}

		if (score >= top_score)
		{
			if (score > top_score)
			{
				topScoring.RemoveAll();
			}

			for (int n = 0; n < sequenceInfo->weight; n++)
			{
				topScoring.AddToTail(sequenceInfo->seqnum);
			}

			top_score = score;
		}
	}

	if (topScoring.Count())
	{
		return topScoring[g_Csgo.RandomInt(0, topScoring.Count() - 1)];
	}
	
	return -1;
}

#define AE_TYPE_SERVER			( 1 << 0 )
#define AE_TYPE_SCRIPTED		( 1 << 1 )		// see scriptevent.h
#define AE_TYPE_SHARED			( 1 << 2 )
#define AE_TYPE_WEAPON			( 1 << 3 )
#define AE_TYPE_CLIENT			( 1 << 4 )
#define AE_TYPE_FACEPOSER		( 1 << 5 )

#define AE_TYPE_NEWEVENTSYSTEM  ( 1 << 10 ) //Temporary flag.

#define AE_NOT_AVAILABLE		-1

struct animevent_t
{
#ifdef CLIENT_DLL
	// see mstudioevent_for_client_server_t comment below
	union
	{
		unsigned short	_event_highword;
		unsigned short	event_newsystem;
	};
	unsigned short	_event_lowword;
#else
	// see mstudioevent_for_client_server_t comment below
	unsigned short	_event_highword;
	union
	{
		unsigned short	_event_lowword;
		unsigned short	event_newsystem;
	};
#endif

	const char* options;
	float			cycle;
	float			eventtime;
	int				type;
	Player* pSource;
	bool			m_bHandledByScript;

	// see mstudioevent_for_client_server_t comment below
	int Event_OldSystem(void) const { return *static_cast<const int*>(static_cast<const void*>(&_event_highword)); }
	void Event_OldSystem(int nEvent) { *static_cast<int*>(static_cast<void*>(&_event_highword)) = nEvent; }
	int Event(void) const
	{
		if (type & AE_TYPE_NEWEVENTSYSTEM)
			return event_newsystem;

		return Event_OldSystem();
	}
	void Event(int nEvent)
	{
		if (type & AE_TYPE_NEWEVENTSYSTEM)
		{
			event_newsystem = nEvent;
		}
		else
		{
			Event_OldSystem(nEvent);
		}
	}
};

int GetSequenceFlags(CStudioHdr* pstudiohdr, int sequence)
{
	if (!pstudiohdr ||
		!pstudiohdr->SequencesAvailable() ||
		sequence < 0 ||
		sequence >= pstudiohdr->GetNumSeq())
	{
		return 0;
	}

	mstudioseqdesc_t& seqdesc = pstudiohdr->pSeqdesc(sequence);

	return seqdesc.flags;
}

struct mstudioevent_for_client_server_t
{
	float				cycle;
#ifdef CLIENT_DLL
	union
	{
		// Client shares with the high word
		unsigned short	_event_highword;
		unsigned short	event_newsystem;
	};
	unsigned short	_event_lowword;		// Placeholder for the low word
#else
	unsigned short	_event_highword;	// Placeholder for the high word
	union
	{
		// Server shares with the low word
		unsigned short	_event_lowword;
		unsigned short	event_newsystem;
	};
#endif

	int					type;
	inline const char* pszOptions(void) const { return options; }
	char				options[64];

	int					szeventindex;
	inline char* const pszEventName(void) const { return ((char*)this) + szeventindex; }

	// For setting and getting through Event, check the AE_TYPE_NEWEVENTSYSTEM flag to decide
	// if we should set/get just the short used by the client/server or use the whole int.
	int Event_OldSystem(void) const { return *static_cast<const int*>(static_cast<const void*>(&_event_highword)); }
	void Event_OldSystem(int nEvent) { *static_cast<int*>(static_cast<void*>(&_event_highword)) = nEvent; }
	int Event(void) const
	{
		if (type & AE_TYPE_NEWEVENTSYSTEM)
			return event_newsystem;

		return Event_OldSystem();
	}
	void Event(int nEvent)
	{
		if (type & AE_TYPE_NEWEVENTSYSTEM)
		{
			event_newsystem = nEvent;
		}
		else
		{
			Event_OldSystem(nEvent);
		}
	}
};

struct eventlist_t
{
	int					eventIndex;
	int					iType;
	unsigned short		stringKey;
	short				isPrivate;
};

struct StringTable_t /*: public CUtlDict<int, unsigned short>*/
{
};

//-----------------------------------------------------------------------------
// Purpose: Just a convenience/legacy wrapper for CUtlDict<> .
//-----------------------------------------------------------------------------
class CStringRegistry
{
private:
	StringTable_t* m_pStringList;

public:
	// returns a key for a given string
	unsigned short AddString(const char* stringText, int stringID);

	// This is optimized.  It will do 2 O(logN) searches
	// Only one of the searches needs to compare strings, the other compares symbols (ints)
	// returns -1 if the string is not present in the registry.
	int		GetStringID(const char* stringText);

	// This is unoptimized.  It will linearly search (but only compares ints, not strings)
	const char* GetStringText(int stringID);

	// This is O(1).  It will not search.  key MUST be a value that was returned by AddString
	const char* GetStringForKey(unsigned short key);
	// This is O(1).  It will not search.  key MUST be a value that was returned by AddString
	int		GetIDForKey(unsigned short key);

	void	ClearStrings(void);


	// Iterate all the keys.
	unsigned short First() const;
	unsigned short Next(unsigned short key) const;
	unsigned short InvalidIndex() const;

	//~CStringRegistry(void);			// Need to free allocated memory
	//CStringRegistry(void);
};
//
//unsigned short CStringRegistry::AddString(const char *stringText, int stringID)
//{
//	return m_pStringList->Insert( stringText, stringID );
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Given string text get the string ID
//// Input  :	Text of string to find
//// Output : Return string id or -1 if no such string exists
////-----------------------------------------------------------------------------
//int	CStringRegistry::GetStringID( const char *stringText )
//{
//	unsigned short index = m_pStringList->Find( stringText );
//	if ( m_pStringList->IsValidIndex( index ) )
//	{
//		return (*m_pStringList)[index];
//	}
//
//	return -1;
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Given a string ID return the string text
//// Input  : ID of string to find
//// Output : Return string text of NULL of no such ID exists
////-----------------------------------------------------------------------------
//char const *CStringRegistry::GetStringText( int stringID )
//{
//	for( unsigned short index = m_pStringList->First() ; index != m_pStringList->InvalidIndex(); index = m_pStringList->Next( index ) )
//	{
//		if ( (*m_pStringList)[index] == stringID )
//		{
//			return m_pStringList->GetElementName( index );
//		}
//	}
//
//	return NULL;
//}
//
//
////-----------------------------------------------------------------------------
//// Purpose: Given a key return the string text
////-----------------------------------------------------------------------------
//char const *CStringRegistry::GetStringForKey( unsigned short key )
//{
//	if ( !m_pStringList->IsValidIndex( key ) )
//		return NULL;
//
//	return m_pStringList->GetElementName( key );
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Given a key return the string text
////-----------------------------------------------------------------------------
//int CStringRegistry::GetIDForKey( unsigned short key )
//{
//	if ( !m_pStringList->IsValidIndex( key ) )
//		return 0;
//
//	return (*m_pStringList)[key];
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Clear all strings from the string registry
////-----------------------------------------------------------------------------
//void CStringRegistry::ClearStrings(void)
//{
//	m_pStringList->RemoveAll();
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Destructor - delete the list of strings and maps
//// Input  :
//// Output :
////-----------------------------------------------------------------------------
//CStringRegistry::~CStringRegistry(void)
//{
//	delete m_pStringList;
//}
//
////-----------------------------------------------------------------------------
//// Purpose: Constructor
//// Input  :
//// Output :
////-----------------------------------------------------------------------------
//CStringRegistry::CStringRegistry(void)
//{
//	m_pStringList = new StringTable_t;
//}
//
//
//unsigned short CStringRegistry::First() const
//{
//	return m_pStringList->First();
//}
//
//unsigned short CStringRegistry::Next( unsigned short key ) const
//{
//	return m_pStringList->Next( key );
//}
//
//unsigned short CStringRegistry::InvalidIndex() const
//{
//	return m_pStringList->InvalidIndex();
//}

CUtlVector<eventlist_t> g_EventList;
CStringRegistry	g_EventStrings;

static eventlist_t* ListFromString(const char* pString)
{
	// just use the string registry to do this search/map
	int stringID = 0/*g_EventStrings.GetStringID(pString)*/;
	if (stringID < 0)
		return NULL;

	return &g_EventList[stringID];
}

int EventList_IndexForName(const char* pszEventName)
{
	// this is a fast O(lgn) search (actually does 2 O(lgn) searches)
	eventlist_t* pList = ListFromString(pszEventName);

	if (pList)
	{
		return pList->eventIndex;
	}

	return -1;
}
typedef enum
{
	AE_INVALID = -1,			// So we have something more succint to check for than '-1'
	AE_EMPTY,
	AE_NPC_LEFTFOOT, // #define	NPC_EVENT_LEFTFOOT			2050
	AE_NPC_RIGHTFOOT, // #define NPC_EVENT_RIGHTFOOT			2051
	AE_NPC_BODYDROP_LIGHT, //#define NPC_EVENT_BODYDROP_LIGHT	2001
	AE_NPC_BODYDROP_HEAVY, //#define NPC_EVENT_BODYDROP_HEAVY	2002
	AE_NPC_SWISHSOUND, //#define NPC_EVENT_SWISHSOUND		2010
	AE_NPC_180TURN, //#define NPC_EVENT_180TURN			2020
	AE_NPC_ITEM_PICKUP, //#define NPC_EVENT_ITEM_PICKUP					2040
	AE_NPC_WEAPON_DROP, //#define NPC_EVENT_WEAPON_DROP					2041
	AE_NPC_WEAPON_SET_SEQUENCE_NAME, //#define NPC_EVENT_WEAPON_SET_SEQUENCE_NAME		2042
	AE_NPC_WEAPON_SET_SEQUENCE_NUMBER, //#define NPC_EVENT_WEAPON_SET_SEQUENCE_NUMBER	2043
	AE_NPC_WEAPON_SET_ACTIVITY, //#define NPC_EVENT_WEAPON_SET_ACTIVITY			2044
	AE_NPC_HOLSTER,
	AE_NPC_DRAW,
	AE_NPC_WEAPON_FIRE,

	AE_CL_PLAYSOUND, // #define CL_EVENT_SOUND				5004	// Emit a sound 
	AE_SV_PLAYSOUND,
	AE_CL_STOPSOUND,

	AE_START_SCRIPTED_EFFECT,
	AE_STOP_SCRIPTED_EFFECT,

	AE_CLIENT_EFFECT_ATTACH,

	AE_MUZZLEFLASH,			// Muzzle flash from weapons held by the player
	AE_NPC_MUZZLEFLASH,		// Muzzle flash from weapons held by NPCs

	AE_THUMPER_THUMP,		//Thumper Thump!
	AE_AMMOCRATE_PICKUP_AMMO,	//Ammo crate pick up ammo!

	AE_NPC_RAGDOLL,

	AE_NPC_ADDGESTURE,
	AE_NPC_RESTARTGESTURE,

	AE_NPC_ATTACK_BROADCAST,

	AE_NPC_HURT_INTERACTION_PARTNER,
	AE_NPC_SET_INTERACTION_CANTDIE,

	AE_SV_DUSTTRAIL,

	AE_CL_CREATE_PARTICLE_EFFECT,
	AE_CL_STOP_PARTICLE_EFFECT,
	AE_CL_ADD_PARTICLE_EFFECT_CP,
	AE_CL_CREATE_PARTICLE_BRASS,

	AE_RAGDOLL,

	AE_CL_ENABLE_BODYGROUP,
	AE_CL_DISABLE_BODYGROUP,
	AE_CL_BODYGROUP_SET_VALUE,
	AE_CL_BODYGROUP_SET_VALUE_CMODEL_WPN,

	AE_CL_BODYGROUP_SET_TO_CLIP,	// Updates the value of bodygroup 1 to the number of bullets in m_iClip1
	AE_CL_BODYGROUP_SET_TO_NEXTCLIP,	// Updates the value of bodygroup 1 to the number of bullets that can be reloaded next

	AE_CL_HIDE_SILENCER,
	AE_CL_ATTACH_SILENCER_COMPLETE,
	AE_CL_SHOW_SILENCER,
	AE_CL_DETACH_SILENCER_COMPLETE,

	AE_CL_EJECT_MAG, // Create a physical magazine to drop in the world and hide the fake-animated one
	AE_CL_EJECT_MAG_UNHIDE, //Unhide the non-physical mag that AE_CL_EJECT_MAG hid

	AE_WPN_PRIMARYATTACK,	// Used by weapons that want their primary attack to occur during an attack anim (i.e. grenade throwing)
	AE_WPN_INCREMENTAMMO,

	AE_WPN_HIDE,		// Used to hide player weapons
	AE_WPN_UNHIDE,		// Used to unhide player weapons

	AE_WPN_PLAYWPNSOUND,	// Play a weapon sound from the weapon script file

	AE_WPN_COMPLETE_RELOAD,	// the event that fires when a weapon has completed a gun reload visually according to the animation

	// Alien Swarm Infested shared events
	AE_ASW_FOOTSTEP,		// asw, played as each foot steps down
	AE_MARINE_FOOTSTEP,
	AE_MARINE_RELOAD_SOUND_A,	// anim event fired reloading sound
	AE_MARINE_RELOAD_SOUND_B,	// anim event fired reloading sound
	AE_MARINE_RELOAD_SOUND_C,	// anim event fired reloading sound
	AE_REMOVE_CLIENT_AIM,	// asw, removes this entity from the client autoaim list

	AE_MELEE_DAMAGE,
	AE_MELEE_START_COLLISION_DAMAGE,
	AE_MELEE_STOP_COLLISION_DAMAGE,
	AE_SCREEN_SHAKE,
	AE_START_DETECTING_COMBO,
	AE_STOP_DETECTING_COMBO,
	AE_COMBO_TRANSITION,
	AE_ALLOW_MOVEMENT,
	AE_SKILL_EVENT,				// marine skill event triggered (event options describes the skill)

	AE_TUG_INCAP,

	AE_CLIENT_EJECT_BRASS,

	AE_BEGIN_TAUNT_LOOP,

	AE_CL_SET_STATTRAK_GLOW,

	AE_WPN_CZ_DUMP_CURRENT_MAG,
	AE_WPN_CZ_UPDATE_BODYGROUP,

	AE_WPN_CLIP_TO_POSEPARAM,
	AE_WPN_NEXTCLIP_TO_POSEPARAM,
	AE_WPN_EMPTYSHOTS_TO_POSEPARAM,

	LAST_SHARED_ANIMEVENT,
} Animevent;

static int g_HighestEvent = 0;

eventlist_t* EventList_AddEventEntry(const char* pName, int iEventIndex, bool isPrivate, int iType)
{
	int index = g_EventList.AddToTail();
	eventlist_t* pList = &g_EventList[index];
	pList->eventIndex = iEventIndex;
	pList->stringKey = 0/*g_EventStrings.AddString(pName, index)*/;
	pList->isPrivate = isPrivate;
	pList->iType = iType;

	// UNDONE: This implies that ALL shared activities are added before ANY custom activities
	// UNDONE: Segment these instead?  It's a 32-bit int, how many activities do we need?
	if (iEventIndex > g_HighestEvent)
	{
		g_HighestEvent = iEventIndex;
	}

	return pList;
}

Animevent EventList_RegisterPrivateEvent(const char* pszEventName)
{
	eventlist_t* pList = ListFromString(pszEventName);
	if (pList)
	{
		// this activity is already in the list. If the activity we collided with is also private, 
		// then the collision is OK. Otherwise, it's a bug.
		if (pList->isPrivate)
		{
			return (Animevent)pList->eventIndex;
		}
		else
		{
			return AE_INVALID;
		}
	}

	int iType = AE_TYPE_SERVER;

#ifdef CLIENT_DLL
	iType = AE_TYPE_CLIENT;
#endif

	pList = EventList_AddEventEntry(pszEventName, g_HighestEvent + 1, true, iType);
	return (Animevent)pList->eventIndex;
}

static eventlist_t* ListFromEvent(int eventIndex)
{
	// ugly linear search
	for (int i = 0; i < g_EventList.Count(); i++)
	{
		if (g_EventList[i].eventIndex == eventIndex)
		{
			return &g_EventList[i];
		}
	}

	return NULL;
}

int EventList_GetEventType(int eventIndex)
{
	eventlist_t* pEvent = ListFromEvent(eventIndex);

	if (pEvent)
	{
		return pEvent->iType;
	}

	return -1;
}

void SetEventIndexForSequence(mstudioseqdesc_t& seqdesc)
{
	if (&seqdesc == NULL)
		return;

	if (seqdesc.numevents == 0)
		return;

#ifndef CLIENT_DLL
	seqdesc.flags |= STUDIO_EVENT;
#else
	seqdesc.flags |= STUDIO_EVENT_CLIENT;
#endif

	for (int index = 0; index < (int)seqdesc.numevents; index++)
	{
		mstudioevent_for_client_server_t* pevent = (mstudioevent_for_client_server_t*)seqdesc.pEvent(index);

		if (!pevent)
			continue;

		if (pevent->type & AE_TYPE_NEWEVENTSYSTEM)
		{
			const char* pEventName = pevent->pszEventName();

			int iEventIndex = EventList_IndexForName(pEventName);

			if (iEventIndex == -1)
			{
				pevent->event_newsystem = EventList_RegisterPrivateEvent(pEventName);
			}
			else
			{
				pevent->event_newsystem = iEventIndex;
				pevent->type |= EventList_GetEventType(iEventIndex);
			}
		}
	}
}

mstudioevent_for_client_server_t* GetEventIndexForSequence(mstudioseqdesc_t& seqdesc)
{
#ifndef CLIENT_DLL
	if (!(seqdesc.flags & STUDIO_EVENT))
#else
	if (!(seqdesc.flags & STUDIO_EVENT_CLIENT))
#endif
	{
		SetEventIndexForSequence(seqdesc);
	}

	return (mstudioevent_for_client_server_t*)seqdesc.pEvent(0);
}

#define EVENT_CLIENT			5000

int GetAnimationEvent(CStudioHdr* pstudiohdr, int sequence, animevent_t* pNPCEvent, float flStart, float flEnd, int index)
{
	if (!pstudiohdr || sequence >= pstudiohdr->GetNumSeq() || !pNPCEvent)
		return 0;

	mstudioseqdesc_t& seqdesc = pstudiohdr->pSeqdesc(sequence);
	if (seqdesc.numevents == 0 || index >= (int)seqdesc.numevents)
		return 0;

	// Msg( "flStart %f flEnd %f (%d) %s\n", flStart, flEnd, seqdesc.numevents, seqdesc.label );
	mstudioevent_for_client_server_t* pevent = GetEventIndexForSequence(seqdesc);
	for (; index < (int)seqdesc.numevents; index++)
	{
		// Don't send client-side events to the server AI
		if (pevent[index].type & AE_TYPE_NEWEVENTSYSTEM)
		{
			if (!(pevent[index].type & AE_TYPE_SERVER))
				continue;
		}
		else if (pevent[index].Event_OldSystem() >= EVENT_CLIENT) //Adrian - Support the old event system
			continue;

		bool bOverlapEvent = false;

		if (pevent[index].cycle >= flStart && pevent[index].cycle < flEnd)
		{
			bOverlapEvent = true;
		}
		// FIXME: doesn't work with animations being played in reverse
		else if ((seqdesc.flags & STUDIO_LOOPING) && flEnd < flStart)
		{
			if (pevent[index].cycle >= flStart || pevent[index].cycle < flEnd)
			{
				bOverlapEvent = true;
			}
		}

		if (bOverlapEvent)
		{
			pNPCEvent->pSource = NULL;
			pNPCEvent->cycle = pevent[index].cycle;
#if !defined( MAKEXVCD )
			pNPCEvent->eventtime = g_Csgo.m_Globals->m_Curtime;
#else
			pNPCEvent->eventtime = 0.0f;
#endif
			pNPCEvent->type = pevent[index].type;
			pNPCEvent->Event(pevent[index].Event());
			pNPCEvent->options = pevent[index].pszOptions();
			return index + 1;
		}
	}
	return 0;
}

void Player::DispatchAnimEvents(Player* eventHandler)
{
	if (this->m_flPlaybackRate() == 0.0)
		return;

	animevent_t	event;

	CStudioHdr* pstudiohdr = GetModelPtr();
	if (!pstudiohdr)
		return;

	if (!pstudiohdr->SequencesAvailable())
		return;

	if (pstudiohdr->pSeqdesc(this->m_nSequence()).numevents == 0)
		return;

	static float m_flLastEventCheck;
	static bool m_bSequenceLoops = ((GetSequenceFlags(pstudiohdr, this->m_nSequence()) & STUDIO_LOOPING) != 0);

	float flCycleRate = GetSequenceCycleRate(pstudiohdr, this->m_nSequence()) * this->m_flPlaybackRate();
	float flStart = m_flLastEventCheck;
	float flEnd = this->m_flCycle();

	bool m_bSequenceFinished = reinterpret_cast <CCommandContext*>(reinterpret_cast<uintptr_t>(this) + 0x26F8);

	if (!m_bSequenceLoops && m_bSequenceFinished)
		flEnd = 1.01f;

	m_flLastEventCheck = flEnd;

	int index = 0;
	while ((index = GetAnimationEvent(pstudiohdr, this->m_nSequence(), &event, flStart, flEnd, index)) != 0)
	{
		event.pSource = this;
		// calc when this event should happen
		if (flCycleRate > 0.0)
		{
			float flCycle = event.cycle;
			if (flCycle > this->m_flCycle())
				flCycle = flCycle - 1.0;

			event.eventtime = m_flAnimTime() + (flCycle - this->m_flCycle()) / flCycleRate + std::clamp(g_Csgo.m_Globals->m_Curtime - m_flAnimTime(), 0.f, 0.2f);
		}

		event.m_bHandledByScript = false;

		if (false)
			event.m_bHandledByScript = true;

		int nEvent = event.Event();

		if ((event.type & AE_TYPE_NEWEVENTSYSTEM) && (event.type & AE_TYPE_SERVER))
		{
			if (nEvent == AE_SV_PLAYSOUND)
			{
				//eventHandler->EmitSound(event.options);
				return;
			}
			else if (nEvent == AE_RAGDOLL)
			{
				//eventHandler->BecomeRagdollOnClient(vec3_origin);
				return;
			}
		}

		CStudioHdr* pNowStudioHdr = GetModelPtr();
		if (pNowStudioHdr != pstudiohdr)
			break;
	}
}