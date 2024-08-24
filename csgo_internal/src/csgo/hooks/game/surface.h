#pragma once

namespace vgui {
	typedef unsigned long HFont;
	typedef unsigned int VPANEL;
};

enum EFontFlags : int {
	FONTFLAG_NONE = 0,
	FONTFLAG_ITALIC = (1 << 0),
	FONTFLAG_UNDERLINE = (1 << 1),
	FONTFLAG_STRIKEOUT = (1 << 2),
	FONTFLAG_SYMBOL = (1 << 3),
	FONTFLAG_ANTIALIAS = (1 << 4),
	FONTFLAG_GAUSSIANBLUR = (1 << 5),
	FONTFLAG_ROTARY = (1 << 6),
	FONTFLAG_DROPSHADOW = (1 << 7),
	FONTFLAG_ADDITIVE = (1 << 8),
	FONTFLAG_OUTLINE = (1 << 9),
	FONTFLAG_CUSTOM = (1 << 10),
	FONTFLAG_BITMAP = (1 << 11)
};

enum FontDrawType_t {
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum VGuiPanel_t {
	PANEL_ROOT = 0, // buy menu problems
	PANEL_GAMEUIDLL, // only called in main menu
	PANEL_CLIENTDLL, // only called ingame
	PANEL_TOOLS, // perfect
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS
};

struct Vertex_t {
	Vertex_t() { }
	Vertex_t(const Vector2D& pos, const Vector2D& coord = Vector2D(0, 0)) {
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D& pos, const Vector2D& coord = Vector2D(0, 0)) {
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D m_Position;
	Vector2D m_TexCoord;
};

class IEngineVGui {
public:
	virtual	~IEngineVGui(void) {}
	virtual uint32_t GetPanel(VGuiPanel_t type) = 0;
	virtual bool IsGameUIVisible() = 0;
};

class IPanel {
public:
	enum indices : size_t {
		SETMOUSEINPUT = 32,
		GETNAME = 36,
		PAINTTRAVERSE = 41
	};

public:
	__forceinline const char* GetName(vgui::VPANEL vgui_panel) {
		return util::get_method< const char* (__thiscall*)(decltype(this), uint32_t) >(this, GETNAME)(this, vgui_panel);
	}

	void SetMouseInputEnabled(unsigned int iPanel, bool bState) {
		return util::get_method<void(__thiscall*)(PVOID, int, bool) >(this, SETMOUSEINPUT)(this, iPanel, bState);
	}
};

//-----------------------------------------------------------------------------
// Purpose: Wraps contextless windows system functions
//-----------------------------------------------------------------------------
class ISurface {
public:
	enum indices : size_t {
		DRAWSETCOLOR = 14,
		DRAWFILLEDRECT = 16,
		DRAWOUTLINEDRECT = 18,
		DRAWLINE = 19,
		DRAWSETTEXTFONT = 23,
		DRAWSETTEXTCOLOR = 24,
		DRAWSETTEXTPOS = 26,
		DRAWPRINTTEXT = 28,
		DRAWUNICODECHAR = 29,
		DRAWSETTEXTURERGBA = 37,
		DRAWSETTEXTURE = 38,
		DRAWTEXTUREDRECT = 41,
		CREATENEWTEXTUREID = 43,
		GETSCREENSIZE = 44,
		UNLOCKCURSOR = 66,
		LOCKCURSOR = 67,
		CREATEFONT = 71,
		SETFONTGLYPHSET = 72,
		GETTEXTSIZE = 79,
		PLAYSOUND = 82,
		DRAWOUTLINEDCIRCLE = 103,
		DRAWTEXTURDPOLYLINE = 104,
		DRAWTEXTUREDPOLYGON = 106,
		ONSCREENSIZECHANGED = 116,
		DRAWFILLEDRECTFADE = 123,
		DRAWCOLOREDCIRCLE = 162,
	};
public:

	__forceinline void DrawSetColor(Color col) {
		return util::get_method< void(__thiscall*)(decltype(this), Color) >(this, DRAWSETCOLOR)(this, col);
	}

	__forceinline void DrawFilledRect(int x0, int y0, int x1, int y1) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, int, int) >(this, DRAWFILLEDRECT)(this, x0, y0, x1, y1);
	}

