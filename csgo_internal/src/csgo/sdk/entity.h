#pragma once

#define CLIENT_CLASS_DLL
#define MAX_WEAPONS	48
#define PPOFFSET(func, type, offset) __forceinline type& func { return **reinterpret_cast<type**>(reinterpret_cast<uintptr_t>(this) + offset); }
#define OFFSET(type, name, offset)\
type &name##() const\
{\
        return *(type*)(uintptr_t(this) + offset);\
}

enum RenderFlags_t : uint32_t {
	STUDIO_NONE = 0x00000000,
	STUDIO_RENDER = 0x00000001,
	STUDIO_VIEWXFORMATTACHMENTS = 0x00000002,
	STUDIO_DRAWTRANSLUCENTSUBMODELS = 0x00000004,
	STUDIO_TWOPASS = 0x00000008,
	STUDIO_STATIC_LIGHTING = 0x00000010,
	STUDIO_WIREFRAME = 0x00000020,
	STUDIO_ITEM_BLINK = 0x00000040,
	STUDIO_NOSHADOWS = 0x00000080,
	STUDIO_WIREFRAME_VCOLLIDE = 0x00000100,
	STUDIO_NOLIGHTING_OR_CUBEMAP = 0x00000200,
	STUDIO_SKIP_FLEXES = 0x00000400,
	STUDIO_DONOTMODIFYSTENCILSTATE = 0x00000800,
	STUDIO_TRANSPARENCY = 0x80000000,
	STUDIO_SHADOWDEPTHTEXTURE = 0x40000000,
	STUDIO_SHADOWTEXTURE = 0x20000000,
	STUDIO_SKIP_DECALS = 0x10000000
};

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01	// bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL		0x02	// procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL		0x04	// bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE	0x08	// bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER	0x10	// bone aligns to the screen, constrained by it's own axis.
#define BONE_WORLD_ALIGN			0x20	// bone is rigidly aligned to the world (but can still translate)

enum BoneFlags_t : int {
	BONE_USED_BY_ANYTHING = 0x0007FF00,
	BONE_USED_BY_HITBOX = 0x00000100, // bone (or child) is used by a hit box
	BONE_USED_BY_ATTACHMENT = 0x00000200, // bone (or child) is used by an attachment point
	BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
	BONE_USED_BY_VERTEX_LOD0 = 0x00000400, // bone (or child) is used by the toplevel model via skinned vertex
	BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
	BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
	BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
	BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
	BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
	BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
	BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
	BONE_USED_BY_BONE_MERGE = 0x00040000,
	BONE_ALWAYS_SETUP = 0x00080000,
	BONE_USED_BY_SERVER = BONE_USED_BY_HITBOX | BONE_USED_BY_VERTEX_LOD0 | BONE_USED_BY_VERTEX_LOD1 | BONE_USED_BY_VERTEX_LOD2 | BONE_USED_BY_VERTEX_LOD3 | BONE_USED_BY_VERTEX_LOD4 | BONE_USED_BY_VERTEX_LOD5 | BONE_USED_BY_VERTEX_LOD6 | BONE_USED_BY_VERTEX_LOD7
};

enum teams_t : int {
	TEAM_NOTEAM = 0,
	TEAM_SPECTATOR,
	TEAM_TERRORISTS,
	TEAM_COUNTERTERRORISTS
};

enum {
	ANIMLODFLAG_DISTANT = 0x01,
	ANIMLODFLAG_OUTSIDEVIEWFRUSTUM = 0x02,
	ANIMLODFLAG_INVISIBLELOCALPLAYER = 0x04,
	ANIMLODFLAG_DORMANT = 0x08,
};

enum effects_t : int {
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
	EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
	EF_MAX_BITS = 10
};

enum ik_t : int
{
	ENTCLIENTFLAG_GETTINGSHADOWRENDERBOUNDS = 0x0001,
	ENTCLIENTFLAG_DONTUSEIK = 0x0002,
	ENTCLIENTFLAG_ALWAYS_INTERPOLATE = 0x0004
};

enum DataUpdateType_t : int {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum LifeStates_t : int {
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum PlayerFlags_t : int {
	FL_ONGROUND = (1 << 0),
	FL_DUCKING = (1 << 1),
	FL_WATERJUMP = (1 << 3),
	FL_ONTRAIN = (1 << 4),
	FL_INRAIN = (1 << 5),
	FL_FROZEN = (1 << 6),
	FL_ATCONTROLS = (1 << 7),
	FL_CLIENT = (1 << 8),
	FL_FAKECLIENT = (1 << 9),
	FL_INWATER = (1 << 10),
};

enum class PoseParam_t {
	STRAFE_YAW,
	STAND,
	LEAN_YAW,
	SPEED,
	LADDER_YAW,
	LADDER_SPEED,
	JUMP_FALL,
	MOVE_YAW,
	MOVE_BLEND_CROUCH,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	BODY_YAW,
	BODY_PITCH,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_COURCH_IDLE,
	AIM_BLEND_CROUCH_WALK,
	DEATH_YAW
};

enum MoveType_t : int {
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4,
};

enum CSWeaponType : int {
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_HEAVYPISTOL,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_EQUIPMENT,
	WEAPONTYPE_STACKABLEITEM,
	WEAPONTYPE_UNKNOWN
};

// todo: enum class.
enum Weapons_t : int {
	UNKOWN = 0,
	DEAGLE = 1,
	ELITE = 2,
	FIVESEVEN = 3,
	GLOCK = 4,
	AK47 = 7,
	AUG = 8,
	AWP = 9,
	FAMAS = 10,
	G3SG1 = 11,
	GALIL = 13,
	M249 = 14,
	M4A4 = 16,
	MAC10 = 17,
	P90 = 19,
	MP5SD = 23,
	UMP45 = 24,
	XM1014 = 25,
	BIZON = 26,
	MAG7 = 27,
	NEGEV = 28,
	SAWEDOFF = 29,
	TEC9 = 30,
	ZEUS = 31,
	P2000 = 32,
	MP7 = 33,
	MP9 = 34,
	NOVA = 35,
	P250 = 36,
	SCAR20 = 38,
	SG553 = 39,
	SSG08 = 40,
	KNIFE_T = 59,
	FLASHBANG = 43,
	HEGRENADE = 44,
	SMOKE = 45,
	MOLOTOV = 46,
	DECOY = 47,
	FIREBOMB = 48,
	C4 = 49,
	MUSICKIT = 58,
	KNIFE_CT = 42,
	M4A1S = 60,
	USPS = 61,
	TRADEUPCONTRACT = 62,
	CZ75A = 63,
	REVOLVER = 64,
	TAG_GRENADE = 68,
	KNIFE_BAYONET = 500,
	KNIFE_FLIP = 505,
	KNIFE_GUT = 506,
	KNIFE_KARAMBIT = 507,
	KNIFE_M9_BAYONET = 508,
	KNIFE_HUNTSMAN = 509,
	KNIFE_FALCHION = 512,
	KNIFE_BOWIE = 514,
	KNIFE_BUTTERFLY = 515,
	KNIFE_SHADOW_DAGGERS = 516,
};

struct RenderableInstance_t {
	uint8_t m_alpha;
	__forceinline RenderableInstance_t() : m_alpha{ 255ui8 } {}
};

enum InvalidatePhysicsBits_t : int {
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,		// Means cycle has changed, or any other event which would cause Render-to-texture shadows to need to be rerendeded
	BOUNDS_CHANGED = 0x10,		    // Means Render bounds have changed, so shadow decal projection is required, etc.
	SEQUENCE_CHANGED = 0x20,		// Means sequence has changed, only interesting when surrounding bounds depends on sequence	
};

class Entity;

// This is used by the player to access vehicles. It's an interface so the
// vehicles are not restricted in what they can derive from.
class IVehicle
{
public:
	// Get and set the current driver. Use PassengerRole_t enum in shareddefs.h for adding passengers
	virtual Entity* GetPassenger(int nRole = 0) = 0;
	virtual int						GetPassengerRole(Entity* pPassenger) = 0;

	// Where is the passenger seeing from?
	virtual void			GetVehicleViewPosition(int nRole, Vector* pOrigin, QAngle* pAngles, float* pFOV = NULL) = 0;

	// Does the player use his normal weapons while in this mode?
	virtual bool			IsPassengerUsingStandardWeapons(int nRole = 0) = 0;

	// Process movement
	virtual void			SetupMove(Entity* player, Command* ucmd, IMoveHelper* pHelper, CMoveData* move) = 0;
	virtual void			ProcessMovement(Entity* pPlayer, CMoveData* pMoveData) = 0;
	virtual void			FinishMove(Entity* player, Command* ucmd, CMoveData* move) = 0;

	// Process input
	virtual void			ItemPostFrame(Entity* pPlayer) = 0;
};

class IHandleEntity {
public:
	virtual ~IHandleEntity() { }
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const ulong_t& GetRefEHandle() const = 0;
};

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

class ReadBF {
public:
	ReadBF() = default;

	ReadBF(const void* pData, int nBytes, int nBits = -1) {
		StartReading(pData, nBytes, 0, nBits);
	}

	const char* szDebugName;
	bool bOverflow;
	int nDataBits;
	unsigned int nDataBytes;
	unsigned int nInBufWord;
	int nBitsAvail;
	const unsigned int* iDataIn;
	const unsigned int* iBufferEnd;
	const unsigned int* iData;

	void StartReading(const void* pData, int nBytes, int iStartBit, int nBits) {
		iData = (uint32_t*)pData;
		iDataIn = iData;
		nDataBytes = nBytes;

		if (nBits == -1)
		{
			nDataBits = nBytes << 3;
		}
		else
		{
			nDataBits = nBits;
		}

		bOverflow = false;

		iBufferEnd = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(iData) + nBytes);

		if (iData)
			Seek(iStartBit);
	}

	bool Seek(int nPosition)
	{
		bool bSucc = true;

		if (nPosition < 0 || nPosition > nDataBits)
		{
			bOverflow = true;
			bSucc = false;
			nPosition = nDataBits;
		}

		int nHead = nDataBytes & 3;
		int nByteOfs = nPosition / 8;

		if ((nDataBytes < 4) || (nHead && (nByteOfs < nHead)))
		{
			uint8_t const* pPartial = (uint8_t const*)iData;

			if (iData)
			{
				nInBufWord = *(pPartial++);

				if (nHead > 1)
					nInBufWord |= (*pPartial++) << 8;
				else if (nHead > 2)
					nInBufWord |= (*pPartial++) << 16;
			}

			iDataIn = (uint32_t const*)pPartial;
			nInBufWord >>= (nPosition & 31);
			nBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else {
			int nAdjPosition = nPosition - (nHead << 3);

			iDataIn = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(iData) + ((nAdjPosition / 32) << 2) + nHead);

			if (iData)
			{
				nBitsAvail = 32;
				GrabNextDWord();
			}
			else
			{
				nInBufWord = 0;
				nBitsAvail = 1;
			}

			nInBufWord >>= (nAdjPosition & 31);
			nBitsAvail = std::min(nBitsAvail, 32 - (nAdjPosition & 31)); // in case grabnextdword overflowed
		}
		return bSucc;
	}

	FORCEINLINE void GrabNextDWord(bool bOverFlowImmediately = false)
	{
		if (iDataIn == iBufferEnd)
		{
			nBitsAvail = 1;
			nInBufWord = 0;
			iDataIn++;

			if (bOverFlowImmediately)
				bOverflow = true;
		}
		else if (iDataIn > iBufferEnd)
		{
			bOverflow = true;
			nInBufWord = 0;
		}
		else
		{
			nInBufWord = DWORD(*(iDataIn++));
		}
	}
};

class WriteBF {
public:
	unsigned char* iData;
	int nDataBytes;
	int nDataBits;
	int iCurBit;
	bool bOverflow;
	bool bAssertOnOverflow;
	const char* szDebugName;

	void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nBits = -1) {
		nBytes &= ~3;

		iData = (unsigned char*)pData;
		nDataBytes = nBytes;

		if (nBits == -1) {
			nDataBits = nBytes << 3;
		}
		else {
			nDataBits = nBits;
		}

		iCurBit = iStartBit;
		bOverflow = false;
	}

	WriteBF() {
		iData = NULL;
		nDataBytes = 0;
		nDataBits = -1; // set to -1 so we generate overflow on any operation
		iCurBit = 0;
		bOverflow = false;
		bAssertOnOverflow = true;
		szDebugName = NULL;
	}

	WriteBF(void* pData, int nBytes, int nBits = -1)
	{
		bAssertOnOverflow = true;
		szDebugName = NULL;
		StartWriting(pData, nBytes, 0, nBits);
	}

	WriteBF(const char* pDebugName, void* pData, int nBytes, int nBits = -1)
	{
		bAssertOnOverflow = true;
		szDebugName = pDebugName;
		StartWriting(pData, nBytes, 0, nBits);
	}
};

class IClientUnknown;
class IPhysicsObject;
class ICollideable
{
public:
	virtual IHandleEntity*		GetEntityHandle() = 0;
	virtual Vector&				OBBMins() const = 0;
	virtual Vector&				OBBMaxs() const = 0;
	virtual void				WorldSpaceTriggerBounds(Vector* pVecWorldMins, Vector* pVecWorldMaxs) const = 0;
	virtual bool				TestCollision(const Ray& ray, unsigned int fContentsMask, trace_t& tr) = 0;
	virtual bool				TestHitboxes(const Ray& ray, unsigned int fContentsMask, trace_t& tr) = 0;
	virtual int					GetCollisionModelIndex() = 0;
	virtual const model_t*		GetCollisionModel() = 0;
	virtual const Vector&		GetCollisionOrigin() const = 0;
	virtual const QAngle&		GetCollisionAngles() const = 0;
	virtual const matrix3x4_t&	CollisionToWorldTransform() const = 0;
	virtual SolidType_t			GetSolid() const = 0;
	virtual int					GetSolidFlags() const = 0;
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual int					GetCollisionGroup() const = 0;
	virtual void				WorldSpaceSurroundingBounds(Vector* pVecMins, Vector* pVecMaxs) = 0;
	virtual UINT				GetRequiredTriggerFlags() const = 0;
	virtual const matrix3x4_t*	GetRootParentToWorldTransform() const = 0;
	virtual IPhysicsObject*		GetVPhysicsObject() const = 0;
};

//class CCollisionProperty
//{
//public:
//	Vector& OBBMins();
//	Vector& OBBMaxs();
//};

class IClientNetworkable;
class C_BaseEntity;
class IClientRenderable;
class ICollideable;
class IClientEntity;
class IClientThinkable;
class IClientModelRenderable;
class IClientAlphaProperty;

class IClientUnknown : public IHandleEntity {
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};

