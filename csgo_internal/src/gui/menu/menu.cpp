#include "../../precompiled.h"
#include "other/blur.hpp"
#include "other/easing.h"
#include "other/testmenu.h"
#include "char items.h"
#include <shellapi.h>

static bool loaded = false;
static bool settingsopen = false;
static bool loadlocation = true;//currently shit
static bool savelocation = false;//currently shit

void Menu::InitializeStyle() const noexcept
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::StyleColorsDark();

	style.Alpha = 1.0f;
	style.WindowRounding = 0.f;
	style.FramePadding = { 4, 3 };
	style.WindowPadding = { 8, 8 };
	style.ItemInnerSpacing = { 4, 4 };
	style.ItemSpacing = { 8, 4 };
	style.FrameRounding = 0.f;
	style.ScrollbarSize = 2.f;
	style.ScrollbarRounding = 0.f;
	style.ChildRounding = 0.f;

	style.Colors[ImGuiCol_ChildBg] = RGBToFloat(30, 30, 30, 255 / 2);
	style.Colors[ImGuiCol_Border] = RGBToFloat(50, 50, 50, 255 / 2);
	style.Colors[ImGuiCol_FrameBg] = RGBToFloat(30, 30, 30, 255/2);
	style.Colors[ImGuiCol_PopupBg] = RGBToFloat(26, 26, 26, 220);
	style.Colors[ImGuiCol_ScrollbarBg] = RGBToFloat(15, 15, 15, 255 / 2);
	style.Colors[ImGuiCol_ScrollbarGrab] = RGBToFloat(26, 26, 26, 255 / 2);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = RGBToFloat(55, 55, 55, 255 / 2);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = RGBToFloat(55, 55, 55, 255 / 2);
}

