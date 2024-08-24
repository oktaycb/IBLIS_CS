#pragma once

class LagRecord {
public:
	Player* m_Player{ nullptr };
	matrix3x4a_t* m_CachedBones{ nullptr };

	struct {
		Vector m_Velocity{ 0.f,0.f,0.f };
		QAngle m_EyeAngles{ 0.f,0.f,0.f };

		float m_FootYaw{ 0.0f };
	} m_Client;

	Vector m_Origin{ 0.f,0.f,0.f };
	Vector m_OldOrigin{ 0.f,0.f,0.f };
	Vector m_Velocity, m_AnimationVelocity{ 0.f,0.f,0.f };
	Vector m_OBBMins{ 0.f,0.f,0.f };
	Vector m_OBBMaxs{ 0.f,0.f,0.f };
	QAngle m_EyeAngles{ 0.f,0.f,0.f };
	QAngle m_AbsAngles{ 0.f,0.f,0.f };

	C_AnimationLayer m_Layers[ANIMATION_LAYER_COUNT];
	float            m_Poses[MAXSTUDIOPOSEPARAM];

	float m_SimulationTime{ 0.0f };
	float m_OldSimulationTime{ 0.0f };
	float m_Immune{ 0.0f };
	float m_LowerbodyYaw{ 0.0f };
	float m_FootYaw{ 0.0f };
	float m_DuckAmount{ 0.0f };
	float m_Lerp{ 0.0f };

	int m_Tick{ 0 };
	int m_ChokedCommands{ 0 };
	int m_Flags{ 0 };
	int m_CachedBoneCount{ 0 };
	int m_CachedWritableBones{ 0 };
	int m_MoveType{ 0 };
	int m_MoveState{ 0 };

	size_t m_ResolverMode{ 0 };

	bool m_Dormant{ false };
	bool m_Setup{ false };
	bool m_BrokeLagCompensation{ false };
	bool m_Legit{ false };
	bool m_Resolved{ false };
	bool m_UnsafeVelocityTransition{ false };
	bool m_FakeWalk{ false };
	bool m_Shot{ false };
	bool m_FakeFlick{ false };
public:
	__forceinline LagRecord() :
		m_Setup{ false },
		m_BrokeLagCompensation{ false },
		m_Lerp{ 0.f },
		m_FakeWalk{ false },
		m_Shot{ false },
		m_ChokedCommands{},
		m_CachedBones{} {}

	__forceinline LagRecord(Player* pPlayer) :
		m_Setup{ false },
		m_BrokeLagCompensation{ false },
		m_Lerp{ 0.f },
		m_FakeWalk{ false },
		m_Shot{ false },
		m_ChokedCommands{},
		m_CachedBones{} {

		Store(pPlayer, true);
	}

	__forceinline ~LagRecord() {
		g_Csgo.m_MemAlloc->Free(m_CachedBones);
	}

	__forceinline void Invalidate() {
		g_Csgo.m_MemAlloc->Free(m_CachedBones);
		m_Setup = false;
		m_CachedBones = (matrix3x4a_t*)g_Csgo.m_MemAlloc->Alloc(sizeof(matrix3x4a_t) * 128);
	}

public:
	void Store(Player* pPlayer, bool allocate = false);
	bool IsValidTime(int iShiftAmount);
};