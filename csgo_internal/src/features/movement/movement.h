#pragma once

class Movement {
public:
	Vector m_OBBMins = Vector(0.f, 0.f, 0.f), m_OBBMaxs = Vector(0.f, 0.f, 0.f), m_Origin = Vector(0.f, 0.f, 0.f);
	float  m_VelocityLenghtXY = 0.f, m_Ideal = 0.f, m_switch_value = 1.f, m_old_yaw = 0.f, m_circle_yaw = 0.f;
	bool   m_invert = false;
	bool   m_WalkSwitch = false, m_MoveSwitch = false;

	struct AutoPeek {
		void Reset(Vector vecOrigin = Vector(0.f, 0.f, 0.f)) {
			m_Origin = vecOrigin;
			m_ShouldRetrack = m_CanRetrack = m_AutoPeeking = m_AutoPeekingDone = false;
			m_ForwardMove = m_SideMove = 0.f;
		}

		bool m_AutoPeekingDone = false ,m_AutoPeeking = false,m_ShouldRetrack = false,m_CanRetrack = false;
		float m_SideMove = 0.f, m_ForwardMove = 0.f;
		Vector m_Origin = Vector(0.f, 0.f, 0.f);
	} m_AutomaticPeek;
public:
	void JumpRelated();
	void Strafe();
	void DoPrespeed();
	bool GetClosestPlane(Vector& plane);
	bool WillCollide(float time, float step);
	void FixMove(QAngle wish_angle);
	void FastStop();
	void AutoPeek();
	void SlowWalk(bool bForcedWalk = false);
	void StopMovement(bool bSlideMovement = false);
	void SlideStop();
	void FakeWalk();
};

inline Movement g_Movement;