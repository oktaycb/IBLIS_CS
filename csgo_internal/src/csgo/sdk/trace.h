#pragma once

using ShouldHitFunc_t = bool (__cdecl *)( Entity *, int );

enum {
	CHAR_TEX_ANTLION     = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE    = 'C',
	CHAR_TEX_DIRT        = 'D',
	CHAR_TEX_EGGSHELL    = 'E',
	CHAR_TEX_FLESH       = 'F',
	CHAR_TEX_GRATE       = 'G',
	CHAR_TEX_ALIENFLESH  = 'H',
	CHAR_TEX_CLIP        = 'I',
	CHAR_TEX_PLASTIC     = 'L',
	CHAR_TEX_METAL       = 'M',
	CHAR_TEX_SAND        = 'N',
	CHAR_TEX_FOLIAGE     = 'O',
	CHAR_TEX_COMPUTER    = 'P',
	CHAR_TEX_SLOSH       = 'S',
	CHAR_TEX_TILE        = 'T',
	CHAR_TEX_CARDBOARD   = 'U',
	CHAR_TEX_VENT        = 'V',
	CHAR_TEX_WOOD        = 'W',
	CHAR_TEX_GLASS       = 'Y',
	CHAR_TEX_WARPSHIELD  = 'Z',
};

// m_takeDamage
enum {
	DAMAGE_NO          = 0,
	DAMAGE_EVENTS_ONLY = 1,
	DAMAGE_YES         = 2,
	DAMAGE_AIM         = 3,
};

enum Collision_Group_t {
	COLLISION_GROUP_NONE = 0,
	COLLISION_GROUP_DEBRIS,
	COLLISION_GROUP_DEBRIS_TRIGGER,
	COLLISION_GROUP_INTERACTIVE_DEBRIS,
	COLLISION_GROUP_INTERACTIVE,
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,
	COLLISION_GROUP_NPC,
	COLLISION_GROUP_IN_VEHICLE,
	COLLISION_GROUP_WEAPON,
	COLLISION_GROUP_VEHICLE_CLIP,
	COLLISION_GROUP_PROJECTILE,
	COLLISION_GROUP_DOOR_BLOCKER,
	COLLISION_GROUP_PASSABLE_DOOR,
	COLLISION_GROUP_DISSOLVING,
	COLLISION_GROUP_PUSHAWAY,
	COLLISION_GROUP_NPC_ACTOR,
	COLLISION_GROUP_NPC_SCRIPTED,
	COLLISION_GROUP_PZ_CLIP,
	COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE,
	LAST_SHARED_COLLISION_GROUP
};

enum TraceType_t {
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS
};

enum Contents_t {
	CONTENTS_EMPTY                = 0,
	CONTENTS_SOLID                = 0x1,
	CONTENTS_WINDOW               = 0x2,
	CONTENTS_AUX                  = 0x4,
	CONTENTS_GRATE                = 0x8,
	CONTENTS_SLIME                = 0x10,
	CONTENTS_WATER                = 0x20,
	CONTENTS_BLOCKLOS             = 0x40,
	CONTENTS_OPAQUE               = 0x80,
	CONTENTS_TESTFOGVOLUME        = 0x100,
	CONTENTS_UNUSED               = 0x200,
	CONTENTS_BLOCKLIGHT           = 0x400,
	CONTENTS_TEAM1                = 0x800,
	CONTENTS_TEAM2                = 0x1000,
	CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000,
	CONTENTS_MOVEABLE             = 0x4000,
	CONTENTS_AREAPORTAL           = 0x8000,
	CONTENTS_PLAYERCLIP           = 0x10000,
	CONTENTS_MONSTERCLIP          = 0x20000,
	CONTENTS_CURRENT_0            = 0x40000,
	CONTENTS_CURRENT_90           = 0x80000,
	CONTENTS_CURRENT_180          = 0x100000,
	CONTENTS_CURRENT_270          = 0x200000,
	CONTENTS_CURRENT_UP           = 0x400000,
	CONTENTS_CURRENT_DOWN         = 0x800000,
	CONTENTS_ORIGIN               = 0x1000000,
	CONTENTS_MONSTER              = 0x2000000,
	CONTENTS_DEBRIS               = 0x4000000,
	CONTENTS_DETAIL               = 0x8000000,
	CONTENTS_TRANSLUCENT          = 0x10000000,
	CONTENTS_LADDER               = 0x20000000,
	CONTENTS_HITBOX               = 0x40000000,
};