void Menu::Uninitialize() const
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::DarkModeIndicators() {

	if (g_Config.i(g_Items.m_Theme) > 0 || !loaded)
		return;

	auto easingFunction = getEasingFunction(easing_functions::EaseInOutQuint); // easing float/double 255 degil 1.f

	keybinds.clear();

	keybinds.emplace_back(std::string("Third person").substr(0, 16), g_Config.i(g_Items.m_Visuals.m_ThirdpersonKeyMode), (g_Config.b(g_Items.m_Visuals.m_ThirdpersonEnabled) && g_KeyInput.Key(g_Items.m_Visuals.m_ThirdpersonKey, g_Items.m_Visuals.m_ThirdpersonKeyMode)));

	keybinds.emplace_back(std::string("Rapid fire").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_TickbaseManipulation) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_TickbaseManipulationKey, g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode)));

	keybinds.emplace_back(std::string("Wait resolved").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdate) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateKey, g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode)));

	keybinds.emplace_back(std::string("Hit chance").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideHitChance) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideHitChanceKey, g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode)));

	keybinds.emplace_back(std::string("Minimum damage").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideDamageMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideDamage) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideDamageKey, g_Items.m_Aimbot.m_General.m_OverrideDamageMode)));

	keybinds.emplace_back(std::string("Force body").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_ForceBodyMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_ForceBody) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_ForceBodyKey, g_Items.m_Aimbot.m_General.m_ForceBodyMode)));

	keybinds.emplace_back(std::string("Automatic peek").substr(0, 25), g_Config.i(g_Items.m_Misc.m_AutomaticPeekKeyBindsMode), (g_Config.b(g_Items.m_Misc.m_AutomaticPeek) && g_KeyInput.Key(g_Items.m_Misc.m_AutomaticPeekKeyBinds, g_Items.m_Misc.m_AutomaticPeekKeyBindsMode)));

	keybinds.emplace_back(std::string("Fake walk").substr(0, 16), g_Config.i(g_Items.m_Misc.m_FakeWalkBindsMode), (g_Config.b(g_Items.m_Misc.m_FakeWalk) && g_KeyInput.Key(g_Items.m_Misc.m_FakeWalkKeyBinds, g_Items.m_Misc.m_FakeWalkBindsMode)));

	keybinds.emplace_back(std::string("Fake latency").substr(0, 16), g_Config.i(g_Items.m_Misc.m_FakeLatencyMode), (g_Config.b(g_Items.m_Misc.m_FakeLatency) && g_KeyInput.Key(g_Items.m_Misc.m_FakeLatencyKey, g_Items.m_Misc.m_FakeLatencyMode)));

	//Key bind List
	bool keybindlist = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(6);
	if (loadlocation) {
		ImGui::SetWindowPos(ImVec2(g_Config.i(g_Items.m_KeybindX), g_Config.i(g_Items.m_KeybindY)));
		loadlocation = false;
	}
	ImGui::Begin("#KeybindList", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		//definition main
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImGui::PushFont(CRender::Fonts::m_menu_font); auto inditextsize = ImGui::CalcTextSize("Key Bind List"); ImGui::PopFont();
		static float maxtextlength = inditextsize.x + 20;
		int i2 = keybinds.size() - 1;
		static float animatedmaxlength;

		if (animatedmaxlength > maxtextlength) {
			animatedmaxlength -= (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		else if (animatedmaxlength < maxtextlength) {
			animatedmaxlength += (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		animatedmaxlength = std::clamp(animatedmaxlength, 0.f, maxtextlength);

		//definition misc
		ImVec2 size = ImVec2(animatedmaxlength + 70, 40 + 20 * (i2 + 1) + 90);
		ImVec2 pos = ImGui::GetWindowPos();
		if (savelocation) {
			g_Config.i(g_Items.m_KeybindX) = pos.x;
			g_Config.i(g_Items.m_KeybindY) = pos.y;
			savelocation = false;
		}
		ImGui::SetWindowSize(size);

		//animation
		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f * 2) * ((keybindlist) ? 1 : -1)), 0.f, 255.f);

		//DrawBackgroundBlur(draw, alpha);

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() / 255 * alpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() / 255 * alpha);
		ImColor backgroundcolor = ImColor(15,15, 15, alpha);

		static float posanim[20];
		static int posy;
		posy = -1;

		ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + (size.x / 2) - (inditextsize.x / 2), pos.y + 30 / 2 - inditextsize.y / 2), ImColor(255, 255, 255, alpha), "Key bind List"); ImGui::PopFont();//indicator text

		for (int i = keybinds.size() - 1; i >= 0; i--)
		{
			if (keybindlist) {

				if (keybinds[i].enabled) {
					keybinds[i].alpha = std::clamp(keybinds[i].alpha + ((ImGui::GetIO().DeltaTime * 2.f)), 0.f, 1.f);
				}
				else {
					keybinds[i].alpha = std::clamp(keybinds[i].alpha - ((ImGui::GetIO().DeltaTime * 2.f)), 0.f, 1.f);
				}

				if (keybinds[i].alpha > 0) {
					posy += 1;
					posanim[i] = std::clamp(posanim[i] + ((ImGui::GetIO().DeltaTime * 5.f)), 0.f, (float)posy);
				}

				if (!m_Open) {
					if (keybinds[i].alpha <= 0.f)
						continue;

					std::string currentkeybindmode;

					switch (keybinds[i].keybindmode) {
					case 0:
						currentkeybindmode = "Always";
						break;
					case 1:
						currentkeybindmode = "Toggle";
						break;
					case 2:
						currentkeybindmode = "Enable";
						break;
					case 3:
						currentkeybindmode = "Disable";
						break;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logotextsize = ImGui::CalcTextSize(currentkeybindmode.c_str()); ImGui::PopFont();
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(keybinds[i].keybindstr.c_str()); ImGui::PopFont();

					double easiedalpha = easingFunction(keybinds[i].alpha);

					//g_notify.add("anim:" + std::to_string(keybinds[i].alpha));

					draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * (posanim[i])), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * (posanim[i])), ImColor(0.2f, 0.2f, 0.2f, easiedalpha / 7));

					if (maxtextlength < logtextsize.x) {
						maxtextlength = logtextsize.x;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + 5 + 2, pos.y + 2 + 30 + (20) * (posanim[i])), ImColor(1.f, 1.f, 1.f, easiedalpha), keybinds[i].keybindstr.c_str()); ImGui::PopFont();//log
					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x - logotextsize.x - 5 - 2, pos.y + 2 + 30 + (20) * (posanim[i])), ImColor(1.f, 1.f, 1.f, easiedalpha), currentkeybindmode.c_str()); ImGui::PopFont();//log
				}
				else {
					std::string currentkeybindmode;
					switch (keybinds[i].keybindmode) {
					case 0:
						currentkeybindmode = "Always";
						break;
					case 1:
						currentkeybindmode = "Toggle";
						break;
					case 2:
						currentkeybindmode = "Enable";
						break;
					case 3:
						currentkeybindmode = "Disable";
						break;
					}
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logotextsize = ImGui::CalcTextSize(currentkeybindmode.c_str()); ImGui::PopFont();
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(keybinds[i].keybindstr.c_str()); ImGui::PopFont();
					double easiedalpha = ImGui::GetStyle().Alpha;

					//g_notify.add("anim:" + std::to_string(keybinds[i].alpha));

					draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * (i)), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * (i)), ImColor(0.2f, 0.2f, 0.2f, easiedalpha / 7));

					if (maxtextlength < logtextsize.x) {
						maxtextlength = logtextsize.x;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + 5 + 2, pos.y + 2 + 30 + (20) * (i)), ImColor(1.f, 1.f, 1.f, easiedalpha), keybinds[i].keybindstr.c_str()); ImGui::PopFont();//log
					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x - logotextsize.x - 5 - 2, pos.y + 2 + 30 + (20) * (i)), ImColor(1.f, 1.f, 1.f, easiedalpha), currentkeybindmode.c_str()); ImGui::PopFont();//log
				}
			}
		}

		if (g_Config.i(g_Items.m_GradientStyle) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + 1, pos.y), ImVec2((pos.x + 30), pos.y + 2),
				maincolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x - 5, pos.y), ImVec2(pos.x + 5.f, pos.y + 30),
				maincolor, maincolor,
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y + 30), ImVec2((pos.x + size.x) - 30, pos.y + 31),
				gradientcolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 5.f, pos.y + 30),
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
				gradientcolor, gradientcolor);//right bar(ok)
		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 30), ImVec2(pos.x + size.x, pos.y + 31),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 5, pos.y + 30), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 5, pos.y + 30), ImColor(gradientcolor)); // right bar
		}


		draw->AddRectFilled(ImVec2(pos.x + 5, pos.y + 2), ImVec2(pos.x + 1 + size.x - 3 - 3, pos.y + 30), backgroundcolor, 0.f);

		ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + (size.x / 2) - (inditextsize.x / 2), pos.y + 30 / 2 - inditextsize.y / 2), ImColor(255, 255, 255, alpha), "Keybind List"); ImGui::PopFont();//indicator text

	}ImGui::End();


	//Watermark
	bool watermarkinfo = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(2);
	ImGui::Begin("#Wtrmrk", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		//animation
		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f) * ((watermarkinfo) ? 1 : -1)), 0.f, 255.f);

		//definition	
		ImVec2 size, pos;
		ImDrawList* draw;

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() / 255 * alpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() / 255 * alpha);
		ImColor maincoloralpha = ImColor(maincolor.Value.x - 50, maincolor.Value.y - 50, maincolor.Value.z - 50, 0.f);
		ImColor backgroundcolor = ImColor(15, 15, 15, 255 * alpha);
		ImColor backgroundcoloralpha = ImColor(25, 25, 25, 255 * alpha);

		//setup
		pos = ImVec2(ImGui::GetWindowPos().x + 15, ImGui::GetWindowPos().y + 15);
		size = ImVec2(270, 30);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(ImVec2(size.x + 30, size.y + 30));
		ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - size.x / 2, ImGui::GetIO().DisplaySize.y - size.y - 1 - 15));

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y),
			backgroundcoloralpha, backgroundcolor,
			backgroundcolor, backgroundcoloralpha
		);

		draw->AddRectFilledMultiColor(ImVec2(pos.x + 6, pos.y + 6), ImVec2(pos.x + 8, pos.y + size.y - 6),
			maincolor, maincolor,
			gradientcolor, gradientcolor
		);

		if (g_Config.i(g_Items.m_GradientStyle) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2((pos.x + 30), pos.y + 1),
				maincolor, maincoloralpha,
				ImColor(maincolor.Value.x, maincolor.Value.y, maincolor.Value.z, 0.f), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1.f, pos.y + 30),
				maincolor, maincolor,
				maincoloralpha, maincoloralpha);//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + size.x, pos.y + size.y - 1), ImVec2(pos.x + size.x - 30, pos.y + size.y),
				gradientcolor, maincoloralpha,
				maincoloralpha, gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 1.f, pos.y + 30),
				maincoloralpha, maincoloralpha,
				gradientcolor, gradientcolor);//right bar(ok)

		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 1),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 30), ImVec2(pos.x + size.x, pos.y + 30 - 1),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1, pos.y + 30), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 1, pos.y + 30), ImColor(gradientcolor)); // right bar
		}

		auto GetPing = []() -> int
		{
			static float m_AvgLatency = 0;
			m_AvgLatency = g_Client.m_LatencyOutgoing; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L593

			if (g_Csgo.m_Engine->IsPlayingDemo())
				m_AvgLatency = 0.0f; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L740

			float flAdjust = 0.0f;

			if (g_Csgo.cl_updaterate->GetFloat() > 0.001f)
			{
				flAdjust = -0.5f / g_Csgo.cl_updaterate->GetFloat();

				m_AvgLatency += flAdjust;
			}

			flAdjust *= 1000.0f;

			// Can't be below zero
			m_AvgLatency = std::max(0.0f, m_AvgLatency);

			return int(m_AvgLatency * 1000.0f); // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
		};

		auto GetLoss = []() -> int
		{
			if (!g_Csgo.m_Engine->GetNetChannelInfo())
				return 0;

			return int(g_Csgo.m_Engine->GetNetChannelInfo()->GetAvgLoss(INetChannel::FLOW_INCOMING) * 100.0f); // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L591
		};

		auto GetFPS = []() -> int
		{
			static float m_Framerate = 0;
			m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * g_Csgo.m_Globals->m_AbsFrametime; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
			return int(1.f / m_Framerate);
		};

		ImGui::PushFont(CRender::Fonts::m_MenuWatermarkIcon); draw->AddText(ImVec2(pos.x + 18 + 2, pos.y + 30 / 2 - 19 / 2 - 2), ImColor(255, 255, 255, alpha), "A"); ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuFontSmall); draw->AddText(ImVec2(pos.x + 40 + 5 + 5 + 1, pos.y + 30 / 2 - 12 / 2 - 2), ImColor(255, 255, 255, alpha), std::format("Iblis.club | Loss: {} | Ping: {} | FPS: {}", GetLoss(), GetPing(), GetFPS()).c_str()); ImGui::PopFont();//231,12

	}ImGui::End();

	if (!g_Client.m_Processing)
		return;

	std::vector< std::pair<std::string, float> > spectators = {};

	for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i)
	{
		Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
		if (!pPlayer)
			continue;

		if (pPlayer->IsLocalPlayer())
			continue;

		if (pPlayer->GetClientNetworkable()->IsDormant())
			continue;

		if (pPlayer->m_lifeState() == LIFE_ALIVE)
			continue;

		if (pPlayer->GetObserverTarget() != g_Client.m_LocalPlayer)
			continue;

		player_info_t info;
		if (!g_Csgo.m_Engine->GetPlayerInfo(i, &info))
			continue;

		spectators.push_back({ std::string(info.m_Name).substr(0, 16), 0 });
	}

	//Spectator List
	bool spectatorlist = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(0);
	if (loadlocation) {
		ImGui::SetWindowPos(ImVec2(g_Config.i(g_Items.m_SpecX), g_Config.i(g_Items.m_SpecY)));
		loadlocation = false;
	}
	ImGui::Begin("#SpecList", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImGui::PushFont(CRender::Fonts::m_menu_font); auto inditextsize = ImGui::CalcTextSize("Spectators"); ImGui::PopFont();
		static float maxtextlength = inditextsize.x + 20;
		int i2 = spectators.size() - 1;

		static float animatedmaxlength;
		if (animatedmaxlength > maxtextlength) {
			animatedmaxlength -= (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		else if (animatedmaxlength < maxtextlength) {
			animatedmaxlength += (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		animatedmaxlength = std::clamp(animatedmaxlength, 0.f, maxtextlength);

		//definition
		ImVec2 size = ImVec2(animatedmaxlength + 40, 40 + 20 * (i2 + 1));
		ImVec2 pos;
		pos = ImGui::GetWindowPos();
		if (savelocation) {
			g_Config.i(g_Items.m_SpecX) = pos.x;
			g_Config.i(g_Items.m_SpecY) = pos.y;
			savelocation = false;
		}
		ImGui::SetWindowSize(size);//440,300

		//animation
		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f) * ((spectatorlist) ? 1 : -1)), 0.f, 255.f);

		//DrawBackgroundBlur(draw, alpha);

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), alpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), alpha);
		ImColor backgroundcolor = ImColor(15, 15, 15, alpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 30), backgroundcolor, 0.f);

		if (g_Config.i(g_Items.m_GradientStyle) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + 1, pos.y), ImVec2((pos.x + 30), pos.y + 2),
				maincolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x - 5, pos.y), ImVec2(pos.x + 5.f, pos.y + 30),
				maincolor, maincolor,
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y + 30), ImVec2((pos.x + size.x) - 30, pos.y + 29),
				gradientcolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 5.f, pos.y + 30),
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
				gradientcolor, gradientcolor);//right bar(ok)
		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 32), ImVec2(pos.x + size.x, pos.y + 30 - 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 5, pos.y + 30), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 5, pos.y + 30), ImColor(gradientcolor)); // right bar
		}

		ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + (size.x / 2) - (inditextsize.x / 2), pos.y + 30 / 2 - inditextsize.y / 2), ImColor(255, 255, 255, alpha), "Spectators"); ImGui::PopFont();//indicator text

		for (int i = spectators.size() - 1; i >= 0; i--)
		{
			if (spectatorlist) {
				ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(spectators[i].first.c_str()); ImGui::PopFont();

				spectators[i].second = std::clamp(spectators[i].second + ImGui::GetIO().DeltaTime, 0.f, 1.f);

				draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * i), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * i), ImGui::GetColorU32(ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, spectators[i].second / 5)));

				if (maxtextlength < logtextsize.x) {
					maxtextlength = logtextsize.x;
				}

				ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x / 2 - logtextsize.x / 2, pos.y + 30 + (20) * i), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), spectators[i].first.c_str()); ImGui::PopFont();//log
			}
		}

	}ImGui::End();

	//Slowed Down Indicator
	if (g_Config.vb(g_Items.m_Visuals.m_Indicators).at(1)) {

		float sloweddown = 1.f - g_Client.GetLocalPlayer()->m_flVelocityModifier() * 1.f;

		sloweddown = std::clamp(sloweddown, 0.f, 1.f);

		bool sloweddownindicator = sloweddown > 0.f && g_Client.GetLocalPlayer()->IsAlive();

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 200 / 2, 150));
		ImGui::Begin("SlowedDown", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		{
			ImVec2 size = ImVec2(200, 65);
			ImVec2 pos = ImGui::GetWindowPos();
			ImDrawList* draw = ImGui::GetWindowDrawList();
			ImGui::SetWindowSize(size);

			static float alpha;
			alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 2) * ((sloweddownindicator) ? 1 : -1)), 0.f, 1.f);
			easingFunction = getEasingFunction(easing_functions::EaseInQuart);
			float easiedalpha = easingFunction(alpha);
			easiedalpha = std::clamp(easiedalpha, 0.f, 1.f);

			//restore easing
			easingFunction = getEasingFunction(easing_functions::EaseInOutQuint);

			ImColor backgroundcolor = ImColor(15.f/ 255.f, 15.f / 255.f, 15.f / 255.f, easiedalpha);

			//DrawBackgroundBlur(draw, alpha);

			draw->AddRectFilled(ImVec2(pos.x + 5, pos.y + (30 + 5 * easiedalpha)), ImVec2(pos.x + 5 + 190, pos.y + (45 + 5 * easiedalpha)), backgroundcolor, 12.f);//velocity bar shadow
			draw->AddRectFilled(ImVec2(pos.x + 5 + 5, pos.y + (30 + 5 * easiedalpha) + 5), ImVec2(pos.x + 5 + 5 + ((190 - 6) * sloweddown), pos.y + (45 + 5 * easiedalpha) - 6), ImColor(1.f, 1.f, 1.f, easiedalpha), 12.f);//velocity bar


			ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); auto iconsize = ImGui::CalcTextSize("P"); ImGui::PopFont();

			ImGui::PushFont(CRender::Fonts::m_menu_font); auto textsize = ImGui::CalcTextSize("Slowed Down!"); draw->AddText(ImVec2(pos.x + 200 / 2 - textsize.x / 2 + iconsize.x / 2, pos.y + 7 * easiedalpha), ImColor(1.f, 1.f, 1.f, easiedalpha), "Slowed Down!"); ImGui::PopFont();
			ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); draw->AddText(ImVec2(pos.x + 200 / 2 - iconsize.x / 2 - textsize.x / 2 - 5, pos.y + 7 * easiedalpha), ImColor(1.f, 1.f, 1.f, easiedalpha), "P"); ImGui::PopFont();

		}ImGui::End();
	}

	//Doubletap / Tickbase Manuplation Indicator/Key bind
	bool tickbasemanuplation = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(3) && g_Aimbot.m_Config.m_TickbaseManipulation;
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Rapidfire", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((tickbasemanuplation) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		double easiedalphaflo = easingFunction(alphaflo);

		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);
		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 0)));

		float value = static_cast<float>(std::min(g_TickbaseManipulation.m_Data.m_TicksAllowedForProcessing, g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2)) / (g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2);

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() * easiedalpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() * easiedalpha);
		ImColor backgroundcolor = ImColor(15, 15, 15, easiedalpha);

		//DrawBackgroundBlur(draw, easiedalpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), backgroundcolor, 0.f); // background

		if (g_Config.i(g_Items.m_Theme) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + 1, pos.y), ImVec2((pos.x + size.y), pos.y + 2),
				maincolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x - 5, pos.y), ImVec2(pos.x + 5.f, pos.y + size.y),
				maincolor, maincolor,
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y + size.y), ImVec2((pos.x + size.x) - size.y + 2, pos.y + size.y - 2.f),
				gradientcolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 5.f, pos.y + size.y),
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
				gradientcolor, gradientcolor);//right bar(ok)
		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 32), ImVec2(pos.x + size.x, pos.y + size.y - 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 5, pos.y + size.y), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 5, pos.y + size.y), ImColor(gradientcolor)); // right bar
		}

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto tickbasetextsize = ImGui::CalcTextSize("Rapid fire");  draw->AddText(ImVec2(pos.x + 17, pos.y + 32 / 2 - tickbasetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), "Rapid fire"); ImGui::PopFont();

		bool textanim = tickbasemanuplation && g_TickbaseManipulation.m_Data.m_TicksAllowedForProcessing >= g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2;
		static float textalpha;
		textalpha = std::clamp(textalpha + ((ImGui::GetIO().DeltaTime) * ((textanim) ? 1 : -1)), 0.f, 1.f);
		double easiedtextalpha = easingFunction(textalpha) * 255;

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto readytextsize = ImGui::CalcTextSize("Ready");  draw->AddText(ImVec2(pos.x + 193 - 35.f - readytextsize.x, pos.y + 32 / 2 - readytextsize.y / 2), ImColor(255, 255, 255, easiedtextalpha), "Ready"); ImGui::PopFont();

		ImVec2 center = ImVec2(193 - 5, (435) + ((32 + 7) * 0) + 16);
		static float arc_size = 0.45f;
		static float radius = 8.f;
		static float thickness = 3.f;

		// Background
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 30.f);
		draw->PathStroke(ImColor(30, 30, 30, easiedalpha / 5), 0, thickness);

		// Foreground
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, ((value * 2) * IM_PI), 30.f);
		draw->PathStroke(ImColor(255, 255, 255, easiedalpha), 0, thickness);

	}ImGui::End();

	//Fakelag Indicator/Key bind
	bool fakelag = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(4) && g_FakeLag.m_ChokedCommands > 0 && !tickbasemanuplation;
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Fakelag", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((fakelag) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		double easiedalphaflo = easingFunction(alphaflo);


		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);
		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 0)));

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() * easiedalpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() * easiedalpha);
		ImColor backgroundcolor = ImColor(15, 15, 15, easiedalpha);

		//DrawBackgroundBlur(draw, easiedalpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), backgroundcolor, 0.f); // background

		if (g_Config.i(g_Items.m_GradientStyle) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + 1, pos.y), ImVec2((pos.x + size.y), pos.y + 2),
				maincolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x - 5, pos.y), ImVec2(pos.x + 5.f, pos.y + size.y),
				maincolor, maincolor,
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y + size.y), ImVec2((pos.x + size.x) - size.y + 2, pos.y + size.y - 2.f),
				gradientcolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 5.f, pos.y + size.y),
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
				gradientcolor, gradientcolor);//right bar(ok)
		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 32), ImVec2(pos.x + size.x, pos.y + size.y - 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 5, pos.y + size.y), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 5, pos.y + size.y), ImColor(gradientcolor)); // right bar
		}

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto tickbasetextsize = ImGui::CalcTextSize("Fake lag");  draw->AddText(ImVec2(pos.x + 17, pos.y + 32 / 2 - tickbasetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), "Fake lag"); ImGui::PopFont();

		bool textanim = fakelag && g_Client.m_LagCompensationBreaked;
		static float textalpha;
		textalpha = std::clamp(textalpha + ((ImGui::GetIO().DeltaTime * 2.f) * ((textanim) ? 1 : -1)), 0.f, 1.f);
		double easiedtextalpha = easingFunction(textalpha) * 255;

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto readytextsize = ImGui::CalcTextSize("LC");  draw->AddText(ImVec2(pos.x + 193 - 35.f - readytextsize.x, pos.y + 32 / 2 - readytextsize.y / 2), ImColor(255, 255, 255, easiedtextalpha), "LC"); ImGui::PopFont();

		ImVec2 center = ImVec2(193 - 5, (435) + ((32 + 7) * 0) + 16);
		static float arc_size = 0.45f;
		static float radius = 8.f;
		static float thickness = 3.f;
		static float value = 360.f;

		// Background
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 30.f);
		draw->PathStroke(ImColor(75, 75, 75, easiedalpha), 0, thickness);

		// Foreground
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, ((std::min(static_cast<float>(g_Client.m_ChokedCommands) / g_Client.m_MaxLagAmount, 1.f) * 2) * IM_PI), 30.f);
		draw->PathStroke(ImColor(255, 255, 255, easiedalpha), 0, thickness);
	}ImGui::End();

	//Velocity Indicator
	bool velocity = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(5);
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Speed", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((velocity) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		float speed{ std::min(g_Client.GetLocalPlayer()->m_vecVelocity().Length() / g_Client.m_MaxPlayerSpeed, 1.f) };
		double easiedspeed = easingFunction(speed);

		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);
		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 1)));

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() * easiedalpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() * easiedalpha);
		ImColor backgroundcolor = ImColor(15, 15, 15, easiedalpha);

		//DrawBackgroundBlur(draw, easiedalpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), backgroundcolor, 0.f); // background

		if (g_Config.i(g_Items.m_GradientStyle) < 1) {

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x + 1, pos.y), ImVec2((pos.x + size.y), pos.y + 2),
				maincolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x - 5, pos.y), ImVec2(pos.x + 5.f, pos.y + size.y),
				maincolor, maincolor,
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y + size.y), ImVec2((pos.x + size.x) - size.y + 2, pos.y + size.y - 2.f),
				gradientcolor, ImColor(0, 0, 0, 0),
				ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((pos.x + size.x), pos.y), ImVec2((pos.x + size.x) - 5.f, pos.y + size.y),
				ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
				gradientcolor, gradientcolor);//right bar(ok)
		}
		else {
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//top bar

			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 32), ImVec2(pos.x + size.x, pos.y + size.y - 2),
				maincolor, gradientcolor,
				gradientcolor, maincolor);//bottom bar

			draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 5, pos.y + size.y), ImColor(maincolor)); // left bar

			draw->AddRectFilled(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - 5, pos.y + size.y), ImColor(gradientcolor)); // right bar
		}

		static float standvelocityalpha;
		static float movevelocityalpha;

		if (g_Client.GetLocalPlayer()->m_vecVelocity().Length() <= 0) {
			standvelocityalpha = std::clamp(standvelocityalpha + (7.f * ImGui::GetIO().DeltaTime * 1), 0.f, 1.f);
			movevelocityalpha = std::clamp(movevelocityalpha + (7.f * ImGui::GetIO().DeltaTime * -1), 0.f, 1.f);
		}
		else {
			standvelocityalpha = std::clamp(standvelocityalpha + (7.f * ImGui::GetIO().DeltaTime * -1), 0.f, 1.f);
			movevelocityalpha = std::clamp(movevelocityalpha + (7.f * ImGui::GetIO().DeltaTime * 1), 0.f, 1.f);
		}

		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); const auto velocitytextsize = ImGui::CalcTextSize("P");  draw->AddText(ImVec2(pos.x + 8, pos.y + 32 / 2 - velocitytextsize.y / 2), ImColor(255, 255, 255, easiedalpha * movevelocityalpha), "P"); ImGui::PopFont();
		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); const auto velocitytextsize2 = ImGui::CalcTextSize("C");  draw->AddText(ImVec2(pos.x + 19, pos.y + 32 / 2 - velocitytextsize2.y / 2), ImColor(255, 255, 255, easiedalpha * standvelocityalpha), "C"); ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto valuetextsize = ImGui::CalcTextSize(std::to_string((int)g_Client.GetLocalPlayer()->m_vecVelocity().Length()).c_str());  draw->AddText(ImVec2(pos.x + size.x - 8 - valuetextsize.x, pos.y + 32 / 2 - valuetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), std::to_string((int)g_Client.GetLocalPlayer()->m_vecVelocity().Length()).c_str()); ImGui::PopFont();

		draw->AddRectFilled(ImVec2(pos.x + 40, pos.y + 12), ImVec2(pos.x + 40 + (size.x - 40 - 44), pos.y + 12 + 8), ImColor(0, 0, 0, easiedalpha / 3), 12.f);// 65 default text size
		draw->AddRectFilled(ImVec2(pos.x + 40, pos.y + 12), ImVec2(pos.x + 40 + (size.x - 40 - 44) * ((easiedspeed)), pos.y + 12 + 8), ImColor(255, 255, 255, easiedalpha), 12.f);// slider
	}ImGui::End();

	//for eventlog hud
	/*
	draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32),
			maincolor, ImColor(15,15,15,0),
			ImColor(15, 15, 15, 0), maincolor); // left to right grad

	draw->AddRectFilledMultiColor(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x - (size.x / 2), pos.y + 32),
		maincolor, ImColor(15, 15, 15, 0),
		ImColor(15, 15, 15, 0), maincolor); // right to left grad
	*/
}

