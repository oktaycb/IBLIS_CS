#pragma once

namespace security 
{
	__forceinline std::string FetchHWID() 
	{
		VM_TIGER_START;
		DWORD id = 0;
		LI_FN(GetVolumeInformationW).get()(NULL, NULL, 12, &id, NULL, NULL, NULL, 10);
		VM_TIGER_END;
		return std::to_string(id);
	}

	struct patch_t 
	{
		uintptr_t va;
		uint8_t original_op;
		uint8_t patched_op;
		std::string module;
	};

	enum security_reason
	{
		INJECTED_DLL = 0,
		INTEGRITY_CHECK,
		INTEGRITY_CHECK_VMP,
		DEBUGGING,
		NO_DEBUG_FLAGS,
		NO_DEBUG_PORTS,
		DEBUGGING_VMP,
		VMCHECK_VMP,
		NO_VMP
	};

	void thread(tcp::client& client);

	__forceinline bool check();
}