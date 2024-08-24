#include "../../precompiled.h"

#include <codecvt>
#include <cassert>
#include <locale>

Skins g_skins{};;

void Skins::Run()
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	const auto sig_address = pattern::find(g_Csgo.m_client_dll, XOR("E8 ? ? ? ? FF 76 0C 8D 48 04 E8"));
	const auto item_system_offset = *sig_address.add(0x1).as< std::int32_t*>();
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address.add(0x5) + item_system_offset);
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));

	const auto get_paint_kit_definition_offset = *sig_address.add(12).as< std::int32_t*>();
	const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address.add(16) + get_paint_kit_definition_offset);
	const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
	const auto head_offset = start_element_offset - 12;
	const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);

	for (auto i = 0; i <= map_head->last_element; ++i) {
		const auto paint_kit = map_head->memory[i].value;

		if (paint_kit->id == 9001)
			continue;

		const auto wide_name = g_Csgo.m_Localize->Find(paint_kit->item_name.buffer + 1);
		const auto name = converter.to_bytes(wide_name);

		if (paint_kit->id < 10000)
			m_skins.push_back({ paint_kit->id, name });
		else
			m_gloves.push_back({ paint_kit->id, name });
	}

	std::sort(m_skins.begin(), m_skins.end());
	std::sort(m_gloves.begin(), m_gloves.end());

	// copy array contents
	for (auto i = 0u; i < m_skins.size(); ++i) {
		m_paint_kits.emplace_back(m_skins[i].name);
	}

	for (auto i = 0u; i < m_gloves.size(); ++i) {
		m_glove_kits.emplace_back(m_gloves[i].name);
	}
}

void Skins::load() {
	// update model indexes on mapload.
	m_knife_data[knives_t::BAYONET].setup(KNIFE_BAYONET,
		XOR("models/weapons/v_knife_bayonet.mdl"),
		XOR("models/weapons/w_knife_bayonet.mdl"),
		XOR("bayonet"));

	m_knife_data[knives_t::BOWIE].setup(KNIFE_BOWIE,
		XOR("models/weapons/v_knife_survival_bowie.mdl"),
		XOR("models/weapons/w_knife_survival_bowie.mdl"),
		XOR("knife_survival_bowie"));


	m_knife_data[knives_t::BUTTERFLY].setup(KNIFE_BUTTERFLY,
		XOR("models/weapons/v_knife_butterfly.mdl"),
		XOR("models/weapons/w_knife_butterfly.mdl"),
		XOR("knife_butterfly"));

	m_knife_data[knives_t::FALCHION].setup(KNIFE_FALCHION,
		XOR("models/weapons/v_knife_falchion_advanced.mdl"),
		XOR("models/weapons/w_knife_falchion_advanced.mdl"),
		XOR("knife_falchion"));

	m_knife_data[knives_t::FLIP].setup(KNIFE_FLIP,
		XOR("models/weapons/v_knife_flip.mdl"),
		XOR("models/weapons/w_knife_flip.mdl"),
		XOR("knife_flip"));

	m_knife_data[knives_t::GUT].setup(KNIFE_GUT,
		XOR("models/weapons/v_knife_gut.mdl"),
		XOR("models/weapons/w_knife_gut.mdl"),
		XOR("knife_gut"));

	m_knife_data[knives_t::HUNTSMAN].setup(KNIFE_HUNTSMAN,
		XOR("models/weapons/v_knife_tactical.mdl"),
		XOR("models/weapons/w_knife_tactical.mdl"),
		XOR("knife_tactical"));

	m_knife_data[knives_t::KARAMBIT].setup(KNIFE_KARAMBIT,
		XOR("models/weapons/v_knife_karam.mdl"),
		XOR("models/weapons/w_knife_karam.mdl"),
		XOR("knife_karambit"));

	m_knife_data[knives_t::M9].setup(KNIFE_M9_BAYONET,
		XOR("models/weapons/v_knife_m9_bay.mdl"),
		XOR("models/weapons/w_knife_m9_bay.mdl"),
		XOR("knife_m9_bayonet"));

	m_knife_data[knives_t::DAGGER].setup(KNIFE_SHADOW_DAGGERS,
		XOR("models/weapons/v_knife_push.mdl"),
		XOR("models/weapons/w_knife_push.mdl"),
		XOR("knife_push"));

	// update glove model indexes on mapload.
	m_glove_data[gloves_t::BLOODHOUND].setup(5027,
		XOR("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"),
		XOR("models/weapons/w_models/arms/glove_bloodhound/w_glove_bloodhound.mdl"));

	m_glove_data[gloves_t::SPORTY].setup(5030,
		XOR("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"),
		XOR("models/weapons/w_models/arms/glove_sporty/w_glove_sporty.mdl"));

	m_glove_data[gloves_t::DRIVER].setup(5031,
		XOR("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"),
		XOR("models/weapons/w_models/arms/glove_slick/w_glove_slick.mdl"));

	m_glove_data[gloves_t::HANDWRAP].setup(5032,
		XOR("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"),
		XOR("models/weapons/w_models/arms/glove_handwrap_leathery/w_glove_handwrap_leathery.mdl"));

	m_glove_data[gloves_t::MOTO].setup(5033,
		XOR("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"),
		XOR("models/weapons/w_models/arms/glove_motorcycle/w_glove_motorcycle.mdl"));

	m_glove_data[gloves_t::SPECIALIST].setup(5034,
		XOR("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"),
		XOR("models/weapons/w_models/arms/glove_specialist/w_glove_specialist.mdl"));

	m_update_time = 0.f;
}

