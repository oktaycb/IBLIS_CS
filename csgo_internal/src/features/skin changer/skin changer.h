#pragma once

constexpr int sequence_default_draw = 0;
constexpr int sequence_default_idle1 = 1;
constexpr int sequence_default_idle2 = 2;
constexpr int sequence_default_heavy_miss1 = 9;
constexpr int sequence_default_heavy_hit1 = 10;
constexpr int sequence_default_heavy_backstab = 11;
constexpr int sequence_default_lookat01 = 12;

constexpr int sequence_butterfly_draw = 0;
constexpr int sequence_butterfly_draw2 = 1;
constexpr int sequence_butterfly_lookat01 = 13;
constexpr int sequence_butterfly_lookat03 = 15;

constexpr int sequence_falchion_idle1 = 1;
constexpr int sequence_falchion_heavy_miss1 = 8;
constexpr int sequence_falchion_heavy_miss1_noflip = 9;
constexpr int sequence_falchion_lookat01 = 12;
constexpr int sequence_falchion_lookat02 = 13;

constexpr int sequence_push_idle1 = 1;
constexpr int sequence_push_heavy_miss2 = 11;

class CCStrike15ItemSchema;

class CCStrike15ItemSystem;

template <typename Key, typename Value>

struct Node_t {
	int previous_id;        //0x0000
	int next_id;            //0x0004
	void* _unknown_ptr;        //0x0008
	int _unknown;            //0x000C
	Key key;                //0x0010
	Value value;            //0x0014
};

template <typename Key, typename Value>

struct Head_t {
	Node_t<Key, Value>* memory;        //0x0000
	int allocation_count;            //0x0004
	int grow_size;                    //0x0008
	int start_element;                //0x000C
	int next_available;                //0x0010
	int _unknown;                    //0x0014
	int last_element;                //0x0018
}; //Size=0x001C

struct String_t {
	char* buffer;    //0x0000
	int capacity;    //0x0004
	int grow_size;    //0x0008
	int Length;        //0x000C
}; //Size=0x0010

struct CPaintKit {
	int id;                        //0x0000

	String_t name;                //0x0004
	String_t description;        //0x0014
	String_t item_name;            //0x0024
	String_t material_name;        //0x0034
	String_t image_inventory;    //0x0044

	char pad_0x0054[0x8C];        //0x0054
}; //Size=0x00E0

struct paint_kit_t {
	int id;
	std::string name;

	auto operator < (const paint_kit_t& other) const -> bool {
		return name < other.name;
	}
};

struct KnifeData {
	int         m_id;
	std::string m_model, m_world_model, m_Name;
	int         m_model_index, m_world_model_index;

	__forceinline KnifeData( ) : m_id{}, m_model{}, m_world_model{}, m_Name {}, m_model_index{}, m_world_model_index{} {}

	__forceinline void setup( int id, const std::string& model, const std::string& world_model, const std::string& name ) {
		m_id                = id;
		m_model             = model;
		m_Name              = name;
		m_model_index       = g_Csgo.m_ModelInfo->GetModelIndex( model.data( ) );
		m_world_model_index = g_Csgo.m_ModelInfo->GetModelIndex( world_model.data( ) );
	}
};

struct GloveData {
	int         m_id;
	std::string m_model, m_world_model;
	int         m_model_index, m_world_model_index;

	__forceinline GloveData( ) : m_id{}, m_model{}, m_model_index{} {}

	__forceinline void setup( int id, const std::string& model, const std::string& world_model ) {
		m_id                = id;
		m_model             = model;
		m_world_model       = world_model;
		m_model_index       = g_Csgo.m_ModelInfo->GetModelIndex( model.data( ) );
		m_world_model_index = g_Csgo.m_ModelInfo->GetModelIndex( world_model.data( ) );
	}
};

struct HudWeapons_t {
	std::int32_t* GetWeaponCount() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

class Skins {
public:
	enum knives_t {
		BAYONET = 0,
		BOWIE,
		BUTTERFLY,
		FALCHION ,
		FLIP,
		GUT,
		HUNTSMAN,
		KARAMBIT,
		M9,
		DAGGER,
		KNIFE_MAX
	};

	enum gloves_t {
		BLOODHOUND = 0,
		SPORTY,
		DRIVER,
		HANDWRAP,
		MOTO,
		SPECIALIST,
		GLOVE_MAX
	};

public:
	KnifeData   m_knife_data[ knives_t::KNIFE_MAX ];
	GloveData   m_glove_data[ gloves_t::GLOVE_MAX ];
	int         m_last_seq;
	CBaseHandle m_glove_handle;
	bool        m_update;
	float       m_update_time;

	std::vector<paint_kit_t> m_skins;
	std::vector<std::string> m_paint_kits;

	std::vector<paint_kit_t> m_gloves;
	std::vector<std::string> m_glove_kits;
public:
	void Run();
	void load( );
	void think( );
	void UpdateItem( Weapon* item );
	void UpdateAnimations( Entity* ent );
	void UpdateHud();
};

extern Skins g_skins;