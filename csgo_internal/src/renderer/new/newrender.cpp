#include "../../precompiled.h"
#include "../../gui/menu/other/blur.hpp"
#include "fonts.h"

void CRender::Init()
{
	if (!g_Csgo.m_Device)
		return;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

	g_Menu.InitializeStyle();

	if (!ImGui_ImplWin32_Init(g_Csgo.m_Window))
		return;

	if (!ImGui_ImplDX9_Init(g_Csgo.m_Device))
		return;

	g_Hooks.m_old_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Csgo.m_Window, GWLP_WNDPROC, util::force_cast<LONG_PTR>(Hooks::WndProc)));

	m_DrawList = new ImDrawList(ImGui::GetDrawListSharedData());
	m_DrawListAct = new ImDrawList(ImGui::GetDrawListSharedData());
	m_DrawListRender = new ImDrawList(ImGui::GetDrawListSharedData());

	Fonts::InitFonts();
}

void CRender::InitD3D(IDirect3DDevice9* device) noexcept
{
	if (device == nullptr) {
		MessageBox(0, XOR("IDirect3DDevice9 is null! did you define it correctly?"), 0, 0);
		return;
	}

	m_Device = device;
	BlurData::device = device;
	device->GetViewport(&m_ViewPort);
	m_Initialized = true;
}

void CRender::SceneStart() const noexcept
{
	m_DrawList->_ResetForNewFrame();
	m_DrawList->PushClipRectFullScreen();
	m_DrawList->Flags = ImDrawListFlags_AntiAliasedLines | ImDrawListFlags_AntiAliasedFill;
}

void CRender::SceneEnd() noexcept
{
	m_RenderMutex.lock();
	*m_DrawListAct = *m_DrawList;
	m_RenderMutex.unlock();
}

ImDrawList* CRender::RenderScene() noexcept
{
	if (m_RenderMutex.try_lock())
	{
		*m_DrawListRender = *m_DrawListAct;
		m_RenderMutex.unlock();
	}

	return m_DrawListRender;
}

void CRender::ClearDrawList() noexcept
{
	m_RenderMutex.lock();
	m_DrawListAct->_ResetForNewFrame();
	m_RenderMutex.unlock();
}