void Menu::LightModeIndicators() {

	if (g_Config.i(g_Items.m_Theme) < 1 || !loaded)
		return;

	auto easingFunction = getEasingFunction(easing_functions::EaseInOutQuint); // easing float/double 255 degil 1.f

	keybinds.clear();

	keybinds.emplace_back(std::string("Third person").substr(0, 16), g_Config.i(g_Items.m_Visuals.m_ThirdpersonKeyMode), (g_Config.b(g_Items.m_Visuals.m_ThirdpersonEnabled) && g_KeyInput.Key(g_Items.m_Visuals.m_ThirdpersonKey, g_Items.m_Visuals.m_ThirdpersonKeyMode)));

	keybinds.emplace_back(std::string("Rapid fire").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_TickbaseManipulation) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_TickbaseManipulationKey, g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode)));

	keybinds.emplace_back(std::string("Wait resolved").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdate) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateKey, g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode)));

	keybinds.emplace_back(std::string("Hit chance").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideHitChance) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideHitChanceKey, g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode)));

	keybinds.emplace_back(std::string("Minimum damage").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideDamageMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideDamage) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_OverrideDamageKey, g_Items.m_Aimbot.m_General.m_OverrideDamageMode)));

	keybinds.emplace_back(std::string("Force body").substr(0, 16), g_Config.i(g_Items.m_Aimbot.m_General.m_ForceBodyMode), (g_Config.b(g_Items.m_Aimbot.m_General.m_ForceBody) && g_KeyInput.Key(g_Items.m_Aimbot.m_General.m_ForceBodyKey, g_Items.m_Aimbot.m_General.m_ForceBodyMode)));

	keybinds.emplace_back(std::string("Automatic peek").substr(0, 25), g_Config.i(g_Items.m_Misc.m_AutomaticPeekKeyBindsMode), (g_Config.b(g_Items.m_Misc.m_AutomaticPeek) && g_KeyInput.Key(g_Items.m_Misc.m_AutomaticPeekKeyBinds, g_Items.m_Misc.m_AutomaticPeekKeyBindsMode)));

	keybinds.emplace_back(std::string("Fake walk").substr(0, 16), g_Config.i(g_Items.m_Misc.m_FakeWalkBindsMode), (g_Config.b(g_Items.m_Misc.m_FakeWalk) && g_KeyInput.Key(g_Items.m_Misc.m_FakeWalkKeyBinds, g_Items.m_Misc.m_FakeWalkBindsMode)));

	//Keybind List
	bool keybindlist = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(0);
	ImGui::Begin("#KeybindList", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		//definition main
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImGui::PushFont(CRender::Fonts::m_menu_font); auto inditextsize = ImGui::CalcTextSize("Keybind List"); ImGui::PopFont();
		static float maxtextlength = inditextsize.x + 20;
		int i2 = keybinds.size() - 1;
		static float animatedmaxlength;

		if (animatedmaxlength > maxtextlength) {
			animatedmaxlength -= (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		else if (animatedmaxlength < maxtextlength) {
			animatedmaxlength += (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		animatedmaxlength = std::clamp(animatedmaxlength, 0.f, maxtextlength);

		//definition misc
		ImVec2 size = ImVec2(animatedmaxlength + 70, 40 + 20 * (i2 + 1) + 90);
		ImVec2 pos;
		pos = ImGui::GetWindowPos();
		ImGui::SetWindowSize(size);

		//animation
		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f * 2) * ((keybindlist) ? 1 : -1)), 0.f, 255.f);

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() / 255 * alpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() / 255 * alpha);
		ImColor backgroundcolor = ImColor(255, 255, 255, alpha / 30);

		static float posanim[20];
		static int posy;
		posy = -1;

		for (int i = keybinds.size() - 1; i >= 0; i--)
		{
			if (keybindlist) {


				if (keybinds[i].enabled) {
					keybinds[i].alpha = std::clamp(keybinds[i].alpha + ((ImGui::GetIO().DeltaTime * 2.f)), 0.f, 1.f);
				}
				else {
					keybinds[i].alpha = std::clamp(keybinds[i].alpha - ((ImGui::GetIO().DeltaTime * 2.f)), 0.f, 1.f);
				}

				if (keybinds[i].alpha > 0) {
					posy += 1;
					posanim[i] = std::clamp(posanim[i] + ((ImGui::GetIO().DeltaTime * 5.f)), 0.f, (float)posy);
				}

				if (!m_Open) {
					if (keybinds[i].alpha <= 0.f)
						continue;

					std::string currentkeybindmode;

					switch (keybinds[i].keybindmode) {
					case 0:
						currentkeybindmode = "Always";
						break;
					case 1:
						currentkeybindmode = "Toggle";
						break;
					case 2:
						currentkeybindmode = "Enable";
						break;
					case 3:
						currentkeybindmode = "Disable";
						break;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logotextsize = ImGui::CalcTextSize(currentkeybindmode.c_str()); ImGui::PopFont();
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(keybinds[i].keybindstr.c_str()); ImGui::PopFont();

					double easiedalpha = easingFunction(keybinds[i].alpha);

					//g_notify.add("anim:" + std::to_string(keybinds[i].alpha));

					draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * (posanim[i])), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * (posanim[i])), ImColor(0.2f, 0.2f, 0.2f, easiedalpha / 7));

					if (maxtextlength < logtextsize.x) {
						maxtextlength = logtextsize.x;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + 5 + 2, pos.y + 2 + 30 + (20) * (posanim[i])), ImColor(1.f, 1.f, 1.f, easiedalpha), keybinds[i].keybindstr.c_str()); ImGui::PopFont();//log
					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x - logotextsize.x - 5 - 2, pos.y + 2 + 30 + (20) * (posanim[i])), ImColor(1.f, 1.f, 1.f, easiedalpha), currentkeybindmode.c_str()); ImGui::PopFont();//log
				}
				else {
					std::string currentkeybindmode;
					switch (keybinds[i].keybindmode) {
					case 0:
						currentkeybindmode = "Always";
						break;
					case 1:
						currentkeybindmode = "Toggle";
						break;
					case 2:
						currentkeybindmode = "Enable";
						break;
					case 3:
						currentkeybindmode = "Disable";
						break;
					}
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logotextsize = ImGui::CalcTextSize(currentkeybindmode.c_str()); ImGui::PopFont();
					ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(keybinds[i].keybindstr.c_str()); ImGui::PopFont();
					double easiedalpha = ImGui::GetStyle().Alpha;

					//g_notify.add("anim:" + std::to_string(keybinds[i].alpha));

					draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * (i)), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * (i)), ImColor(0.2f, 0.2f, 0.2f, easiedalpha / 7));

					if (maxtextlength < logtextsize.x) {
						maxtextlength = logtextsize.x;
					}

					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + 5 + 2, pos.y + 2 + 30 + (20) * (i)), ImColor(1.f, 1.f, 1.f, easiedalpha), keybinds[i].keybindstr.c_str()); ImGui::PopFont();//log
					ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x - logotextsize.x - 5 - 2, pos.y + 2 + 30 + (20) * (i)), ImColor(1.f, 1.f, 1.f, easiedalpha), currentkeybindmode.c_str()); ImGui::PopFont();//log
				}
			}
		}

		draw->AddRectFilled(ImVec2(pos.x + 5, pos.y + 2), ImVec2(pos.x + 1 + size.x - 3 - 3, pos.y + 30), backgroundcolor, 0.f);// background

		ImColor alphacolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), 0);

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
			maincolor, alphacolor,
			alphacolor, maincolor);

		ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + (size.x / 2) - (inditextsize.x / 2), pos.y + 30 / 2 - inditextsize.y / 2), ImColor(255, 255, 255, alpha), "Keybind List"); ImGui::PopFont();//indicator text

	}ImGui::End();

	//Watermark
	bool watermarkinfo = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(2);
	ImGui::Begin("#Wtrmrk", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		//animation
		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f) * ((watermarkinfo) ? 1 : -1)), 0.f, 255.f);

		//definition	
		ImVec2 size, pos;
		ImDrawList* draw;
		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() / 255 * alpha);
		ImColor gradientcolor = ImColor(255,255,255,alpha);
		ImColor maincoloralpha = ImColor(maincolor.Value.x - 50, maincolor.Value.y - 50, maincolor.Value.z - 50, 0.f);
		ImColor backgroundcolor = ImColor(255, 255, 255, 10 * alpha / 255);
		ImColor backgroundcoloralpha = ImColor(backgroundcolor.Value.x, backgroundcolor.Value.y, backgroundcolor.Value.z, 0.f);
		ImColor alphacolor = ImColor(maincolor.Value.x, maincolor.Value.y, maincolor.Value.z, 0.f);

		//setup
		pos = ImVec2(ImGui::GetWindowPos().x + 15, ImGui::GetWindowPos().y + 15);
		size = ImVec2(270, 30);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(ImVec2(size.x + 30, size.y + 30));
		ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - size.x / 2, ImGui::GetIO().DisplaySize.y - size.y - 1 - 15));
		DrawBackgroundBlur(draw,alpha - 155);

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y),
			backgroundcolor, backgroundcoloralpha,
			backgroundcoloralpha, backgroundcolor
		);

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + 2, pos.y + size.y),
			maincolor, maincolor,
			gradientcolor, gradientcolor
		);

		auto GetPing = []() -> int
		{
			static float m_AvgLatency = 0;
			m_AvgLatency = g_Client.m_LatencyOutgoing; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L593

			if (g_Csgo.m_Engine->IsPlayingDemo())
				m_AvgLatency = 0.0f; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L740

			float flAdjust = 0.0f;

			if (g_Csgo.cl_updaterate->GetFloat() > 0.001f)
			{
				flAdjust = -0.5f / g_Csgo.cl_updaterate->GetFloat();

				m_AvgLatency += flAdjust;
			}

			flAdjust *= 1000.0f;

			// Can't be below zero
			m_AvgLatency = std::max(0.0f, m_AvgLatency);

			return int(m_AvgLatency * 1000.0f); // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
		};

		auto GetLoss = []() -> int
		{
			if (!g_Csgo.m_Engine->GetNetChannelInfo())
				return 0;

			return int(g_Csgo.m_Engine->GetNetChannelInfo()->GetAvgLoss(INetChannel::FLOW_INCOMING) * 100.0f); // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L591
		};

		auto GetFPS = []() -> int
		{
			static float m_Framerate = 0;
			m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * g_Csgo.m_Globals->m_AbsFrametime; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
			return int(1.f / m_Framerate);
		};

		ImGui::PushFont(CRender::Fonts::m_MenuWatermarkIcon); draw->AddText(ImVec2(pos.x + 18, pos.y + 30 / 2 - 19 / 2 - 2), ImColor(255, 255, 255,alpha), "A"); ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuFontSmall); draw->AddText(ImVec2(pos.x + 20 + 5 + 5 + 1, pos.y + 30 / 2 - 12 / 2 - 2), ImColor(255, 255, 255, alpha), std::format("Iblis.club | Loss: {} | Ping: {} | FPS: {}", GetLoss(), GetPing(), GetFPS()).c_str()); ImGui::PopFont();//231,12

	}ImGui::End();

	if (!g_Client.m_Processing)
		return;

	std::vector< std::pair<std::string, float> > spectators = {};

	for (int i{ 1 }; i <= g_Csgo.m_Globals->m_MaxClients; ++i)
	{
		Player* pPlayer = g_Csgo.m_EntityList->GetClientEntity< Player* >(i);
		if (!pPlayer)
			continue;

		if (pPlayer->IsLocalPlayer())
			continue;

		if (pPlayer->GetClientNetworkable()->IsDormant())
			continue;

		if (pPlayer->m_lifeState() == LIFE_ALIVE)
			continue;

		if (pPlayer->GetObserverTarget() != g_Client.m_LocalPlayer)
			continue;

		player_info_t info;
		if (!g_Csgo.m_Engine->GetPlayerInfo(i, &info))
			continue;

		spectators.push_back({ std::string(info.m_Name).substr(0, 16), 255 });
	}

	//Spectator List
	bool spectatorlist = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(0);
	ImGui::Begin("#SpecList", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImGui::PushFont(CRender::Fonts::m_menu_font); auto inditextsize = ImGui::CalcTextSize("Spectators"); ImGui::PopFont();
		static float maxtextlength = inditextsize.x + 20;
		int i2 = spectators.size() - 1;

		static float animatedmaxlength;

		if (animatedmaxlength > maxtextlength) {
			animatedmaxlength -= (ImGui::GetIO().DeltaTime * maxtextlength);
		}
		else if (animatedmaxlength < maxtextlength) {
			animatedmaxlength += (ImGui::GetIO().DeltaTime * maxtextlength);
		}

		animatedmaxlength = std::clamp(animatedmaxlength, 0.f, maxtextlength);

		ImVec2 size = ImVec2(animatedmaxlength + 40, 40 + 20 * (i2 + 1));
		ImVec2 pos;
		pos = ImGui::GetWindowPos();
		ImGui::SetWindowSize(size);//440,300

		static int alpha;
		alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f) * ((spectatorlist) ? 1 : -1)), 0.f, 255.f);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 30), ImColor(255, 255, 255, alpha / 30), 0.f);

		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), alpha * 255);
		ImColor alphacolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), 0);

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 2),
			maincolor, alphacolor,
			alphacolor, maincolor);

		ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + (size.x / 2) - (inditextsize.x / 2), pos.y + 30 / 2 - inditextsize.y / 2), ImColor(255, 255, 255, alpha), "Spectators"); ImGui::PopFont();//indicator text

		for (int i = spectators.size() - 1; i >= 0; i--)
		{
			if (spectatorlist) {
				ImGui::PushFont(CRender::Fonts::m_menu_font); auto logtextsize = ImGui::CalcTextSize(spectators[i].first.c_str()); ImGui::PopFont();

				spectators[i].second = std::clamp(spectators[i].second + (2.2f * ImGui::GetIO().DeltaTime * 1.f), 0.f, 1.f);

				draw->AddRectFilled(ImVec2(pos.x, pos.y + 30 + 20 * i), ImVec2(pos.x + size.x, pos.y + 30 + 20 + 20 * i), ImGui::GetColorU32(ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, spectators[i].second / 255 * 30.f)));

				if (maxtextlength < logtextsize.x) {
					maxtextlength = logtextsize.x;
				}

				ImGui::PushFont(CRender::Fonts::m_menu_font);  draw->AddText(ImVec2(pos.x + size.x / 2 - logtextsize.x / 2, pos.y + 30 + (20) * i), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, spectators[i].second)), spectators[i].first.c_str()); ImGui::PopFont();//log
			}
		}

	}ImGui::End();

	//Slowed Down Indicator
	if (g_Config.vb(g_Items.m_Visuals.m_Indicators).at(1)) {

		float sloweddown = 1.f - g_Client.GetLocalPlayer()->m_flVelocityModifier() * 1.f;

		sloweddown = std::clamp(sloweddown, 0.f, 1.f);

		bool sloweddownindicator = sloweddown > 0.f && g_Client.GetLocalPlayer()->IsAlive();

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 200 / 2, 150));
		ImGui::Begin("SlowedDown", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		{
			ImVec2 size = ImVec2(200, 65);
			ImVec2 pos = ImGui::GetWindowPos();
			ImDrawList* draw = ImGui::GetWindowDrawList();
			ImGui::SetWindowSize(size);

			static float alpha;
			alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 2) * ((sloweddownindicator) ? 1 : -1)), 0.f, 1.f);

			DrawBackgroundBlur(draw, alpha);

			//draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 200, pos.y + 65), ImColor(0.2f, 0.2f, 0.2f, alpha * 0.3f));//background

			draw->AddRectFilled(ImVec2(pos.x + 5, pos.y + (30 + 5 * alpha)), ImVec2(pos.x + 5 + 190, pos.y + (45 + 5 * alpha)), ImColor(0.f, 0.f, 0.f, alpha * 0.4f));//velocity bar shadow
			draw->AddRectFilled(ImVec2(pos.x + 5 + 5, pos.y + (30 + 5 * alpha) + 5), ImVec2(pos.x + 5 + 5 + (sloweddown * (190 - 6)), pos.y + (45 + 5 * alpha) - 6), ImColor(1.f, 1.f, 1.f, alpha));//velocity bar


			ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); auto iconsize = ImGui::CalcTextSize("P"); ImGui::PopFont();

			ImGui::PushFont(CRender::Fonts::m_menu_font); auto textsize = ImGui::CalcTextSize("Slowed Down!"); draw->AddText(ImVec2(pos.x + 200 / 2 - textsize.x / 2 + iconsize.x / 2, pos.y + 7 * alpha), ImColor(1.f, 1.f, 1.f, alpha), "Slowed Down!"); ImGui::PopFont();
			ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); draw->AddText(ImVec2(pos.x + 200 / 2 - iconsize.x / 2 - textsize.x / 2 - 5, pos.y + 7 * alpha), ImColor(1.f, 1.f, 1.f, alpha), "P"); ImGui::PopFont();

		}ImGui::End();
	}

	//Doubletap / Tickbase Manuplation Indicator/Key bind
	bool tickbasemanuplation = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(3) && g_Aimbot.m_Config.m_TickbaseManipulation;
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Rapidfire", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((tickbasemanuplation) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		double easiedalphaflo = easingFunction(alphaflo);

		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		//setup
		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);
		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 0)));

		static float bluralpha;
		bluralpha = std::clamp(bluralpha + ((ImGui::GetIO().DeltaTime * 255) * ((tickbasemanuplation) ? 1 : -1)), 0.f, 255.f);

		DrawBackgroundBlur(draw, bluralpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), ImColor(255, 255, 255, easiedalpha / 30), 12.f); // background

		draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32),
			ImColor(255, 255, 255, easiedalpha / 20), ImColor(255, 255, 255, easiedalpha / 20),
			ImColor(255, 255, 255, easiedalpha / 20), ImColor(255, 255, 255, easiedalpha / 20)); // left grad

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto tickbasetextsize = ImGui::CalcTextSize("Rapid fire");  draw->AddText(ImVec2(pos.x + 17, pos.y + 32 / 2 - tickbasetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), "Rapid fire"); ImGui::PopFont();

		bool textanim = tickbasemanuplation && g_TickbaseManipulation.m_Data.m_TicksAllowedForProcessing >= g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2;
		static float textalpha;
		textalpha = std::clamp(textalpha + ((ImGui::GetIO().DeltaTime) * ((textanim) ? 1 : -1)), 0.f, 1.f);
		double easiedtextalpha = easingFunction(textalpha) * 255;

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto readytextsize = ImGui::CalcTextSize("Ready");  draw->AddText(ImVec2(pos.x + 193 - 35.f - readytextsize.x, pos.y + 32 / 2 - readytextsize.y / 2), ImColor(255, 255, 255, easiedtextalpha), "Ready"); ImGui::PopFont();

		ImVec2 center = ImVec2(193 - 5, (435) + (32 + 7 * 1) + 16);
		static float arc_size = 0.45f;
		static float radius = 8.f;
		static float thickness = 3.f;
		float value = static_cast<float>(std::min(g_TickbaseManipulation.m_Data.m_TicksAllowedForProcessing, g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2)) / (g_Csgo.m_MaxUserCommandProcessTicks->GetInt() - 2);

		// Background
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 30.f);
		draw->PathStroke(ImColor(75, 75, 75, easiedalpha), 0, thickness);

		// Foreground
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, ((value * 2) * IM_PI), 30.f);
		draw->PathStroke(ImColor(255, 255, 255, easiedalpha), 0, thickness);

	}ImGui::End();

	//Fakelag Indicator/Key bind
	bool fakelag = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(4) && g_FakeLag.m_ChokedCommands > 0 && !tickbasemanuplation;
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Fakelag", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((fakelag) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		double easiedalphaflo = easingFunction(alphaflo);


		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		//setup
		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);
		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 0)));

		static float bluralpha;
		bluralpha = std::clamp(bluralpha + ((ImGui::GetIO().DeltaTime * 255) * ((fakelag) ? 1 : -1)), 0.f, 255.f);

		DrawBackgroundBlur(draw, bluralpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), ImColor(255, 255, 255, easiedalpha / 30), 12.f); // background

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto tickbasetextsize = ImGui::CalcTextSize("Fake lag");  draw->AddText(ImVec2(pos.x + 17, pos.y + 32 / 2 - tickbasetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), "Fake lag"); ImGui::PopFont();

		bool textanim = fakelag && g_Client.m_LagCompensationBreaked;
		static float textalpha;
		textalpha = std::clamp(textalpha + ((ImGui::GetIO().DeltaTime * 2.f) * ((textanim) ? 1 : -1)), 0.f, 1.f);
		double easiedtextalpha = easingFunction(textalpha) * 255;

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto readytextsize = ImGui::CalcTextSize("LC");  draw->AddText(ImVec2(pos.x + 193 - 35.f - readytextsize.x, pos.y + 32 / 2 - readytextsize.y / 2), ImColor(255, 255, 255, easiedtextalpha), "LC"); ImGui::PopFont();

		ImVec2 center = ImVec2(193 - 5, (435) + ((32 + 7) * 0) + 16);
		static float arc_size = 0.45f;
		static float radius = 8.f;
		static float thickness = 3.f;
		static float value = 360.f;

		// Background
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 30.f);
		draw->PathStroke(ImColor(75, 75, 75, easiedalpha), 0, thickness);

		// Foreground
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, ((std::min(static_cast<float>(g_Client.m_ChokedCommands) / g_Client.m_MaxLagAmount, 1.f) * 2) * IM_PI), 30.f);
		draw->PathStroke(ImColor(255, 255, 255, easiedalpha), 0, thickness);
	}ImGui::End();

	//Velocity Indicator
	bool velocity = g_Config.vb(g_Items.m_Visuals.m_Indicators).at(5);
	ImGui::SetNextWindowSize(ImVec2(185 + 8, 32));
	ImGui::Begin("#Speed", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		//animation
		static float alphaflo;
		alphaflo = std::clamp(alphaflo + ((ImGui::GetIO().DeltaTime) * ((velocity) ? 1 : -1)), 0.f, 1.f);
		double easiedalpha = easingFunction(alphaflo) * 255;
		float speed{ std::min(g_Client.GetLocalPlayer()->m_vecVelocity().Length() / g_Client.m_MaxPlayerSpeed, 1.f) };
		double easiedspeed = easingFunction(speed);

		//definition
		ImVec2 size, pos;
		ImDrawList* draw;

		//setup
		pos = ImGui::GetWindowPos();
		size = ImVec2(193, 32);
		draw = ImGui::GetWindowDrawList();
		ImGui::SetWindowSize(size);

		ImGui::SetWindowPos(ImVec2(16, 435 + ((32 + 7) * 1)));

		static float bluralpha;
		bluralpha = std::clamp(bluralpha + ((ImGui::GetIO().DeltaTime * 255) * ((velocity) ? 1 : -1)), 0.f, 255.f);

		DrawBackgroundBlur(draw, bluralpha);

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 32), ImColor(255, 255, 255, easiedalpha / 30), 12.f); // background

		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon); const auto velocitytextsize = ImGui::CalcTextSize("P");  draw->AddText(ImVec2(pos.x + 8, pos.y + 32 / 2 - velocitytextsize.y / 2), ImColor(255, 255, 255, easiedalpha), "P"); ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_menu_font); const auto valuetextsize = ImGui::CalcTextSize(std::to_string((int)g_Client.GetLocalPlayer()->m_vecVelocity().Length()).c_str());  draw->AddText(ImVec2(pos.x + size.x - 8 - valuetextsize.x, pos.y + 32 / 2 - valuetextsize.y / 2), ImColor(255, 255, 255, easiedalpha), std::to_string((int)g_Client.GetLocalPlayer()->m_vecVelocity().Length()).c_str()); ImGui::PopFont();

		draw->AddRectFilled(ImVec2(pos.x + 40, pos.y + 12), ImVec2(pos.x + 40 + (size.x - 40 - 44), pos.y + 12 + 8), ImColor(0, 0, 0, easiedalpha / 3), 12.f);// 65 default text size
		draw->AddRectFilled(ImVec2(pos.x + 40, pos.y + 12), ImVec2(pos.x + 40 + (size.x - 40 - 44) * ((easiedspeed)), pos.y + 12 + 8), ImColor(255, 255, 255, easiedalpha), 12.f);// slider
	}ImGui::End();
}

