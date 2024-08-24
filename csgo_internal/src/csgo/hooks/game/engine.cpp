#include "../../../precompiled.h"

void Hooks::FireEvents() {
#ifdef LEGACY
	if (!this || !g_Csgo.m_Engine || !g_Csgo.m_ClientState->m_Events)
		return g_Hooks.m_Engine.GetOldMethod< FireEvents_t >(IVEngineClient::FIREEVENTS)(this);

	for (CEventInfo* Event{ g_Csgo.m_ClientState->m_Events }; Event != nullptr; Event = Event->m_Next) {
		if (!Event->m_ClassID)
			continue;

		Event->m_FireDelay = 0.0f;
	}
#endif
	return g_Hooks.m_Engine.GetOldMethod< FireEvents_t >(IVEngineClient::FIREEVENTS)(this);
}

bool Hooks::IsConnected() {
	if (!this || !g_Csgo.m_Engine)
		return false;

	Stack stack;

	static Address IsLoadoutAllowed{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 75 04 B0 01 5F")) };

	//if (c_config::get()->b["unlock_inventory"] && stack.ReturnAddress() == IsLoadoutAllowed)
	//	return false;

	return g_Hooks.m_Engine.GetOldMethod< IsConnected_t >(IVEngineClient::ISCONNECTED)(this);
}

bool Hooks::IsHLTV() {
	if (!this || !g_Csgo.m_Engine)
		return false;

	Stack stack;

	static const Address SetupVelocity{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")) };
	static const Address AccumulateLayers{ pattern::find(g_Csgo.m_client_dll, XOR("84 C0 75 0D F6 87")) };

	if (stack.ReturnAddress() == SetupVelocity ||
		stack.ReturnAddress() == AccumulateLayers)
		return true;

	return g_Hooks.m_Engine.GetOldMethod< IsHLTV_t >(IVEngineClient::ISHLTV)(this);
}

bool Hooks::IsPaused() {
	if (!this || !g_Csgo.m_Engine)
		return false;

	Stack stack;

	static const Address Extrapolation{ pattern::find(g_Csgo.m_client_dll, XOR("FF D0 A1 ? ? ? ? B9 ? ? ? ? D9 1D ? ? ? ? FF 50 34 85 C0 74 22 8B 0D ? ? ? ?")).add(0x29) };
	static const Address Interpolation{ pattern::find(g_Csgo.m_client_dll, XOR("FF D0 84 C0 74 07 C6 05")) };

	if (stack.ReturnAddress() == Interpolation && !g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation) ||
		stack.ReturnAddress() == Extrapolation )
		return true;

	return g_Hooks.m_Engine.GetOldMethod< IsPaused_t >(IVEngineClient::ISPAUSED)(this);
}

bool Hooks::IsPlayingTimeDemo() {
	if (!this || !g_Csgo.m_Engine)
		return false;

	Stack stack;

	static const Address Interpolation{ pattern::find(g_Csgo.m_client_dll, XOR("FF D0 84 C0 75 14 8B 0D ? ? ? ? 8B 01 8B 80 68 01 00 00")) };

	// disable interpolation.
	if (stack.ReturnAddress() == Interpolation && !g_Config.b(g_Items.m_Aimbot.m_General.m_SmoothInterpolation))
		return true;

	return g_Hooks.m_Engine.GetOldMethod< IsPlayingTimeDemo_t >(IVEngineClient::ISPLAYINGTIMEDEMO)(this);
}

void Hooks::EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity) {
	// g_Visuals.UpdateSoundPosition(); todo@ : remake.

	if (strstr(pSample, XOR("weapon")) && (strstr(pSample, XOR("draw")) || strstr(pSample, XOR("deploy"))))
	{
		static hash_t iPrevHash = 0;
		const hash_t iHash = RT_HASH(pSample);

		if (iPrevHash == iHash)
			return;

		iPrevHash = iHash;
	}

	return g_Hooks.m_EngineSound.GetOldMethod<EmitSound_t>(IEngineSound::EMITSOUND)(this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
}