void CRender::OnInvalidate() const noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void CRender::OnCreate() const noexcept
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void CRender::SetupRenderStates() const noexcept
{
	D3DVIEWPORT9 view_port;
	m_Device->GetViewport(&view_port);

	m_Device->SetVertexShader(nullptr);
	m_Device->SetPixelShader(nullptr);

	m_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	m_Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_Device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	m_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	m_Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	m_Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
		D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

Vector2D CRender::TextSize(const std::string_view& text, const ImFont* font) const noexcept
{
	const ImVec2& size = font->CalcTextSizeA(font->FontSize, std::numeric_limits<float>::max(), 0.0f, text.data());
	return { size.x, size.y };
}

void CRender::Circle(const Vector2D& centre, const float& radius, const Color& color, const float thickness) const noexcept
{
	m_DrawList->AddCircle({ centre.x, centre.y }, radius, get_u32(color), 12, thickness);
}

void CRender::CircleFilled(const Vector2D& centre, const float& radius, const Color& color) const noexcept
{
	m_DrawList->AddCircleFilled({ centre.x, centre.y }, radius, get_u32(color));
}

void CRender::Circle3D(const Vector& origin, const float& radius, Color color, const bool& filled) const noexcept
{
	static float step = Math::pi_2 / 72.f;
	static Vector  dummy_vec = {};

	Vector screen_pos = {};

	if (g_Csgo.m_DebugOverlay->ScreenPosition(origin, screen_pos))
		return;

	for (float rotation = 0.f; rotation <= Math::pi_2; rotation += step)
	{
		Vector world_pos = Vector(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);
		
		if (g_Csgo.m_DebugOverlay->ScreenPosition(world_pos, screen_pos))
			continue;

		Line(dummy_vec.x, dummy_vec.y, screen_pos.x, screen_pos.y, color, 1.0f);

		if (filled) {

			Vector centre;
			if (g_Csgo.m_DebugOverlay->ScreenPosition(origin, centre))
				continue;

			Triangle
			(
				Vector2D(screen_pos.x, screen_pos.y),
				Vector2D(screen_pos.x, screen_pos.y),
				Vector2D(centre.x, centre.y),
				Color(color.r(), color.g(), color.b(), color.a() / 4)
			);
		}

		dummy_vec = screen_pos;
	}
}

void CRender::Line(const float& x, const float& y, const float& w, const float& h, const Color& color, const float& thickness) const noexcept
{
	m_DrawList->AddLine(ImVec2(x, y), ImVec2(w, h), get_u32(color), thickness);
}

void CRender::Line(const Vector2D& start, const Vector2D& end, const Color& color, const float& thickness) const noexcept
{
	m_DrawList->AddLine({ start.x, start.y }, { end.x, end.y }, get_u32(color), thickness);
}

void CRender::Rect(const Vector2D& start, const Vector2D& end, const Color& color, const float& thickness) const noexcept
{
	m_DrawList->AddRect({ start.x, start.y }, { end.x, end.y }, get_u32(color), 0.0f, ImDrawFlags_Closed, thickness);
}

void CRender::Rect(const float& x, const float& y, const float& w, const float& h, const Color& color, const float& thickness) const noexcept {
	m_DrawList->AddRect({ x, y }, { x + w, y + h }, get_u32(color), 0.0f, ImDrawFlags_Closed, thickness);
}

void CRender::RectFilled(const Vector2D& start, const Vector2D& end, const Color& color) const noexcept
{
	m_DrawList->AddRectFilled({ start.x, start.y }, { end.x, end.y }, get_u32(color));
}

void CRender::RectFilled(const float& x, const float& y, const float& w, const float& h, const Color& color) const noexcept
{
	m_DrawList->AddRectFilled({ x, y }, { x + w, y + h }, get_u32(color));
}

void CRender::RectFilled3D(const Vector& centre, const Vector2D& size, const Color& color, int angle) const noexcept {
	Vector add_top_left, add_top_right,
		add_bot_left, add_bot_right;

	switch (angle) {
	case 0: // Z-AXIS
		add_top_left = Vector(-size.x, -size.y, 0.f);
		add_top_right = Vector(size.x, -size.y, 0.f);
		//
		add_bot_left = Vector(-size.x, size.y, 0.f);
		add_bot_right = Vector(size.x, size.y, 0.f);

		break;
	case 1: // Y-AXIS
		add_top_left = Vector(-size.x, 0.f, -size.y);
		add_top_right = Vector(size.x, 0.f, -size.y);
		//
		add_bot_left = Vector(-size.x, 0.f, size.y);
		add_bot_right = Vector(size.x, 0.f, size.y);

		break;
	case 2: // X-AXIS
		add_top_left = Vector(0.f, -size.y, -size.x);
		add_top_right = Vector(0.f, -size.y, size.x);
		//
		add_bot_left = Vector(0.f, size.y, -size.x);
		add_bot_right = Vector(0.f, size.y, size.x);

		break;
	}

	//top line
	Vector c_top_left = centre + add_top_left;
	Vector c_top_right = centre + add_top_right;

	//bottom line
	Vector c_bot_left = centre + add_bot_left;
	Vector c_bot_right = centre + add_bot_right;

	Vector2D top_left, top_right, bot_left, bot_right;
	if (Math::WorldToScreen(c_top_left, top_left) &&
		Math::WorldToScreen(c_top_right, top_right) &&
		Math::WorldToScreen(c_bot_left, bot_left) &&
		Math::WorldToScreen(c_bot_right, bot_right)) {

		Vertex_t vertices[4];
		static int tex = g_Csgo.m_Surface->CreateNewTextureID(true);

		g_Csgo.m_Surface->DrawSetTexture(tex);
		g_Csgo.m_Surface->DrawSetColor(color);

		vertices[0].Init(Vector2D(top_left.x, top_left.y));
		vertices[1].Init(Vector2D(top_right.x, top_right.y));
		vertices[2].Init(Vector2D(bot_right.x, bot_right.y));
		vertices[3].Init(Vector2D(bot_left.x, bot_left.y));

		g_Csgo.m_Surface->DrawTexturedPolygon(4, vertices, true);
	}
}

void CRender::CornerBox(const float& x, const float& y, const float& w, const float& h, const Color& color, const Color& outline_color) {

	float w1 = (w / 5.f);
	float h1 = (h / 6.f);

	// outline.
	Line(x - 1.f, y - 1.f, x + w1, y - 1.f, outline_color); // top left.
	Line(x - 1.f, y - 1.f, x - 1.f, y + h1, outline_color);
	Line(x - 1.f, y + h - h1, x - 1.f, y + h + 1.f, outline_color); // bottom left.
	Line(x - 1.f, y + h + 1.f, x + w1, y + h + 1.f, outline_color);
	Line(x + w - w1, y - 1.f, x + w + 1.f, y - 1.f, outline_color); // top right.
	Line(x + w + 1.f, y - 1.f, x + w + 1.f, y + h1, outline_color);
	Line(x + w + 1.f, y + h - h1, x + w + 1.f, y + h + 2.f, outline_color); // bottom right.
	Line(x + w - w1, y + h + 1.f, x + w + 1.f, y + h + 1.f, outline_color);

	// inline.
	Line(x, y, x, y + h1, color); // top left.
	Line(x, y, x + w1, y, color);
	Line(x + w - w1, y, x + w, y, color); // top right.
	Line(x + w, y, x + w, y + h1, color);
	Line(x, y + h - h1, x, y + h, color); // bottom left.
	Line(x, y + h, x + w1, y + h, color);
	Line(x + w - w1, y + h, x + w, y + h, color); // bottom right.
	Line(x + w, y + h - h1, x + w, y + h + 1.f, color);
}

void CRender::Box3D(const Vector& top, const Vector& bottom, const float& angle, const int& width, const Color& color, const float& thickness) const noexcept
{
	Vector b1, b2, b3, b4, t1, t2, t3, t4;

	int height = abs(top.z - bottom.z);

	b1.z = b2.z = b3.z = b4.z = bottom.z;

	b1.x = bottom.x + (cos(Math::DEG2RAD(angle + 45)) * width);
	b1.y = bottom.y + (sin(Math::DEG2RAD(angle + 45)) * width);

	b2.x = bottom.x + (cos(Math::DEG2RAD(angle + 135)) * width);
	b2.y = bottom.y + (sin(Math::DEG2RAD(angle + 135)) * width);

	b3.x = bottom.x + (cos(Math::DEG2RAD(angle + 225)) * width);
	b3.y = bottom.y + (sin(Math::DEG2RAD(angle + 225)) * width);

	b4.x = bottom.x + (cos(Math::DEG2RAD(angle + 315)) * width);
	b4.y = bottom.y + (sin(Math::DEG2RAD(angle + 315)) * width);

	t1.x = b1.x;
	t1.y = b1.y;
	t1.z = b1.z + height;

	t2.x = b2.x;
	t2.y = b2.y;
	t2.z = b2.z + height;

	t3.x = b3.x;
	t3.y = b3.y;
	t3.z = b3.z + height;

	t4.x = b4.x;
	t4.y = b4.y;
	t4.z = b4.z + height;

	Vector2D box1, box2, box3, box4, boxt1, boxt2, boxt3, boxt4;

	if (Math::WorldToScreen(b1, box1) && Math::WorldToScreen(b2, box2) && Math::WorldToScreen(b3, box3) && Math::WorldToScreen(b4, box4) && Math::WorldToScreen(t1, boxt1) && Math::WorldToScreen(t2, boxt2) && Math::WorldToScreen(t3, boxt3) && Math::WorldToScreen(t4, boxt4)) {

		//collumns
		Line(boxt1, box1, color, thickness);
		Line(boxt2, box2, color, thickness);
		Line(boxt3, box3, color, thickness);
		Line(boxt4, box4, color, thickness);

		//top base
		Line(boxt1, boxt2, color, thickness);
		Line(boxt2, boxt3, color, thickness);
		Line(boxt3, boxt4, color, thickness);
		Line(boxt4, boxt1, color, thickness);

		//bottom base
		Line(box1, box2, color, thickness);
		Line(box2, box3, color, thickness);
		Line(box3, box4, color, thickness);
		Line(box4, box1, color, thickness);
	}
}

void CRender::Triangle(const Vector2D& first, const Vector2D& second, const Vector2D& third, const Color& color) const noexcept
{
	m_DrawList->AddTriangleFilled({ first.x, first.y }, { second.x, second.y }, { third.x, third.y }, get_u32(color));
}

void CRender::PollyLine(const ImVec2* points, const int& points_count, const Color& color) const noexcept
{
	m_DrawList->AddPolyline(points, points_count, get_u32(color), ImDrawFlags_Closed, 1.f);
}

void CRender::PollyFilled(const ImVec2* points, const int& points_count, const Color& color) const noexcept
{
	m_DrawList->AddConvexPolyFilled(points, points_count, get_u32(color));
}

void CRender::Arc(const Vector2D& centre, const float& radius, const float& min, const float& max, const Color& color, const float& thickness) const noexcept
{
	m_DrawList->PathArcTo({ centre.x, centre.y }, radius, Math::DEG2RAD(min), Math::DEG2RAD(max), 32);
	m_DrawList->PathStroke(get_u32(color), ImDrawFlags_None, thickness);
}

void CRender::String(const Vector2D& pos, const ImFont* font, const Color& color, std::string_view text) const noexcept
{
	if (!font->ContainerAtlas)
		return;

	m_DrawList->PushTextureID(font->ContainerAtlas->TexID);

	m_DrawList->AddText(font, font->FontSize, { pos.x, pos.y }, get_u32(color), text.data());
	m_DrawList->PopTextureID();
}

void CRender::StringShadow(const Vector2D& pos, const ImFont* font, const Color& color, const Color& color2, std::string_view text) const noexcept
{
	this->String(pos + 1.0f, font, color, text);
	this->String(pos, font, color2, text);
}

#include "../../gui/imgui/freetype/imgui_freetype.h"
void CRender::Fonts::InitFonts() noexcept
{
	ImFontConfig esp;
	esp.SizePixels = 8.f;
	esp.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bitmap;

	ImFontConfig bold;
	bold.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_Bitmap;

	m_menu_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::sfcompactrounded_compressed_data, fonts::sfcompactrounded_compressed_size, 16.f);
	m_esp_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::miniline_compressed_data, fonts::miniline_compressed_size, esp.SizePixels, &esp);
	m_esp_small_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::pixel7_compressed_data, fonts::pixel7_compressed_size, 10.0f);
	m_big_bold_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::tahoma_compressed_data, fonts::tahoma_compressed_size, 16.0f, &bold);
	m_weapon_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::weapon_compressed_data, fonts::weapon_compressed_size, 14.0f, &esp);

	m_MenuFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::sfcompactrounded_compressed_data, fonts::sfcompactrounded_compressed_size, 15.f, &esp);
	m_MenuFontSmall = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::sfcompactrounded_compressed_data, fonts::sfcompactrounded_compressed_size, 12.f, &esp);
	m_MenuFontInner = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::sfcompactrounded_compressed_data, fonts::sfcompactrounded_compressed_size, 15.f);
	m_MenuBigFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::sfcompactrounded_compressed_data, fonts::sfcompactrounded_compressed_size, 20.f, &esp);
	m_MenuIconFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::prime_compressed_data, fonts::prime_compressed_size, 80.f, &esp);
	m_MenuWatermarkIcon = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::prime_compressed_data, fonts::prime_compressed_size, 20.f, &esp);
	m_MenuIconSmallFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::prime_compressed_data, fonts::prime_compressed_size, 45.f, &esp);
	m_MenuSubtabIcon = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::subtabfont_compressed_data, fonts::subtabfont_compressed_size, 25.f, &esp);
	m_MenuIconXSmallFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fonts::prime_compressed_data, fonts::prime_compressed_size, 17.f, &esp);
}