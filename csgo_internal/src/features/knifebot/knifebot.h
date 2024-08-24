#pragma once

class KnifeBot {
private:
	struct BestTarget_t {
		bool stab;
		QAngle angle; 
		LagRecord* record; 
	};

	struct Table_t {
		uint8_t swing[2][2][2]; // [ first ][ armor ][ back ]
		uint8_t stab[2][2];		// [ armor ][ back ]
	};

	const Table_t m_knife_dmg{ { { { 25, 90 }, { 21, 76 } }, { { 40, 90 }, { 34, 76 } } }, { { 65, 180 }, { 55, 153 } } };

	std::array< QAngle, 12 > m_knife_ang{
		QAngle{ 0.f, 0.f, 0.f }, QAngle{ 0.f, -90.f, 0.f }, QAngle{ 0.f, 90.f, 0.f }, QAngle{ 0.f, 180.f, 0.f },
		QAngle{ -80.f, 0.f, 0.f }, QAngle{ -80.f, -90.f, 0.f }, QAngle{ -80.f, 90.f, 0.f }, QAngle{ -80.f, 180.f, 0.f },
		QAngle{ 80.f, 0.f, 0.f }, QAngle{ 80.f, -90.f, 0.f }, QAngle{ 80.f, 90.f, 0.f }, QAngle{ 80.f, 180.f, 0.f }
	};
public:
	auto Work() -> void;
	auto CanKnife(LagRecord* record, QAngle angle, bool& stab) -> bool;
	auto KnifeTrace(Vector dir, bool stab, CGameTrace* trace) -> bool;
	auto KnifeIsBehind(LagRecord* record) -> bool;
};
extern KnifeBot g_knifebot;