static int backgroundalpha = 30;

void Menu::Render()
{
	auto easingFunction = getEasingFunction(easing_functions::EaseInOutQuint); // easing float/double 255 degil 1.f

	//Time Save
	static bool firstanim = true;
	int currenttime = (float)(clock() / 1000.f);
	static int startedtime = 0;
	static bool savetime = true;
	if (savetime) {
		startedtime = (float)(clock() / 1000.f);
		savetime = false;
	}

	if (currenttime < startedtime + (firstanim ? 6.f : 2.f)) {

		// Animation
		static float position = 0.f;
		position = ImLerp(position, IM_PI * 2.f, ImGui::GetIO().DeltaTime * 2.3f);
		static float alpha = std::clamp(alpha + ((ImGui::GetIO().DeltaTime * 255.f) * 1), 0.f, 255.f);

		// Setup
		auto draw = ImGui::GetBackgroundDrawList();
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		static ImColor fore_color(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f * alpha / 255.f);
		static ImColor back_color(75.f / 255.f, 75.f / 255.f, 75.f / 255.f, 75.f * alpha / 255.f);
		static float arc_size = 0.45f; // 0.f < x < 2.f
		static float radius = 35.f;
		static float thickness = 4.f;

		draw->AddRectFilled(ImVec2(0, 0), ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 150.f / 255.f));

		//Loading Text
		ImGui::PushFont(CRender::Fonts::m_MenuBigFont); draw->AddText(ImVec2(center.x - ImGui::CalcTextSize("Loading").x / 2, center.y + 50), ImColor(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f * alpha / 255.f), "Loading"); ImGui::PopFont();

		// Background
		draw->PathClear();
		draw->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 40.f);
		draw->PathStroke(ImGui::GetColorU32(back_color.Value), 0, thickness);

		// Foreground
		draw->PathClear();
		draw->PathArcTo(center, radius, IM_PI * 1.5f + position, IM_PI * (1.5f + arc_size) + position, 40.f);
		draw->PathStroke(ImGui::GetColorU32(fore_color.Value), 0, thickness);

		// Reset animation
		if (position >= IM_PI * 1.90f)
			position = 0.f;
	}

	//Erasing
	if (currenttime > startedtime + (firstanim ? 5.f : 1.f)) {
		loaded = true;
		firstanim = false;
	}

	static int oldtheme = 0;
	if (oldtheme != g_Config.i(g_Items.m_Theme)) {
		bool animend = false;
		if (loaded && !animend) {
			loaded = false;
			startedtime = 0;
			savetime = true;
			animend = true;
		}
		oldtheme = g_Config.i(g_Items.m_Theme);
	}

	if (!loaded)
		return;

	static float m_alpha = 0.0002f;
	m_alpha = std::clamp(m_alpha + (/*g_Config.f(g_Items.m_AnimationSpeed)*/ 3.f * ImGui::GetIO().DeltaTime * (g_Menu.m_Open ? 1.f : -1.f)), 0.f, 1.f);
	double easiedm_alpha = easingFunction(m_alpha);
	if (!m_alpha)
		return;

	static ImVec2 mainmenupos = ImVec2(0,0);

	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, (100.f / 255.f) * easiedm_alpha));

	// Push Style/Color on loop
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, easiedm_alpha);

	ImGui::SetWindowSize({ 755.f, 565.f }, ImGuiCond_FirstUseEver);

	ImGui::Begin(XOR("##Iblis"), &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar); {

		const ImVec2 vecPos = ImGui::GetWindowPos();
		mainmenupos = vecPos;
		const ImVec2 vecMousePos = ImGui::GetMousePos();
		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() * (double)m_alpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() * (double)m_alpha);
		bool bMouseOnPos = vecMousePos.x > vecPos.x + 15.f && vecMousePos.x < vecPos.x + 110.f - 15.f + (m_TabAnimation * 75.f) && (vecMousePos.y > vecPos.y + 10.f && !(vecMousePos.y > vecPos.y + 565.f - 10.f));

		m_TabAnimation = bMouseOnPos ? m_TabAnimation += ImGui::GetIO().DeltaTime * 4.f : m_TabAnimation -= ImGui::GetIO().DeltaTime * 4.f;
		m_TabAnimation = std::clamp(m_TabAnimation, 0.f, 1.f);
		easiedm_TabAnimation = easingFunction(m_TabAnimation);

		ImGui::SetWindowSize({ 755.f + easiedm_TabAnimation * 75.f, 565.f });

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30 / 255.f, 30 / 255.f, 30 / 255.f, (255 / 2) / 255.f));

		if (g_Config.i(g_Items.m_Theme) < 1) {
			
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x, vecPos.y }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f, vecPos.y + 565.f }, ImColor(40.f / 255.f, 40.f / 255.f, 40.f / 255.f, 255.f * m_alpha / 255.f));                 //background shadow 2
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 10.f, vecPos.y + 5.f }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f - 10.f, vecPos.y + 565.f - 5.f }, ImColor(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 255 * m_alpha / 255.f));     //background shadow 1
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 15.f, vecPos.y + 10.f }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f - 15.f, vecPos.y + 565.f - 10.f }, ImColor(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 255 * m_alpha / 255.f));  //background

			if (g_Config.i(g_Items.m_GradientStyle) < 1) {
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y), ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f), vecPos.y + 2),
					maincolor, ImColor(0, 0, 0, 0),
					ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x - 5, vecPos.y), ImVec2(vecPos.x + 5.f, vecPos.y + 565.f),
					maincolor, maincolor,
					ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f), vecPos.y + 565.f), ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f) - 755.f + easiedm_TabAnimation * 75.f, vecPos.y + 565.f - 2.f),
					gradientcolor, ImColor(0, 0, 0, 0),
					ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f), vecPos.y), ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f) - 5.f, vecPos.y + 565.f),
					ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
					gradientcolor, gradientcolor);//right bar(ok)
			}
			else {
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y), ImVec2(vecPos.x + 755.f + easiedm_TabAnimation * 75.f, vecPos.y + 2),
					maincolor, gradientcolor,
					gradientcolor, maincolor);//top bar

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y + 565.f), ImVec2(vecPos.x + 755.f + easiedm_TabAnimation * 75.f, vecPos.y + 565.f - 2),
					maincolor, gradientcolor,
					gradientcolor, maincolor);//bottom bar

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(vecPos.x, vecPos.y), ImVec2(vecPos.x + 5, vecPos.y + 565.f), ImColor(maincolor)); // left bar

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(vecPos.x + 755.f + easiedm_TabAnimation * 75.f, vecPos.y), ImVec2((vecPos.x + 755.f + easiedm_TabAnimation * 75.f) - 5, vecPos.y + 565.f), ImColor(gradientcolor)); // right bar
			}
		}
		else {
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x, vecPos.y }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f, vecPos.y + 565.f }, ImColor(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 255.f * m_alpha / 255.f));                 //background shadow 2
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 10.f, vecPos.y + 5.f }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f - 10.f, vecPos.y + 565.f - 5.f }, ImColor(40.f / 255.f, 40.f / 255.f, 40.f / 255.f, 255 * m_alpha / 255.f));     //background shadow 1
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 15.f, vecPos.y + 10.f }, { vecPos.x + 755.f + easiedm_TabAnimation * 75.f - 15.f, vecPos.y + 565.f - 10.f }, ImColor(35.f / 255.f, 35.f / 255.f, 35.f / 255.f, 255 * m_alpha / 255.f));  //background
		}

		ImGui::PushFont(CRender::Fonts::m_MenuFontInner);

		HandleTabs();

		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::SetCursorPos({ 15.f, 10.f });

		ImGui::BeginChild("tabs", { 110.f - 15.f - 10.f + easiedm_TabAnimation * 75.f, 565.f - 10.f - 10.f }, false);

		ImGui::PushFont(CRender::Fonts::m_MenuBigFont);
		ImGui::SetCursorPos({ 90 , 55 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Iblis");
		ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuFont);
		ImGui::SetCursorPos({ 80, 50 + 75 + 10 + 20 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Rage bot");
		ImGui::SetCursorPos({ 80, 95 + 75 + 40 + 20 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Legit bot");
		ImGui::SetCursorPos({ 80, 140 + 75 + 80 + 20 });  ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Anti aim");
		ImGui::SetCursorPos({ 80, 185 + 75 + 120 + 20 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Visuals");
		ImGui::SetCursorPos({ 80, 230 + 75 + 160 + 20 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, easiedm_TabAnimation), "Misc");
		ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuIconFont);
		ImGui::SetCursorPos({ 60.f - 15.f, 20.f });
		ImGui::TextColored(ImColor(255, 255, 255), "A");
		ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuIconSmallFont);
		ImGui::SetCursorPos(ImVec2(25, 50 + 75 + 10));
		if (ImGui::TabButton("B", 1, m_CurrentTab == tabs::TAB_RAGE, ImVec2(45.f, 45.f), false))
			m_CurrentTab = tabs::TAB_RAGE;

		ImGui::SetCursorPos(ImVec2(25, 95 + 5 + 75 + 40));
		if (ImGui::TabButton("C", 2, m_CurrentTab == tabs::TAB_LEGIT, ImVec2(45.f, 45.f), false))
			m_CurrentTab = tabs::TAB_LEGIT;

		ImGui::SetCursorPos(ImVec2(25, 140 + 75 + 80));
		if (ImGui::TabButton("D", 3, m_CurrentTab == tabs::TAB_ANTIAIM, ImVec2(45.f, 45.f), false))
			m_CurrentTab = tabs::TAB_ANTIAIM;

		ImGui::SetCursorPos(ImVec2(25, 185 + 75 + 120));
		if (ImGui::TabButton("E", 4, m_CurrentTab == tabs::TAB_VISUALS, ImVec2(45.f, 45.f), false))
			m_CurrentTab = tabs::TAB_VISUALS;

		ImGui::SetCursorPos(ImVec2(25, 230 + 5 + 75 + 160));
		if (ImGui::TabButton("F", 5, m_CurrentTab == tabs::TAB_MISC, ImVec2(45.f, 45.f), false))
			m_CurrentTab = tabs::TAB_MISC;
		ImGui::PopFont();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopFont();

	}ImGui::End();

	ImGui::PopStyleVar();

	bool settings = m_Open && (m_CurrentTab == tabs::TAB_MISC) && settingsopen;
	static float m_settingsalpha = 0.0002f;
	m_settingsalpha = std::clamp(m_settingsalpha + (3.f * ImGui::GetIO().DeltaTime * (settings ? 1.f : -1.f)), 0.f, 1.f);
	double easiedm_settingsalpha = easingFunction(m_settingsalpha);

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, easiedm_settingsalpha * m_alpha);

	ImGui::Begin(XOR("##IblisSettings"), &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove); {

		const ImVec2 vecPos = ImGui::GetWindowPos();
		ImVec2 size = ImVec2(250.f, 565.f);
		const ImVec2 vecMousePos = ImGui::GetMousePos();
		ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), g_Config.c(g_Items.m_AccentColor).a() * (double)easiedm_settingsalpha);
		ImColor gradientcolor = ImColor(g_Config.c(g_Items.m_AccentColor2).r(), g_Config.c(g_Items.m_AccentColor2).g(), g_Config.c(g_Items.m_AccentColor2).b(), g_Config.c(g_Items.m_AccentColor2).a() * (double)easiedm_settingsalpha);

		ImGui::SetWindowPos(ImVec2(mainmenupos.x + (755.f + easiedm_TabAnimation * 75.f) + 5.f, mainmenupos.y));
		ImGui::SetWindowSize(ImVec2(size.x, size.y));

		if (g_Config.i(g_Items.m_Theme) < 1) {
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x, vecPos.y }, { (vecPos.x + size.x), vecPos.y + size.y }, ImColor(40.f / 255.f, 40.f / 255.f, 40.f / 255.f, 255.f * easiedm_settingsalpha / 255.f));                 //background shadow 2
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 10.f, vecPos.y + 5.f }, { (vecPos.x + size.x) - 10.f, vecPos.y + size.y - 5.f }, ImColor(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 255 * easiedm_settingsalpha / 255.f));     //background shadow 1
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 15.f, vecPos.y + 10.f }, { (vecPos.x + size.x) - 15.f, vecPos.y + size.y - 10.f }, ImColor(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 255 * easiedm_settingsalpha / 255.f));  //background

			if (g_Config.i(g_Items.m_GradientStyle) < 1) {
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y), ImVec2((vecPos.x + size.x), vecPos.y + 2),
					maincolor, ImColor(0, 0, 0, 0),
					ImColor(0, 0, 0, 0), maincolor);//top bar(ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x - 5, vecPos.y), ImVec2(vecPos.x + 5.f, vecPos.y + size.y),
					maincolor, maincolor,
					ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));//left bar(ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((vecPos.x + size.x), vecPos.y + size.y), ImVec2((vecPos.x + size.x) - size.x, vecPos.y + size.y - 2.f),
					gradientcolor, ImColor(0, 0, 0, 0),
					ImColor(0, 0, 0, 0), gradientcolor);//bottom bar (ok)

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2((vecPos.x + size.x), vecPos.y), ImVec2((vecPos.x + size.x) - 5.f, vecPos.y + size.y),
					ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0),
					gradientcolor, gradientcolor);//right bar(ok)
			}
			else {
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y), ImVec2(vecPos.x + size.x, vecPos.y + 2),
					maincolor, gradientcolor,
					gradientcolor, maincolor);//top bar

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(vecPos.x, vecPos.y + size.y), ImVec2(vecPos.x + size.x, vecPos.y + size.y - 2),
					maincolor, gradientcolor,
					gradientcolor, maincolor);//bottom bar

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(vecPos.x, vecPos.y), ImVec2(vecPos.x + 5, vecPos.y + size.y), ImColor(maincolor)); // left bar

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(vecPos.x + size.x, vecPos.y), ImVec2(vecPos.x + size.x - 5, vecPos.y + size.y), ImColor(gradientcolor)); // right bar
			}
		}
		else {
			DrawBackgroundBlur(ImGui::GetWindowDrawList(), (easiedm_alpha * 255));

			ImColor maincolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), easiedm_alpha * 255);
			ImColor alphacolor = ImColor(g_Config.c(g_Items.m_AccentColor).r(), g_Config.c(g_Items.m_AccentColor).g(), g_Config.c(g_Items.m_AccentColor).b(), 0);
			
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x, vecPos.y }, { (vecPos.x + size.x), vecPos.y + size.y }, ImColor(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 255.f * easiedm_settingsalpha / 255.f));                 //background shadow 2
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 10.f, vecPos.y + 5.f }, { (vecPos.x + size.x) - 10.f, vecPos.y + size.y - 5.f }, ImColor(40.f / 255.f, 40.f / 255.f, 40.f / 255.f, 255 * easiedm_settingsalpha / 255.f));     //background shadow 1
			ImGui::GetWindowDrawList()->AddRectFilled({ vecPos.x + 15.f, vecPos.y + 10.f }, { (vecPos.x + size.x) - 15.f, vecPos.y + size.y - 10.f }, ImColor(35.f / 255.f, 35.f / 255.f, 35.f / 255.f, 255 * easiedm_settingsalpha / 255.f));  //background
		}

		ImGui::PushFont(CRender::Fonts::m_MenuFontInner);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));

		//settings
		ImGui::SetCursorPos({ 15 , 10 });
		ImGui::BeginChild("child1", ImVec2(250 - 30, 565 - 10 - 10)); {
			ImGui::Separator("Settings");

			const char* Themes[] = {"Dark","Light"};
			if (ImGui::Combo("Theme", &g_Config.i(g_Items.m_Theme), Themes, IM_ARRAYSIZE(Themes))) {
			}

			if (g_Config.i(g_Items.m_Theme) < 1) {
				const char* styles[] = { "Outline","Full" };
				if (ImGui::Combo("Gradient style", &g_Config.i(g_Items.m_GradientStyle), styles, IM_ARRAYSIZE(styles))) {

				}
				ImGui::Spacing();

				ImGui::SetCursorPosX(10); ImGui::ColorPicker("Right Accent color", &g_Config.c(g_Items.m_AccentColor2), 2);
			}

			ImGui::SetCursorPosX(10); ImGui::ColorPicker((g_Config.i(g_Items.m_Theme) < 1) ? "Left Accent color" : "Accent Color", & g_Config.c(g_Items.m_AccentColor), 1);

			//ImGui::SliderFloat("Animation Speed", &g_Config.f(g_Items.m_AnimationSpeed), 0.1f , 10.f);

		}ImGui::EndChild();
		
		ImGui::PopStyleColor();

		ImGui::PopFont();
	}ImGui::End();

	ImGui::PopStyleVar();

}