enum Masks_t {
	MASK_ALL                   = 0xFFFFFFFF,
	MASK_SOLID                 = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_PLAYERSOLID           = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCSOLID              = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCFLUID              = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_WATER                 = CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME,
	MASK_OPAQUE                = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE,
	MASK_OPAQUE_AND_NPCS       = MASK_OPAQUE | CONTENTS_MONSTER,
	MASK_BLOCKLOS              = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS,
	MASK_BLOCKLOS_AND_NPCS     = MASK_BLOCKLOS | CONTENTS_MONSTER,
	MASK_VISIBLE               = MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_VISIBLE_AND_NPCS      = MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_SHOT                  = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE | CONTENTS_HITBOX,
	MASK_SHOT_BRUSHONLY        = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS,
	MASK_SHOT_HULL             = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE,
	MASK_SHOT_PORTAL           = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER,
	MASK_SOLID_BRUSHONLY       = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE,
	MASK_PLAYERSOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE,
	MASK_NPCSOLID_BRUSHONLY    = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC        = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC_FLUID  = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP,
	MASK_SPLITAREPORTAL        = CONTENTS_WATER | CONTENTS_SLIME,
	MASK_CURRENT               = CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN,
	MASK_DEADSOLID             = CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE,
};

enum {
	SURF_LIGHT     = 0x0001,
	SURF_SKY2D     = 0x0002,
	SURF_SKY       = 0x0004,
	SURF_WARP      = 0x0008,
	SURF_TRANS     = 0x0010,
	SURF_NOPORTAL  = 0x0020,
	SURF_TRIGGER   = 0x0040,
	SURF_NODRAW    = 0x0080,
	SURF_HINT      = 0x0100,
	SURF_SKIP      = 0x0200,
	SURF_NOLIGHT   = 0x0400,
	SURF_BUMPLIGHT = 0x0800,
	SURF_NOSHADOWS = 0x1000,
	SURF_NODECALS  = 0x2000,
	SURF_NOPAINT   = SURF_NODECALS,
	SURF_NOCHOP    = 0x4000,
	SURF_HITBOX    = 0x8000
};

enum SurfaceFlags_t {
	DISPSURF_FLAG_SURFACE   = ( 1 << 0 ),
	DISPSURF_FLAG_WALKABLE  = ( 1 << 1 ),
	DISPSURF_FLAG_BUILDABLE = ( 1 << 2 ),
	DISPSURF_FLAG_SURFPROP1 = ( 1 << 3 ),
	DISPSURF_FLAG_SURFPROP2 = ( 1 << 4 ),
};

struct BoxTraceInfo_t {
    float m_T1;
    float m_T2;
    int   m_HitSide;
    bool  m_StartSolid;

    __forceinline BoxTraceInfo_t( ) : m_T1{ -1.f }, m_T2{ 1.f }, m_HitSide{ -1 }, m_StartSolid{ true } {}
};

struct cplane_t {
	Vector       m_Normal;
	float       m_Distance;
	uint8_t     m_Type;
	uint8_t     m_SignBits;
	PAD( 0x2 );
};

struct csurface_t {
	const char* m_Name;
	short       m_SurfaceProps;
	uint16_t    m_Flags;
};

