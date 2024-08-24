#pragma once

class Detours {
public:
	void Init();
	void RemoveDetours();

public:
	static bool __fastcall Interpolate(void* ecx, void* edx, float currentTime);
	decltype(&Interpolate) m_Interpolate;

	static void __fastcall AddBaseAnimatingInterpolatedVars(Player* m_Player, void* edx);
	decltype(&AddBaseAnimatingInterpolatedVars) m_AddBaseAnimatingInterpolatedVars;

	static bool __fastcall IsSelfAnimating(Player* m_Player, void* edx);
	decltype(&IsSelfAnimating) m_IsSelfAnimating;

	static void __cdecl CL_Move(float accumulated_extra_samples, bool bFinalTick);
	decltype(&CL_Move) m_CL_Move;
	
	static bool __cdecl CLCMsg_Move(void* msg);
	decltype(&CLCMsg_Move) m_CLCMsg_Move;

	static bool __cdecl ReportHit(Hit_t* pHurtInfo);
	decltype(&ReportHit) m_ReportHit;

	static void __fastcall UpdateClientSideAnimation(Player* m_Player, void* edx);
	decltype(&UpdateClientSideAnimation) m_UpdateClientSideAnimation;
	
	static void __fastcall UpdateClientSideAnimations(Player* m_Player, void* edx);
	decltype(&UpdateClientSideAnimations) m_UpdateClientSideAnimations;

	static void __vectorcall UpdateAnimationState(CCSGOPlayerAnimState* m_AnimationState, void* edx, QAngle angAngle, bool bForce);
	decltype(&UpdateAnimationState) m_UpdateAnimationState;

	static void __fastcall StudioFrameAdvance(Player* m_Player, void* edx);
	decltype(&StudioFrameAdvance) m_StudioFrameAdvance;

	static void __fastcall CheckForSequenceChange(void* ecx, void* edx, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate);
	decltype(&CheckForSequenceChange) m_CheckForSequenceChange;

	static bool IsUsingStaticPropDebugModes();
	decltype(&IsUsingStaticPropDebugModes) m_IsUsingStaticPropDebugModes;

	static void __fastcall ColorModulate(IMaterial* m_Material, void* edx, float* m_Red, float* m_Green, float* m_Blue);
	decltype(&ColorModulate) m_ColorModulate;

	static bool __fastcall ShouldSkipAnimationFrame(Player* m_Player, void* edx);
	decltype(&ShouldSkipAnimationFrame) m_ShouldSkipAnimationFrame;

	static void __fastcall ResetAnimationState(CCSGOPlayerAnimState* m_AnimationState, void* edx);
	decltype(&ResetAnimationState) m_ResetAnimationState;

	static void __fastcall ModifyEyePosition(CCSGOPlayerAnimState* m_AnimationState, void* edx, Vector& vecPosition);
	decltype(&ModifyEyePosition) m_ModifyEyePosition;
};

extern Detours g_Detours;