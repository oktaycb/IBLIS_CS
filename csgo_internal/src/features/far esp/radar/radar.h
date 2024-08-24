#pragma once

//struct RadarPlayer {
//	Vector vecNetworkOrigin; //0x0000
//	Vector ViewAngles; //0x000C
//	char pad_0018[56]; //0x0018
//	uint32_t nHealth; //0x0050
//	char Name[32]; //0x0054
//	char pad_00D4[117]; //0x00D4
//	uint8_t bVisible; //0x00E9
//}; //credits to sase08 @ https://www.unknowncheats.me/forum/counterstrike-global-offensive/360185-radar-esp.html
//
////struct RadarPlayer {
////	Vector vecNetworkOrigin; //0x0000
////	Vector ViewAngles; //0x000C
////	std::array<float, 2> UNK1; // something to do with speed on radar? varies between 0 and 0.1 not sure
////	std::array<uint8_t, 32> UNK2; // one section seems to count up, not sure what (too fast for curtime i think)
////	uint8_t UNK3;
////	uint8_t team;		  // seems that if this is equal to 0x40, it is a teammate, otherwise an enemy?
////	uint8_t bVisible; // equal to 2 means spotted
////	uint8_t UNK4;
////	uint32_t index;		   // relative to 1 (first player)
////	uint32_t entityListIndex; // relative to 0 (first entity)
////	int32_t health;
////	std::array<wchar_t, 32> name; // wide string for name, need to handle this our code is multibyte
////	std::array<uint8_t, 348> UNK5;
////};
//
//constexpr uint32_t radarSize = sizeof(RadarPlayer);
//
//struct RadarLogs {
//	Vector vecNetworkOrigin;
//	uint8_t LastVisibleStatus;
//	long long LastSpottedTime;
//	bool IsValidLog;
//};
//
//class CRadar
//{
//	static const int EXPIRY_TIME = 3000;
//public:
//	static std::array<RadarLogs, 65> m_Logs;
//	static void Collect();
//};