void Menu::HandleTabs() noexcept
{
	switch (m_CurrentTab)
	{
	case tabs::TAB_RAGE:
	{
		Rage();
		break;
	}
	case tabs::TAB_LEGIT:
	{
		LegitBot();
		break;
	}
	case tabs::TAB_ANTIAIM:
	{
		AntiAim();
		break;
	}
	case tabs::TAB_VISUALS:
	{
		Visuals();
		break;
	}
	case tabs::TAB_MISC:
	{
		Misc();
		break;
	}
	default:;
	}
}

void Menu::HandleSubTabsRage()
{
	const auto SetUpTabs = [&](bool bOnlyGeneral = false, Items::Aimbot::General* General = nullptr,
		Items::Aimbot::Pistol* Pistol = nullptr,
		Items::Aimbot::HeavyPistol* HeavyPistol = nullptr,
		Items::Aimbot::Rifle* Rifle = nullptr,
		Items::Aimbot::Scout* Scout = nullptr,
		Items::Aimbot::AWP* AWP = nullptr,
		Items::Aimbot::Auto* Auto = nullptr,
		Items::Aimbot::Taser* Taser = nullptr)
	{
		Items::Aimbot::General* m_Used = General ? General :
			reinterpret_cast<Items::Aimbot::General*>(Pistol) ? reinterpret_cast<Items::Aimbot::General*>(Pistol) :
			reinterpret_cast<Items::Aimbot::General*>(HeavyPistol) ? reinterpret_cast<Items::Aimbot::General*>(HeavyPistol) :
			reinterpret_cast<Items::Aimbot::General*>(Rifle) ? reinterpret_cast<Items::Aimbot::General*>(Rifle) :
			reinterpret_cast<Items::Aimbot::General*>(Scout) ? reinterpret_cast<Items::Aimbot::General*>(Scout) :
			reinterpret_cast<Items::Aimbot::General*>(AWP) ? reinterpret_cast<Items::Aimbot::General*>(AWP) :
			reinterpret_cast<Items::Aimbot::General*>(Auto) ? reinterpret_cast<Items::Aimbot::General*>(Auto) :
			reinterpret_cast<Items::Aimbot::General*>(Taser) ? reinterpret_cast<Items::Aimbot::General*>(Taser) : nullptr;

		if (!m_Used)
			return;

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationRage * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 240 - m_SubTabAnimationRage * 10.f)); {
			ImGui::Separator("General");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(m_Used->m_Enabled));

			ImGui::SliderInt("Maximum FOV", &g_Config.i(m_Used->m_MaximumFOV), 0, 360, "%d", ImGuiSliderFlags_None);

			ImGui::Spacing();
			ImGui::Combo("Target priority", &g_Config.i(g_Items.m_Aimbot.m_General.m_TargetSelection), aimbot_priority, IM_ARRAYSIZE(aimbot_priority));

			ImGui::Spacing();
			ImGui::MultiCombobox("Hitbox selection", g_Config.vb(m_Used->m_Hitbox), hitboxes.data(), hitboxes.size());
			ImGui::Spacing();

			ImGui::MultiCombobox("Multi point selection", g_Config.vb(m_Used->m_Multipoint), multipoint.data(), multipoint.size());
			ImGui::Spacing();

			if (!g_Config.vb(m_Used->m_Multipoint).empty())
				ImGui::SliderInt("Multi point head scale", &g_Config.i(m_Used->m_MultipointHeadScale), 0, 100, "%d", ImGuiSliderFlags_None);

			if (!g_Config.vb(m_Used->m_Multipoint).empty())
				ImGui::SliderInt("Multi point body scale", &g_Config.i(m_Used->m_MultipointBodyScale), 0, 100, "%d", ImGuiSliderFlags_None);

			ImGui::Checkbox("Silent aim", &g_Config.b(m_Used->m_Silent));

			ImGui::Checkbox("Automatic fire", &g_Config.b(m_Used->m_AutoFire));
			ImGui::Checkbox("Automatic penetration", &g_Config.b(m_Used->m_AutoWall));

			ImGui::SliderInt("Minimum hit chance", &g_Config.i(m_Used->m_HitChanceAmount), 0, 100, "%d HC", ImGuiSliderFlags_None);
			ImGui::SliderInt("Minimum damage", &g_Config.i(m_Used->m_MinimumDamage), 0, 100, g_Config.i(m_Used->m_MinimumDamage) == 0 ? "DYNAMIC" : "%d DMG", ImGuiSliderFlags_None);

			ImGui::Checkbox("Scale damage on HP", &g_Config.b(m_Used->m_ScaleDamage));
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Optimisation", &g_Config.i(g_Items.m_Aimbot.m_General.m_OptimizationLevel), optimization_level, IM_ARRAYSIZE(optimization_level));

			ImGui::SliderInt("Minimum FPS", &g_Config.i(g_Items.m_Aimbot.m_General.m_DetermineFPS), 0, 144, g_Config.i(g_Items.m_Aimbot.m_General.m_DetermineFPS) == 0 ? "AVERAGE" : "%d FPS", ImGuiSliderFlags_None);
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationRage * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 240 - m_SubTabAnimationRage * 10.f)); {
			ImGui::Separator("Prediction");
			ImGui::Spacing();

			ImGui::Checkbox("Anti aim prediction", &g_Config.b(g_Items.m_Aimbot.m_General.m_AntiAimPrediction));
			ImGui::Checkbox("Damage prediction", &g_Config.b(m_Used->m_PredictiveDamage));
			ImGui::Checkbox("Smooth interpolation", &g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 35 + 250 + 15 });
		ImGui::BeginChild("child3", ImVec2(300, 240)); {
			ImGui::Separator("Miscellaneous");
			ImGui::Spacing();

			ImGui::Checkbox("Automatic scope", &g_Config.b(m_Used->m_AutoScope));
			ImGui::Checkbox("Automatic stop", &g_Config.b(m_Used->m_AutoStop));

			if (g_Config.b(m_Used->m_AutoStop))
			{
				ImGui::Spacing();
				ImGui::MultiCombobox("Automatic stop flags", g_Config.vb(m_Used->m_AutoStopFlags), AutoStopFlags.data(), AutoStopFlags.size());
				ImGui::Spacing();

				ImGui::Spacing();
				ImGui::MultiCombobox("Automatic stop conditions", g_Config.vb(m_Used->m_AutoStopConditions), AutoStopConditions.data(), AutoStopConditions.size());
				ImGui::Spacing();
			}

			ImGui::Spacing();
			ImGui::MultiCombobox("Prefer body aim", g_Config.vb(m_Used->m_PreferBodyAim), PreferBodyAim.data(), PreferBodyAim.size());
			ImGui::Spacing();
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75, 35 + 250 + 15 });
		ImGui::BeginChild("child4", ImVec2(300, 240)); {
			ImGui::Separator("Key binds");
			ImGui::Spacing();

			ImGui::Checkbox("Tickbase manipulation", &g_Config.b(g_Items.m_Aimbot.m_General.m_TickbaseManipulation));
			ImGui::SameLine();
			ImGui::KeyBind("##TICKBASEMANIPULATION", &g_Config.i(g_Items.m_Aimbot.m_General.m_TickbaseManipulationKey), &g_Config.i(g_Items.m_Aimbot.m_General.m_TickbaseManipulationMode));

			ImGui::Spacing();
			ImGui::MultiCombobox("Tickbase manipulation modes", g_Config.vb(g_Items.m_Aimbot.m_General.m_TickbaseManipulationSelection), TickbaseManipulation.data(), TickbaseManipulation.size());
			ImGui::Spacing();

			ImGui::Checkbox("Force body", &g_Config.b(g_Items.m_Aimbot.m_General.m_ForceBody));
			ImGui::SameLine();
			ImGui::KeyBind("##FORCEBODY", &g_Config.i(g_Items.m_Aimbot.m_General.m_ForceBodyKey), &g_Config.i(g_Items.m_Aimbot.m_General.m_ForceBodyMode));

			ImGui::Checkbox("Wait resolved", &g_Config.b(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdate));
			ImGui::SameLine();
			ImGui::KeyBind("##WAITRESOLVED", &g_Config.i(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateKey), &g_Config.i(g_Items.m_Aimbot.m_General.m_WaitForLowerbodyYawUpdateMode));

			ImGui::Checkbox("Override hit chance", &g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideHitChance));
			ImGui::SameLine();
			ImGui::KeyBind("##OVERRIDEHC", &g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideHitChanceKey), &g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideHitChanceMode));

			ImGui::SliderInt("Minimum hit chance", &g_Config.i(m_Used->m_OverrideHitChanceAmount), 0, 100, "%d HC", ImGuiSliderFlags_None);

			ImGui::Checkbox("Override damage", &g_Config.b(g_Items.m_Aimbot.m_General.m_OverrideDamage));
			ImGui::SameLine();
			ImGui::KeyBind("##OVERRIDEDMG", &g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideDamageKey), &g_Config.i(g_Items.m_Aimbot.m_General.m_OverrideDamageMode));

			ImGui::SliderInt("Minimum damage", &g_Config.i(m_Used->m_OverrideDamageAmount), 0, 100, g_Config.i(m_Used->m_OverrideDamageAmount) == 0 ? "DYNAMIC" : "%d DMG", ImGuiSliderFlags_None);
		}ImGui::EndChild();
	};

	switch (m_CurrentSubTabRage)
	{
	case sub_tabs::SUB_GENERAL:
	{
		SetUpTabs(true, &g_Items.m_Aimbot.m_General);
		break;
	}
	case sub_tabs::SUB_PISTOL:
	{
		SetUpTabs(false, nullptr, &g_Items.m_Aimbot.m_Pistol);
		break;
	}
	case sub_tabs::SUB_HEAVY_PISTOL:
	{
		SetUpTabs(false, nullptr, nullptr, &g_Items.m_Aimbot.m_HeavyPistol);
		break;
	}
	case sub_tabs::SUB_RIFLE:
	{
		SetUpTabs(false, nullptr, nullptr, nullptr, &g_Items.m_Aimbot.m_Rifle);
		break;
	}
	case sub_tabs::SUB_SCOUT:
	{
		SetUpTabs(false, nullptr, nullptr, nullptr, nullptr, &g_Items.m_Aimbot.m_Scout);
		break;
	}
	case sub_tabs::SUB_AWP:
	{
		SetUpTabs(false, nullptr, nullptr, nullptr, nullptr, nullptr, &g_Items.m_Aimbot.m_AWP);
		break;
	}
	case sub_tabs::SUB_AUTO:
	{
		SetUpTabs(false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &g_Items.m_Aimbot.m_Auto);
		break;
	}
	case sub_tabs::SUB_TASER:
	{
		SetUpTabs(false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &g_Items.m_Aimbot.m_Taser);
		break;
	}
	default:;
	}
}