struct CBaseTrace {
	Vector   m_StartPosition;
	Vector   m_EndPosition;
	cplane_t m_Plane;
	float    m_Fraction;
	int      m_Contents;
	uint16_t m_DisplacementFlags;
	bool     m_AllSolid;
	bool     m_StartSolid;
};

class CGameTrace : public CBaseTrace {
public:
	float      m_FractionLeftSolid;
	csurface_t m_Surface;
	int        m_HitGroup;
	short      m_PhysicsBone;
	uint16_t   m_WorldSurfaceIndex;
	Entity*    m_Player;
	int        m_Hitbox;

	__forceinline bool hit( ) const {
		return m_Fraction < 1.f || m_AllSolid || m_StartSolid;
	}
	
	void m_clear()
	{
		m_StartPosition = Vector{};
		m_EndPosition = Vector{};
		m_Plane = cplane_t();
		m_Fraction = 0;
		m_Contents = 0;
		m_DisplacementFlags = 0;
		m_AllSolid = 0;
		m_StartSolid = 0;
		m_Surface.m_Name = XOR("**empty**");
	}
};

class Ray {
public:
	__forceinline Ray( ) : m_WorldAxisTransform{}, m_IsRay{}, m_IsSwept{} {}

	__forceinline Ray( const Vector &start, const Vector &end ) {
		m_Delta                = vec_aligned_t{ end - start };
		m_IsSwept             = m_Delta.LengthSqr( ) != 0.f;
		m_WorldAxisTransform = nullptr;
		m_IsRay               = true;
		m_Start                = start;
		m_StartOffset.clear( );
		m_Extents.clear( );
	}

	__forceinline Ray( const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs ) {
		m_Delta                = vec_aligned_t{ end - start };
		m_WorldAxisTransform = nullptr;
		m_IsSwept             = m_Delta.LengthSqr( ) != 0.f;
		m_Extents              = vec_aligned_t{ maxs - mins };
		m_Extents              *= 0.5f;
		m_IsRay               = m_Extents.LengthSqr( ) < 1e-6;
		m_StartOffset         = vec_aligned_t{ mins + maxs };
		m_StartOffset         *= 0.5f;
		m_Start                = vec_aligned_t{ start + m_StartOffset };
		m_StartOffset         *= -1.f;
	}

public:
	vec_aligned_t      m_Start;
	vec_aligned_t      m_Delta;
	vec_aligned_t	   m_StartOffset;
	vec_aligned_t	   m_Extents;
	const matrix3x4_t* m_WorldAxisTransform;
	bool               m_IsRay;
	bool               m_IsSwept;
};

class ITraceFilter {
public:
	virtual bool        ShouldHitEntity( Entity *pEntity, int contents_mask ) = 0;
	virtual TraceType_t	GetTraceType( ) const = 0;
};

