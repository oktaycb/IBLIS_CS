#include "../../precompiled.h"

#define MAX_OVERLAYS 15

#ifdef CLIENT_DLL
void C_AnimationLayer::SetOwner(Player* pOverlay)
{
	pOwner = pOverlay;
}

Player* C_AnimationLayer::GetOwner() const
{
	return pOwner;
}
#endif

void C_AnimationLayer::Reset()
{
#ifdef CLIENT_DLL
	if (pOwner)
	{
		int nFlags = 0;
		if (nSequence != 0 || flWeight != 0.0f)
		{
			nFlags |= BOUNDS_CHANGED;
		}
		if (flCycle != 0.0f)
		{
			nFlags |= ANIMATION_CHANGED;
		}
		if (nFlags)
		{
			pOwner->InvalidatePhysicsRecursive(nFlags);
		}
	}
#endif

	nSequence = 0;
	flPrevCycle = 0;
	flWeight = 0;
	flWeightDeltaRate = 0;
	flPlaybackRate = 0;
	flCycle = 0;
	flLayerAnimtime = 0;
	flLayerFadeOuttime = 0;
}

void C_AnimationLayer::SetSequence(int seq)
{
#ifdef CLIENT_DLL
	if (pOwner && nSequence != seq)
		pOwner->InvalidatePhysicsRecursive(BOUNDS_CHANGED);
#endif
	
	nSequence = seq;
}

void C_AnimationLayer::SetCycle(float cycle)
{
#ifdef CLIENT_DLL
	if (pOwner && flCycle != cycle)
		pOwner->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
#endif
	
	flCycle = cycle;
}

void C_AnimationLayer::SetOrder(int order)
{
#ifdef CLIENT_DLL
	if (pOwner && (nOrder != order))
	{
		if (nOrder == MAX_OVERLAYS || order == MAX_OVERLAYS)
		{
			pOwner->InvalidatePhysicsRecursive(BOUNDS_CHANGED);
		}
	}
#endif

	nOrder = order;
}

void C_AnimationLayer::SetWeight(float weight)
{
#ifdef CLIENT_DLL
	if (pOwner && flWeight != weight)
	{
		if (flWeight == 0.0f || weight == 0.0f)
		{
			pOwner->InvalidatePhysicsRecursive(BOUNDS_CHANGED);
		}
	}
#endif

	flWeight = weight;
}