void Menu::HandleSubTabsAntiAim()
{
	switch (m_CurrentSubTabAntiAim)
	{
	case sub_tabs::SUB_STAND:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child1", ImVec2(300, 240)); {
			ImGui::Separator("General");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_General.m_Enable));

			ImGui::Spacing();
			ImGui::Combo("Pitch", &g_Config.i(g_Items.m_AntiAim.m_Stand.m_Pitch), anti_aim_pitch, IM_ARRAYSIZE(anti_aim_pitch));
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Yaw", &g_Config.i(g_Items.m_AntiAim.m_Stand.m_Yaw), anti_aim_yaw, IM_ARRAYSIZE(anti_aim_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Automatic direction", &g_Config.b(g_Items.m_AntiAim.m_Stand.m_Freestanding));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child2", ImVec2(300, 240)); {
			ImGui::Separator("Fake Yaw");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Fake yaw", &g_Config.i(g_Items.m_AntiAim.m_Stand.m_FakeYaw), anti_aim_fake_yaw, IM_ARRAYSIZE(anti_aim_fake_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Hide shots", &g_Config.b(g_Items.m_AntiAim.m_Stand.m_HideShots));

		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 35 + 250 + 15 });
		ImGui::BeginChild("child3", ImVec2(300, 240)); {
			ImGui::Separator("Fake lag");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_Stand.m_Fakelag.m_Enable));
			ImGui::SliderInt("Limitation", &g_Config.i(g_Items.m_AntiAim.m_Stand.m_Fakelag.m_Limit), 0, 14, "%d", ImGuiSliderFlags_None);
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75, 35 + 250 + 15 });
		ImGui::BeginChild("child4", ImVec2(300, 240)); {
			ImGui::Separator("LBY");
			ImGui::Spacing();

			ImGui::Checkbox("Lowerbody yaw breaker", &g_Config.b(g_Items.m_AntiAim.m_Stand.m_LowerbodyBreak));
			ImGui::SliderInt("LBY breaking amount", &g_Config.i(g_Items.m_AntiAim.m_Stand.m_LowerbodyBreakAmount), -180, 180, "%d", ImGuiSliderFlags_None);
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_MOVING:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child1", ImVec2(300, 240)); {
			ImGui::Separator("General");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_General.m_Enable));

			ImGui::Spacing();
			ImGui::Combo("Pitch", &g_Config.i(g_Items.m_AntiAim.m_Move.m_Pitch), anti_aim_pitch, IM_ARRAYSIZE(anti_aim_pitch));
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Yaw", &g_Config.i(g_Items.m_AntiAim.m_Move.m_Yaw), anti_aim_yaw, IM_ARRAYSIZE(anti_aim_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Automatic direction", &g_Config.b(g_Items.m_AntiAim.m_Move.m_Freestanding));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child2", ImVec2(300, 240)); {
			ImGui::Separator("Fake Yaw");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Fake yaw", &g_Config.i(g_Items.m_AntiAim.m_Move.m_FakeYaw), anti_aim_fake_yaw, IM_ARRAYSIZE(anti_aim_fake_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Hide shots", &g_Config.b(g_Items.m_AntiAim.m_Move.m_HideShots));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 35 + 250 + 15 });
		ImGui::BeginChild("child3", ImVec2(300, 240)); {
			ImGui::Separator("Fake lag");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_Move.m_Fakelag.m_Enable));
			ImGui::SliderInt("Limitation", &g_Config.i(g_Items.m_AntiAim.m_Move.m_Fakelag.m_Limit), 0, 14, "%d", ImGuiSliderFlags_None);
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_AIR:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child1", ImVec2(300, 240)); {
			ImGui::Separator("General");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_General.m_Enable));

			ImGui::Spacing();
			ImGui::Combo("Pitch", &g_Config.i(g_Items.m_AntiAim.m_Air.m_Pitch), anti_aim_pitch, IM_ARRAYSIZE(anti_aim_pitch));
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Yaw", &g_Config.i(g_Items.m_AntiAim.m_Air.m_Yaw), anti_aim_yaw, IM_ARRAYSIZE(anti_aim_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Automatic direction", &g_Config.b(g_Items.m_AntiAim.m_Air.m_Freestanding));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 });
		ImGui::BeginChild("child2", ImVec2(300, 240)); {
			ImGui::Separator("Fake Yaw");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::Combo("Fake yaw", &g_Config.i(g_Items.m_AntiAim.m_Air.m_FakeYaw), anti_aim_fake_yaw, IM_ARRAYSIZE(anti_aim_fake_yaw));
			ImGui::Spacing();

			ImGui::Checkbox("Hide shots", &g_Config.b(g_Items.m_AntiAim.m_Air.m_HideShots));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 35 + 250 + 15 });
		ImGui::BeginChild("child3", ImVec2(300, 240)); {
			ImGui::Separator("Fake lag");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_AntiAim.m_Air.m_Fakelag.m_Enable));
			ImGui::SliderInt("Limitation", &g_Config.i(g_Items.m_AntiAim.m_Air.m_Fakelag.m_Limit), 0, 14, "%d", ImGuiSliderFlags_None);
		}ImGui::EndChild();
		break;
	}
	default:;
	}
}

