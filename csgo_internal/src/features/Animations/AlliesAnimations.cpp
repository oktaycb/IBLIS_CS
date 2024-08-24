#include "../../precompiled.h"

void AimPlayer::UpdateAlliesAnimations()
{
    if (g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals)[REMOVALS::REMOVALS_VISUAL_TEAMMATES])
        return;

    LagRecord* pRecord = m_Records.front().get();
    if (pRecord == nullptr)
        return;

    CCSGOPlayerAnimState* pAnimationState = m_Player->m_PlayerAnimStateCSGO();
    if (pAnimationState == nullptr)
        return;

    if (m_SpawnTime != m_Player->m_flSpawnTime()) {
        m_SpawnTime = m_Player->m_flSpawnTime();

        pAnimationState->Reset();
    }

    pAnimationState->Update(m_Player->EyeAngles());

    if (m_Player->m_nSequence() != -1)
        m_Player->OnLatchInterpolatedVariables(LATCH_ANIMATION_VAR);

    if (m_Player->m_bKilledByTaser())
        m_Player->HandleTaserAnimation();

    m_Player->GetPoseParameters(pRecord->m_Poses);
    pRecord->m_AbsAngles = m_Player->GetAbsAngles();

    if (m_Player->GetModelPtr()) 
    {
        Backup_t pBackup;
        pBackup.m_Origin = m_Player->GetAbsOrigin();
        pBackup.m_Effects = m_Player->m_fEffects();

        m_Player->AddEffect(EF_NOINTERP);
        m_Player->SetAbsOrigin(pRecord->m_Origin);

        m_Player->InvalidatePhysicsRecursive(ANIMATION_CHANGED | ANGLES_CHANGED | POSITION_CHANGED);
        m_Player->SetupBones_AttachmentHelper(m_Player->GetModelPtr());

        m_Player->SetEffect(pBackup.m_Effects);
        m_Player->SetAbsOrigin(pBackup.m_Origin);
    }
}