enum ShouldTransmitState_t
{
	SHOULDTRANSMIT_START = 0,
	SHOULDTRANSMIT_END
};

enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM
};

class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged(bool bInPVS) = 0;
};

class IClientRenderable
{
public:
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin(void) = 0;
	virtual QAngle const&			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual int					    GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void					Unused(void) const {}
	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t&	RenderHandle() = 0;
	virtual const model_t*			GetModel() const = 0;
	virtual int						DrawModel(int flags, const RenderableInstance_t& instance) = 0;
	virtual int						GetBody() = 0;
	virtual void					GetColorModulation(float* color) = 0;
	virtual bool					LODTest() = 0;
	virtual bool					SetupBones(matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void					SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void					DoAnimationEvents(void) = 0;
	virtual IPVSNotify*				GetPVSNotifyInterface() = 0;
	virtual void					GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void					GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void					GetShadowRenderBounds(Vector& mins, Vector& maxs, ShadowType_t shadowType) = 0;
	virtual bool					ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool					GetShadowCastDistance(float* pDist, ShadowType_t shadowType) const = 0;
	virtual bool					GetShadowCastDirection(Vector* pDirection, ShadowType_t shadowType) const = 0;
	virtual bool					IsShadowDirty() = 0;
	virtual void					MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable*		GetShadowParent() = 0;
	virtual IClientRenderable*		FirstShadowChild() = 0;
	virtual IClientRenderable*		NextShadowPeer() = 0;
	virtual ShadowType_t			ShadowCastType() = 0;
	virtual void					Unused2() {}
	virtual void					CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t	GetModelInstance() = 0;
	virtual const matrix3x4_t&		RenderableToWorldTransform() = 0;
	virtual int						LookupAttachment(const char* pAttachmentName) = 0;
	virtual	bool					GetAttachment(int number, Vector& origin, QAngle& angles) = 0;
	virtual bool					GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual bool					ComputeLightingOrigin(int nAttachmentIndex, Vector modelLightingCenter, const matrix3x4_t& matrix, Vector& transformedLightingCenter) = 0;
	virtual float*					GetRenderClipPlane(void) = 0;
	virtual int						GetSkin() = 0;
	virtual void					OnThreadedDrawSetup() = 0;
	virtual bool					UsesFlexDelayedWeights() = 0;
	virtual void					RecordToolMessage() = 0;
	virtual bool					ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual UINT8					OverrideAlphaModulation(UINT8 nAlpha) = 0;
	virtual UINT8					OverrideShadowAlphaModulation(UINT8 nAlpha) = 0;
	virtual IClientModelRenderable* GetClientModelRenderable() = 0;
};

class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;
	virtual void			NotifyShouldTransmit(ShouldTransmitState_t state) = 0;
	virtual void			OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void			OnDataChanged(DataUpdateType_t updateType) = 0;
	virtual void			PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void			PostDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void			OnDataUnchangedInPVS() = 0;
	virtual bool			IsDormant(void) const = 0;
	virtual int				GetIndex(void) const = 0;
	virtual void			ReceiveMessage(int classID, void*& msg) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;
};

enum PrecipitationType_t : int
{
	PRECIPITATION_TYPE_RAIN = 0,
	PRECIPITATION_TYPE_SNOW,
	PRECIPITATION_TYPE_ASH,
	PRECIPITATION_TYPE_SNOWFALL,
	PRECIPITATION_TYPE_PARTICLERAIN,
	PRECIPITATION_TYPE_PARTICLEASH,
	PRECIPITATION_TYPE_PARTICLERAINSTORM,
	PRECIPITATION_TYPE_PARTICLESNOW,
	NUM_PRECIPITATION_TYPES
};
struct ikchainresult_t
{
	// accumulated offset from ideal footplant location
	int			target;
	Vector		pos;
	Quaternion	q;
	float		flWeight;
};



struct ikcontextikrule_t
{
	int			index;

	int			type;
	int			chain;

	int			bone;

	int			slot;	// iktarget slot.  Usually same as chain.
	float		height;
	float		radius;
	float		floor;
	Vector		pos;
	float		pad1;
	Quaternion	q;

	float		start;	// beginning of influence
	float		peak;	// start of full influence
	float		tail;	// end of full influence
	float		end;	// end of all influence

	float		top;
	float		drop;

	float		commit;		// frame footstep target should be committed
	float		release;	// frame ankle should end rotation from latched orientation

	float		flWeight;		// processed version of start-end cycle
	float		flRuleWeight;	// blending weight
	float		latched;		// does the IK rule use a latched value?
	char* szLabel;

	Vector		kneeDir;
	float		pad2;
	Vector		kneePos;
	float		pad3;

	ikcontextikrule_t() {}

private:
	// No copy constructors allowed
	ikcontextikrule_t(const ikcontextikrule_t& vOther);
};

class CIKTarget
{
public:
	void SetOwner(int entindex, const Vector& pos, const QAngle& angles);
	void ClearOwner(void);
	int GetOwner(void);
	void UpdateOwner(int entindex, const Vector& pos, const QAngle& angles);
	void SetPos(const Vector& pos);
	void SetAngles(const QAngle& angles);
	void SetQuaternion(const Quaternion& q);
	void SetNormal(const Vector& normal);
	void SetPosWithNormalOffset(const Vector& pos, const Vector& normal);
	void SetOnWorld(bool bOnWorld = true);

	bool IsActive(void);
	void IKFailed(void);
	int chain;
	int type;
	void MoveReferenceFrame(Vector& deltaPos, QAngle& deltaAngles);
	// accumulated offset from ideal footplant location
public:
	struct x2 {
		char* pAttachmentName;
		Vector		pos;
		Quaternion	q;
	} offset;
private:
	struct x3 {
		Vector		pos;
		Quaternion	q;
	} ideal;
public:
	struct x4 {
		float		latched;
		float		release;
		float		height;
		float		floor;
		float		radius;
		float		flTime;
		float		flWeight;
		Vector		pos;
		Quaternion	q;
		bool		onWorld;
	} est; // estimate contact position
	struct x5 {
		float		hipToFoot;	// distance from hip
		float		hipToKnee;	// distance from hip to knee
		float		kneeToFoot;	// distance from knee to foot
		Vector		hip;		// location of hip
		Vector		closest;	// closest valid location from hip to foot that the foot can move to
		Vector		knee;		// pre-ik location of knee
		Vector		farthest;	// farthest valid location from hip to foot that the foot can move to
		Vector		lowest;		// lowest position directly below hip that the foot can drop to
	} trace;
private:
	// internally latched footset, position
	struct x1 {
		// matrix3x4a_t		worldTarget;
		bool		bNeedsLatch;
		bool		bHasLatch;
		float		influence;
		int			iFramecounter;
		int			owner;
		Vector		absOrigin;
		QAngle		absAngles;
		Vector		pos;
		Quaternion	q;
		Vector		deltaPos;	// acculated error
		Quaternion	deltaQ;
		Vector		debouncePos;
		Quaternion	debounceQ;
	} latched;
	struct x6 {
		float		flTime; // time last error was detected
		float		flErrorTime;
		float		ramp;
		bool		bInError;
	} error;

	friend class CIKContext;
};

class CIKContext
{
public:
	CIKContext();
	void Init(const CStudioHdr* pStudioHdr, const QAngle& angles, const Vector& pos, float flTime, int iFramecounter, int boneMask);
	void AddDependencies(mstudioseqdesc_t& seqdesc, int iSequence, float flCycle, const float poseParameters[], float flWeight = 1.0f);

#if defined( _PS3 )
	void AddAllDependencies_PS3(ikcontextikrule_t* ikRules, int numRules);
#endif

	void ClearTargets(void);
	void UpdateTargets(Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed);
	void AutoIKRelease(void);
	void SolveDependencies(Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed);

	void AddAutoplayLocks(Vector pos[], Quaternion q[]);
	void SolveAutoplayLocks(Vector pos[], Quaternion q[]);

	void AddSequenceLocks(mstudioseqdesc_t& SeqDesc, Vector pos[], Quaternion q[]);
	void SolveSequenceLocks(mstudioseqdesc_t& SeqDesc, Vector pos[], Quaternion q[]);

	void AddAllLocks(Vector pos[], Quaternion q[]);
	void SolveAllLocks(Vector pos[], Quaternion q[]);

	void SolveLock(const mstudioiklock_t* plock, int i, Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed);

	CUtlVectorFixed< CIKTarget, 12 >	m_target;

	void CopyTo(CIKContext* pOther, const unsigned short* iRemapping);

	void BuildBoneChain(const Vector pos[], const Quaternion q[], int iBone, matrix3x4a_t* pBoneToWorld, CBoneBitList& boneComputed);

	const int GetBoneMask(void) { return m_boneMask; }
private:

	CStudioHdr const* m_pStudioHdr;

	bool Estimate(int iSequence, float flCycle, int iTarget, const float poseParameter[], float flWeight = 1.0f);


	// virtual IK rules, filtered and combined from each sequence
	CUtlVector< CUtlVector< ikcontextikrule_t > > m_ikChainRule;
	CUtlVector< ikcontextikrule_t > m_ikLock;
	matrix3x4a_t m_rootxform;

	int m_iFramecounter;
	float m_flTime;
	int m_boneMask;
};

class Entity : public IClientUnknown {
public:
	// helper methods.
	template< typename t >
	__forceinline t& get(size_t offset) {
		return *(t*)((uintptr_t)this + offset);
	}

	template< typename t >
	__forceinline void set(size_t offset, const t& val) {
		*(t*)((uintptr_t)this + offset) = val;
	}

	template< typename t >
	__forceinline t as() {
		return (t)this;
	}

public:
	// netvars / etc.
	__forceinline Vector& m_vecOrigin() {
		return get< Vector >(g_Offsets.m_vecOrigin);
	}

	__forceinline Vector& m_vecOldOrigin() {
		return get< Vector >(g_Offsets.m_vecOldOrigin);
	}

	__forceinline Vector& m_vecVelocity() {
		return get< Vector >(g_Offsets.m_vecVelocity);
	}

	__forceinline PrecipitationType_t& m_nPrecipType() {
		return get< PrecipitationType_t >(g_Offsets.m_nPrecipType);
	}

	__forceinline float& m_flGravity() {
		return get< float >(g_Offsets.m_flGravity);
	}

	__forceinline int& m_iTeamNum() {
		return get< int >(g_Offsets.m_iTeamNum);
	}

	__forceinline float& m_flSpawnTime_Grenade()
	{
		return get< float >(g_Offsets.m_flSpawnTime_Grenade);
	}

	__forceinline EHANDLE& m_hThrower()
	{
		return get< EHANDLE >(g_Offsets.m_hThrower);
	}

	__forceinline EHANDLE& m_hOwnerEntity() {
		return get< EHANDLE >(g_Offsets.m_hOwnerEntity);
	}

	__forceinline int& m_iHealth() {
		return get< int >(g_Offsets.m_iHealth);
	}

	__forceinline float& get_creation_time() {
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + 0x29B0);
	}

	__forceinline int& m_nPredictedViewModelSequence() {
		return get< int >(g_Offsets.m_nPredictedViewModelSequence);
	}

	__forceinline float& m_flPlayerCycle() {
		return get< float >(g_Offsets.m_flPlayerCycle);
	}

	__forceinline int& m_nSequence() {
		return get< int >(g_Offsets.m_nSequence);
	}

	__forceinline float& m_flCycle() {
		return get< float >(g_Offsets.m_flCycle);
	}

	__forceinline float& m_flAnimTime() {
		return get< float >(g_Offsets.m_flAnimTime);
	}

	__forceinline float& m_flPlaybackRate() {
		return get< float >(g_Offsets.m_flPlaybackRate);
	}

	__forceinline float& m_flC4Blow() {
		return get< float >(g_Offsets.m_flC4Blow);
	}

	__forceinline bool& m_bBombTicking() {
		return get< bool >(g_Offsets.m_bBombTicking);
	}

	__forceinline int& m_nForceBone() {
		return get< int >(g_Offsets.m_nForceBone);
	}

	__forceinline float& m_flVelocityModifier() {
		return get< float >(g_Offsets.m_flVelocityModifier);
	}

	__forceinline int& m_iAccumulatedBoneMask() {
		return get< int >(g_Offsets.m_iAccumulatedBoneMask);
	}

	__forceinline int& m_iPrevBoneMask() {
		return get< int >(g_Offsets.m_iPrevBoneMask);
	}

	__forceinline int& m_nLastNonSkippedFrame() {
		return get< int >(g_Offsets.m_nLastNonSkippedFrame);
	}

	__forceinline int& m_nComputedLODframe() {
		return get< int >(g_Offsets.m_nComputedLODframe);
	}

	__forceinline uintptr_t& m_nAnimLODflags() {
		return get< uintptr_t >(g_Offsets.m_nAnimLODflags);
	}

	__forceinline uintptr_t& m_nAnimLODflagsOld() {
		return get< uintptr_t >(g_Offsets.m_nAnimLODflagsOld);
	}

	__forceinline int& m_EntClientFlags() {
		return get< int >(g_Offsets.m_EntClientFlags);
	}

	__forceinline bool& m_bIsToolRecording() {
		return get< bool >(g_Offsets.m_bIsToolRecording);
	}

	__forceinline CIKContext*& m_pIk() {
		return get< CIKContext* >(g_Offsets.m_pIk);
	}

	__forceinline int& m_nCustomBlendingRuleMask() {
		return get< int >(g_Offsets.m_nCustomBlendingRuleMask);
	}

	__forceinline int& m_fEffects() {
		return get< int >(g_Offsets.m_fEffects);
	}

	__forceinline int& m_nModelIndex() {
		return get< int >(g_Offsets.m_nModelIndex);
	}

	__forceinline bool& m_bReadyToDraw() {
		return get< bool >(g_Offsets.m_bReadyToDraw);
	}

public:
	// virtual indices
	enum indices : size_t {
		SETUPBONES = 13,
		WORLDSPACECENTER = 78,
		SIMULATE = 97,
		GETMAXHEALTH = 122,
		ESTIMATEABSVELOCITY = 141,
		PHYSICSSIMULATE = 149,
		ISAlIVE = 150,
		ISPLAYER = 152,
		ISBASECOMBATWEAPON = 160,
		ISFOLLOWINGENTITY = 172,
		SHOULDINTERPOLATE = 173,
	};