class CTraceFilter : public ITraceFilter {
public:
	virtual TraceType_t	GetTraceType() const {
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterSimple : public CTraceFilter {
public:
	const Entity*   m_PassEntity;
	int             m_CollisionGroup;
	ShouldHitFunc_t m_ShouldHitCheckFn;

public:
    __forceinline CTraceFilterSimple() {}

	__forceinline CTraceFilterSimple( const Entity* pass_ent1, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr ) :
        m_PassEntity{ pass_ent1 },
        m_CollisionGroup{ collision_group },
        m_ShouldHitCheckFn{ shouldhit_check_fn } {}
    
	virtual bool ShouldHitEntity( Entity* entity, int contents_mask ) {
        return entity != m_PassEntity;
	}

    virtual void SetPassEntity( Entity *pass_ent1 ) { 
        m_PassEntity = pass_ent1; 
    }

	virtual void SetCollisionGroup( int collision_group ) { 
        m_CollisionGroup = collision_group; 
    }
};

class CTraceFilterSkipTwoEntities : public CTraceFilterSimple {
public:
    const Entity* m_PassEntity;

public:
    __forceinline CTraceFilterSkipTwoEntities( const Entity* pass_ent1, const Entity* pass_ent2, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr ) :
        m_PassEntity{ pass_ent2 },
        CTraceFilterSimple( pass_ent1, collision_group, shouldhit_check_fn ) {}
    
	virtual bool ShouldHitEntity( Entity* entity, int contents_mask ) {
        return entity != m_PassEntity;
	}

    virtual void SetPassEntity2( Entity *pass_ent2 ) { 
        m_PassEntity = pass_ent2; 
    }
};

class CTraceFilterWorldOnly : public ITraceFilter {
public:
    virtual bool ShouldHitEntity( Entity* entity, int contents_mask ) {
        return false;
	}

    virtual TraceType_t	GetTraceType( ) const {
		return TRACE_WORLD_ONLY;
	}
};


struct surfacephysicsparams_t {
	float m_friction;
	float m_elasticity;
	float m_density;
	float m_thickness;
	float m_dampening;
};

struct surfaceaudioparams_t {
	float m_audio_reflectivity;
	float m_audio_hardness_factor;
	float m_audio_roughness_factor;
	float m_scrape_rough_threshold;
	float m_impact_hard_threshold;
	float m_audio_hard_min_velocity;
	float m_high_pitch_occlusion;
	float m_mid_pitch_occlusion;
	float m_low_pitch_occlusion;
};

struct surfacegameprops_t {
	float    m_max_speed_factor;
	float    m_jump_factor;
	float    m_PenetrationModifier;
	float    m_DamageModifier;
	uint16_t m_Material;
	uint8_t  m_climbable;
};

struct surfacesoundnames_t {
	short m_walk_left;
	short m_walk_right;
	short m_run_left;
	short m_run_right;
	short m_impact_soft;
	short m_impact_hard;
	short m_scrape_smooth;
	short m_scrape_rough;
	short m_bullet_impact;
	short m_rolling;
	short m_break_sound;
	short m_strain;
};

struct surfacedata_t {
	surfacephysicsparams_t m_physics;
	surfaceaudioparams_t   m_audio;
	surfacesoundnames_t    m_sounds;
	surfacegameprops_t     m_Game;
	PAD( 48 );
}; // size = 0x94

class IPhysicsSurfaceProps {
public:
	__forceinline surfacedata_t* GetSurfaceData( int surfaceDataIndex ) {
		return util::get_method< surfacedata_t*( __thiscall* )( decltype( this ), int ) >( this, 5 )( this, surfaceDataIndex );
	}
};

class IHandleEntity;
class ICollideable;

class IEngineTrace {
public:
	enum indices : size_t {
		GETPOINTCONTENTS = 0,
		GETPOINTCONTENTSWORLDONLY = 1,
		CLIPRAYTOENTITY = 3,
		CLIPRAYTOCOLLIDEABLE = 4,
		TRACERAY = 5,
	};

public:
	// Returns the contents mask + entity at a particular world-space position
	virtual int		GetPointContents(const Vector& vecAbsPosition, int contentsMask = MASK_ALL, IHandleEntity** ppEntity = NULL) = 0;

	// Returns the contents mask of the world only @ the world-space position (static props are ignored)
	virtual int		GetPointContents_WorldOnly(const Vector& vecAbsPosition, int contentsMask = MASK_ALL) = 0;

	// Get the point contents, but only test the specific entity. This works
	// on static props and brush models.
	//
	// If the entity isn't a static prop or a brush model, it returns CONTENTS_EMPTY and sets
	// bFailed to true if bFailed is non-null.
	virtual int		GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToEntity(const Ray& ray, unsigned int fMask, IHandleEntity* pEnt, trace_t* pTrace) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToCollideable(const Ray& ray, unsigned int fMask, ICollideable* pCollide, trace_t* pTrace) = 0;
	
	void TraceRay(const Ray& ray, uint32_t mask, ITraceFilter* filter, CGameTrace* trace);
};