void Menu::HandleSubTabsVisual()
{
	switch (m_CurrentSubTabVisual)
	{
	case sub_tabs::SUB_ENEMY:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("General");
			ImGui::Spacing();
			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_Enabled));
			ImGui::Checkbox("Dormant", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_DormantEnabled));
			ImGui::Checkbox("Box", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_BoxEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Enemies.m_BoxEnabled))
			{
				ImGui::Spacing();
				ImGui::Combo("Box type", &g_Config.i(g_Items.m_Visuals.m_Enemies.m_BoxType), BoxType, IM_ARRAYSIZE(BoxType));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Box color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_BoxColor));
			}

			ImGui::Checkbox("Name", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_NameEnabled));
			ImGui::Checkbox("Health bar", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_HealthBarEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Enemies.m_HealthBarEnabled))
			{
				ImGui::Checkbox("Health override color", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_HealthBarOverrideEnabled));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Health override color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_HealthBarOverrideColor));
			}

			ImGui::Checkbox("Flash effect", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_FlashEffectEnabled));
			ImGui::Checkbox("Hit damage", &g_Config.b(g_Items.m_Visuals.m_DamageEnabled));

			ImGui::Spacing();
			ImGui::MultiCombobox("Player flags", g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Flags), PlayerFlags.data(), PlayerFlags.size());

			ImGui::Checkbox("Skeleton", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_SkeletonEnabled));
			//ImGui::ColorPicker("##SKELETONENEMY", c_config::get()->c["enemy_skeleton_color"]);

			ImGui::Checkbox("Weapon text", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_WeaponTextEnabled));
			ImGui::Checkbox("Weapon icon", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_WeaponIconEnabled));
			//ImGui::ColorPicker("##WEAPONENEMY", c_config::get()->c["enemy_weapon_color"]);

			ImGui::Checkbox("Ammo", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_AmmoBarEnabled));
			//ImGui::ColorPicker("##AMMOENEMY", c_config::get()->c["enemy_ammo_color"]);

			ImGui::Checkbox("Lowerbody update", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_LBYBarEnabled));
			ImGui::SetCursorPosX(6); ImGui::ColorPicker("Lowerbody update color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_BoxColor));

			ImGui::Checkbox("Distance", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_DistanceEnabled));
			ImGui::Checkbox("Glow", &g_Config.b(g_Items.m_Visuals.m_Enemies.m_GlowEnabled));
			ImGui::Spacing();
			ImGui::SetCursorPosX(6); ImGui::ColorPicker("Glow color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_GlowColor));

			ImGui::Checkbox("Out of FOV arrow", &g_Config.b(g_Items.m_Visuals.m_OffScreenEnabled));
			//ImGui::ColorPicker("##OFFSCREENENEMY", &c_config::get()->c["enemy_offscreen_color"]);

			if (g_Config.b(g_Items.m_Visuals.m_OffScreenEnabled))
			{
				ImGui::SliderFloat("Out of FOV size", &g_Config.f(g_Items.m_Visuals.m_OffScreenSize), 0, 200, "%1.f PX", ImGuiSliderFlags_None);
				ImGui::SliderFloat("Out of FOV distance", &g_Config.f(g_Items.m_Visuals.m_OffScreenDistance), 0, 200, "%1.f DIST", ImGuiSliderFlags_None);
				ImGui::Spacing();
				ImGui::MultiCombobox("Out of FOV flags", g_Config.vb(g_Items.m_Visuals.m_OffScreenFlags), OutOfFovlags.data(), OutOfFovlags.size());
			}
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75, 40 + 250 + 15 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Chams");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::MultiCombobox("Activation type", g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_ActivationType), ActivationType.data(), ActivationType.size());

			ImGui::Spacing();
			ImGui::Combo("Model selection", &g_Config.i(g_Items.m_Visuals.m_Enemies.m_Chams.m_ModelSelection), visuals_chams_mode, IM_ARRAYSIZE(visuals_chams_mode));
			ImGui::Spacing();

			if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_ActivationType)[0])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Visible color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_VisibleColor));
			}

			if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_ActivationType)[1])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Behind wall color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_BehindWallColor));
			}

			ImGui::Spacing();
			ImGui::MultiCombobox("History activation type", g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryActivationType), ActivationType.data(), ActivationType.size());

			ImGui::Spacing();
			ImGui::Combo("History model selection", &g_Config.i(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryModelSelection), visuals_chams_mode, IM_ARRAYSIZE(visuals_chams_mode));
			ImGui::Spacing();

			if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryActivationType)[0])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Visible history color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryVisibleColor));
			}

			if (g_Config.vb(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryActivationType)[1])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Behind wall history color", &g_Config.c(g_Items.m_Visuals.m_Enemies.m_Chams.m_HistoryBehindWallColor));
			}

			ImGui::Checkbox("Disable model occulusion", &g_Config.b(g_Items.m_Visuals.m_DisableModelOcculusion));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child3", ImVec2(300, 490 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Preview");

			ImGui::Spacing();
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_TEAM:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("General");
			ImGui::Spacing();

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_Visuals.m_Allies.m_Enabled));
			ImGui::Checkbox("Dormant", &g_Config.b(g_Items.m_Visuals.m_Allies.m_DormantEnabled));
			ImGui::Checkbox("Box", &g_Config.b(g_Items.m_Visuals.m_Allies.m_BoxEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Allies.m_BoxEnabled))
			{
				ImGui::Spacing();
				ImGui::Combo("Box type", &g_Config.i(g_Items.m_Visuals.m_Allies.m_BoxType), BoxType, IM_ARRAYSIZE(BoxType));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Box color", &g_Config.c(g_Items.m_Visuals.m_Allies.m_BoxColor));
			}

			ImGui::Checkbox("Name", &g_Config.b(g_Items.m_Visuals.m_Allies.m_NameEnabled));
			ImGui::Checkbox("Health bar", &g_Config.b(g_Items.m_Visuals.m_Allies.m_HealthBarEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Allies.m_HealthBarEnabled))
			{
				ImGui::Checkbox("Health override color", &g_Config.b(g_Items.m_Visuals.m_Allies.m_HealthBarOverrideEnabled));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Health override color", &g_Config.c(g_Items.m_Visuals.m_Allies.m_HealthBarOverrideColor));
			}

			ImGui::Checkbox("Flash effect", &g_Config.b(g_Items.m_Visuals.m_Allies.m_FlashEffectEnabled));

			ImGui::Spacing();
			ImGui::MultiCombobox("Player flags", g_Config.vb(g_Items.m_Visuals.m_Allies.m_Flags), PlayerFlags.data(), PlayerFlags.size());

			ImGui::Checkbox("Skeleton", &g_Config.b(g_Items.m_Visuals.m_Allies.m_SkeletonEnabled));

			ImGui::Checkbox("Weapon text", &g_Config.b(g_Items.m_Visuals.m_Allies.m_WeaponTextEnabled));
			ImGui::Checkbox("Weapon icon", &g_Config.b(g_Items.m_Visuals.m_Allies.m_WeaponIconEnabled));
			//ImGui::ColorPicker("##WEAPONENEMY", c_config::get()->c["enemy_weapon_color"]);

			ImGui::Checkbox("Ammo", &g_Config.b(g_Items.m_Visuals.m_Allies.m_AmmoBarEnabled));
			//ImGui::ColorPicker("##AMMOENEMY", c_config::get()->c["enemy_ammo_color"]);

			ImGui::Checkbox("Distance", &g_Config.b(g_Items.m_Visuals.m_Allies.m_DistanceEnabled));

			ImGui::Checkbox("Glow", &g_Config.b(g_Items.m_Visuals.m_Allies.m_GlowEnabled));
			ImGui::Spacing();
			ImGui::SetCursorPosX(6); ImGui::ColorPicker("Glow color", &g_Config.c(g_Items.m_Visuals.m_Allies.m_GlowColor));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75, 40 + 250 + 15 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Chams");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::MultiCombobox("Activation type", g_Config.vb(g_Items.m_Visuals.m_Allies.m_Chams.m_ActivationType), ActivationType.data(), ActivationType.size());

			ImGui::Spacing();
			ImGui::Combo("Model selection", &g_Config.i(g_Items.m_Visuals.m_Allies.m_Chams.m_ModelSelection), visuals_chams_mode, IM_ARRAYSIZE(visuals_chams_mode));
			ImGui::Spacing();

			if (g_Config.vb(g_Items.m_Visuals.m_Allies.m_Chams.m_ActivationType)[0])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Visible color", &g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_VisibleColor));
			}

			if (g_Config.vb(g_Items.m_Visuals.m_Allies.m_Chams.m_ActivationType)[1])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Behind wall color", &g_Config.c(g_Items.m_Visuals.m_Allies.m_Chams.m_BehindWallColor));
			}

			ImGui::Checkbox("Disable model occulusion", &g_Config.b(g_Items.m_Visuals.m_DisableModelOcculusion));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child3", ImVec2(300, 490 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Preview");

			ImGui::Spacing();
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_LOCAL:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("General");

			ImGui::Checkbox("Enable", &g_Config.b(g_Items.m_Visuals.m_Local.m_Enabled));
			ImGui::Checkbox("Box", &g_Config.b(g_Items.m_Visuals.m_Local.m_BoxEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Local.m_BoxEnabled))
			{
				ImGui::Spacing();
				ImGui::Combo("Box type", &g_Config.i(g_Items.m_Visuals.m_Local.m_BoxType), BoxType, IM_ARRAYSIZE(BoxType));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Box color", &g_Config.c(g_Items.m_Visuals.m_Local.m_BoxColor));
			}

			ImGui::Checkbox("Name", &g_Config.b(g_Items.m_Visuals.m_Local.m_NameEnabled));
			ImGui::Checkbox("Health bar", &g_Config.b(g_Items.m_Visuals.m_Local.m_HealthBarEnabled));
			if (g_Config.b(g_Items.m_Visuals.m_Local.m_HealthBarEnabled))
			{
				ImGui::Checkbox("Health override color", &g_Config.b(g_Items.m_Visuals.m_Local.m_HealthBarOverrideEnabled));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Health override color", &g_Config.c(g_Items.m_Visuals.m_Local.m_HealthBarOverrideColor));
			}

			ImGui::Checkbox("Flash effect", &g_Config.b(g_Items.m_Visuals.m_Local.m_FlashEffectEnabled));
			ImGui::Spacing();
			ImGui::MultiCombobox("Player flags", g_Config.vb(g_Items.m_Visuals.m_Local.m_Flags), PlayerFlags.data(), PlayerFlags.size());

			ImGui::Checkbox("Skeleton", &g_Config.b(g_Items.m_Visuals.m_Local.m_SkeletonEnabled));

			ImGui::Checkbox("Weapon text", &g_Config.b(g_Items.m_Visuals.m_Local.m_WeaponTextEnabled));
			ImGui::Checkbox("Weapon icon", &g_Config.b(g_Items.m_Visuals.m_Local.m_WeaponIconEnabled));
			//ImGui::ColorPicker("##WEAPONENEMY", c_config::get()->c["enemy_weapon_color"]);

			ImGui::Checkbox("Ammo", &g_Config.b(g_Items.m_Visuals.m_Local.m_AmmoBarEnabled));
			//ImGui::ColorPicker("##AMMOENEMY", c_config::get()->c["enemy_ammo_color"]);

			ImGui::Checkbox("Distance", &g_Config.b(g_Items.m_Visuals.m_Local.m_DistanceEnabled));

			ImGui::Checkbox("Glow", &g_Config.b(g_Items.m_Visuals.m_Local.m_GlowEnabled));
			ImGui::SetCursorPosX(6);  ImGui::ColorPicker("Glow color", &g_Config.c(g_Items.m_Visuals.m_Local.m_GlowColor));

			ImGui::Checkbox("Third person", &g_Config.b(g_Items.m_Visuals.m_ThirdpersonEnabled));
			ImGui::SameLine();
			ImGui::KeyBind("##KEYBINDTHIRDPERSON", &g_Config.i(g_Items.m_Visuals.m_ThirdpersonKey), &g_Config.i(g_Items.m_Visuals.m_ThirdpersonKeyMode));
			
			if (g_Config.b(g_Items.m_Visuals.m_ThirdpersonEnabled))
				ImGui::SliderInt("Third person distance", &g_Config.i(g_Items.m_Visuals.m_ThirdpersonDistance), 0, 200);
			else
				ImGui::Spacing();

			ImGui::Checkbox("Automatic peek circle", &g_Config.b(g_Items.m_Visuals.m_AutoPeekIndicator));
			if (g_Config.b(g_Items.m_Visuals.m_AutoPeekIndicator)) {
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Automatic peek circle color", &g_Config.c(g_Items.m_Visuals.m_AutoPeekIndicatorColor));
			}

			ImGui::Checkbox("Penetration crosshair", &g_Config.b(g_Items.m_Visuals.m_PenCrosshairEnabled));

			if (g_Config.b(g_Items.m_Visuals.m_PenCrosshairEnabled))
			{
				ImGui::Spacing();
				ImGui::Combo("Penetration crosshair type", &g_Config.i(g_Items.m_Visuals.m_PenCrosshairType), PenCrosshairType, IM_ARRAYSIZE(PenCrosshairType));

				if (g_Config.i(g_Items.m_Visuals.m_PenCrosshairType) == 1)
					ImGui::SetCursorPosX(6);  ImGui::ColorPicker("Penetration crosshair color", &g_Config.c(g_Items.m_Visuals.m_PenColor));

				ImGui::Checkbox("Penetration damage", &g_Config.b(g_Items.m_Visuals.m_PenCrosshairDmgEnabled));
			}

		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75, 40 + 250 + 15 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Chams");
			ImGui::Spacing();

			ImGui::Spacing();
			ImGui::MultiCombobox("Activation type", g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_ActivationType), ActivationType.data(), ActivationType.size());

			ImGui::Spacing();
			ImGui::Combo("Model selection", &g_Config.i(g_Items.m_Visuals.m_Local.m_Chams.m_ModelSelection), visuals_chams_mode, IM_ARRAYSIZE(visuals_chams_mode));
			ImGui::Spacing();

			if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_ActivationType)[0])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Visible color", &g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_VisibleColor));
			}

			if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_ActivationType)[1])
			{
				ImGui::SetCursorPosX(6);
				ImGui::ColorPicker("Behind wall color", &g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_BehindWallColor));
			}

			//ImGui::Spacing();
			//ImGui::MultiCombobox("Fake lag activation type", g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagActivationType), ActivationType.data(), ActivationType.size());

			//ImGui::Spacing();
			//ImGui::Combo("Fake lag model selection", &g_Config.i(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagModelSelection), visuals_chams_mode, IM_ARRAYSIZE(visuals_chams_mode));
			//ImGui::Spacing();

			//if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagActivationType)[0])
			//{
			//	ImGui::SetCursorPosX(6);
			//	ImGui::ColorPicker("Visible fakelag color", &g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagVisibleColor));
			//}

			//if (g_Config.vb(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagActivationType)[1])
			//{
			//	ImGui::SetCursorPosX(6);
			//	ImGui::ColorPicker("Behind wall fakelag color", &g_Config.c(g_Items.m_Visuals.m_Local.m_Chams.m_FakelagBehindWallColor));
			//}

			ImGui::Checkbox("Transexual when scoped", &g_Config.b(g_Items.m_Visuals.m_Local.m_Chams.m_BlendWhenScoped));
			ImGui::Checkbox("Disable model occulusion", &g_Config.b(g_Items.m_Visuals.m_DisableModelOcculusion));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child3", ImVec2(300, 490 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Preview");

			ImGui::Spacing();
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_WORLD:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("World");

			ImGui::Spacing();

			ImGui::MultiCombobox("Brightness adjustment", g_Config.vb(g_Items.m_Visuals.m_World.m_BrightnessAdjustment), BrightnessAdjustment.data(), BrightnessAdjustment.size());
			ImGui::Spacing();

			if (g_Config.vb(g_Items.m_Visuals.m_World.m_BrightnessAdjustment).at(0))
			{
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("World color", &g_Config.c(g_Items.m_Visuals.m_World.m_WorldColor));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Skybox Color", &g_Config.c(g_Items.m_Visuals.m_World.m_SkyColor));
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Props Color", &g_Config.c(g_Items.m_Visuals.m_World.m_PropColor));
			}

			ImGui::Checkbox("World items", &g_Config.b(g_Items.m_Visuals.m_World.m_ItemsEnabled));

			if (g_Config.b(g_Items.m_Visuals.m_World.m_ItemsEnabled))
			{
				ImGui::Spacing();
				ImGui::MultiCombobox("World item flags", g_Config.vb(g_Items.m_Visuals.m_World.m_ItemFlags), ItemFlags.data(), ItemFlags.size());
			}

			ImGui::Checkbox("Projectiles", &g_Config.b(g_Items.m_Visuals.m_World.m_ProjectileEnabled));

			if (g_Config.b(g_Items.m_Visuals.m_World.m_ProjectileEnabled))
			{
				ImGui::Spacing();
				ImGui::MultiCombobox("Projectile flags", g_Config.vb(g_Items.m_Visuals.m_World.m_ProjectileType), ProjectileFlags.data(), ProjectileFlags.size());
			}

			ImGui::Checkbox("Bullet server impact", &g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactServer));
			if (g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactServer)) {
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Bullet server color (Server)", &g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactServerColor));
			}

			ImGui::Checkbox("Bullet client impact", &g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactClient));
			if (g_Config.b(g_Items.m_Visuals.m_World.m_BulletImpactClient)) {
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Bullet impact color (Client)", &g_Config.c(g_Items.m_Visuals.m_World.m_BulletImpactClientColor));
			}

		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 240 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Effects");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::MultiCombobox("Removals", g_Config.vb(g_Items.m_Visuals.m_Effects.m_Removals), Removals.data(), Removals.size());
			ImGui::Spacing();
			ImGui::Checkbox("Override Fog", &g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFog));
			if (g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFog)) {
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPosX(6); ImGui::ColorPicker("Fog Color", &g_Config.c(g_Items.m_Visuals.m_Effects.m_FogColor));

				ImGui::SliderInt("Fog Start", &g_Config.i(g_Items.m_Visuals.m_Effects.m_FogStart), 0, 1000, "%d", ImGuiSliderFlags_None);
				ImGui::SliderInt("Fog End", &g_Config.i(g_Items.m_Visuals.m_Effects.m_FogEnd), 0, 1000, "%d", ImGuiSliderFlags_None);
			}

			ImGui::Checkbox("Hitmarker", &g_Config.b(g_Items.m_Visuals.m_HitmarkerEnabled));
			ImGui::Checkbox("Hitsound", &g_Config.b(g_Items.m_Visuals.m_Effects.m_EnableHitSound));
			ImGui::Checkbox("Party mode", &g_Config.b(g_Items.m_Visuals.m_Effects.m_PartyMode));

			ImGui::Checkbox("Aspect ratio", &g_Config.b(g_Items.m_Visuals.m_AspectRatio));
			ImGui::SliderFloat("Aspect ratio amount", &g_Config.f(g_Items.m_Visuals.m_AspectRatioAmount), 0, 5, "%0.1f", ImGuiSliderFlags_None);

			ImGui::Checkbox("Force enable crosshair", &g_Config.b(g_Items.m_Visuals.m_ForceCrosshair));
		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 35 + 250 + 15 });
		ImGui::BeginChild("child3", ImVec2(300, 240)); {
			ImGui::Separator("Fov");
			ImGui::Spacing();

			ImGui::Checkbox("Override FOV", &g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfView));
			if (g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfView)) {
				ImGui::SliderInt("Override FOV amount", &g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmount), 1, 180, "%d", ImGuiSliderFlags_None);
				ImGui::SliderInt("Add FOV amount (first zoom)", &g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmountScoped), 0, 90, "%d", ImGuiSliderFlags_None);
				ImGui::SliderInt("Add FOV amount (second zoom)", &g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideFieldOfViewAmountZoomed), 0, 90, "%d", ImGuiSliderFlags_None);
			}

			ImGui::Spacing();
			ImGui::Checkbox("Override Viewmodel FOV", &g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideViewModel));
			if (g_Config.b(g_Items.m_Visuals.m_Effects.m_OverrideViewModel)) {
				ImGui::SliderInt("Viewmodel FOV amount", &g_Config.i(g_Items.m_Visuals.m_Effects.m_OverrideViewModelFov), 1, 180, "%d", ImGuiSliderFlags_None);
			}
			ImGui::Spacing();

		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75, 35 + 250 + 15 });
		ImGui::BeginChild("child4", ImVec2(300, 240)); {
			ImGui::Separator("4");
			ImGui::Spacing();
		}ImGui::EndChild();
		break;
	}
	case sub_tabs::SUB_SKIN:
	{
		ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child1", ImVec2(300, 515 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("General");

		}ImGui::EndChild();

		ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 50 + m_SubTabAnimationVisuals * 10.f });
		ImGui::BeginChild("child2", ImVec2(300, 490 - m_SubTabAnimationVisuals * 10.f)); {
			ImGui::Separator("Preview");

			ImGui::Spacing();
		}ImGui::EndChild();
		break;
	}
	default:;
	}
}

void Menu::Rage()
{
	const ImVec2 mousepos = ImGui::GetMousePos();
	const ImVec2 windowpos = ImGui::GetWindowPos();

	//subtab
	bool bMouseOnPos_SubRage = false &&
		mousepos.x > windowpos.x + 110.f //x left
		&& mousepos.x < windowpos.x + (85.f + 655.f - 15.f) + (easiedm_TabAnimation * 75.f) //x right
		&& (mousepos.y > windowpos.y + 10.f //y up
			&& !(mousepos.y > windowpos.y + (15.f + 20.f + m_SubTabAnimationRage * 10.f))); //y down

	m_SubTabAnimationRage = bMouseOnPos_SubRage ? m_SubTabAnimationRage += g_Csgo.m_Globals->m_FrameTime * 5.f : m_SubTabAnimationRage -= g_Csgo.m_Globals->m_FrameTime * 5.f;
	m_SubTabAnimationRage = std::clamp(m_SubTabAnimationRage, 0.f, 1.f);

	ImGui::SetCursorPos(ImVec2(110.f + easiedm_TabAnimation * 75, 15.f));
	ImGui::BeginChild("subtabsrage", { 615.f, 30.f + m_SubTabAnimationRage * 10.f }, false, ImGuiWindowFlags_NoScrollbar); {

		ImGui::PushFont(CRender::Fonts::m_MenuFontSmall);
		ImGui::SetCursorPos({ 10 + 20 * 0 + 2, 25 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "General");
		ImGui::SetCursorPos({ 10 + 20 * 4 + 5, 25 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "Pistol");
		ImGui::SetCursorPos({ 10 + 20 * 8 + 4, 25 });  ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "H.Pistol");
		ImGui::SetCursorPos({ 10 + 20 * 12 + 6, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "Auto");
		ImGui::SetCursorPos({ 10 + 20 * 16 + 5, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "Scout");
		ImGui::SetCursorPos({ 10 + 20 * 20 + 6, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "AWP");
		ImGui::SetCursorPos({ 10 + 20 * 24 + 6, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "Rifle");
		ImGui::SetCursorPos({ 10 + 20 * 28 + 5, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationRage), "Taser");
		ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon);
		ImGui::SetCursorPos(ImVec2(10 + 20 * 0, 0));
		if (ImGui::TabButton("D", 1, m_CurrentSubTabRage == sub_tabs::SUB_GENERAL, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_GENERAL;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 4, 0));
		if (ImGui::TabButton("F", 2, m_CurrentSubTabRage == sub_tabs::SUB_PISTOL, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_PISTOL;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 8, 0));
		if (ImGui::TabButton("G", 3, m_CurrentSubTabRage == sub_tabs::SUB_HEAVY_PISTOL, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_HEAVY_PISTOL;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 12, 0));
		if (ImGui::TabButton("H", 4, m_CurrentSubTabRage == sub_tabs::SUB_AUTO, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_AUTO;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 16, 0));
		if (ImGui::TabButton("M", 5, m_CurrentSubTabRage == sub_tabs::SUB_SCOUT, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_SCOUT;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 20, 0));
		if (ImGui::TabButton("I", 5, m_CurrentSubTabRage == sub_tabs::SUB_AWP, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_AWP;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 24, 0));
		if (ImGui::TabButton("J", 5, m_CurrentSubTabRage == sub_tabs::SUB_RIFLE, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_RIFLE;

		ImGui::SetCursorPos(ImVec2(10 + 20 * 28, 0));
		if (ImGui::TabButton("K", 5, m_CurrentSubTabRage == sub_tabs::SUB_TASER, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabRage = sub_tabs::SUB_TASER;


		ImGui::PopFont();


		//ImGui::PopStyleColor();
		//ImGui::PopFont();
	}ImGui::EndChild();

	HandleSubTabsRage();
}

void Menu::LegitBot()
{
	ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 25 });
	ImGui::BeginChild("child1", ImVec2(300, 250)); {
		ImGui::Separator("General");

	}ImGui::EndChild();

	ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 25 });
	ImGui::BeginChild("child2", ImVec2(300, 250)); {
		ImGui::Separator("Humanizer");
		ImGui::Spacing();
	}ImGui::EndChild();

	ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 25 + 250 + 15 });
	ImGui::BeginChild("child3", ImVec2(300, 250)); {
		ImGui::Separator("Misc");
		ImGui::Spacing();
	}ImGui::EndChild();

	ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75, 25 + 250 + 15 });
	ImGui::BeginChild("child4", ImVec2(300, 250)); {
		ImGui::Separator("Smoothness");
		ImGui::Spacing();
	}ImGui::EndChild();
}