public:
	// normal table.
	__forceinline const Vector& GetAbsOrigin() {
		return util::get_method< const Vector& (__thiscall*)(void*) >(this, 10)(this);
	}

	__forceinline const QAngle& GetAbsAngles() {
		return util::get_method< const QAngle& (__thiscall*)(void*) >(this, 11)(this);
	}

	__forceinline bool IsPlayer() {
		return util::get_method< bool(__thiscall*)(void*) >(this, ISPLAYER)(this);
	}

	__forceinline bool IsBaseCombatWeapon() {
		return util::get_method< bool(__thiscall*)(void*) >(this, ISBASECOMBATWEAPON)(this);
	}

	__forceinline std::string GetBombsiteName() {
		std::string out;

		// note - dex; bomb_target + 0x150 has a char array for site name... not sure how much memory gets allocated for it.
		out.resize(32u);

		std::memcpy(&out[0], (const void*)((uintptr_t)this + 0x150), 32u);

		return out;
	}

	__forceinline void InvalidatePhysicsRecursive(int bits) {
		using InvalidatePhysicsRecursive_t = void(__thiscall*)(decltype(this), int);
		g_Csgo.InvalidatePhysicsRecursive.as< InvalidatePhysicsRecursive_t >()(this, bits);
	}

	__forceinline void SetAbsAngles(CStudioHdr* studio) {
		using AttachmentHelper_t = void(__thiscall*)(decltype(this), CStudioHdr*);
		g_Csgo.AttachmentHelper.as< AttachmentHelper_t >()(this, studio);
	}

	__forceinline void SetAbsAngles(const QAngle& angles) {
		using SetAbsAngles_t = void(__thiscall*)(decltype(this), const QAngle&);
		g_Csgo.SetAbsAngles.as< SetAbsAngles_t >()(this, angles);
	}

	__forceinline void SetAbsOrigin(const Vector& origin) {
		using SetAbsOrigin_t = void(__thiscall*)(decltype(this), const Vector&);
		g_Csgo.SetAbsOrigin.as< SetAbsOrigin_t >()(this, origin);
	}

	__forceinline void CalcAbsoluteVelocity() {
		using CalcAbsoluteVelocity_t = void(__thiscall*)(decltype(this));
		g_Csgo.CalcAbsoluteVelocity.as< CalcAbsoluteVelocity_t >()(this);
	}

	__forceinline void SetAbsVelocity(const Vector& velocity) {
		using SetAbsVelocity_t = void(__thiscall*)(decltype(this), const Vector&);
		g_Csgo.SetAbsVelocity.as< SetAbsVelocity_t >()(this, velocity);
	}

	__forceinline void SetEffect(int effects) {
		m_fEffects() = effects;
	}

	__forceinline void AddEffect(int effects) {
		m_fEffects() |= effects;
	}

	__forceinline void RemoveEffect(int effects) {
		m_fEffects() &= ~effects;
	}

	__forceinline void SetClientEffect(int effects) {
		m_EntClientFlags() = effects;
	}

	__forceinline void AddClientEffect(int effects) {
		m_EntClientFlags() |= effects;
	}

	__forceinline void RemoveClientEffect(int effects) {
		m_EntClientFlags() &= ~effects;
	}

	__forceinline int GetClassID() {
		ClientClass* cc{ GetClientNetworkable()->GetClientClass() };
		if (!cc)
			return -1;

		return cc->m_ClassID;
	}

	__forceinline bool is(hash_t hash) {
		return g_Netvars.GetClientID(hash) == GetClassID();
	}
};

class CBaseViewModel
{
public:
	enum indices : size_t {
		INTERPOLATE = 112,
	};
public:
	void UpdateAllViewmodelAddons();
};

enum animtag_indices
{
	ANIMTAG_INVALID = -1,
	ANIMTAG_UNINITIALIZED = 0,
	ANIMTAG_STARTCYCLE_N,
	ANIMTAG_STARTCYCLE_NE,
	ANIMTAG_STARTCYCLE_E,
	ANIMTAG_STARTCYCLE_SE,
	ANIMTAG_STARTCYCLE_S,
	ANIMTAG_STARTCYCLE_SW,
	ANIMTAG_STARTCYCLE_W,
	ANIMTAG_STARTCYCLE_NW,
	ANIMTAG_AIMLIMIT_YAWMIN_IDLE,
	ANIMTAG_AIMLIMIT_YAWMAX_IDLE,
	ANIMTAG_AIMLIMIT_YAWMIN_WALK,
	ANIMTAG_AIMLIMIT_YAWMAX_WALK,
	ANIMTAG_AIMLIMIT_YAWMIN_RUN,
	ANIMTAG_AIMLIMIT_YAWMAX_RUN,
	ANIMTAG_AIMLIMIT_YAWMIN_CROUCHIDLE,
	ANIMTAG_AIMLIMIT_YAWMAX_CROUCHIDLE,
	ANIMTAG_AIMLIMIT_YAWMIN_CROUCHWALK,
	ANIMTAG_AIMLIMIT_YAWMAX_CROUCHWALK,
	ANIMTAG_AIMLIMIT_PITCHMIN_IDLE,
	ANIMTAG_AIMLIMIT_PITCHMAX_IDLE,
	ANIMTAG_AIMLIMIT_PITCHMIN_WALKRUN,
	ANIMTAG_AIMLIMIT_PITCHMAX_WALKRUN,
	ANIMTAG_AIMLIMIT_PITCHMIN_CROUCH,
	ANIMTAG_AIMLIMIT_PITCHMAX_CROUCH,
	ANIMTAG_AIMLIMIT_PITCHMIN_CROUCHWALK,
	ANIMTAG_AIMLIMIT_PITCHMAX_CROUCHWALK,
	ANIMTAG_WEAPON_POSTLAYER,
	ANIMTAG_FLASHBANG_PASSABLE,
	ANIMTAG_COUNT
};

enum animstate_layer_t
{
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT,
};

enum animstate_pose_param_idx_t
{
	PLAYER_POSE_PARAM_FIRST = 0,
	PLAYER_POSE_PARAM_LEAN_YAW = PLAYER_POSE_PARAM_FIRST,
	PLAYER_POSE_PARAM_SPEED,
	PLAYER_POSE_PARAM_LADDER_SPEED,
	PLAYER_POSE_PARAM_LADDER_YAW,
	PLAYER_POSE_PARAM_MOVE_YAW,
	PLAYER_POSE_PARAM_RUN,
	PLAYER_POSE_PARAM_BODY_YAW,
	PLAYER_POSE_PARAM_BODY_PITCH,
	PLAYER_POSE_PARAM_DEATH_YAW,
	PLAYER_POSE_PARAM_STAND,
	PLAYER_POSE_PARAM_JUMP_FALL,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_IDLE,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_IDLE,
	PLAYER_POSE_PARAM_STRAFE_DIR,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_RUN,
	PLAYER_POSE_PARAM_MOVE_BLEND_CROUCH_WALK,
	PLAYER_POSE_PARAM_COUNT,
};

class IInterpolatedVar
{
public:
	virtual		 ~IInterpolatedVar() {}

	virtual void Setup(void* pValue, int type) = 0;
	virtual void SetInterpolationAmount(float seconds) = 0;

	virtual void NoteLastNetworkedValue() = 0;
	virtual bool NoteChanged(float flCurrentTime, float flChangeTime, bool bUpdateLastNetworkedValue) = 0;
	virtual void Reset(float flCurrentTime) = 0;

	virtual int Interpolate(float currentTime) = 0;

	virtual int	 GetType() const = 0;
	virtual void RestoreToLastNetworked() = 0;
	virtual void Copy(IInterpolatedVar* pSrc) = 0;

	virtual const char* GetDebugName() = 0;
	virtual void SetDebugName(const char* pName) = 0;
};

class VarMapEntry_t
{
public:
	unsigned short		m_Type;
	unsigned short		m_NeedsToInterpolate;	// Set to false when this var doesn't
												// need Interpolate() called on it anymore.
	void* m_Data;
	IInterpolatedVar* m_Watcher;
};

struct VarMapping_t
{
	CUtlVector<VarMapEntry_t> m_Entries;
	int m_InterpolatedEntries;
	float m_LastInterpolationTime;
};

#define ANIM_LAYER_ACTIVE		0x1

class C_AnimationLayer {
public:
	void Reset();

#ifdef CLIENT_DLL
	void	SetOwner(Player* pOverlay);
	Player* GetOwner() const;
#endif

	void	SetOrder(int order);
	bool	IsActive(void);
	float	 GetFadeout(float flCurTime);

	void SetSequence(int nSequence);
	void SetCycle(float flCycle);
	void SetWeight(float flWeight);

	FORCEINLINE void SetPrevCycle(float flCycle)
	{
		flPrevCycle = flCycle;
	}

	FORCEINLINE void SetPlaybackRate(float flRate)
	{
		flPlaybackRate = flRate;
	}

	FORCEINLINE void SetWeightDeltaRate(float flDelta)
	{
		flWeightDeltaRate = flDelta;
	}

	FORCEINLINE int	GetSequence() const
	{
		return nSequence;
	}

	FORCEINLINE float GetCycle() const
	{
		return flCycle;
	}

	FORCEINLINE float GetPrevCycle() const
	{
		return flPrevCycle;
	}

	FORCEINLINE float GetPlaybackRate() const
	{
		return flPlaybackRate;
	}

	FORCEINLINE float GetWeight() const
	{
		return flWeight;
	}

	FORCEINLINE float GetWeightDeltaRate() const
	{
		return flWeightDeltaRate;
	}

	FORCEINLINE int GetOrder() const
	{
		return nOrder;
	}

#ifdef CLIENT_DLL
	// If the weights, cycle or sequence #s changed due to interpolation then 
	//  we'll need to recompute the bbox
	int GetInvalidatePhysicsBits() const;
	void SetInvalidatePhysicsBits(int iBit) { nInvalidatePhysicsBits = iBit; }
#endif

public:
	float		flLayerAnimtime;
	float		flLayerFadeOuttime;

	// dispatch flags.
	CStudioHdr* pDispatchedStudioHdr;
	int			nDispatchedSrc;
	int			nDispatchedDst;

private:
	int			nOrder;
	int			nSequence;
	float		flPrevCycle;
	float		flWeight;
	float		flWeightDeltaRate;

	// used for automatic crossfades between sequence changes.
	float		flPlaybackRate;
	float		flCycle;
public:
	Player*		pOwner;
	int			nInvalidatePhysicsBits;

	//public:
		//int			fFlags;
}; // size: 0x0038

#define USE_ANIMLAYER_RAW_INDEX false
typedef const int* animlayerpreset;
#define get_animlayerpreset( _n ) s_animLayerOrder ## _n
#define REGISTER_ANIMLAYER_ORDER( _n ) static const int s_animLayerOrder ## _n [ANIMATION_LAYER_COUNT]

REGISTER_ANIMLAYER_ORDER(Default) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

// animations can trigger the player to re-order their layers according to hardcoded presets, e.g.:
REGISTER_ANIMLAYER_ORDER(WeaponPost) = {
	ANIMATION_LAYER_AIMMATRIX,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
};

#define ANIM_TRANSITION_WALK_TO_RUN 0
#define ANIM_TRANSITION_RUN_TO_WALK 1

#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX 58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN -58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX 90.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN -90.0f

#define CURRENT_ANIMSTATE_VERSION 2

#define CSGO_ANIM_BOMBPLANT_ABORT_SPEED 12.0f
#define CSGO_ANIM_DEFUSE_ABORT_SPEED 8.0f
#define CSGO_ANIM_TWITCH_ABORT_SPEED 6.0f
#define CSGO_ANIM_BOMBPLANT_BLEND_RATE 1.2f

// CS Team IDs.
#define TEAM_TERRORIST			2
#define	TEAM_CT					3
#define TEAM_MAXCOUNT			4	// update this if we ever add teams (unlikely)

struct animstate_pose_param_cache_t
{
	bool		m_bInitialized;
	int			m_nIndex;
	const char* m_szName;

	animstate_pose_param_cache_t()
	{
		m_bInitialized = false;
		m_nIndex = -1;
		m_szName = "";
	}

	int		GetIndex(void);
	float	GetValue(Player* pPlayer);
	void	SetValue(Player* pPlayer, float flValue);
	bool	Init(Player* pPlayer, const char* szPoseParamName);
};

struct procedural_foot_t
{
	Vector vecPosAnim;
	Vector vecPosAnimLast;
	Vector vecPosPlant;
	Vector vecPlantVel;
	float flLockAmount;
	float flLastPlantTime;

	procedural_foot_t()
	{
		vecPosAnim.Init();
		vecPosAnimLast.Init();
		vecPosPlant.Init();
		vecPlantVel.Init();
		flLockAmount = 0;
		flLastPlantTime = 0;
	}

	void Init(Vector vecNew)
	{
		vecPosAnim = vecNew;
		vecPosAnimLast = vecNew;
		vecPosPlant = vecNew;
		vecPlantVel.Init();
		flLockAmount = 0;
		flLastPlantTime = 0;
	}
};

struct aimmatrix_transition_t
{
	float	flDurationStateHasBeenValid;
	float	flDurationStateHasBeenInValid;
	float	flHowLongToWaitUntilTransitionCanBlendIn;
	float	flHowLongToWaitUntilTransitionCanBlendOut;
	float	flBlendValue;

	void UpdateTransitionState(bool bStateShouldBeValid, float flTimeInterval, float flSpeed)
	{
		if (bStateShouldBeValid)
		{
			flDurationStateHasBeenInValid = 0;
			flDurationStateHasBeenValid += flTimeInterval;
			if (flDurationStateHasBeenValid >= flHowLongToWaitUntilTransitionCanBlendIn)
			{
				flBlendValue = Math::Approach(1, flBlendValue, flSpeed);
			}
		}
		else
		{
			flDurationStateHasBeenValid = 0;
			flDurationStateHasBeenInValid += flTimeInterval;
			if (flDurationStateHasBeenInValid >= flHowLongToWaitUntilTransitionCanBlendOut)
			{
				flBlendValue = Math::Approach(0, flBlendValue, flSpeed);
			}
		}
	}

	void Init(void)
	{
		flDurationStateHasBeenValid = 0;
		flDurationStateHasBeenInValid = 0;
		flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
		flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
		flBlendValue = 0;
	}

	aimmatrix_transition_t()
	{
		Init();
	}
};

enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY = 0,
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY_OPT, // an optional primary attack for variation in animation. For example, the knife toggles between left AND right slash animations.
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY_SPECIAL1,
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY_OPT_SPECIAL1, // an optional primary special attack for variation in animation.
	PLAYERANIMEVENT_FIRE_GUN_SECONDARY,
	PLAYERANIMEVENT_FIRE_GUN_SECONDARY_SPECIAL1,
	PLAYERANIMEVENT_GRENADE_PULL_PIN,
	PLAYERANIMEVENT_THROW_GRENADE,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_RELOAD,
	PLAYERANIMEVENT_RELOAD_START,	///< w_model partial reload for shotguns
	PLAYERANIMEVENT_RELOAD_LOOP,	///< w_model partial reload for shotguns
	PLAYERANIMEVENT_RELOAD_END,		///< w_model partial reload for shotguns
	PLAYERANIMEVENT_CLEAR_FIRING,	///< clear animations on the firing layer
	PLAYERANIMEVENT_DEPLOY,			///< Used to play deploy animations on third person models.
	PLAYERANIMEVENT_SILENCER_ATTACH,
	PLAYERANIMEVENT_SILENCER_DETACH,

	// new events
	PLAYERANIMEVENT_THROW_GRENADE_UNDERHAND,
	PLAYERANIMEVENT_CATCH_WEAPON,
	PLAYERANIMEVENT_COUNT
};

enum
{
	PITCH = 0,
	YAW,
	ROLL
};

#define MAX_ANIMSTATE_ANIMNAME_CHARS 64

#define CSGO_ANIM_SPEED_TO_CHANGE_AIM_MATRIX 0.8f
#define CSGO_ANIM_SPEED_TO_CHANGE_AIM_MATRIX_SCOPED 4.2f

#define CSGO_ANIM_DEPLOY_RATELIMIT 0.15f

#define CSGO_ANIM_DUCK_APPROACH_SPEED_DOWN 3.1f
#define CSGO_ANIM_DUCK_APPROACH_SPEED_UP 6.0f

#define CSGO_ANIM_MAX_VEL_LIMIT 1.2f

#define DMG_BURN			(1 << 3)	// heat burned

const float CS_PLAYER_SPEED_RUN = 260.0f;
const float CS_PLAYER_SPEED_VIP = 227.0f;
const float CS_PLAYER_SPEED_SHIELD = 160.0f;
const float CS_PLAYER_SPEED_HAS_HOSTAGE = 200.0f;
const float CS_PLAYER_SPEED_STOPPED = 1.0f;
const float CS_PLAYER_SPEED_OBSERVER = 900.0f;

const float CS_PLAYER_SPEED_DUCK_MODIFIER = 0.34f;
const float CS_PLAYER_SPEED_WALK_MODIFIER = 0.52f;
const float CS_PLAYER_SPEED_CLIMB_MODIFIER = 0.34f;
const float CS_PLAYER_HEAVYARMOR_FLINCH_MODIFIER = 0.5f;

const float CS_PLAYER_DUCK_SPEED_IDEAL = 8.0f;

//#define CLIENT_DLL
#define CURRENT_ANIMSTATE_VERSION 2
#define CSGO_ANIM_LOWER_CATCHUP_IDLE	100.0f
#define CSGO_ANIM_AIM_NARROW_WALK	0.8f
#define CSGO_ANIM_AIM_NARROW_RUN	0.5f
#define CSGO_ANIM_AIM_NARROW_CROUCHMOVING	0.5f
#define CSGO_ANIM_LOWER_CATCHUP_WITHIN	3.0f
#define CSGO_ANIM_LOWER_REALIGN_DELAY	1.1f
#define CSGO_ANIM_READJUST_THRESHOLD	120.0f
#define EIGHT_WAY_WIDTH 22.5f

#define CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED 2.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH 8.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH_CROUCH 16.0f
#define CSGO_ANIM_LADDER_CLIMB_COVERAGE 100.0f
#define CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER 0.85f

#define CSGO_ANIM_LOWER_REALIGN_DELAY	1.1f
#define LATCH_ANIMATION_VAR  (1<<0)		// use AnimTime as sample basis
#define LATCH_SIMULATION_VAR (1<<1)		// use SimulationTime as sample basis

const char* const g_szWeaponPrefixLookupTable[] = {
	"knife",
	"pistol",
	"smg",
	"rifle",
	"shotgun",
	"sniper",
	"heavy",
	"c4",
	"grenade",
	"knife"
};

#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f

class Weapon;
class CCSGOPlayerAnimState {
public:
	bool CacheSequences();

	void Update(QAngle angAngle, bool bForce = true);
	void Reset();

	void SetUpAimMatrix();
	void SetUpVelocity();
	void SetUpWeaponAction();
	void SetUpLean();
	void SetUpMovement();
	void SetUpWholeBodyAction();
	void SetUpFlinch();
	void SetUpAliveloop();
	void SetUpFlashedReaction();
	void ModifyEyePosition(Vector& vecPosition);
	void SetSequence(int nSequence);
	void SetCycle(float flCycle);
	void SetPlaybackrate(float flPlaybackrate);

	const char* GetWeaponPrefix(void);

	int SelectSequenceFromActMods(Activity activity);
	int GetLayerSequence(animstate_layer_t nLayerIndex);
	Activity GetLayerActivity(animstate_layer_t nLayerIndex);
	float GetLayerWeight(animstate_layer_t nLayerIndex);
	float GetLayerCycle(animstate_layer_t nLayerIndex);
	float GetLayerIdealWeightFromSeqCycle(animstate_layer_t nLayerIndex);

	void SetLayerWeight(animstate_layer_t nLayerIndex, float flWeight);
	void SetLayerWeightRate(animstate_layer_t nLayerIndex, float flPrevious);
	void SetLayerCycle(animstate_layer_t nLayerIndex, float flCycle);
	void SetLayerSequence(animstate_layer_t nLayerIndex, int sequence);
	void DoAnimationEvent(PlayerAnimEvent_t animEvent);
	void SetLayerRate(animstate_layer_t nLayerIndex, float flRate);

	void AddActivityModifier(const char* name);
	void UpdateActivityModifiers(void);
	void ApplyLayerOrderPreset(animlayerpreset nNewPreset, bool bForce = false);
	void UpdateLayerOrderPreset(animstate_layer_t nLayerIndex, int nSequence);

	void IncrementLayerCycleWeightRateGeneric(animstate_layer_t nLayerIndex);
	void IncrementLayerWeight(animstate_layer_t nLayerIndex);
	void IncrementLayerCycle(animstate_layer_t nLayerIndex, bool is_looping);

	void UpdateAnimLayer(animstate_layer_t nLayerIndex, int nSequence, float flPlaybackRate, float flWeight, float flCycle);
	bool LayerSequenceHasActMod(animstate_layer_t nLayerIndex, const char* szActMod);
	bool IsLayerSequenceCompleted(animstate_layer_t nLayerIndex);
	bool LayerToIndex(const C_AnimationLayer* pLayer, int& nIndex);
public:
	animlayerpreset				pLayerOrderPreset;
	bool						bFirstRunSinceInit;
#ifdef CLIENT_CLASS_DLL
	bool						bFirstFootPlantSinceInit;
	int							iLastUpdateTick;
	float						flEyePositionSmoothLerp;
	float						flStrafeChangeWeightSmoothFallOff;
#endif
#ifndef CLIENT_CLASS_DLL
	float					    flFlashedAmountEaseOutStart;
	float					    flFlashedAmountEaseOutEnd;
#endif
	aimmatrix_transition_t	    tStandWalkAim;
	aimmatrix_transition_t	    tStandRunAim;
	aimmatrix_transition_t	    tCrouchWalkAim;

	int							iCachedModelIndex;
#ifdef CLIENT_CLASS_DLL
	float						flStepHeightLeft;
	float						flStepHeightRight;
	Weapon* pWeaponLastBoneSetup;
#endif

	Player* pPlayer;
	Weapon* pWeapon;
	Weapon* pWeaponLast;
	float						flLastUpdateTime;
	int							nLastUpdateFrame;
	float						flLastUpdateIncrement;
	float						flEyeYaw;
	float						flEyePitch;
	float						flFootYaw;
	float						flFootYawLast;
	float						flMoveYaw;
	float						flMoveYawIdeal;
	float						flMoveYawCurrentToIdeal;
	float						flTimeToAlignLowerBody;
	float						flPrimaryCycle;
	float						flMoveWeight;
	float						flMoveWeightSmoothed;
	float						flAnimDuckAmount;
	float						flDuckAdditional;
	float						flRecrouchWeight;
	Vector						vecPositionCurrent;
	Vector						vecPositionLast;
	Vector						vecVelocity;
	Vector						vecVelocityNormalized;
	Vector						vecVelocityNormalizedNonZero;
	float						flVelocityLengthXY;
	float						flVelocityLengthZ;
	float						flSpeedAsPortionOfRunTopSpeed;
	float						flSpeedAsPortionOfWalkTopSpeed;
	float						flSpeedAsPortionOfCrouchTopSpeed;
	float						flDurationMoving;
	float						flDurationStill;
	bool						bOnGround;
#ifndef CLIENT_CLASS_DLL
	bool					    bJumping;
	float					    flLowerBodyRealignTimer;
#endif
	bool						bLanding;
	float						flJumpToFall;
	float						flDurationInAir;
	float						flLeftGroundHeight;
	float						flLandAnimMultiplier;
	float						flWalkToRunTransition;
	bool						bLandedOnGroundThisFrame;
	bool						bLeftTheGroundThisFrame;
	float						flInAirSmoothValue;
	bool						bOnLadder;
	float						flLadderWeight;
	float						flLadderSpeed;
	bool						bWalkToRunTransitionState;
	bool						bDefuseStarted;
	bool						bPlantAnimStarted;
	bool						bTwitchAnimStarted;
	bool						bAdjustStarted;
	CUtlVector<CUtlSymbol> 		ActivityModifiers;
	float						flNextTwitchTime;
	float						flTimeOfLastKnownInjury;
	float						flLastVelocityTestTime;
	Vector						vecVelocityLast;
	Vector						vecTargetAcceleration;
	Vector						vecAcceleration;
	float						flAccelerationWeight;
	float						flAimMatrixTransition;
	float						flAimMatrixTransitionDelay;
	bool						bFlashed;
	float						flStrafeChangeWeight;
	float						flStrafeChangeTargetWeight;
	float						flStrafeChangeCycle;
	int							nStrafeSequence;
	bool						bStrafeChanging;
	float						flDurationStrafing;
	float						flFootLerp;
	bool						bFeetCrossed;
	bool						bPlayerIsAccelerating;
	animstate_pose_param_cache_t tPoseParamMappings[20];
#ifndef CLIENT_CLASS_DLL
	bool					    bDeployRateLimiting;
#endif
	float						flDurationMoveWeightIsTooHigh;
	float						flStaticApproachSpeed;
	int							nPreviousMoveState;
	float						flStutterStep;
	float						flActionWeightBiasRemainder;
#ifdef CLIENT_CLASS_DLL
	procedural_foot_t			footLeft;
	procedural_foot_t			footRight;
	float						flCameraSmoothHeight;
	bool						bSmoothHeightValid;
	float						flLastTimeVelocityOverTen;
#endif
	float						flAimYawMin;
	float						flAimYawMax;
	float						flAimPitchMin;
	float						flAimPitchMax;
	int							nAnimstateModelVersion;
#ifndef CLIENT_DLL
	float					    flFlashedAmountEaseOutStart;
	float					    flFlashedAmountEaseOutEnd;
#endif
#ifndef CLIENT_DLL
	bool					    bJumping;
	float					    flLowerBodyRealignTimer;
#endif
#ifndef CLIENT_DLL
	bool					    bDeployRateLimiting;
	bool					    nButtons;
#endif
}; // size: 0x344

class C_BaseAnimating;
class CBoneAccessor {
public:
	// Only used in the client DLL for debug verification.
	const C_BaseAnimating* m_pAnimating;

	matrix3x4a_t* m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
public:
	inline const matrix3x4a_t& GetBone(int iBone) const
	{
		return m_pBones[iBone];
	}

	inline int GetReadableBones()
	{
		return m_ReadableBones;
	}

	inline void SetReadableBones(int flags)
	{
		m_ReadableBones = flags;
	}

	inline int GetWritableBones()
	{
		return m_WritableBones;
	}

	inline matrix3x4a_t& GetBoneForWrite(int iBone)
	{
		return m_pBones[iBone];
	}

	inline void SetWritableBones(int flags)
	{
		m_WritableBones = flags;
	}
};

class CBoneCache {
public:
	matrix3x4a_t* m_pCachedBones;
	PAD(0x8);
	int        m_CachedBoneCount;
};

class Ragdoll : public Entity {
public:
	__forceinline Player* GetPlayer() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle< Player* >(m_hPlayer());
	}

	__forceinline EHANDLE& m_hPlayer() {
		return get< EHANDLE >(g_Offsets.m_hPlayer);
	}

	__forceinline float& m_flDeathYaw() {
		return get< float >(g_Offsets.m_flDeathYaw);
	}

	__forceinline float& m_flAbsYaw() {
		return get< float >(g_Offsets.m_flAbsYaw);
	}
};
// entity flags, CBaseEntity::m_iEFlags
enum
{
	EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

	EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

	EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	EFL_HAS_PLAYER_CHILD = (1 << 16),	// One of the child entities is a player.

	EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
											// This forces it to pass the "in PVS" for transmission.
											EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
											EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating

											// FIXME: Not really sure where I should add this...
											EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
											EFL_NO_ROTORWASH_PUSH = (1 << 21),// I shouldn't be pushed by the rotorwash
											EFL_NO_THINK_FUNCTION = (1 << 22),
											EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),

											EFL_CHECK_UNTOUCH = (1 << 24),
											EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
											EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
											EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
											EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
											EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
											EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
											EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
};

enum RelativeDamagedDirection_t
{
	DAMAGED_DIR_NONE = 0,
	DAMAGED_DIR_FRONT,
	DAMAGED_DIR_BACK,
	DAMAGED_DIR_LEFT,
	DAMAGED_DIR_RIGHT,

	DAMAGED_DIR_TOTAL
};

// Spectator Movement modes
enum
{
	OBS_MODE_NONE = 0,	// not in spectator mode
	OBS_MODE_DEATHCAM,	// special mode for death cam animation
	OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
	OBS_MODE_FIXED,		// view from a fixed camera position
	OBS_MODE_IN_EYE,	// follow a player in first person view
	OBS_MODE_CHASE,		// follow a player in third person view
	OBS_MODE_ROAMING,	// free roaming

	NUM_OBSERVER_MODES,
};

struct inferno_t : public Entity
{
	OFFSET(float, get_spawn_time, 0x20);

	static float get_expiry_time()
	{
		return 7.03125f;
	}
};

struct Smoke_t : public Entity
{
	OFFSET(float, m_GetSpawnTime, 0x20);

	__forceinline int& m_SmokeEffectTickBegin() {
		return get< int >(g_Offsets.m_nSmokeEffectTickBegin);
	}

	__forceinline bool& m_DidSmokeEffect() {
		return get< bool >(g_Offsets.m_bDidSmokeEffect);
	}

	static float GetExpiryTime()
	{
		return 17.43f;
	}
};