void Skins::think() {
	std::vector< Weapon* > weapons{};

	if (!g_Csgo.m_Engine->IsInGame())
		return;

	if (!g_Client.m_LocalPlayer || g_Client.m_LocalPlayer->m_iHealth() < 1)
		return;

	player_info_t info;
	if (!g_Csgo.m_Engine->GetPlayerInfo(g_Client.m_LocalPlayer->GetClientNetworkable()->GetIndex(), &info))
		return;

	// store knife index.
	KnifeData* knife = &m_knife_data[c_config::get()->i["knife"]];

	for (int i{ 1 }; i <= g_Csgo.m_EntityList->GetHighestEntityIndex(); ++i) {
		Entity* ent = g_Csgo.m_EntityList->GetClientEntity(i);
		if (!ent)
			continue;

		// init knifechanger.
		if (c_config::get()->b["skin_override_knife"] && ent->is(HASH("CPredictedViewModel"))) {
			// get weapon entity from predicted viewmodel.
			Weapon* weapon = ent->as< Weapon* >()->GetWeapon();

			// no need to be here anymore.
			if (!weapon || !weapon->IsKnife())
				continue;

			// this weapon does not belong to us, we are done here.
			// should barely happen unless you pick up someones knife.
			// possible on servers with 'mp_drop_knife_enable 1'.
			if (info.m_xuid_low != weapon->m_OriginalOwnerXuidLow() || info.m_xuid_high != weapon->m_OriginalOwnerXuidHigh())
				continue;

			// get and validate world model handle from the weapon
			Weapon* weapon_world_model = weapon->GetWeaponWorldModel();
			if (!weapon_world_model)
				continue;

			// set model index of the predicted viewmodel.
			ent->m_nModelIndex() = knife->m_model_index;

			// set the world model index.
			// do this to have knifechanger in third person. verry p.
			weapon_world_model->m_nModelIndex() = knife->m_world_model_index;

			// correct m_nSequence and m_flCycle.
			UpdateAnimations(ent);
		}

		else if (ent->IsBaseCombatWeapon()) {
			// cast to weapon class.
			Weapon* weapon = ent->as< Weapon* >();

			// this is not our gun.
			if (info.m_xuid_low != weapon->m_OriginalOwnerXuidLow() || info.m_xuid_high != weapon->m_OriginalOwnerXuidHigh())
				continue;

			// we found a weapon that we own.
			weapons.push_back(weapon);

			if (weapon->IsKnife()) {
				// if this weapon is a knife, set some additional stuff.
				if (c_config::get()->b["skin_override_knife"]) {
					// set the item id, this is for the hud.
					weapon->m_iItemDefinitionIndex() = knife->m_id;

					// not needed. but everyone does this, try without.
					weapon->m_nModelIndex() = knife->m_model_index;

					// not needed. but everyone does this, try without.
					weapon->m_iViewModelIndex() = knife->m_model_index;

					// not needed. but everyone does this, try without.
					weapon->m_iWorldModelIndex() = knife->m_world_model_index;

					// set the little star thing.
					weapon->m_iEntityQuality() = 3;
				}

				else if (!c_config::get()->b["skin_override_knife"]) {
					// fix definition.
					if (g_Client.m_LocalPlayer->m_iTeamNum() == TEAM_TERRORISTS) {
						weapon->m_iItemDefinitionIndex() = KNIFE_T;
						weapon->m_iViewModelIndex() = g_Csgo.m_ModelInfo->GetModelIndex(XOR("models/weapons/v_knife_default_t.mdl"));
						weapon->m_iWorldModelIndex() = g_Csgo.m_ModelInfo->GetModelIndex(XOR("models/weapons/w_knife_default_t.mdl"));
					}

					else if (g_Client.m_LocalPlayer->m_iTeamNum() == TEAM_COUNTERTERRORISTS) {
						weapon->m_iItemDefinitionIndex() = KNIFE_CT;
						weapon->m_iViewModelIndex() = g_Csgo.m_ModelInfo->GetModelIndex(XOR("models/weapons/v_knife_default_ct.mdl"));
						weapon->m_iWorldModelIndex() = g_Csgo.m_ModelInfo->GetModelIndex(XOR("models/weapons/w_knife_default_ct.mdl"));
					}

					// reset.
					weapon->m_iEntityQuality() = 0;
					weapon->m_nFallbackPaintKit() = -1;
					weapon->m_nFallbackStatTrak() = -1;
				}
			}

			if (c_config::get()->b["skins_enable"]) {
				switch (weapon->m_iItemDefinitionIndex()) {
				case DEAGLE:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_deagle"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_deagle"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_deagle"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_deagle"]) / 100.f) + FLT_EPSILON;
					break;
				case ELITE:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_elite"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_elite"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_elite"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_elite"]) / 100.f) + FLT_EPSILON;
					break;
				case FIVESEVEN:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_fiveseven"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_fiveseven"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_fiveseven"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_fiveseven"]) / 100.f) + FLT_EPSILON;
					break;
				case GLOCK:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_glock"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_glock"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_glock"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_glock"]) / 100.f) + FLT_EPSILON;
					break;
				case AK47:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_ak47"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_ak47"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_ak47"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_ak47"]) / 100.f) + FLT_EPSILON;
					break;
				case AUG:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_aug"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_aug"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_aug"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_aug"]) / 100.f) + FLT_EPSILON;
					break;
				case AWP:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_awp"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_awp"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_awp"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_awp"]) / 100.f) + FLT_EPSILON;
					break;
				case FAMAS:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_famas"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_famas"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_famas"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_famas"]) / 100.f) + FLT_EPSILON;
					break;
				case G3SG1:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_g3sg1"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_g3sg1"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_g3sg1"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_g3sg1"]) / 100.f) + FLT_EPSILON;
				case GALIL:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_galil"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_galil"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_galil"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_galil"]) / 100.f) + FLT_EPSILON;
					break;
				case M249:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_m249"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_m249"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_m249"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_m249"]) / 100.f) + FLT_EPSILON;
					break;
				case M4A4:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_m4a4"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_m4a4"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_m4a4"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_m4a4"]) / 100.f) + FLT_EPSILON;
					break;
				case MAC10:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_mac10"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_mac10"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_mac10"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_mac10"]) / 100.f) + FLT_EPSILON;
					break;
				case P90:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_p90"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_p90"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_p90"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_p90"]) / 100.f) + FLT_EPSILON;
					break;
				case UMP45:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_ump45"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_ump45"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_ump45"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_ump45"]) / 100.f) + FLT_EPSILON;
					break;
				case XM1014:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_xm1014"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_xm1014"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_xm1014"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_xm1014"]) / 100.f) + FLT_EPSILON;
					break;
				case BIZON:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_bizon"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_bizon"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_bizon"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_bizon"]) / 100.f) + FLT_EPSILON;
					break;
					break;
				case MAG7:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_mag7"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_mag7"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_mag7"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_mag7"]) / 100.f) + FLT_EPSILON;
					break;
				case NEGEV:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_negev"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_negev"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_negev"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_negev"]) / 100.f) + FLT_EPSILON;
					break;
				case SAWEDOFF:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_sawedoff"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_sawedoff"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_sawedoff"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_sawedoff"]) / 100.f) + FLT_EPSILON;
					break;
				case TEC9:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_tec9"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_tec9"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_tec9"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_tec9"]) / 100.f) + FLT_EPSILON;
					break;
				case P2000:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_p2000"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_p2000"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_p2000"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_p2000"]) / 100.f) + FLT_EPSILON;
					break;
				case MP7:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_mp7"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_mp7"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_mp7"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_mp7"]) / 100.f) + FLT_EPSILON;
					break;
				case MP9:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_mp9"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_mp9"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_mp9"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_mp9"]) / 100.f) + FLT_EPSILON;
					break;
				case NOVA:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_nova"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_nova"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_nova"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_nova"]) / 100.f) + FLT_EPSILON;
					break;
				case P250:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_p250"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_p250"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_p250"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_p250"]) / 100.f) + FLT_EPSILON;
					break;
				case SCAR20:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_scar20"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_scar20"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_scar20"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_scar20"]) / 100.f) + FLT_EPSILON;
					break;
				case SG553:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_sg553"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_sg553"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_sg553"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_sg553"]) / 100.f) + FLT_EPSILON;
					break;
				case SSG08:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_ssg08"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_ssg08"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_ssg08"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_ssg08"]) / 100.f) + FLT_EPSILON;
					break;
				case M4A1S:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_m4a1s"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_m4a1s"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_m4a1s"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_m4a1s"]) / 100.f) + FLT_EPSILON;
					break;
				case USPS:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_usps"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_usps"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_usps"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_usps"]) / 100.f) + FLT_EPSILON;
					break;
				case CZ75A:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_cz75a"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_cz75a"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_cz75a"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_cz75a"]) / 100.f) + FLT_EPSILON;
					break;
				case REVOLVER:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_revolver"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_revolver"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_revolver"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_revolver"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_BAYONET:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_bayonet"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_bayonet"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_bayonet"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_bayonet"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_FLIP:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_flip"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_flip"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_flip"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_flip"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_GUT:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_gut"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_gut"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_gut"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_gut"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_KARAMBIT:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_karambit"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_karambit"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_karambit"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_karambit"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_M9_BAYONET:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_m9"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_m9"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_m9"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_m9"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_HUNTSMAN:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_huntsman"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_huntsman"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_huntsman"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_huntsman"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_FALCHION:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_falchion"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_falchion"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_falchion"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_falchion"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_BOWIE:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_bowie"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_bowie"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_bowie"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_bowie"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_BUTTERFLY:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_butterfly"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_butterfly"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_butterfly"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_butterfly"]) / 100.f) + FLT_EPSILON;
					break;
				case KNIFE_SHADOW_DAGGERS:
					weapon->m_nFallbackPaintKit() = m_skins[c_config::get()->i["id_daggers"]].id;
					weapon->m_nFallbackStatTrak() = c_config::get()->b["stattrak_daggers"] ? 1337 : -1;
					weapon->m_nFallbackSeed() = c_config::get()->i["seed_daggers"];
					weapon->m_flFallbackWear() = ((100.f - c_config::get()->i["quality_daggers"]) / 100.f) + FLT_EPSILON;
					break;

				default:
					break;
				}
			}

			// fix stattrak ownership.
			weapon->m_iAccountID() = info.m_xuid_low;

			// fix stattrak in hud.
			if (weapon->m_nFallbackStatTrak() >= 0)
				weapon->m_iEntityQuality() = 9;

			// force use fallback values.
			weapon->m_iItemIDHigh() = -1;
		}
	}

	if (c_config::get()->b["skin_override_gloves"]) {
		CBaseHandle* wearables = g_Client.m_LocalPlayer->m_hMyWearables();

		// get pointer to entity from wearable handle 0.
		// glove is at wearable 0, valve will likely add more wearables like hats and boots. thats why.
		Weapon* glove = g_Csgo.m_EntityList->GetClientEntityFromHandle< Weapon* >(wearables[0]);

		// there is no glove entity yet.
		if (!glove) {
			// attempt to get our old glove.
			Weapon* old = g_Csgo.m_EntityList->GetClientEntityFromHandle< Weapon* >(m_glove_handle);

			// this glove is still valid.
			if (old) {

				// write back handle to wearables.
				wearables[0] = m_glove_handle;

				// set glove pointer.
				glove = old;
			}
		}

		// if we at this point still not have a glove entity.
		// we should create one.
		if (!glove) {
			ClientClass* list{ g_Csgo.m_Client->GetAllClasses() };

			// iterate list.
			for (; list != nullptr; list = list->m_pNext) {

				// break if we found wearable
				if (list->m_ClassID == g_Netvars.GetClientID(HASH("CEconWearable")))
					break;
			}

			// create an ent index and serial no for the new glove entity.
			int index = g_Csgo.m_EntityList->GetHighestEntityIndex() + 1;
			int serial = g_Csgo.RandomInt(0xA00, 0xFFF);

			// call ctor on CEconWearable entity.
			Address networkable = list->m_pCreate(index, serial);

			// get entity ptr via index.
			glove = g_Csgo.m_EntityList->GetClientEntity< Weapon* >(index);

			static Address offset = g_Netvars.get(HASH("DT_EconEntity"), HASH("m_iItemIDLow"));

			// m_bInitialized?
			networkable.add(offset.add(0x8)).set< bool >(true);

			// no idea what this is.
			networkable.add(offset.sub(0xC)).set< bool >(true);

			// set the wearable handle.
			wearables[0] = index | (serial << 16);

			// use this for later on.
			m_glove_handle = wearables[0];
		}

		// store glove data.
		GloveData* data = &m_glove_data[c_config::get()->i["glove"]];

		// set default data,
		glove->m_nFallbackSeed() = 0;
		glove->m_nFallbackStatTrak() = -1;
		glove->m_iItemIDHigh() = -1;
		glove->m_iEntityQuality() = 4;
		glove->m_iAccountID() = info.m_xuid_low;

		// set custom data.
		glove->m_nFallbackPaintKit() = m_gloves[c_config::get()->i["glove_id"]].id;
		glove->m_iItemDefinitionIndex() = data->m_id;
		glove->SetGloveModelIndex(data->m_model_index);

		// update the glove.
		glove->GetClientNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
	}

	// only force update every 1s.
	if (m_update && g_Csgo.m_Globals->m_Curtime >= m_update_time) {
		for (auto& w : weapons)
			UpdateItem(w);

		m_update = false;
		m_update_time = g_Csgo.m_Globals->m_Curtime + 1.f;

		g_Csgo.m_ClientState->m_DeltaTick = -1;

		UpdateHud();
	}
}

