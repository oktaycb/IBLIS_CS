#pragma once

class ShotRecord {
public:
	__forceinline ShotRecord() : m_WantedHitbox{}, m_ServerHitgroup{}, m_CurrentTime{}, m_Latency{}, m_Damage{}, m_Player{}, m_Record{}, m_StartPosition{}, m_EndPosition{}, m_Impacts{}, m_DidIntersectPlayer{}, m_HasBeenFired{}, m_HasBeenRegistered{}, m_HasBeenHurted{}, m_ShotTick{} {}

public:
	Player*					m_Player = nullptr;
	LagRecord*				m_Record = nullptr;
	Vector					m_StartPosition = Vector(0.f, 0.f, 0.f), m_EndPosition = Vector(0.f, 0.f, 0.f);
	Hitboxes_t				m_WantedHitbox = Hitboxes_t::INVALID;
	Hitgroups_t				m_ServerHitgroup = Hitgroups_t::UKNOWN;
	std::vector < Vector >	m_Impacts = {};

	int m_ShotTick = 0;
	float m_Damage = 0.f;
	float m_CurrentTime = 0.f;
	float m_Latency = 0.f;

	bool m_DidIntersectPlayer = false;
	bool m_HasBeenFired = false;
	bool m_HasBeenRegistered = false;
	bool m_HasBeenHurted = false;
};

class VisualImpactData_t {
public:
	Vector m_impact_pos, m_EyePosition;
	int    m_tickbase;
	bool   m_ignore, m_hit_player;

public:
	__forceinline VisualImpactData_t(const Vector& impact_pos, const Vector& shoot_pos, int tickbase) :
		m_impact_pos{ impact_pos }, m_EyePosition{ shoot_pos }, m_tickbase{ tickbase }, m_ignore{ false }, m_hit_player{ false } {}
};

struct DamageIndicator_t {
	std::string m_Output = {};
	Color m_Color = {};
	Vector m_Position = {};
	float m_AnimationX = 0.f, m_AnimationY = 0.f, m_RandAnimAmount = 0.f, m_RandAnimVel = 0.f, m_EraseTime = 0.0f;
	int m_Damage = 0, m_Alpha = 0;
	bool m_Complete = false;
};

class ImpactRecord {
public:
	__forceinline ImpactRecord() : m_Shot{}, m_Position{}, m_Tick{}, m_hit{} {}

public:
	ShotRecord* m_Shot;
	int         m_Tick;
	Vector      m_Position;
	bool		m_hit;
};

class HitRecord {
public:
	__forceinline HitRecord() : m_impact{}, m_group{ -1 }, m_Damage{} {}

public:
	ImpactRecord* m_impact;
	int           m_group;
	float         m_Damage;
};

class Shots {
public:
	void OnShotFire(Player* target, float damage, int bullets, LagRecord* record, Hitboxes_t Hitbox, Vector m_hitbox_pos);
	void OnBulletImpact(IGameEvent* evt);
	void OnHurt(IGameEvent* evt);
	void OnWeaponFire(IGameEvent* evt);
	void OnFrameStage();

public:
	std::array< std::string, 11 > m_Groups = {
	   XOR("body"),
	   XOR("head"),
	   XOR("chest"),
	   XOR("stomach"),
	   XOR("left arm"),
	   XOR("right arm"),
	   XOR("left leg"),
	   XOR("right leg"),
	   XOR("neck"),
	   XOR("unknown"),
	   XOR("gear")
	};

	std::deque< ShotRecord >          m_Shots;
	std::vector< DamageIndicator_t >  m_DmgIndicator;
	std::vector< VisualImpactData_t > m_vis_impacts;
	std::deque< HitRecord >           m_hits;

	float m_Damage = 0.f;
	float m_HitmarkerEraseTime = 0.f;
	float m_HitmarkerSize = 0.f;
	int m_HitmarkerAlpha = 0;
	bool m_CompleteHitmarkerSize = false;
	bool m_hit = false;
	bool m_headshot = false;
	bool m_can_shit = false;

	Vector m_Origin, m_OBBMins, m_OBBMaxs;
	Vector2D m_player_bottom, m_player_top;
};

inline Shots g_ShotSystem;