class IRefCounted {
private:
	volatile long refCount;

public:
	virtual void destructor(char bDelete) = 0;
	virtual bool OnFinalRelease() = 0;

	void unreference() {
		if (InterlockedDecrement(&refCount) == 0 && OnFinalRelease()) {
			destructor(1);
		}
	}
};

class WeaponInfo {
private:
	PAD(0x4);											// 0x0000

public:
	const char* m_WeaponName;						// 0x0004 -- actual weapon name, even for usp-s and revolver. ex: "weapon_revolver"
	PAD(0xC);												// 0x0008
	int               m_max_clip1;							// 0x0014
	int				  m_max_clip2;							// 0x0018
	int				  m_default_clip1;						// 0x001C
	int		          m_default_clip2;						// 0x0020
	int               m_max_reserve;						// 0x0024
	PAD(0x4);												// 0x0028
	const char* m_world_model;						// 0x002C
	const char* m_view_model;							// 0x0030
	const char* m_world_dropped_model;				// 0x0034
	PAD(0x48);											// 0x0038
	const char* m_ammo_type;							// 0x0080
	uint8_t           pad_0084[4];						// 0x0084
	const char* m_sfui_name;							// 0x0088
	const char* m_deprecated_weapon_name;				// 0x008C -- shitty weapon name, shows "weapon_deagle" for revolver / etc.
	uint8_t           pad_0090[56];						// 0x0090
	CSWeaponType      m_WeaponType;						// 0x00C8
	int			      m_in_game_price;						// 0x00CC
	int               m_kill_award;							// 0x00D0
	const char* m_animation_prefix;					// 0x00D4
	float			  m_cycletime;							// 0x00D8
	float			  m_cycletime_alt;						// 0x00DC
	float			  m_time_to_idle;						// 0x00E0
	float			  m_idle_interval;						// 0x00E4
	bool			  m_is_full_auto;						// 0x00E5
	PAD(0x3);												// 0x00E8
	int               m_Damage;								// 0x00EC
	float             m_ArmorRatio;						// 0x00F0
	int               m_bullets;							// 0x00F4
	float             m_Penetration;						// 0x00F8
	float             m_flinch_velocity_modifier_large;		// 0x00FC
	float             m_flinch_velocity_modifier_small;		// 0x0100
	float             m_WeaponRange;						// 0x0104
	float             m_RangeModifier;						// 0x0108
	float			  m_throw_velocity;						// 0x010C
	PAD(0xC);												// 0x0118
	bool			  m_has_silencer;						// 0x0119
	PAD(0x3);												// 0x011C
	const char* m_silencer_model;						// 0x0120
	int				  m_crosshair_min_distance;				// 0x0124
	int				  m_crosshair_delta_distance;			// 0x0128
	float             m_MaxPlayerSpeed;					// 0x012C
	float             m_MaxPlayerSpeedAlt;				// 0x0130
	float			  m_spread;								// 0x0134
	float			  m_spread_alt;							// 0x0138
	float             m_inaccuracy_crouch;					// 0x013C
	float             m_inaccuracy_crouch_alt;				// 0x0140
	float             m_inaccuracy_stand;					// 0x0144
	float             m_inaccuracy_stand_alt;				// 0x0148
	float             m_inaccuracy_jump_initial;			// 0x014C
	float             m_inaccuracy_jump;					// 0x0150
	float             m_inaccuracy_jump_alt;				// 0x0154
	float             m_inaccuracy_land;					// 0x0158
	float             m_inaccuracy_land_alt;				// 0x015C
	float             m_inaccuracy_ladder;					// 0x0160
	float             m_inaccuracy_ladder_alt;				// 0x0164
	float             m_inaccuracy_fire;					// 0x0168
	float             m_inaccuracy_fire_alt;				// 0x016C
	float             m_inaccuracy_move;					// 0x0170
	float             m_inaccuracy_move_alt;				// 0x0174
	float             m_inaccuracy_reload;					// 0x0178
	int               m_recoil_seed;						// 0x017C
	float			  m_recoil_angle;						// 0x0180
	float             m_recoil_angle_alt;					// 0x0184
	float             m_recoil_angle_variance;				// 0x0188
	float             m_recoil_angle_variance_alt;			// 0x018C
	float             m_recoil_magnitude;					// 0x0190
	float             m_recoil_magnitude_alt;				// 0x0194
	float             m_recoil_magnitude_variance;			// 0x0198
	float             m_recoil_magnitude_variance_alt;		// 0x019C
	float             m_recovery_time_crouch;				// 0x01A0
	float             m_recovery_time_stand;				// 0x01A4
	float             m_recovery_time_crouch_final;			// 0x01A8
	float             m_recovery_time_stand_final;			// 0x01AC
	float             m_recovery_transition_start_bullet;	// 0x01B0
	float             m_recovery_transition_end_bullet;		// 0x01B4
	bool			  m_unzoom_after_shot;					// 0x01B5
	PAD(0x3);												// 0x01B8
	bool		      m_hide_view_model_zoomed;				// 0x01B9
	bool			  m_zoom_levels;						// 0x01BA
	PAD(0x2);												// 0x01BC
	int				  m_zoom_fov[2];						// 0x01C4
	float			  m_zoom_time[3];						// 0x01D0
	PAD(0x8);												// 0x01D8
	float             m_addon_scale;						// 0x01DC
	PAD(0x8);												// 0x01E4
	int				  m_tracer_frequency;					// 0x01E8
	int				  m_tracer_frequency_alt;				// 0x01EC
	PAD(0x18);											// 0x0200
	int				  m_health_per_shot;					// 0x0204
	PAD(0x8);												// 0x020C
	float			  m_inaccuracy_pitch_shift;				// 0x0210
	float			  m_inaccuracy_alt_sound_threshold;		// 0x0214
	float			  m_bot_audible_range;					// 0x0218
	PAD(0x8);												// 0x0220
	const char* m_wrong_team_msg;						// 0x0224
	bool			  m_has_burst_mode;						// 0x0225
	PAD(0x3);												// 0x0228
	bool			  m_is_revolver;						// 0x0229
	bool			  m_can_shoot_underwater;				// 0x022A
	PAD(0x2);												// 0x022C			
};

class Weapon : public Entity {
public:
	using ref_vec_t = CUtlVector< IRefCounted* >;

	// netvars / etc.
	__forceinline ref_vec_t& m_CustomMaterials() {
		return get< ref_vec_t >(g_Offsets.m_CustomMaterials);
	}

	__forceinline ref_vec_t& m_CustomMaterials2() {
		return get< ref_vec_t >(g_Offsets.m_CustomMaterials2);
	}

	__forceinline ref_vec_t& m_VisualsDataProcessors() {
		return get< ref_vec_t >(g_Offsets.m_VisualsDataProcessors);
	}

	__forceinline bool& m_bCustomMaterialInitialized() {
		return get< bool >(g_Offsets.m_bCustomMaterialInitialized);
	}

	__forceinline int& m_iItemDefinitionIndex() {
		return get< int >(g_Offsets.m_iItemDefinitionIndex);
	}

	const char* GetIcon(int index)
	{
		if (!this || !index)
			return " ";

		switch (m_iItemDefinitionIndex())
		{
		case KNIFE_CT:
			return "j";
		case KNIFE_T:
			return "i";
		case KNIFE_BAYONET:
			return "1";
		case KNIFE_BOWIE:
			return "7";
		case KNIFE_BUTTERFLY:
			return "8";
		case KNIFE_FALCHION:
			return "0";
		case KNIFE_FLIP:
			return "2";
		case KNIFE_GUT:
			return "3";
		case KNIFE_KARAMBIT:
			return "4";
		case KNIFE_M9_BAYONET:
			return "5";
		case DEAGLE:
			return "A";
		case ELITE:
			return "B";
		case FIVESEVEN:
			return "C";
		case GLOCK:
			return "D";
		case P2000:
			return "E";
		case P250:
			return "F";
		case USPS:
			return "G";
		case TEC9:
			return "H";
		case REVOLVER:
			return "J";
		case MAC10:
			return "K";
		case UMP45:
			return "L";
		case BIZON:
			return "M";
		case MP7:
			return "N";
		case MP9:
			return "O";
		case P90:
			return "P";
		case GALIL:
			return "Q";
		case FAMAS:
			return "R";
		case M4A1S:
			return "T";
		case M4A4:
			return "S";
		case AUG:
			return "U";
		case SG553:
			return "V";
		case AK47:
			return "W";
		case G3SG1:
			return "X";
		case SCAR20:
			return "Y";
		case AWP:
			return "Z";
		case SSG08:
			return "a";
		case XM1014:
			return "b";
		case SAWEDOFF:
			return "c";
		case MAG7:
			return "d";
		case NOVA:
			return "e";
		case NEGEV:
			return "f";
		case M249:
			return "g";
		case ZEUS:
			return "h";
		case FLASHBANG:
			return "k";
		case HEGRENADE:
			return "l";
		case SMOKE:
			return "m";
		case MOLOTOV:
			return "n";
		case DECOY:
			return "o";
		case FIREBOMB:
			return "p";
		case C4:
			return "q";
		case CZ75A:
			return "I";
		default:
			return " ";
		}
	}

	__forceinline int& m_iClip1() {
		return get< int >(g_Offsets.m_iClip1);
	}

	__forceinline int& m_iPrimaryReserveAmmoCount() {
		return get< int >(g_Offsets.m_iPrimaryReserveAmmoCount);
	}

	__forceinline int& m_Activity() {
		return get< int >(g_Offsets.m_Activity);
	}

	__forceinline float& m_fFireDuration() {
		return get< float >(g_Offsets.m_fFireDuration);
	}

	__forceinline float& m_flPostponeFireReadyTime() {
		return get< float >(g_Offsets.m_flPostponeFireReadyTime);
	}

	__forceinline int& m_iBurstShotsRemaining() {
		return get< int >(g_Offsets.m_iBurstShotsRemaining);
	}

	__forceinline float& m_flNextPrimaryAttack() {
		return get< float >(g_Offsets.m_flNextPrimaryAttack);
	}

	__forceinline float& m_flNextSecondaryAttack() {
		return get< float >(g_Offsets.m_flNextSecondaryAttack);
	}

	__forceinline float& m_flThrowStrength() {
		return get< float >(g_Offsets.m_flThrowStrength);
	}

	__forceinline float& m_fNextBurstShot() {
		return get< float >(g_Offsets.m_fNextBurstShot);
	}

	__forceinline int& m_zoomLevel() {
		return get< int >(g_Offsets.m_zoomLevel);
	}

	__forceinline float& m_flRecoilIndex() {
		return get< float >(g_Offsets.m_flRecoilIndex);
	}

	__forceinline float& m_fAccuracyPenalty() {
		return get< float >(g_Offsets.m_fAccuracyPenalty);
	}

	__forceinline int& m_weaponMode() {
		return get< int >(g_Offsets.m_weaponMode);
	}

	__forceinline int& m_nFallbackPaintKit() {
		return get< int >(g_Offsets.m_nFallbackPaintKit);
	}

	__forceinline int& m_nFallbackStatTrak() {
		return get< int >(g_Offsets.m_nFallbackStatTrak);
	}

	__forceinline int& m_nFallbackSeed() {
		return get< int >(g_Offsets.m_nFallbackSeed);
	}

	__forceinline float& m_flFallbackWear() {
		return get< float >(g_Offsets.m_flFallbackWear);
	}

	__forceinline int& m_iViewModelIndex() {
		return get< int >(g_Offsets.m_iViewModelIndex);
	}

	__forceinline int& m_nViewModelIndex() {
		return get< int >(g_Offsets.m_nWeaponViewModelIndex);
	}

	__forceinline int& m_iWorldModelIndex() {
		return get< int >(g_Offsets.m_iWorldModelIndex);
	}

	__forceinline int& m_iAccountID() {
		return get< int >(g_Offsets.m_iAccountID);
	}

	__forceinline int& m_iItemIDHigh() {
		return get< int >(g_Offsets.m_iItemIDHigh);
	}

	__forceinline int& m_iEntityQuality() {
		return get< int >(g_Offsets.m_iEntityQuality);
	}

	__forceinline int& m_OriginalOwnerXuidLow() {
		return get< int >(g_Offsets.m_OriginalOwnerXuidLow);
	}

	__forceinline int& m_OriginalOwnerXuidHigh() {
		return get< int >(g_Offsets.m_OriginalOwnerXuidHigh);
	}

	__forceinline bool& m_bPinPulled() {
		return get< bool >(g_Offsets.m_bPinPulled);
	}

	__forceinline bool& m_bInReload() {
		return *(bool*)(this + 0x3245);
	}

	__forceinline float& m_fThrowTime() {
		return get< float >(g_Offsets.m_fThrowTime);
	}

	__forceinline EHANDLE& m_hWeapon() {
		return get< EHANDLE >(g_Offsets.m_hWeapon);
	}

	__forceinline EHANDLE& m_hWeaponWorldModel() {
		return get< EHANDLE >(g_Offsets.m_hWeaponWorldModel);
	}

	__forceinline float& m_flConstraintRadius() {
		return get< float >(g_Offsets.m_flConstraintRadius);
	}

	__forceinline float& m_fLastShotTime() {
		return get< float >(g_Offsets.m_fLastShotTime);
	}

public:
	enum indices : size_t {
		SETMODELINDEX = 75,
		GETMAXCLIP1 = 367,
		GETNAME = 378,
		GETMAXSPEED = 429,
		GETSPREAD = 439,
		GETWPNDATA = 446, // C_WeaponCSBaseGun::GetCSWpnData
		GETINACCURACY = 469,
		UPDATEACCURACYPENALTY = 471,
	};

public:
	// virtuals.
	__forceinline int GetMaxClip1() {
		return util::get_method< int(__thiscall*)(void*) >(this, GETMAXCLIP1)(this);
	}

	//const char* GetName() {
	//	return util::get_method< const char*(__thiscall*)(void*) >(this, GETNAME)(this);
	//}

	__forceinline void SetGloveModelIndex(int index) {
		return util::get_method< void(__thiscall*)(void*, int) >(this, SETMODELINDEX)(this, index);
	}

	__forceinline WeaponInfo* GetWpnData() {
		return util::get_method< WeaponInfo* (__thiscall*)(void*) >(this, GETWPNDATA)(this);
	}

	__forceinline float GetInaccuracy() {
		return util::get_method< float(__thiscall*)(void*) >(this, GETINACCURACY)(this);
	}

	__forceinline float GetMaxSpeed() {
		return util::get_method< float(__thiscall*)(void*) >(this, GETMAXSPEED)(this);
	}

	__forceinline float GetSpread() {
		return util::get_method< float(__thiscall*)(void*) >(this, GETSPREAD)(this);
	}