	__forceinline void DrawOutlinedRect(int x, int y, int w, int h) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, int, int) >(this, DRAWOUTLINEDRECT)(this, x, y, w, h);
	}

	__forceinline void DrawLine(int x0, int y0, int x1, int y1) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, int, int) >(this, DRAWLINE)(this, x0, y0, x1, y1);
	}

	__forceinline void DrawSetTextFont(vgui::HFont font) {
		return util::get_method< void(__thiscall*)(decltype(this), vgui::HFont) >(this, DRAWSETTEXTFONT)(this, font);
	}

	__forceinline void DrawSetTextColor(Color col) {
		return util::get_method< void(__thiscall*)(decltype(this), Color) >(this, DRAWSETTEXTCOLOR)(this, col);
	}

	__forceinline void DrawSetTextPos(int x, int y) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int) >(this, DRAWSETTEXTPOS)(this, x, y);
	}

	__forceinline  void DrawPrintText(const wchar_t* text, int Length, FontDrawType_t type = FontDrawType_t::FONT_DRAW_DEFAULT) {
		return util::get_method< void(__thiscall*)(decltype(this), const wchar_t*, int, FontDrawType_t) >(this, DRAWPRINTTEXT)(this, text, Length, type);
	}

	__forceinline void DrawUnicodeChar(wchar_t wchar, FontDrawType_t type = FontDrawType_t::FONT_DRAW_DEFAULT) {
		return util::get_method< void(__thiscall*)(decltype(this), wchar_t, FontDrawType_t) >(this, DRAWUNICODECHAR)(this, wchar, type);
	}

	__forceinline void DrawSetTextureRGBA(int id, Color* rgba, int wide, int tall) {
		return util::get_method< void(__thiscall*)(decltype(this), int, Color*, int, int) >(this, DRAWSETTEXTURERGBA)(this, id, rgba, wide, tall);
	}

	__forceinline void DrawSetTextureRGBA(int id, const uint8_t* rgba, int wide, int tall) {
		return util::get_method< void(__thiscall*)(decltype(this), int, const uint8_t*, int, int) >(this, DRAWSETTEXTURERGBA)(this, id, rgba, wide, tall);
	}

	__forceinline void DrawSetTexture(int id) {
		return util::get_method< void(__thiscall*)(decltype(this), int) >(this, DRAWSETTEXTURE)(this, id);
	}

	__forceinline void DrawTexturedRect(int x0, int y0, int x1, int y1) {
		return util::get_method< void(__thiscall*)(void*, int, int, int, int) >(this, DRAWTEXTUREDRECT)(this, x0, y0, x1, y1);
	}

	__forceinline int CreateNewTextureID(bool procedural = false) {
		return util::get_method< int(__thiscall*)(decltype(this), bool) >(this, CREATENEWTEXTUREID)(this, procedural);
	}

	__forceinline void UnlockCursor() {
		return util::get_method< void(__thiscall*)(decltype(this))>(this, UNLOCKCURSOR)(this);
	}

	__forceinline vgui::HFont FontCreate() {
		return util::get_method< vgui::HFont(__thiscall*)(decltype(this))>(this, CREATEFONT)(this);
	}

	__forceinline  bool SetFontGlyphSet(vgui::HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int range_min = 0, int range_max = 0) {
		return util::get_method< bool(__thiscall*)(decltype(this), vgui::HFont, const char*, int, int, int, int, int, int, int) >(this, SETFONTGLYPHSET)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, range_min, range_max);
	}

	__forceinline void GetTextSize(vgui::HFont font, const wchar_t* text, int& wide, int& tall) {
		return util::get_method < void(__thiscall*)(decltype(this), vgui::HFont, const wchar_t*, int&, int&) >(this, GETTEXTSIZE)(this, font, text, wide, tall);
	}

	__forceinline void PlaySound(const char* sample) {
		return util::get_method<void(__thiscall*)(decltype(this), const char*)>(this, PLAYSOUND)(this, sample);
	}

	__forceinline void DrawOutlinedCircle(int x, int y, int radius, int segments) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, int, int) >(this, DRAWOUTLINEDCIRCLE)(this, x, y, radius, segments);
	}

	__forceinline void DrawFilledRectFade(int x0, int y0, int x1, int y1, uint32_t alpha0, uint32_t alpha1, bool bHorizontal) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, int, int, uint32_t, uint32_t, bool) >(this, DRAWFILLEDRECTFADE)(this, x0, y0, x1, y1, alpha0, alpha1, bHorizontal);
	}

	__forceinline void DrawTexturedPolyLine(int count, Vertex_t* verts) {
		return util::get_method< void(__thiscall*)(decltype(this), Vertex_t*, int) >(this, DRAWTEXTURDPOLYLINE)(this, verts, count);
	}

	__forceinline void DrawTexturedPolygon(int count, Vertex_t* verts, bool unk = false) {
		return util::get_method< void(__thiscall*)(decltype(this), int, Vertex_t*, bool) >(this, DRAWTEXTUREDPOLYGON)(this, count, verts, unk);
	}

	__forceinline void DrawColoredCircle(int x, int y, float radius, int r, int g, int b, int a) {
		return util::get_method< void(__thiscall*)(decltype(this), int, int, float, int, int, int, int) >(this, DRAWCOLOREDCIRCLE)(this, x, y, radius, r, g, b, a);
	}
};

class IVDebugOverlay {
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, const char* format, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, const char* format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const Vector& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector& origin) = 0;
	virtual void AddCoordFrameOverlay(const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
	virtual int ScreenPosition(const Vector& point, Vector& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
	virtual void* GetFirst(void) = 0;
	virtual void* GetNext(void* current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays(void) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, Vector const& orientation, const Color faceColor, const Color edgeColor, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, int a, float, float) = 0;
	virtual void PurgeTextOverlays() = 0;
	virtual void AddCapsuleOverlay(const Vector& mins, const Vector& max, float& radius, int r, int g, int b, int a, float duration, char unknown, char ignorez) = 0;
};