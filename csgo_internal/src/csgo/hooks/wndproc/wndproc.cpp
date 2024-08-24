#include "../../../precompiled.h"

HRESULT WINAPI Hooks::Present(IDirect3DDevice9* device, const RECT* src, const RECT* dst, HWND dst_window_override, const RGNDATA* dirty_region) 
{
	if (!new_render.IsInitialized())
		new_render.InitD3D(device);
	else {
		DWORD colorWhite, sRGBWhite;

		device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorWhite);
		device->GetRenderState(D3DRS_SRGBWRITEENABLE, &sRGBWhite);

		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		IDirect3DStateBlock9* pStateBlock = nullptr;

		if (device->CreateStateBlock(D3DSBT_ALL, &pStateBlock) != D3D_OK)
			return g_Hooks.m_Device.GetOldMethod< decltype(&Present)>(17)(device, src, dst, dst_window_override, dirty_region);

		pStateBlock->Capture();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_Menu.Render();

		g_Menu.DarkModeIndicators();
		g_Menu.LightModeIndicators();

		ImGui::EndFrame();

		ImGui::Render(new_render.RenderScene());
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pStateBlock->Apply();
		pStateBlock->Release();
		pStateBlock = nullptr;

		device->SetRenderState(D3DRS_COLORWRITEENABLE, colorWhite);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, sRGBWhite);
	}

	return g_Hooks.m_Device.GetOldMethod< decltype(&Present)>(17)(device, src, dst, dst_window_override, dirty_region); 
}

HRESULT WINAPI Hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	if (new_render.IsInitialized())
	{
		new_render.OnInvalidate();
		new_render.OnCreate();
	}

	return g_Hooks.m_Device.GetOldMethod< decltype(&Reset)>(16)(device, params);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Hooks::WndProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
	g_KeyInput.OnWndProc(Msg, WParam);

	if (GImGui == nullptr)
		return CallWindowProc(g_Hooks.m_old_wndproc, Hwnd, Msg, WParam, LParam);

#ifdef LEGACY
	bool bShouldOpen = (GetFocus() && !g_Csgo.m_Engine->IsDrawingLoadingImage()) || (g_Menu.IsOpen() && g_Client.m_PressingButtons);
#else
	// TODO@Monster: add back.
	bool bShouldOpen = GetFocus() || g_Menu.IsOpen() && g_Client.m_PressingButtons;
#endif

	if (g_KeyInput.IsToggled(VK_INSERT))
		g_Menu.IsOpen() = false;
	else g_Menu.IsOpen() = true;

	if ((ImGui_ImplWin32_WndProcHandler(Hwnd, Msg, WParam, LParam) && bShouldOpen) || ImGui::GetIO().WantTextInput)
		return true;

	return CallWindowProc(g_Hooks.m_old_wndproc, Hwnd, Msg, WParam, LParam);
}