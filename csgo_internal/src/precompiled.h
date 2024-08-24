#pragma once

#pragma warning( disable : 4307 ) // '*': integral constant overflow
#pragma warning( disable : 4244 ) // possible loss of data
#pragma warning( disable : 4800 ) // forcing value to bool 'true' or 'false'
#pragma warning( disable : 4838 ) // conversion from '::size_t' to 'int' requires a narrowing conversion
#pragma warning( disable : 4596 ) // illegal qualified name in member declaration

// You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//#define LEGACY

using ulong_t = unsigned long;

// Windows & STL includes.
#include <Windows.h>
#include <intrin.h>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <deque>
#include <functional>
#include <map>
#include <filesystem>
#include <d3d9.h>

// security
#include <ThemidaSDK.h>
#include "security/macros.h"
#include "security/vendor/li/lazy_importer.h"

// util
#include "util/tinyformat.h"
#include "util/hash.h"
#include "util/xorstr.h"
#include "util/pe.h"
#include "util/address.h"
#include "util/modules.h"
#include "util/pattern.h"
#include "util/util.h"
#include "util/vmt.h"
#include "util/stack.h"
#include "util/CRC32.h"
#include "util/x86.h"
#include "util/syscall.h"
#include "util/detours.h"
#include "util/json.h"
#include "util/base64.h"
#include "util/input/input.h"

// sdk & interface
#include "csgo/sdk/resources.h"
#include "csgo/interfaces/interfaces.h"
#include "csgo/sdk/sdk.h"
#include "csgo/interfaces/csgo.h"
#include "features/Penetration/autowall.h"
#include "csgo/offsets and netvars/netvars.h"
#include "csgo/offsets and netvars/entoffsets.h"
#include "csgo/sdk/entity.h"
#include "csgo/sdk/gamerules.h"
#include "csgo/client/client.h"
#include "csgo/sdk/mdlcache.h"
#include "csgo/hooks/hooks.h"
#include "csgo/hooks/detour/detour_hook.h"

// gui
#include "gui/imgui/imgui.h"
#include "gui/imgui/impl/imgui_impl_dx9.h"
#include "gui/imgui/impl/imgui_impl_win32.h"
#include "gui/imgui/imgui_internal.h"
#include "gui/imgui/imconfig.h"
#include "gui/menu/other/extensions/imgui extensions.h"
#include "renderer/new/newrender.h"
#include "gui/menu/menu.h"
#include "gui/config/cfg.h"
#include "gui/config/items.h"
#include "gui/menu/config/config.h"
#include "gui/menu/bind/key.h"

// other
#include "features/Prediction/EnginePrediction.h"
#include "features/Fake Ping/FakePing.h"
#include "features/lagcomp/lagrecord.h"
#include "features/visuals/visuals.h"
#include "features/far esp/sounds/SoundESP.h"
#include "features/weather/weather.h"
#include "features/movement/movement.h"
#include "features/bonesetup/bonesetup.h"
#include "features/anti-aim/anti-aim.h"
#include "features/fakelag/fakelag.h"
#include "features/lagcomp/lagcomp.h"
#include "features/ragebot/ragebot.h"
#include "features/netdata/netdata.h"
#include "features/chams/chams.h"
#include "features/notification/notification.h"
#include "features/resolver/resolver.h"
#include "features/grenades/grenades.h"
#include "features/grenades/grenades_warning.h"
#include "features/skin changer/skin changer.h"
#include "features/events/events.h"
#include "features/shots/shots.h"
#include "features/Tickbase Manipulation/TickbaseManipulation.h"
#include "features/Network/Network.h"