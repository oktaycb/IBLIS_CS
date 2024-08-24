#pragma once

class AdaptiveAngle {
public:
	float m_Yaw;
	float m_Distance;

public:
	__forceinline AdaptiveAngle(float yaw, float penalty = 0.f) {
		m_Yaw = Math::NormalizedAngle(yaw);

		m_Distance = 0.f;
		m_Distance -= penalty;
	}
};

class CAntiAim {
public:
	struct Config {
		int m_Pitch{0}, m_Yaw{ 0 }, m_FakeYaw{ 0 }, m_YawBase{ 0 }, m_DistortionSpeed{ 0 }, m_LowerbodyBreakAmount{ 0 }, m_ManualAA{ 0 };
		float m_JitterRange{ 0.f }, m_RotateRange{ 0.f }, m_RotateSpeed{ 0.f }, m_DesiredAngle{ 0.f }, m_View{ 0.f }, m_DistortionAmount{ 0.f };
		bool  m_Enable{ false }, m_FakeFlick{ false }, m_JitterMode{ false }, m_Freestanding{ false }, m_Left{ false }, m_Right{ false }, m_Back{ false }, m_HideShots{ false }, m_Distortion{ false }, m_LowerbodyBreak{ false };
	} m_Config;
public:
	void AntiAim();
	bool Enabled();

	void FakeFlick();
	void ImposterBreaker();
	void FakeFlickDopium();

	void DoRealAntiAim();
	void DoFakeAntiAim();
	void DesiredYaw();
	void FreeStandYaw();
	bool EdgeYaw(Player* player, QAngle& out);
	void HideShots();
};

inline CAntiAim g_AntiAim;