#pragma once
#include <mutex>

class CRender
{
public:
	~CRender() {
		delete m_DrawList;
		delete m_DrawListRender;
		delete m_DrawListAct;
	}

	struct Fonts
	{
		static void   InitFonts() noexcept;
		static inline ImFont* m_menu_font = nullptr;
		static inline ImFont* m_big_bold_font = nullptr;
		static inline ImFont* m_esp_font = nullptr;
		static inline ImFont* m_esp_small_font = nullptr;
		static inline ImFont* m_weapon_font = nullptr;

		static inline ImFont* m_MenuFont = nullptr;
		static inline ImFont* m_MenuFontSmall = nullptr;
		static inline ImFont* m_MenuFontInner = nullptr;
		static inline ImFont* m_MenuBigFont = nullptr;
		static inline ImFont* m_MenuIconFont = nullptr;
		static inline ImFont* m_MenuWatermarkIcon = nullptr;
		static inline ImFont* m_MenuIconSmallFont = nullptr;
		static inline ImFont* m_MenuIconXSmallFont = nullptr;
		static inline ImFont* m_MenuSubtabIcon = nullptr;
		static inline ImFont* m_IndicatorsIconFont = nullptr;
	};
public:
	void Init();
	void InitD3D(IDirect3DDevice9* device) noexcept;
	ImDrawList* RenderScene() noexcept;
	void SceneStart() const noexcept;
	void SceneEnd() noexcept;
	void ClearDrawList() noexcept;
	void OnInvalidate() const noexcept;
	void OnCreate() const noexcept;
	void SetupRenderStates() const noexcept;
	
	Vector2D TextSize(const std::string_view& text, const ImFont* font) const noexcept;
	void Circle(const Vector2D& centre, const float& radius, const Color& color, float thickness = 1.0f) const noexcept;
	void CircleFilled(const Vector2D& centre, const float& radius, const Color& color) const noexcept;
	void Circle3D(const Vector& origin, const float& radius, Color color, const bool& filled) const noexcept;
	void Line(const float& x, const float& y, const float& w, const float& h, const Color& color, const float& thickness = 1.0f) const noexcept;
	void Line(const Vector2D& start, const Vector2D& end, const Color& color, const float& thickness = 1.0f) const noexcept;
	void Rect(const Vector2D& start, const Vector2D& end, const Color& color, const float& thickness = 1.0f) const noexcept;
	void Rect(const float& x, const float& y, const float& w, const float& h, const Color& color, const float& thickness = 1.0f) const noexcept;
	void RectFilled(const Vector2D& start, const Vector2D& end, const Color& color) const noexcept;
	void RectFilled(const float& x, const float& y, const float& w, const float& h, const Color& color) const noexcept;
	void RectFilled3D(const Vector& centre, const Vector2D& size, const Color& color, int angle) const noexcept;
	void CornerBox(const float& x, const float& y, const float& w, const float& h, const Color& color, const Color& outline_color);
	void Box3D(const Vector& top, const Vector& bottom, const float& angle, const int& width, const Color& color, const float& thickness = 1.0f) const noexcept;
	void Triangle(const Vector2D& first, const Vector2D& second, const Vector2D& third, const Color& color) const noexcept;
	void PollyLine(const ImVec2* points, const int& points_count, const Color& color) const noexcept;
	void PollyFilled(const ImVec2* points, const int& points_count, const Color& color) const noexcept;
	void Arc(const Vector2D& centre, const float& radius, const float& min, const float& max, const Color& color, const float& thickness = 1.0f) const noexcept;
	void String(const Vector2D& pos, const ImFont* font, const Color& color, std::string_view text) const noexcept;
	void StringShadow(const Vector2D& pos, const ImFont* font, const Color& color, const Color& color_2, std::string_view text) const noexcept;

	[[nodiscard("You probably called IsInitialized() in a wrong way!")]] bool IsInitialized() const noexcept
	{
		return m_Initialized;
	}

	ImU32 get_u32(Color color) const noexcept
	{
		return ((color.a() & 0xff) << 24) + ((color.b() & 0xff) << 16) + ((color.g() & 0xff) << 8) + (color.r() & 0xff);
	}
private:
	bool m_Initialized = false;
	IDirect3DDevice9* m_Device = nullptr;
	D3DVIEWPORT9 m_ViewPort = {};
	ImDrawList* m_DrawList = nullptr;
	ImDrawList* m_DrawListAct = nullptr;
	ImDrawList* m_DrawListRender = nullptr;
	std::mutex  m_RenderMutex = {};
};

inline CRender new_render;