	__forceinline void UpdateAccuracyPenalty() {
		return util::get_method< void(__thiscall*)(void*) >(this, UPDATEACCURACYPENALTY)(this);
	}

	// misc funcs.
	__forceinline Weapon* GetWeapon() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle< Weapon* >(m_hWeapon());
	}

	__forceinline Weapon* GetWeaponWorldModel() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle< Weapon* >(m_hWeaponWorldModel());
	}

	__forceinline bool IsKnife() {
		return (GetWpnData()->m_WeaponType == WEAPONTYPE_KNIFE && m_iItemDefinitionIndex() != ZEUS);
	}

	__forceinline bool IsSniper()
	{
		return this->m_iItemDefinitionIndex() == AWP || this->m_iItemDefinitionIndex() == SCAR20 || this->m_iItemDefinitionIndex() == G3SG1 || this->m_iItemDefinitionIndex() == SSG08;
	}

	__forceinline bool IsAutoSniper()
	{
		return this->m_iItemDefinitionIndex() == SCAR20 || this->m_iItemDefinitionIndex() == G3SG1;
	}

	__forceinline bool IsGrenade() { return this->m_iItemDefinitionIndex() >= FLASHBANG && this->m_iItemDefinitionIndex() <= FIREBOMB; }

	__forceinline bool IsGun()
	{
		switch (this->GetWpnData()->m_WeaponType)
		{
		case WEAPONTYPE_C4:
			return false;
		case WEAPONTYPE_GRENADE:
			return false;
		case WEAPONTYPE_KNIFE:
			return false;
		case WEAPONTYPE_UNKNOWN:
			return false;
		default:
			return true;
		}
	}

	__forceinline Vector CalculateSpread(int seed, float inaccuracy, float spread, bool revolver2 = false) {
		WeaponInfo* wep_info;
		int        item_def_index;
		float      recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

		// if we have no bullets, we have no spread.
		wep_info = GetWpnData();
		if (!wep_info || !wep_info->m_bullets)
			return {};

		// get some data for later.
		item_def_index = m_iItemDefinitionIndex();
		recoil_index = m_flRecoilIndex();

		// seed randomseed.
		g_Csgo.RandomSeed((seed & 0xff) + 1);

		// generate needed floats.
		r1 = g_Csgo.RandomFloat(0.f, 1.f);
		r2 = g_Csgo.RandomFloat(0.f, Math::pi_2);

		if ( /*wep_info->m_WeaponType == WEAPONTYPE_SHOTGUN &&*/ g_Csgo.weapon_accuracy_shotgun_spread_patterns->GetInt() > 0)
			g_Csgo.GetShotgunSpread(item_def_index, 0, 0 /*bullet_i*/ + wep_info->m_bullets * recoil_index, &r4, &r3);

		else {
			r3 = g_Csgo.RandomFloat(0.f, 1.f);
			r4 = g_Csgo.RandomFloat(0.f, Math::pi_2);
		}

		// revolver secondary spread.
		if (item_def_index == REVOLVER && revolver2) {
			r1 = 1.f - (r1 * r1);
			r3 = 1.f - (r3 * r3);
		}

		// negev spread.
		else if (item_def_index == NEGEV && recoil_index < 3.f) {
			for (int i{ 3 }; i > recoil_index; --i) {
				r1 *= r1;
				r3 *= r3;
			}

			r1 = 1.f - r1;
			r3 = 1.f - r3;
		}

		// get needed sine / cosine values.
		c1 = std::cos(r2);
		c2 = std::cos(r4);
		s1 = std::sin(r2);
		s2 = std::sin(r4);

		// calculate spread vector.
		return {
			(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
			(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
			0.f
		};
	}

	__forceinline Vector CalculateSpread(int seed, bool revolver2 = false) {
		return CalculateSpread(seed, GetInaccuracy(), GetSpread(), revolver2);
	}

	__forceinline std::string GetLocalizedName() {
		C_EconItemView* item_view;
		CEconItemDefinition* item_def;

		item_view = g_Csgo.GetEconItemView(this);
		if (!item_view)
			return XOR("error");

		item_def = g_Csgo.GetStaticData(item_view);
		if (!item_def)
			return XOR("error");

		return util::WideToMultiByte(g_Csgo.m_Localize->Find(item_def->GetItemBaseName()));
	}

	const char* GetBulletType();
	const char* GetName();
};

struct clientHitVerify_t
{
	clientHitVerify_t()
	{
		vecPosition = Vector(0, 0, 0);
		flTime = 0;
		flExpires = -1;
	}

	clientHitVerify_t(Vector inPos, float flTimestamp, float flExpireTime)
	{
		vecPosition = inPos;
		flTimestamp = flTimestamp;
		flExpireTime = flExpireTime;
	}

	Vector vecPosition;
	float flTime;
	float flExpires;
};

inline uint32_t FindInDataMap(datamap_t* pDataMap, uint32_t iPropName)
{
	while (pDataMap)
	{
		for (int i = 0; i < pDataMap->m_size; i++)
		{
			if (!pDataMap->m_desc[i].m_Name)
				continue;

			if (RT_HASH(pDataMap->m_desc[i].m_Name) == iPropName)
				return pDataMap->m_desc[i].m_offset;

			if (pDataMap->m_desc[i].m_Type != FIELD_EMBEDDED)
				continue;

			if (!pDataMap->m_desc[i].m_td)
				continue;

			uint32_t iOffset = FindInDataMap(pDataMap->m_desc[i].m_td, iPropName);
			if (!iOffset)
				continue;

			return iOffset;
		}

		pDataMap = pDataMap->m_base;
	}

	return 0;
}

class C_CommandContext
{
public:
	bool m_bNeedsProcessing;
	Command m_Cmd;
	int m_nCommandNumber;
};

enum
{
	VPHYS_WALK = 0,
	VPHYS_CROUCH,
	VPHYS_NOCLIP,
};

#define DATAMAP( type, name ) \
__forceinline type& name( ) \
{\
	static uint32_t g_Datamap_##name = 0;\
    if ( !g_Datamap_##name )\
        g_Datamap_##name = _rotl( FindInDataMap( this->GetPredDescMap( ), RT_HASH( #name ) ) ^ ( RT_HASH( #name ) * 5 ), 32 );\
    return *( type* )( ( uintptr_t )( this ) + ( _rotr( g_Datamap_##name, 32 ) ^ ( RT_HASH( #name ) * 5 ) ) );\
}\

class Player : public Entity {
public:
	__forceinline datamap_t* GetPredDescMap() {
		return util::get_method<datamap_t*(__thiscall*)(void*)>(this, 17)(this);
	}

public:
	//DATAMAP(Vector, m_vecAbsVelocity);
	DATAMAP(Vector, m_vecBaseVelocity);
	DATAMAP(float_t, m_flStamina);

public:
	enum indices : size_t {
		// CBaseAnimating
		ONLATCHINTERPOLATEDVARIABLES = 107, // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
		EYEANGLES = 164, // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
		SETSEQUENCE = 213, // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
		STUDIOFRAMEADVANCE, // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
		FRAMEADVANCE,
		GETSEQUENCECYCLERATE,
		GETLAYERSEQUENCECYCLERATE,
		UPDATECLIENTSIDEANIMATION, // 218 // 55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36

		// CBaseEntity
		THINK = 137,

		// CBasePlayer
		PRETHINK = 307,
		RUNPOSTTHINK,
		SELECTITEM = 318,
		UPDATECOLLISIONBOUNDS = 329, // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
		SETLOCALVIEWANGLES = 361,

		// CCSPlayer.
		GETREFEHANDLE = 2,
		TESTHITBOXES = 52,
		//ACCUMULATELAYERS = 182,
		BUILDTRANSFORMATIONS = 184,
		CONTROLMOUTH = 191,
		DOEXTRABONEPROCESSING = 192,
		STANDARDBLENDINGRULES = 200,
		ACCUMULATELAYERS = 201,
		GETACTIVEWEAPON = 262,
		GETEYEPOS = 163,
		WEAPON_SHOOTPOSITION = 277,
		CALCVIEW = 270,
		CALCADDVIEWMODELCAMERAANIMATION = 274,
		SETPLAYERUNDERWATER = 275,
		GETFOV = 321,
		ITEMPREFRAME = 309,
		ITEMPOSTFRAME = 310,
	};

	CUtlVector<clientHitVerify_t>& m_vecBulletVerifyListClient() {
		return get< CUtlVector<clientHitVerify_t> >(0xBA90);
	}

	__forceinline Vector& m_vecAbsVelocity() {
		if (m_iEFlags() & EFL_DIRTY_ABSVELOCITY)
			CalcAbsoluteVelocity();

		return get< Vector >(g_Offsets.m_vecAbsVelocity);
	}

	__forceinline int& m_lifeState() {
		return get< int >(g_Offsets.m_lifeState);
	}

	__forceinline int& m_fFlags() {
		return get< int >(g_Offsets.m_fFlags);
	}

	__forceinline float& m_flFallVelocity() {
		return get< float >(g_Offsets.m_flFallVelocity);
	}

	__forceinline int& m_MoveType() {
		return get< int >(g_Offsets.m_MoveType);
	}

	__forceinline bool& m_bStrafing() {
		return get< bool >(g_Offsets.m_bStrafing);
	}

	__forceinline Vector& m_vecLadderNormal() {
		return get< Vector >(g_Offsets.m_vecLadderNormal);
	}

	__forceinline int& m_iMoveState() {
		return get< int >(g_Offsets.m_iMoveState);
	}

	__forceinline bool& m_bPlayerUnderwater() {
		return get< bool >(g_Offsets.m_bPlayerUnderwater);
	}

	__forceinline float& m_flThirdpersonRecoil() {
		return get< float >(g_Offsets.m_flThirdpersonRecoil);
	}

	__forceinline int& m_iAccount() {
		return get< int >(g_Offsets.m_iAccount);
	}

	__forceinline int& m_hViewModel() {
		return get< int >(g_Offsets.m_hViewModel);
	}

	__forceinline int& m_nViewModelIndex() {
		return get< int >(g_Offsets.m_nViewModelIndex);
	}

	__forceinline bool& m_bIsDefusing() {
		return get< bool >(g_Offsets.m_bIsDefusing);
	}

	__forceinline int& m_nExplodeEffectTickBegin() {
		return get< int >(g_Offsets.m_nExplodeEffectTickBegin);
	}

	__forceinline bool& m_bHasDefuser() {
		return get< bool >(g_Offsets.m_bHasDefuser);
	}

	__forceinline int& m_nHitboxSet() {
		return get< int >(g_Offsets.m_nHitboxSet);
	}

	__forceinline QAngle& m_angAbsRotation() {
		return get< QAngle >(g_Offsets.m_angAbsRotation);
	}

	__forceinline QAngle& m_angRotation() {
		return get< QAngle >(g_Offsets.m_angRotation);
	}

	__forceinline QAngle& m_angNetworkAngles() {
		return get< QAngle >(g_Offsets.m_angNetworkAngles);
	}

	__forceinline bool& m_bUseNewAnimstate() {
		return get< bool >(g_Offsets.m_bUseNewAnimstate);
	}

	__forceinline bool IsLocalPlayer() {
		// .text:101E0078 674     84 C0				   test    al, al          ; Logical Compare
		// .text:101E007A 674     74 17				   jz      short loc_101E0093; Jump if Zero( ZF = 1 )
		// .text:101E007C 674     8A 83 F8 35 00 00	   mov     al, [ ebx + 35F8h ]
		return get< bool >(g_Csgo.IsLocalPlayer);
	}

	__forceinline CCSGOPlayerAnimState* m_PlayerAnimStateCSGO() {
		return get< CCSGOPlayerAnimState* >(g_Csgo.PlayerAnimState); // 0x3874
	}

	__forceinline CCSGOPlayerAnimState* m_PlayerAnimState() {
		return get< CCSGOPlayerAnimState* >(g_Offsets.m_PlayerAnimState);
	}

	__forceinline CStudioHdr* m_studioHdr() {
		// .text:1017E902 08C    8B 86 3C 29 00 00    mov     eax, [ esi + 293Ch ]
		// .text:1017E908 08C    89 44 24 10          mov[ esp + 88h + var_78 ], eax
		return get< CStudioHdr* >(g_Csgo.studioHdr);
	}

	__forceinline ulong_t& m_iMostRecentModelBoneCounter() {
		// .text:101AC9A9 000    89 81 80 26 00 00    mov[ ecx + 2680h ], eax
		return get< ulong_t >(g_Csgo.MostRecentModelBoneCounter);
	}

	__forceinline float& m_flLastBoneSetupTime() {
		// .text:101AC99F 000    C7 81 14 29 00 00 FF FF+    mov     dword ptr [ecx+2914h], 0FF7FFFFFh;
		return get< float >(g_Csgo.LastBoneSetupTime);
	}

	__forceinline int& m_nTickBase() {
		return get< int >(g_Offsets.m_nTickBase);
	}

	__forceinline int& m_nFinalPredictedTick() {
		return get< int >(g_Offsets.m_nFinalPredictedTick);
	}

	__forceinline uintptr_t GetPlayerResource() {
		for (int i{ 1 }; i <= g_Csgo.m_EntityList->GetHighestEntityIndex(); ++i) {
			Entity* ent = g_Csgo.m_EntityList->GetClientEntity(i);
			if (!ent || !ent->is(HASH("CCSPlayerResource"))) 
				continue;

			return uintptr_t(ent);
		}

		return 0u;
	}

	__forceinline int& m_iPing(int index) {
		return *(int*)(GetPlayerResource() + g_Offsets.m_iPing + index * sizeof(int));
	}

	__forceinline int m_iPlayerC4(int index) {
		return index == *(int*)(GetPlayerResource() + g_Offsets.m_iPlayerC4);
	}

	__forceinline float& m_flNextAttack() {
		return get< float >(g_Offsets.m_flNextAttack);
	}

	__forceinline float& m_flDuckAmount() {
		return get< float >(g_Offsets.m_flDuckAmount);
	}

	__forceinline float& m_flDuckSpeed() {
		return get< float >(g_Offsets.m_flDuckSpeed);
	}

	__forceinline bool& m_bDucking() {
		return get< bool >(g_Offsets.m_bDucking);
	}

	__forceinline int& m_nSimulationTick() {
		return get< int >(g_Offsets.m_nSimulationTick);
	}

	//__forceinline CUtlVector< C_CommandContext >& m_CommandContext() {
	//	return get< CUtlVector< C_CommandContext > >(g_Offsets.m_CommandContext);
	//}

	__forceinline int& m_vphysicsCollisionState() {
		return get< int >(g_Offsets.m_vphysicsCollisionState);
	}


	__forceinline float& m_flSimulationTime() {
		return get< float >(g_Offsets.m_flSimulationTime);
	}

	__forceinline void SetSimulationTime(float st) {
		this->m_flSimulationTime() = st;
	}

	__forceinline float& m_flOldSimulationTime() {
		return get< float >(g_Offsets.m_flOldSimulationTime);
	}

	__forceinline float& m_flLowerBodyYawTarget() {
		return get< float >(g_Offsets.m_flLowerBodyYawTarget);
	}

	__forceinline int& m_iThrowGrenadeCounter() {
		return get< int >(g_Offsets.m_iThrowGrenadeCounter);
	}

	__forceinline float& m_fImmuneToGunGameDamageTime() {
		return get< float >(g_Offsets.m_fImmuneToGunGameDamageTime);
	}

	__forceinline bool& m_bHasHelmet() {
		return get< bool >(g_Offsets.m_bHasHelmet);
	}

	__forceinline bool& m_bClientSideAnimation() {
		return get< bool >(g_Offsets.m_bClientSideAnimation);
	}

	__forceinline bool& m_bHasHeavyArmor() {
		return get< bool >(g_Offsets.m_bHasHeavyArmor);
	}

	__forceinline bool& m_bIsScoped() {
		return get< bool >(g_Offsets.m_bIsScoped);
	}

	__forceinline bool& m_bIsWalking() {
		return get< bool >(g_Offsets.m_bIsWalking);
	}

	__forceinline bool& m_bSpotted() {
		return get< bool >(g_Offsets.m_bSpotted);
	}

	__forceinline int& m_iObserverMode() {
		return get< int >(g_Offsets.m_iObserverMode);
	}

	__forceinline int& m_ArmorValue() {
		return get< int >(g_Offsets.m_ArmorValue);
	}

	__forceinline float& m_flMaxspeed() {
		return get< float >(g_Offsets.m_flMaxspeed);
	}

	__forceinline float& m_surfaceFriction() {
		return get< float >(g_Offsets.m_surfaceFriction);
	}

	__forceinline float& m_flFlashBangTime() {
		return get< float >(g_Offsets.m_flFlashBangTime);
	}

	__forceinline float& m_flFlashDuration() {
		return get< float >(g_Offsets.m_flFlashDuration);
	}

	__forceinline float& m_flTimeOfLastInjury() {
		return get< float >(g_Offsets.m_flTimeOfLastInjury);
	}

	__forceinline RelativeDamagedDirection_t m_nRelativeDirectionOfLastInjury() {
		return *(RelativeDamagedDirection_t*)((DWORD)this + g_Offsets.m_nRelativeDirectionOfLastInjury);
	}

	__forceinline int& m_LastHitGroup() {
		return get< int >(g_Offsets.m_LastHitGroup);
	}

	__forceinline int& m_fireCount() {
		return get< int >(g_Offsets.m_fireCount);
	}

	__forceinline QAngle& m_angEyeAngles() {
		return get< QAngle >(g_Offsets.m_angEyeAngles);
	}

	__forceinline QAngle& m_aimPunchAngle() {
		return get< QAngle >(g_Offsets.m_aimPunchAngle);
	}

	__forceinline QAngle& m_viewPunchAngle() {
		return get< QAngle >(g_Offsets.m_viewPunchAngle);
	}

	__forceinline Vector& m_aimPunchAngleVel() {
		return get< Vector >(g_Offsets.m_aimPunchAngleVel);
	}

	__forceinline Vector& m_vecViewOffset() {
		return get< Vector >(g_Offsets.m_vecViewOffset);
	}

	__forceinline Command& m_LastCmd() {
		return get< Command >(g_Offsets.m_LastCommand);
	}

	__forceinline Command*& m_pCurrentCommand() {
		return get< Command* >(g_Offsets.m_pCurrentCommand);
	}

	__forceinline int& m_iEFlags() {
		return get< int >(g_Offsets.m_iEFlags);
	}

	__forceinline float* m_flPoseParameter() {
		return (float*)((uintptr_t)this + g_Offsets.m_flPoseParameter);
	}

	__forceinline CBaseHandle* m_hMyWearables() {
		return (CBaseHandle*)((uintptr_t)this + g_Offsets.m_hMyWearables);
	}

	__forceinline CBoneCache& m_BoneCache() {
		// TODO; sig
		return get< CBoneCache >(g_Offsets.m_BoneCache);
	}

	__forceinline matrix3x4_t**& m_iBoneCache() {
		// TODO; sig
		return get< matrix3x4_t** >(g_Offsets.m_BoneCache);
	}

	__forceinline EHANDLE& m_hObserverTarget() {
		return get< EHANDLE >(g_Offsets.m_hObserverTarget);
	}

	__forceinline EHANDLE& m_hActiveWeapon() {
		return get< EHANDLE >(g_Offsets.m_hActiveWeapon);
	}

	__forceinline EHANDLE& m_hGroundEntity() {
		return get< EHANDLE >(g_Offsets.m_hGroundEntity);
	}

	__forceinline EHANDLE& m_hVehicle() {
		return get< EHANDLE >(g_Offsets.m_hVehicle);
	}

	//__forceinline CBaseHandle *m_hMyWeapons() {
	//	return get< CBaseHandle* >(g_Offsets.m_hMyWeapons);
	//}

	__forceinline int& m_nNextThinkTick() {
		return get< int >(g_Offsets.m_nNextThinkTick);
	}

	__forceinline Vector& m_vecAimDirection() {
		return get< Vector >(0x37D8);
	}

	__forceinline void SetAimDirection(Vector& dir) {
		m_vecAimDirection() = dir;
	}

	__forceinline int& m_nImpulse() {
		return get< int >(0x31EC); // 0x31EC // FindInDatamap(DescMap(), "m_nImpulse")
	}

	__forceinline int& m_afButtonLast() {
		return get< int >(0x31DC); // 0x31DC // FindInDatamap(DescMap(), "m_afButtonLast")
	}

	__forceinline int& m_nButtons() {
		return get< int >(0x31E8); // 0x31E8 // FindInDatamap(DescMap(), "m_nButtons")
	}

	__forceinline int& m_afButtonPressed() {
		return get< int >(0x31E0); // 0x31E0 // FindInDatamap(DescMap(), "m_afButtonPressed")
	}

	__forceinline int& m_afButtonReleased() {
		return get< int >(0x31E4); // 0x31E4 // FindInDatamap(DescMap(), "m_afButtonReleased")
	}

	__forceinline int& m_afButtonForced() {
		return get< int >(g_Offsets.m_afButtonForced);
	}

	__forceinline int& m_afButtonDisabled() {
		return get< int >(g_Offsets.m_afButtonDisabled);
	}

	__forceinline bool& m_bInBuyZone() {
		return get< bool >(g_Offsets.m_bInBuyZone);
	}

	__forceinline VarMapping_t& m_VarMap() {
		return get< VarMapping_t >(g_Offsets.m_VarMap);
	}

	__forceinline C_AnimationLayer* m_AnimOverlay() {
		// .text:1017EAB1 08C    8B 47 1C                mov     eax, [edi+1Ch]
		// .text:1017EAB4 08C    8D 0C D5 00 00 00 00    lea     ecx, ds:0[ edx * 8 ]; Load Effective Address
		// .text:1017EABB 08C    2B CA                   sub     ecx, edx; Integer Subtraction
		// .text:1017EABD 08C    8B 80 70 29 00 00       mov     eax, [ eax + 2970h ]
		// .text:1017EAC3 08C    8D 34 C8                lea     esi, [ eax + ecx * 8 ]; Load Effective Address
		// .text:1017EAC6
		return get< C_AnimationLayer* >(g_Csgo.AnimOverlay);
	}

	//__forceinline bool& m_bUseCustomAutoExposureMin() {
	//	return get< bool >(g_Offsets.m_bUseCustomAutoExposureMin);
	//}

	//__forceinline bool& m_bUseCustomAutoExposureMax() {
	//	return get< bool >(g_Offsets.m_bUseCustomAutoExposureMax);
	//}

	//__forceinline bool& m_bUseCustomBloomScale() {
	//	return get< bool >(g_Offsets.m_bUseCustomBloomScale);
	//}

	//__forceinline float& m_flCustomAutoExposureMin() {
	//	return get< float >(g_Offsets.m_flCustomAutoExposureMin);
	//}

	//__forceinline float& m_flCustomAutoExposureMax() {
	//	return get< float >(g_Offsets.m_flCustomAutoExposureMax);
	//}

	//__forceinline float& m_flCustomBloomScale() {
	//	return get< float >(g_Offsets.m_flCustomBloomScale);
	//}

	__forceinline float& m_flSpawnTime() {
		// .text:10381AB3 00C    F3 0F 10 49 10             movss   xmm1, dword ptr [ecx+10h] ; Move Scalar Single-FP
		// .text:10381AB8 00C    F3 0F 5C 88 90 A2 00 00    subss   xmm1, dword ptr[ eax + 0A290h ]; Scalar Single - FP Subtract
		return get< float >(g_Csgo.SpawnTime);
	}

	__forceinline CBoneAccessor& m_BoneAccessor() {
		// .text:101A9253 1C4    C7 81 A0 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 26A0h ], 0FFF00h
		// .text:101A925D 1C4    C7 81 9C 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 269Ch ], 0FFF00h
		// .text:101A9267 1C4    8B 10                            mov     edx, [ eax ]
		// .text:101A9269 1C4    8D 81 94 26 00 00                lea     eax, [ ecx + 2694h ]; Load Effective Address
		// .text:101A926F 1C4    50                               push    eax
		return get< CBoneAccessor >(g_Csgo.BoneAccessor);
	}
public:
	void SetBodyPitch(float flPitch)
	{
		int m_nBodyPitchPoseParam = LookupPoseParameter("body_pitch");
		if (m_nBodyPitchPoseParam >= 0)
		{
			SetPoseParameter(m_nBodyPitchPoseParam, flPitch);
		}
	}

	__forceinline void SetButtonState(int nUserCmdButtonMask) {
		m_afButtonLast() = m_nButtons();
		m_nButtons() = nUserCmdButtonMask;

		int buttonsChanged = m_afButtonLast() ^ m_nButtons();

		m_afButtonPressed() = buttonsChanged & m_nButtons();
		m_afButtonReleased() = buttonsChanged & (~m_nButtons());
	}

	__forceinline void CheckHasThinkFunction(bool isThinking) {
//		if (IsEFlagSet(EFL_NO_THINK_FUNCTION) && isThinking)
//		{
//			RemoveEFlags(EFL_NO_THINK_FUNCTION);
//		}
//		else if (!isThinking && !IsEFlagSet(EFL_NO_THINK_FUNCTION) && !WillThink())
//		{
//			AddEFlags(EFL_NO_THINK_FUNCTION);
//		}
//#if !defined( CLIENT_DLL )
//		SimThink_EntityChanged(this);
//#endif

		static auto CheckHasThinkFunction = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 56 57 8B F9 8B B7 E4 00 00 00 8B C6 C1 E8 16")).as<void(__thiscall*)(void*, int)>();
		return CheckHasThinkFunction(this, isThinking);
	}

	__forceinline ulong_t GetRefEHandle() {
		using GetRefEHandle_t = ulong_t(__thiscall*)(decltype(this));
		return util::get_method< GetRefEHandle_t >(this, GETREFEHANDLE)(this);
	}

	__forceinline void BuildTransformations(CStudioHdr* hdr, Vector* pos, QuaternionAligned* q, const matrix3x4_t& transform, int mask, CBoneBitList& computed) {
		using BuildTransformations_t = void(__thiscall*)(decltype(this), CStudioHdr*, Vector*, QuaternionAligned*, matrix3x4_t const&, int, CBoneBitList&);
		return util::get_method< BuildTransformations_t >(this, BUILDTRANSFORMATIONS)(this, hdr, pos, q, transform, mask, computed);
	}

	__forceinline void ControlMouth(CStudioHdr* pstudiohdr) {
		using ControlMouth_t = void(__thiscall*)(decltype(this), CStudioHdr*);
		return util::get_method< ControlMouth_t >(this, CONTROLMOUTH)(this, pstudiohdr);
	}

	__forceinline void StandardBlendingRules(CStudioHdr* hdr, Vector* pos, QuaternionAligned* q, float time, int mask) {
		using StandardBlendingRules_t = void(__thiscall*)(decltype(this), CStudioHdr*, Vector*, QuaternionAligned*, float, int);
		return util::get_method< StandardBlendingRules_t >(this, STANDARDBLENDINGRULES)(this, hdr, pos, q, time, mask);
	}

	__forceinline void DoExtraBoneProcessing(CStudioHdr* pStudioHdr, Vector pos[], Quaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed, CIKContext* pIKContext) {
		using DoExtraBoneProcessing_t = void(__thiscall*)(decltype(this), CStudioHdr*, Vector*, Quaternion*, matrix3x4a_t*, CBoneBitList&, CIKContext*);
		return util::get_method< DoExtraBoneProcessing_t >(this, DOEXTRABONEPROCESSING)(this, pStudioHdr, pos, q, boneToWorld, boneComputed, pIKContext);
	}

	__forceinline bool ShouldInterpolate() {
		using ShouldInterpolate_t = bool(__thiscall*)(decltype(this));
		return util::get_method< ShouldInterpolate_t >(this, SHOULDINTERPOLATE)(this);
	}

	__forceinline float GetFOV() {
		return util::get_method< float(__thiscall*)(decltype(this)) >(this, GETFOV)(this);
	}

	__forceinline const Vector& WorldSpaceCenter() {
		return util::get_method< const Vector& (__thiscall*)(void*) >(this, WORLDSPACECENTER)(this);
	}

	__forceinline void PreThink() {
		return util::get_method< void(__thiscall*)(decltype(this)) >(this, PRETHINK)(this);
	}

	__forceinline void Think() {
		return util::get_method< void(__thiscall*)(decltype(this)) >(this, THINK)(this);
	}

	__forceinline void SetLocalViewAngles(QAngle& angle) {
		return util::get_method< void(__thiscall*)(void*, QAngle&) >(this, SETLOCALVIEWANGLES)(this, angle);
	}

	__forceinline void EyePosition(Vector& pos) {
		return util::get_method< void(__thiscall*)(decltype(this), Vector&) >(this, GETEYEPOS)(this, pos);
	}

	__forceinline void Weapon_ShootPosition(Vector& pos) {
		return util::get_method< void(__thiscall*)(decltype(this), Vector&) >(this, WEAPON_SHOOTPOSITION)(this, pos);
	}

	__forceinline Vector Weapon_ShootPosition() {
		Vector vecPos;
		Weapon_ShootPosition(vecPos);
		return vecPos;
	}

	__forceinline void UpdateCollisionBounds() {
		return util::get_method< void(__thiscall*)(decltype(this)) >(this, UPDATECOLLISIONBOUNDS)(this);
	}

	__forceinline void SetSequence(int nSequence) {
		return util::get_method< void(__thiscall*)(void*, int) >(this, SETSEQUENCE)(this, nSequence);
	}

	__forceinline void StudioFrameAdvance() {
		return util::get_method< void(__thiscall*)(void*) >(this, STUDIOFRAMEADVANCE)(this);
	}

	__forceinline void PostThinkClient() {
		return util::get_method< void(__thiscall*)(void*) >(this, RUNPOSTTHINK)(this);
	}

	// misc funcs.
	__forceinline CStudioHdr* GetModelPtr() {
		using LockStudioHdr_t = void(__thiscall*)(decltype(this));

		if (!m_studioHdr())
			g_Csgo.LockStudioHdr.as< LockStudioHdr_t >()(this);

		return m_studioHdr();
	}

	__forceinline Weapon* GetActiveWeapon() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle< Weapon* >(m_hActiveWeapon());
	}

	__forceinline float GetMaxPlayerSpeed() {
		Weapon* pWeapon = this->GetActiveWeapon();
		if (!this->GetActiveWeapon())
			return 260.f;

		WeaponInfo* pWeaponData = pWeapon->GetWpnData();
		if (!pWeaponData)
			return 260.f;

		return m_bIsScoped() ? pWeaponData->m_MaxPlayerSpeedAlt : pWeaponData->m_MaxPlayerSpeed;
	}

	__forceinline Entity* GetObserverTarget() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle(m_hObserverTarget());
	}

	__forceinline Entity* GetGroundEntity() {
		return g_Csgo.m_EntityList->GetClientEntityFromHandle(m_hGroundEntity());
	}

	__forceinline IVehicle* GetVehicle() {
		return (IVehicle*)g_Csgo.m_EntityList->GetClientEntityFromHandle(m_hVehicle());
	}

	__forceinline void SetAnimLayers(C_AnimationLayer* layers) {
		if (!layers)
			return;

		std::memcpy(m_AnimOverlay(), layers, sizeof(C_AnimationLayer) * ANIMATION_LAYER_COUNT);
	}

	__forceinline void GetAnimLayers(C_AnimationLayer* layers) {
		if (!m_AnimOverlay())
			return;

		std::memcpy(layers, m_AnimOverlay(), sizeof(C_AnimationLayer) * ANIMATION_LAYER_COUNT);
	}

	__forceinline void SetPoseParameters(float* poses) {
		if (!poses)
			return;

		std::memcpy(m_flPoseParameter(), poses, sizeof(float) * 24);
	}

	__forceinline void GetPoseParameters(float* poses) {
		if (!m_flPoseParameter())
			return;

		std::memcpy(poses, m_flPoseParameter(), sizeof(float) * 24);
	}

	// virtuals.
	__forceinline bool PhysicsRunThink(int smth = 0) {
		static auto PhysicsRunThink = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87 E4")).as<bool(__thiscall*)(void*, int)>();
		return PhysicsRunThink(this, smth);
	}

	__forceinline bool UsingStandardWeaponsInVehicle() {
		IVehicle* pVehicle = GetVehicle();
		if (!pVehicle)
			return true;

		// NOTE: We *have* to do this before ItemPostFrame because ItemPostFrame
		// may dump us out of the vehicle
		int nRole = pVehicle->GetPassengerRole(this);
		bool bUsingStandardWeapons = pVehicle->IsPassengerUsingStandardWeapons(nRole);

		// Fall through and check weapons, etc. if we're using them 
		if (!bUsingStandardWeapons)
			return false;

		return true;
	}

	__forceinline bool ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs) {
		using ComputeHitboxSurroundingBox_t = bool(__thiscall*)(void*, Vector*, Vector*);

		return g_Csgo.ComputeHitboxSurroundingBox.as< ComputeHitboxSurroundingBox_t >()(this, mins, maxs);
	}

	__forceinline Activity GetSequenceActivity(int sequence) {
		using GetSequenceActivity_t = Activity(__fastcall*)(CStudioHdr*, int);

		return g_Csgo.GetSequenceActivity.as< GetSequenceActivity_t >()(GetModelPtr(), sequence);
	}

	__forceinline void InvalidateBoneCache() {
		m_BoneAccessor().SetWritableBones(0);
		m_BoneAccessor().SetReadableBones(0);

		m_iMostRecentModelBoneCounter() = 0;
		m_flLastBoneSetupTime() = std::numeric_limits< float >::lowest();
	}

	__forceinline void SetupBones_AttachmentHelper(CStudioHdr* studio) {
		using AttachmentHelper_t = void(__thiscall*)(decltype(this), CStudioHdr*);
		g_Csgo.AttachmentHelper.as< AttachmentHelper_t >()(this, studio);
	}


	__forceinline bool IsAlive() {
		return m_lifeState() == LIFE_ALIVE;
		//return util::get_method< bool(__thiscall*)(void*) >(this, ISAlIVE)(this);
	}

	__forceinline bool IsValid() {
		return this && this->IsPlayer() && !this->GetClientNetworkable()->IsDormant() && this->IsAlive();
	}

	__forceinline bool enemy(Player* from) {
		return from && (m_iTeamNum() != from->m_iTeamNum() || g_Csgo.mp_teammates_are_enemies->GetInt()) && !IsLocalPlayer();
	}

	__forceinline bool teammate(Player* from)
	{
		return from && m_iTeamNum() == from->m_iTeamNum() && !g_Csgo.mp_teammates_are_enemies->GetInt() && !IsLocalPlayer();
	}

	__forceinline uint8_t* GetServerEdict() {
		static uintptr_t pServerGlobals = **(uintptr_t**)(pattern::find(g_Csgo.m_server_dll, XOR("8B 15 ? ? ? ? 33 C9 83 7A 18 01")) + 0x2);

		int iMaxClients = *(int*)((uintptr_t)pServerGlobals + 0x18);
		int iIndex = this->GetClientNetworkable()->GetIndex();
		if (iIndex > 0 && iMaxClients >= 1) {
			if (iIndex <= iMaxClients) {
				int v10 = iIndex * 16;
				uintptr_t v11 = *(uintptr_t*)(pServerGlobals + 96);
				if (v11) {
					if (!((*(uintptr_t*)(v11 + v10) >> 1) & 1)) {
						uintptr_t v12 = *(uintptr_t*)(v10 + v11 + 12);
						if (v12) {
							uint8_t* pReturn = nullptr;

							// abusing asm is not good
							__asm
							{
								pushad
								mov ecx, v12
								mov eax, dword ptr[ecx]
								call dword ptr[eax + 0x14]
								mov pReturn, eax
								popad
							}

							return pReturn;
						}
					}
				}
			}
		}
		return nullptr;
	}

	__forceinline void DrawServerHitbox() {
		auto duration = g_Csgo.m_Globals->m_Interval * 2.0f;

		auto serverPlayer = GetServerEdict();
		if (serverPlayer) {
			static auto pCall = (pattern::find(g_Csgo.m_server_dll, XOR("55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE")));
			float fDuration = duration;

			__asm
			{
				pushad
				movss xmm1, fDuration
				push 1 //bool monoColor
				mov ecx, serverPlayer
				call pCall
				popad
			}
		}
	}

	// todo: rebuilt from server.
	__forceinline void PostThink(void) {
		static auto PostThinkVPhysics = pattern::find(g_Csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB ? ? ? ? ? 75 50 8B 0D")).as<bool(__thiscall*)(void*)>();
		static auto SimulatePlayerSimulatedEntities = pattern::find(g_Csgo.m_client_dll, XOR("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72 90 8B 86")).as<void(__thiscall*)(void*)>();

		if (IsAlive())
		{
			UpdateCollisionBounds();

			if (m_fFlags() & FL_ONGROUND)
				m_flFallVelocity() = 0.f;

			if (m_nSequence() == -1)
				SetSequence(0);
			
			StudioFrameAdvance();
			PostThinkVPhysics(this);
		}

		// Even if dead simulate entities
		SimulatePlayerSimulatedEntities(this);
	}

public:
	bool IsEFlagSet(int nEFlagMask);
	float GetPoseParameter(int iParameter);
	int SelectWeightedSequenceFromModifiers(Activity activity, CUtlSymbol* pActivityModifiers, int iModifierCount);
	void EstimateAbsVelocity(Vector& vel);
	float SetPoseParameter(CStudioHdr* pStudioHdr, int iParameter, float flValue);
	float SetPoseParameter(int iParameter, float flValue);
	int LookupPoseParameter(CStudioHdr* pStudioHdr, const char* szName);
	int LookupPoseParameter(const char* szName);
	float GetFirstSequenceAnimTag(int sequence, int nDesiredTag, float flStart, float flEnd);
	float GetAnySequenceAnimTag(int sequence, int tag, float flDefault);
	void CalcAbsolutePosition();
	float GetSequenceCycleRate(CStudioHdr* studio, int sequence);
	float GetLayerSequenceCycleRate(C_AnimationLayer* layer, int sequence);
	float GetSequenceMoveDist(CStudioHdr* pStudioHdr, int nSequence);
	void SetNumAnimOverlays(int num);
	int LookupSequence(const char* name);
	void DoAnimationEvent(PlayerAnimEvent_t event, int nData = 0);
	void GetBulletTypeParameters(const char* iBulletType, float& fPenetrationPower, float& flPenetrationDistance);
	QAngle& EyeAngles();
	float FrameAdvance(float interval);
	void OnLatchInterpolatedVariables(int flags);
	bool& m_bKilledByTaser();
	void HandleTaserAnimation();
	void UpdateViewmodels();
	CBaseViewModel* GetViewModel(int modelindex);
	void UpdateClientSideAnimation();
	int LookupBone(const char* szName);
	matrix3x4_t& GetCoordinateFrame();
	const matrix3x4_t& GetEntityToWorldTransform() const;
	void GetCachedBoneMatrix(int boneIndex, matrix3x4_t& out);
	void GetBoneTransform(int iBone, matrix3x4_t& pBoneToWorld);
	void GetBonePosition(int iBone, Vector& origin, QAngle& angles);
	void SelectItem(const char* string, int sub_type);

	Player* m_hOwner(void);

	void SetClientSideAnimation(bool bSequenceCycle);

	// topic | interpolation velocity related.
	void SetLocalVelocity(const Vector& vecVelocity);
	void SetVelocity(const Vector& vecVelocity);

	// topic | interpolation origin related.
	void SetLocalOrigin(const Vector& vecOrigin);
	void SetOrigin(const Vector& vecOrigin);

	// server - eflags.
	void AddEFlags(int nEFlagMask);
	void RemoveEFlags(int nEFlagMask);
	bool GetPredictable(void);
	void MarkForThreadedBoneSetup();
	bool Teleported();
	inline bool IsEffectActive(int nEffects);
	CThreadFastMutex& m_BoneSetupLock();
	void* m_pRagdoll();
	void* m_bClientSideRagdoll();
	bool IsRagdoll();
	void UpdateIKLocks(float currentTime);
	void CalculateIKLocks(float currentTime);

	// rebuilt setupbones.
	bool SetupBonesServer(matrix3x4a_t* out, int max, int boneMask, float time);
	bool SetupBonesRebuilt(matrix3x4a_t* out, int max, int boneMask, float time);
	bool SetupBones(matrix3x4a_t* out, int max, int boneMask, float time);
	void DispatchAnimEvents(Player* eventHandler);
};

class CTraceFilterSimple_game {
public:
	void* m_vmt;
	const Entity* m_PassEntity;
	int             m_CollisionGroup;
	ShouldHitFunc_t m_ShouldHitCheckFn;

public:
	__forceinline CTraceFilterSimple_game() :
		m_vmt{ g_Csgo.CTraceFilterSimple_vmt.as< void* >() },
		m_PassEntity{},
		m_CollisionGroup{},
		m_ShouldHitCheckFn{} {}

	__forceinline CTraceFilterSimple_game(const Entity* pass_ent1, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr) :
		m_vmt{ g_Csgo.CTraceFilterSimple_vmt.as< void* >() },
		m_PassEntity{ pass_ent1 },
		m_CollisionGroup{ collision_group },
		m_ShouldHitCheckFn{ shouldhit_check_fn } {}

	__forceinline bool ShouldHitEntity(Entity* entity, int contents_mask) {
		// note - dex; game is dumb, this gets the real vmt.
		void* real_vmt = *(void**)m_vmt;

		return util::get_method< bool(__thiscall*)(void*, Entity*, int) >(real_vmt, 0)(real_vmt, entity, contents_mask);
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity(Entity* pass_ent1) {
		m_PassEntity = pass_ent1;

		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 2 )( m_vmt, pass_ent1 );
	}

	__forceinline void SetCollisionGroup(int collision_group) {
		m_CollisionGroup = collision_group;

		// util::get_method< void (__thiscall *)( void *, int ) >( m_vmt, 3 )( m_vmt, collision_group );
	}
};

class CTraceFilterSkipTwoEntities_game {
public:
	void* m_vmt;
	const Entity* m_PassEntity;
	int             m_CollisionGroup;
	ShouldHitFunc_t m_ShouldHitCheckFn;
	const Entity* m_PassEntity2;

public:
	__forceinline CTraceFilterSkipTwoEntities_game() :
		m_vmt{ g_Csgo.CTraceFilterSkipTwoEntities_vmt.as< void* >() },
		m_PassEntity{},
		m_CollisionGroup{},
		m_ShouldHitCheckFn{},
		m_PassEntity2{} {}

	__forceinline CTraceFilterSkipTwoEntities_game(const Entity* pass_ent1, const Entity* pass_ent2, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr) :
		m_vmt{ g_Csgo.CTraceFilterSimple_vmt.as< void* >() },
		m_PassEntity{ pass_ent1 },
		m_CollisionGroup{ collision_group },
		m_ShouldHitCheckFn{ shouldhit_check_fn },
		m_PassEntity2{ pass_ent2 } {}

	__forceinline bool ShouldHitEntity(Entity* entity, int contents_mask) {
		// note - dex; game is dumb, this gets the real vmt.
		void* real_vmt = *(void**)m_vmt;

		return util::get_method< bool(__thiscall*)(void*, Entity*, int) >(m_vmt, 0)(m_vmt, entity, contents_mask);
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity(Entity* pass_ent1) {
		m_PassEntity = pass_ent1;
	}

	__forceinline void SetCollisionGroup(int collision_group) {
		m_CollisionGroup = collision_group;
	}

	__forceinline void SetPassEntity2(Entity* pass_ent2) {
		m_PassEntity2 = pass_ent2;
	}
};