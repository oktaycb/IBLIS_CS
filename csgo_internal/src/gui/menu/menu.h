#pragma once

class Menu
{
public:
	void Render();
	void InitializeStyle() const noexcept;
	void Uninitialize() const;
	void DarkModeIndicators();
	void LightModeIndicators();
	void HandleTabs() noexcept;
	void HandleSubTabsRage();
	void HandleSubTabsAntiAim();
	void HandleSubTabsVisual();

	[[nodiscard("You probably called IsOpen() in a wrong way!")]] bool& IsOpen() noexcept
	{
		return m_Open;
	}
private:
	enum class tabs
	{
		TAB_RAGE,
		TAB_LEGIT,
		TAB_ANTIAIM,
		TAB_VISUALS,
		TAB_MISC
	};

	enum class sub_tabs
	{
		// Rage
		SUB_GENERAL,
		SUB_PISTOL,
		SUB_HEAVY_PISTOL,
		SUB_RIFLE,
		SUB_SCOUT,
		SUB_AWP,
		SUB_AUTO,
		SUB_TASER,

		// Visuals
		SUB_ENEMY,
		SUB_LOCAL,
		SUB_TEAM,
		SUB_WORLD,
		SUB_SKIN,

		// Anti aim
		SUB_STAND,
		SUB_MOVING,
		SUB_AIR
	};
private:
	void Rage();
	void LegitBot();
	void AntiAim();
	void Visuals();
	void Misc();
public:
	float m_TabAnimation = 0.f;
	float easiedm_TabAnimation = 0.f;
	float m_SubTabAnimationRage = 0.f;
	float m_SubTabAnimationVisuals = 0.f;
	float m_SubTabAnimationAntiAim = 0.f;
	bool darktheme = true;
private:
	bool m_Open = true;
	tabs m_CurrentTab = tabs::TAB_MISC; // (for now.. maybe wouldn't touch it.)
	sub_tabs m_CurrentSubTabRage = sub_tabs::SUB_GENERAL;
	sub_tabs m_CurrentSubTabVisual = sub_tabs::SUB_ENEMY;
	sub_tabs m_CurrentSubTabAntiAim = sub_tabs::SUB_STAND;
private:
	__forceinline static ImVec4 RGBToFloat(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
		return ImVec4(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f, static_cast<float>(a) / 255.f);
	}
};

inline Menu g_Menu;