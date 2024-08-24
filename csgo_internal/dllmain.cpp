/*
DISCLAIMER:
DON'T CHANGE ANYTHING IN THIS FILE, ANY SINGLE CODE, ANY SINGLE LINE, ANY SINGLE BYTE.
DON'T ADD ANYTHING IN THIS FILE, ANY SINGLE CODE, ANY SINGLE LINE, ANY SINGLE BYTE.
DON'T DELETE ANYTHING IN THIS FILE, ANY SINGLE CODE, ANY SINGLE LINE, ANY SINGLE BYTE.
ANYTHING CHANGES IN THIS FILE WILL LEAD PRODUCTION TO BREAK.
ANYTHING ADDED IN THIS FILE WILL LEAD PRODUCTION TO BREAK.
ANYTHING DELETED IN THIS FILE WILL LEAD PRODUCTION TO BREAK.
*/

#include "src/precompiled.h"
#include "src/security/network/tcp.h"
#include "src/security/security.h"

void EventHandler(tcp::client& client) 
{
	VM_TIGER_START;

	client.connect_event.add([&]() {
		LOG_INFO(XOR("connected."));
	});

	client.receive_event.add([&](tcp::packet_t packet)
	{
		if (!packet) return;

		const std::string server = packet();
		const uint8_t id = packet.id;

		// the first thing server does is to set "id" to "session"
		if (id == tcp::packet_id::session) 
		{
			// if we have connected, set the client session id to server's. We don't want to send anything to client from client.
			client.session_id = packet.session_id;

			nlohmann::json hwid;
			hwid[XOR("hwid")] = security::FetchHWID();
			hwid[XOR("ver")] = std::stoi(client.ver);
			hwid[XOR("dlc")] = true;

			const int hwd = client.write(tcp::packet_t(hwid.dump(), tcp::packet_type::write, client.session_id, tcp::packet_id::hwid));
			if (hwd <= 0)
			{
				client.shutdown();
				return;
			}
		}

		if (id == tcp::packet_id::dll_login)
		{
			auto parse = nlohmann::json::parse(server);
			const std::string_view l = parse[XOR("dll_login")];
			const int login = client.write(tcp::packet_t(l, tcp::packet_type::write, client.session_id, tcp::packet_id::login_req));
			if (login <= 0)
			{
				client.shutdown();
				return;
			}
		}

		if (id == tcp::packet_id::login_resp) 
		{
			//auto j = nlohmann::json::parse(server);

		}

		if (id == tcp::packet_id::dlc_content)
		{
			//...
		}

		//todo: bring back hwid response and do login response.  
	});

	VM_TIGER_END;
}

void quit()
{
	__asm
	{
		sub esp, 4
		retn
	}
}

void StartClient() {
#ifdef _PRODUCTION
	const std::string i = XOR("37237");
	tcp::_client.start(XOR("34.228.23.187"), std::stoi(i));
	std::thread eventHandlerThread(EventHandler, std::ref(tcp::_client));
	eventHandlerThread.detach();
#endif
	std::thread clientThread([]() {
		std::once_flag flag;
	while (true) {
#ifdef _PRODUCTION
		std::array<char, tcp::message_len> buf;
		int ret = tcp::_client.read(&buf[0], buf.size());
		if (ret <= 0) {
			LI_FN(Beep).get()(300, 500);
			LOG_ERROR(XOR("connection lost.\n"));
			tcp::termination.m_break = true;
		}

		std::string msg(buf.data(), ret);
		tcp::_client.receive_event.call(tcp::packet_t{ msg, tcp::packet_type::read });

		if (!tcp::_client || tcp::termination.m_break) {
			quit();
		}
#endif
		std::call_once(flag, &Client::Init);
	}});
	clientThread.detach();
}

int __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	VM_DIFF_START;

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

#ifdef _PRODUCTION
		DWORD OldProtect = 0;
		char* pBaseAddr = (char*)hModule;
		SYSTEM_INFO systemInfo;
		LI_FN(GetSystemInfo)(&systemInfo);
		LI_FN(VirtualProtect)(pBaseAddr, systemInfo.dwPageSize, PAGE_READWRITE, &OldProtect);
		RtlSecureZeroMemory(pBaseAddr, systemInfo.dwPageSize);

		__asm {
			mov eax, fs: [0x30]					// PEB
			mov eax, [eax + 0x0c]				// PEB_LDR_DATA
			mov eax, [eax + 0x0c]				// InOrderModuleList
			mov dword ptr[eax + 20h], 20000h	// SizeOfImage
		}
#endif
#ifdef ALLOC_CONSOLE
		AllocConsole();
		DWORD mode{};
		GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
#endif
		StartClient();

		VM_DIFF_END;

		return 1;
	}
}