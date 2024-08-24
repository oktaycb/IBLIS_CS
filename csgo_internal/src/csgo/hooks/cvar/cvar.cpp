#include "../../../precompiled.h"

bool Hooks::NetShowFragmentsGetBool() {
	if (!g_Csgo.m_Engine->IsConnected() || !g_Csgo.m_Engine->IsInGame())
		return g_Hooks.m_net_show_fragments.GetOldMethod< GetBool_t >(ConVar::GETINT)(this);

	//if (!c_config::get()->b["fake_latency"] || !keyhandler::get()->auto_check("fake_latency_key"))
	//	return g_Hooks.m_net_show_fragments.GetOldMethod< GetBool_t >(ConVar::GETINT)(this);

	static auto read_sub_channel_data_ret = pattern::find(g_Csgo.m_engine_dll, XOR("85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C")).as< uintptr_t*>();
	static auto check_receiving_list_ret = pattern::find(g_Csgo.m_engine_dll, XOR("8B 1D ? ? ? ? 85 C0 74 16 FF B6")).as< uintptr_t*>();

	static uint32_t last_fragment = 0;

	if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
	{
		// todo: make em you know.
		const auto data = &reinterpret_cast<uint32_t*>(g_Csgo.m_ClientState->m_NetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		if (bytes_fragments == last_fragment)
		{
			auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
			buffer = 0;
		}
	}

	if (_ReturnAddress() == check_receiving_list_ret)
	{
		const auto data = &reinterpret_cast<uint32_t*>(g_Csgo.m_ClientState->m_NetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		last_fragment = bytes_fragments;
	}

	return g_Hooks.m_net_show_fragments.GetOldMethod< GetBool_t >(ConVar::GETINT)(this);
}

int Hooks::CsmShadowGetInt()
{
	return 0;
}