void Menu::AntiAim()
{
	const ImVec2 mousepos = ImGui::GetMousePos();
	const ImVec2 windowpos = ImGui::GetWindowPos();

	//subtab
	bool bMouseOnPos_SubAntiAim = false &&
		mousepos.x > windowpos.x + 110.f //x left
		&& mousepos.x < windowpos.x + (85.f + 655.f - 15.f) + (easiedm_TabAnimation * 75.f) //x right
		&& (mousepos.y > windowpos.y + 10.f //y up
			&& !(mousepos.y > windowpos.y + (15.f + 20.f + m_SubTabAnimationAntiAim * 10.f))); //y down

	m_SubTabAnimationAntiAim = bMouseOnPos_SubAntiAim ? m_SubTabAnimationAntiAim += g_Csgo.m_Globals->m_FrameTime * 5.f : m_SubTabAnimationAntiAim -= g_Csgo.m_Globals->m_FrameTime * 5.f;
	m_SubTabAnimationAntiAim = std::clamp(m_SubTabAnimationAntiAim, 0.f, 1.f);

	ImGui::SetCursorPos(ImVec2(110.f + easiedm_TabAnimation * 75, 15.f));
	ImGui::BeginChild("subtabsvisual", { 615.f, 30.f + m_SubTabAnimationAntiAim * 10.f }, false, ImGuiWindowFlags_NoScrollbar); {

		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon);
		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 - 40.f * 2, 0));
		if (ImGui::TabButton("C", 1, m_CurrentSubTabAntiAim == sub_tabs::SUB_STAND, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabAntiAim = sub_tabs::SUB_STAND;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2, 0));
		if (ImGui::TabButton("N", 2, m_CurrentSubTabAntiAim == sub_tabs::SUB_MOVING, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabAntiAim = sub_tabs::SUB_MOVING;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 + 40.f * 2, 0));
		if (ImGui::TabButton("O", 3, m_CurrentSubTabAntiAim == sub_tabs::SUB_AIR, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabAntiAim = sub_tabs::SUB_AIR;

		ImGui::PopFont();
	}ImGui::EndChild();

	HandleSubTabsAntiAim();
}

void Menu::Visuals() {
	const ImVec2 mousepos = ImGui::GetMousePos();
	const ImVec2 windowpos = ImGui::GetWindowPos();

	//subtab
	bool bMouseOnPos_SubVisual = false &&
		mousepos.x > windowpos.x + 110.f //x left
		&& mousepos.x < windowpos.x + (85.f + 655.f - 15.f) + (easiedm_TabAnimation * 75.f) //x right
		&& (mousepos.y > windowpos.y + 10.f //y up
			&& !(mousepos.y > windowpos.y + (15.f + 20.f + m_SubTabAnimationVisuals * 10.f))); //y down

	m_SubTabAnimationVisuals = bMouseOnPos_SubVisual ? m_SubTabAnimationVisuals += g_Csgo.m_Globals->m_FrameTime * 5.f : m_SubTabAnimationVisuals -= g_Csgo.m_Globals->m_FrameTime * 5.f;
	m_SubTabAnimationVisuals = std::clamp(m_SubTabAnimationVisuals, 0.f, 1.f);

	ImGui::SetCursorPos(ImVec2(110.f + easiedm_TabAnimation * 75, 15.f));
	ImGui::BeginChild("subtabsvisual", { 615.f, 30.f + m_SubTabAnimationVisuals * 10.f }, false, ImGuiWindowFlags_NoScrollbar); {

		ImGui::PushFont(CRender::Fonts::m_MenuFontSmall);
		ImGui::SetCursorPos({ 10 + 20 * 0 + 0, 25 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationVisuals), "Enemy");
		ImGui::SetCursorPos({ 10 + 20 * 4 + 0, 25 });   ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationVisuals), "Team");
		ImGui::SetCursorPos({ 10 + 20 * 8 + 0, 25 });  ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationVisuals), "Local");
		ImGui::SetCursorPos({ 10 + 20 * 12 + 0, 25 }); ImGui::TextColored(ImColor(255.f, 255.f, 255.f, m_SubTabAnimationVisuals), "World");
		ImGui::PopFont();

		ImGui::PushFont(CRender::Fonts::m_MenuSubtabIcon);
		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 - 80.f * 2, 0));
		if (ImGui::TabButton("A", 1, m_CurrentSubTabVisual == sub_tabs::SUB_ENEMY, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabVisual = sub_tabs::SUB_ENEMY;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 - 40.f * 2, 0));
		if (ImGui::TabButton("B", 2, m_CurrentSubTabVisual == sub_tabs::SUB_TEAM, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabVisual = sub_tabs::SUB_TEAM;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2, 0));
		if (ImGui::TabButton("C", 3, m_CurrentSubTabVisual == sub_tabs::SUB_LOCAL, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabVisual = sub_tabs::SUB_LOCAL;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 + 40.f * 2, 0));
		if (ImGui::TabButton("L", 4, m_CurrentSubTabVisual == sub_tabs::SUB_WORLD, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabVisual = sub_tabs::SUB_WORLD;

		ImGui::SetCursorPos(ImVec2(615 / 2 - (30.f) / 2 - 2 + 80.f * 2, 0));
		if (ImGui::TabButton("E", 4, m_CurrentSubTabVisual == sub_tabs::SUB_SKIN, ImVec2(30.f, 30.f), true))
			m_CurrentSubTabVisual = sub_tabs::SUB_SKIN;

		ImGui::PopFont();
	}ImGui::EndChild();

	HandleSubTabsVisual();
}

void Menu::Misc() {

	ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 25 });
	ImGui::BeginChild("child1", ImVec2(300, 250)); {
		ImGui::Separator("General");
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::MultiCombobox("Indicators", g_Config.vb(g_Items.m_Visuals.m_Indicators), Indicators.data(), Indicators.size());
		ImGui::Spacing();

		ImGui::Checkbox("Settings", &settingsopen);
		ImGui::Checkbox("Automatic jump", &g_Config.b(g_Items.m_Misc.m_AutomaticJump));
		ImGui::Checkbox("Automatic strafe", &g_Config.b(g_Items.m_Misc.m_AutomaticStrafe));
		ImGui::Checkbox("Automatic peek", &g_Config.b(g_Items.m_Misc.m_AutomaticPeek));
		ImGui::SameLine();
		ImGui::KeyBind("##AUTOPEEKKEY", &g_Config.i(g_Items.m_Misc.m_AutomaticPeekKeyBinds), &g_Config.i(g_Items.m_Misc.m_AutomaticPeekKeyBindsMode));

		ImGui::Checkbox("Fake walk", &g_Config.b(g_Items.m_Misc.m_FakeWalk));
		ImGui::SameLine();
		ImGui::KeyBind("##FAKEWALKKEY", &g_Config.i(g_Items.m_Misc.m_FakeWalkKeyBinds), &g_Config.i(g_Items.m_Misc.m_FakeWalkBindsMode));

		ImGui::Checkbox("Fake latency", &g_Config.b(g_Items.m_Misc.m_FakeLatency));
		ImGui::SameLine();
		ImGui::KeyBind("##FAKELATENCYKEY", &g_Config.i(g_Items.m_Misc.m_FakeLatencyKey), &g_Config.i(g_Items.m_Misc.m_FakeLatencyMode));
	
		ImGui::SliderInt("Fake latency amount", &g_Config.i(g_Items.m_Misc.m_FakeLatencyAmount), 0, 1000, "%d", ImGuiSliderFlags_None);
	}ImGui::EndChild();

	ImGui::SetCursorPos({ 110 + easiedm_TabAnimation * 75 , 25 + 250 + 15 });
	ImGui::BeginChild("child3", ImVec2(300, 250)); {
		ImGui::Separator("Other");
		ImGui::Spacing();

		ImGui::Checkbox("Anti untrusted", &g_Config.b(g_Items.m_AntiAim.m_General.m_AntiUntrusted));
		ImGui::Checkbox("Unlock hidden cvars", &g_Config.b(g_Items.m_Misc.m_UnlockHiddenCvars));
		ImGui::Checkbox("Preverse killfeed", &g_Config.b(g_Items.m_Visuals.m_PreverseKillfeedEnabled));

		ImGui::Spacing();
		ImGui::Combo("Clan tag spammer", &g_Config.i(g_Items.m_Misc.m_ClantagSpammer), clantag, IM_ARRAYSIZE(clantag));
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::MultiCombobox("Notifications", g_Config.vb(g_Items.m_Misc.m_Notifications), Notifications.data(), Notifications.size());
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::MultiCombobox("Automatic buy weapons", g_Config.vb(g_Items.m_Misc.m_AutoBuyMain), AutoBuyWeapons.data(), AutoBuyWeapons.size());
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::MultiCombobox("Automatic buy secondary", g_Config.vb(g_Items.m_Misc.m_AutoBuySecondary), AutoBuySecondary.data(), AutoBuySecondary.size());
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::MultiCombobox("Automatic buy misc", g_Config.vb(g_Items.m_Misc.m_AutoBuyMisc), AutoBuyMisc.data(), AutoBuyMisc.size());
		ImGui::Spacing();

	}ImGui::EndChild();

	ImGui::SetCursorPos({ 110 + 15 + 300 + easiedm_TabAnimation * 75 , 25 });
	ImGui::BeginChild("child4", ImVec2(300, 515)); {
		if (g_Config.i(g_Items.m_Theme) < 1) {}

		ImGui::Separator("Configs"); ImGui::Spacing();

		ImGui::BeginGroup();

		static std::vector<std::string> config_names;
		static std::vector<std::string> config_paths;
		static int current_config = 0;
		static bool should_rename = false;
		static std::once_flag flag;

		static const auto refresh_config_list = []() -> void
		{
			config_names.clear();
			config_paths.clear();

			for (const auto& entry : std::filesystem::directory_iterator(g_Config.GetConfigDirectory()))
			{
				const auto path = std::filesystem::path(entry);

				std::string name = path.filename().string();

				if (!std::filesystem::is_empty(path))
				{
					config_names.push_back(name.substr(0, name.find(("."))));
					config_paths.push_back(name); // dont push any paths if the dir is empty.
				}
			}
		};

		std::call_once(flag, []() -> void
			{
				config_names.clear();
				config_paths.clear();
				for (const auto& entry : std::filesystem::directory_iterator(g_Config.GetConfigDirectory()))
				{
					const auto path = std::filesystem::path(entry);
					std::string name = path.filename().string();

					if (!std::filesystem::is_empty(path))
					{
						config_names.push_back(name.substr(0, name.find(("."))));
						config_paths.push_back(name); // dont push any paths if the dir is empty.
					}
				}
			});

		current_config = std::clamp(current_config, 0, static_cast<int>(config_names.size())); // make sure the config count doesn't go under min or over max

		static char config_name[50];

		ImGui::SetCursorPosX(300 / 2 - 195 / 2);
		if (ImGui::BeginListBox("##Config"))
		{
			//if (!config_names.empty())
			//	ImGui::Text(("config id: %i"), current_config);

			//ImGui::Text(("total id: %i"), config_names.size());

			for (auto i = 0u; i < config_names.size(); ++i)
			{
				if (config_names.empty())
					break;

				const bool is_selected = current_config == i;
				std::string& item_name = config_names.at(i);

				if (ImGui::Selectable(item_name.c_str(), is_selected))
					current_config = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (!should_rename)
		{
			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::SetCursorPosX(300 / 2 - 195 / 2);
			ImGui::InputText(("Name"), config_name, sizeof(config_name));
		}
		else
		{
			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::SetCursorPosX(300 / 2 - 195 / 2);
			ImGui::InputText(("New name"), config_name, sizeof(config_name));

			std::string new_name = std::string(config_name) + (".cfg");

			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button("Apply Rename", ImVec2(141, 20)))
			{
				if (g_Config.Rename(config_paths.at(current_config), new_name))
				{
					g_notify.add(tfm::format("renamed config from %s to %s successfully!\n", config_paths.at(current_config).c_str(), new_name.c_str()), colors::white, 5.f);

					config_names.at(current_config) = std::string(config_name);
					config_paths.at(current_config) = new_name;
					should_rename = false;
				}
			}
			ImGui::Spacing();
		}

		ImGui::Spacing();
		ImGui::SetCursorPosX(300 / 2 - 141 / 2);
		if (ImGui::Button(("Create"), ImVec2(141, 20)))
		{
			config_names.emplace_back(config_name);
			config_paths.push_back(std::string(config_name) + (".cfg"));

			if (g_Config.Create(config_paths.at(config_names.size() - 1u)) && strlen(config_name) != 0)
				g_notify.add(tfm::format("Created config %s successfully!\n", config_paths.at(config_names.size() - 1u).c_str()), colors::white, 5.f);
			else
				g_notify.add(tfm::format("Failed to create config %s\n", config_paths.at(config_names.size() - 1u).c_str()), colors::white, 5.f);
		}

		if (config_names.size() > 0)
		{
			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Save"), ImVec2(141, 20)))
			{
				savelocation = true;//save keybind , spectator
				if (g_Config.Save(config_paths.at(current_config))) {
					g_notify.add(tfm::format("Saved config %s successfully!\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
					
			/*		for (size_t i = 0; i <= 50; i++)
					{
						config_name[i] = 0;
					}*/
				}
			}
			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Load"), ImVec2(141, 20)))
			{
				loadlocation = true; //load keybind , spectator

				if (g_Config.Load(config_paths.at(current_config)))
					g_notify.add(tfm::format("Loaded config %s successfully!\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
				else
					g_notify.add(tfm::format("Failed to load %s!\n", config_paths.at(current_config).c_str()), colors::red, 5.f);
			}

			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Rename"), ImVec2(141, 20)))
				should_rename = true;

			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Delete"), ImVec2(141, 20)))
			{
				if (g_Config.Remove(config_paths.at(current_config)) && !config_names.empty())
					g_notify.add(tfm::format("Deleted config %s successfully!\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
				else
					g_notify.add(tfm::format("Failed to delete config %s\n", config_paths.at(current_config).c_str()), colors::white, 5.f);

				config_names.erase(config_names.begin() + current_config);
				config_paths.erase(config_paths.begin() + current_config);

				current_config -= 1;
			}

			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Import"), ImVec2(141, 20)))
			{
				if (g_Config.ImportFromClipboard(config_paths.at(current_config)))
					g_notify.add(tfm::format("Imported config %s successfully!\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
				else
					g_notify.add(tfm::format("Failed to import config %s\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
			}
			ImGui::Spacing();
			ImGui::SetCursorPosX(300 / 2 - 141 / 2);
			if (ImGui::Button(("Export"), ImVec2(141, 20)))
			{
				if (g_Config.ExportToClipboard(config_paths.at(current_config)))
					g_notify.add(tfm::format("Exported config %s successfully!\n", config_paths.at(current_config).c_str()), colors::white, 5.f);
			}
		}

		ImGui::Spacing();
		ImGui::SetCursorPosX(300 / 2 - 141 / 2);
		if (ImGui::Button(("Refresh"), ImVec2(141, 20)))
			refresh_config_list();

		ImGui::Spacing();
		ImGui::SetCursorPosX(300 / 2 - 141 / 2);
		if (ImGui::Button(("Open config directory"), ImVec2(141, 20)))
		{
			ShellExecuteA(nullptr, ("open"), g_Config.GetConfigDirectory().c_str(), nullptr, nullptr, SW_SHOW);
		}

		ImGui::EndGroup();

	}ImGui::EndChild();
}