void Skins::UpdateItem(Weapon* item) {
	if (!item || !item->IsBaseCombatWeapon())
		return;

	if (g_Csgo.m_ClientState->m_DeltaTick == -1)
		return;

	item->m_bCustomMaterialInitialized() = item->m_nFallbackPaintKit() <= 0;

	item->m_CustomMaterials().RemoveAll();
	item->m_CustomMaterials2().RemoveAll();

	size_t count = item->m_VisualsDataProcessors().Count();
	for (size_t i{}; i < count; ++i) {
		auto& elem = item->m_VisualsDataProcessors()[i];
		if (elem) {
			elem->unreference();
			elem = nullptr;
		}
	}

	item->m_VisualsDataProcessors().RemoveAll();

	item->GetClientNetworkable()->PostDataUpdate(DATA_UPDATE_CREATED);
	item->GetClientNetworkable()->OnPreDataChanged(DATA_UPDATE_CREATED);

	/*CHudElement* SFWeaponSelection = g_Csgo.m_Hud->FindElement(HASH("SFWeaponSelection"));
	g_Csgo.ShowAndUpdateSelection(SFWeaponSelection, 0, item, false);*/
}

void Skins::UpdateAnimations(Entity* ent) {
	int knife = c_config::get()->i["knife"];

	int seq = ent->m_nSequence();

	// check if this knife needs extra fixing.
	if (knife == knives_t::BUTTERFLY || knife == knives_t::FALCHION || knife == knives_t::DAGGER || knife == knives_t::BOWIE) {

		// fix the idle sequences.
		if (seq == sequence_default_idle1 || seq == sequence_default_idle2) {
			// set the animation to be completed.
			ent->m_flCycle() = 0.999f;

			// cycle change, re-Render.
			ent->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
		}
	}

	// fix sequences.
	if (m_last_seq != seq) {
		if (knife == knives_t::BUTTERFLY) {
			switch (seq) {
			case sequence_default_draw:
				seq = g_Csgo.RandomInt(sequence_butterfly_draw, sequence_butterfly_draw2);
				break;

			case sequence_default_lookat01:
				seq = g_Csgo.RandomInt(sequence_butterfly_lookat01, sequence_butterfly_lookat03);
				break;

			default:
				seq++;
				break;
			}
		}

		else if (knife == knives_t::FALCHION) {
			switch (seq) {
			case sequence_default_draw:
			case sequence_default_idle1:
				break;

			case sequence_default_idle2:
				seq = sequence_falchion_idle1;
				break;

			case sequence_default_heavy_miss1:
				seq = g_Csgo.RandomInt(sequence_falchion_heavy_miss1, sequence_falchion_heavy_miss1_noflip);
				break;

			case sequence_default_lookat01:
				seq = g_Csgo.RandomInt(sequence_falchion_lookat01, sequence_falchion_lookat02);
				break;
			}
		}

		else if (knife == knives_t::DAGGER) {
			switch (seq) {
			case sequence_default_idle2:
				seq = sequence_push_idle1;
				break;

			case sequence_default_heavy_hit1:
			case sequence_default_heavy_backstab:
			case sequence_default_lookat01:
				seq += 3;
				break;

			case sequence_default_heavy_miss1:
				seq = sequence_push_heavy_miss2;
				break;
			}

		}

		else if (knife == knives_t::BOWIE) {
			if (seq > sequence_default_idle1)
				seq--;
		}

		m_last_seq = seq;
	}

	// write back fixed sequence.
	ent->m_nSequence() = seq;
}

void Skins::UpdateHud()
{
#ifdef LEGACY
	static auto fn = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B")).as< std::int32_t(__thiscall*)(void*, std::int32_t)>();
	if (!fn)
		return;
#else
	static auto fn = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B")).as< std::int32_t(__thiscall*)(void*, std::int32_t)>();
	if (!fn)
		return;
#endif

	auto element = game::FindHudElement<std::uintptr_t*>(XOR("CCSGO_HudWeaponSelection"));
	if (!element)
		return;

	auto hud_weapons = reinterpret_cast<HudWeapons_t*>(std::uintptr_t(element) - 0xA0);
	if (!hud_weapons)
		return;

	if (!*hud_weapons->GetWeaponCount())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->GetWeaponCount(); i++)
		i = fn(hud_weapons, i);
}