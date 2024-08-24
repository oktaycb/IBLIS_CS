#pragma once

enum LightType_t {
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};

enum {
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,
	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
	STUDIORENDER_DRAW_WIREFRAME = 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200
};

enum beam_types {
	beam_normal = 0,
	beam_disk = 2,
	beam_cylinder,
	beam_follow,
	beam_ring,
	beam_spline,
	beam_ring_point,
	beam_laser,
	beam_tesla,
};

struct LightDesc_t {
	LightType_t  m_Type;										//< MATERIAL_LIGHT_xxx
	Vector       m_Color;											//< color+intensity 
	Vector       m_Position;										//< light source center position
	Vector       m_Direction;										//< for SPOT, direction it is pointing
	float        m_Range;											//< distance range for light.0=infinite
	float        m_Falloff;										//< angular falloff exponent for spot lights
	float        m_Attenuation0;									//< constant distance falloff term
	float        m_Attenuation1;									//< linear term of falloff
	float        m_Attenuation2;									//< quadatic term of falloff
	float        m_Theta;											//< inner cone angle. no angular falloff 
	float        m_Phi;											//< outer cone angle
	float        m_ThetaDot;
	float        m_PhiDot;
	float        m_OneOverThetaDotMinusPhiDot;
	unsigned int m_Flags;
protected:
	float        m_RangeSquared;
};

class IMaterial;

struct DrawModelInfo_t {
	studiohdr_t* m_pStudioHdr;
	PAD(0x8);
	int				 m_Skin;
	int				 m_Body;
	int				 m_HitboxSet;
	void* m_pClientEntity;
	int				 m_Lod;
	void* m_pColorMeshes;
	bool			 m_bStaticLighting;
	Vector			 m_vecAmbientCube[6];
	int				 m_nLocalLightCount;
	LightDesc_t		 m_LocalLightDescs[4];
};

struct DrawModelState_t {
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	void* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	PAD(0x4);
	int					 m_drawFlags;
	int					 m_lod;
};

struct ModelRenderInfo_t {
	Vector				m_Origin;			// 0
	QAngle				m_angles;			// 12
	void* m_Renderable;		// 24
	const model_t* m_model;			// 28
	const matrix3x4_t* m_model_to_world;	// 32
	const matrix3x4_t* m_lighting_offset;	// 36
	const Vector* m_lighting_origin;	// 40
	int					m_Flags;			// 44
	int					m_Index;			// 48
	int					m_skin;				// 52
	int					m_LowerbodyYaw;				// 56
	int					m_hitboxset;		// 60
	uint16_t			m_instance;			// 64

	__forceinline ModelRenderInfo_t() : m_model_to_world{}, m_lighting_offset{}, m_lighting_origin{} {}
};

class ISpatialLeafEnumerator
{
public:
	virtual bool EnumerateLeaf(int leaf, int context) = 0;
};

class ISpatialQuery
{
public:
	enum indices : size_t {
		LISTLEAVESINBOX = 6,
	};

	// Returns the number of leaves
	virtual int LeafCount() const = 0;

	// Enumerates the leaves along a ray, box, etc.
	virtual bool EnumerateLeavesAtPoint(const Vector& pt, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesInBox(const Vector& mins, const Vector& maxs, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesInSphere(const Vector& center, float radius, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesAlongRay(Ray const& ray, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesInSphereWithFlagSet(const Vector& center, float radius, ISpatialLeafEnumerator* pEnum, int context, int nFlagsCheck) = 0;
	virtual int ListLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* pList, int listMax) = 0;

	// Used to determine which leaves passed in (specified by leafcount, pLeafs, and nLeafStride ) 
	// are within radius flRadius of vecCenter and have the flag set.
	// The result is placed in the pLeafsInSphere array, which specifies _indices_ into the original pLeafs array
	// The number of leaves found within the sphere is the return value.
	// The caller is expected to have allocated at least nLeafCount pLeafsInSphere to place the results into
	virtual int ListLeavesInSphereWithFlagSet(int* pLeafsInSphere, const Vector& vecCenter, float flRadius, int nLeafCount, const UINT16* pLeafs, int nLeafStride = sizeof(UINT16), int nFlagsCheck = 0xFFFFFFFF) = 0;
};

typedef CBaseHandle ClientEntityHandle_t;
typedef unsigned short SpatialPartitionHandle_t;
struct FlashlightState_t;

enum ShadowReceiver_t
{
	SHADOW_RECEIVER_BRUSH_MODEL = 0,
	SHADOW_RECEIVER_STATIC_PROP,
	SHADOW_RECEIVER_STUDIO_MODEL,
};

struct WorldListLeafData_t
{
	//LeafIndex_t	leafIndex;	// 16 bits
	//int16	waterData;
	//uint16 	firstTranslucentSurface;	// engine-internal list index
	//uint16	translucentSurfaceCount;	// count of translucent surfaces+disps
};

typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
typedef unsigned short ModelInstanceHandle_t;

class IClientShadowMgr {
public:
	enum indices : size_t {
		UNLOCKALLSHADOWDEPTHTEXTURES = 30,
		COMPUTESHADOWDEPTHTEXTURES = 43,
	};

	// Create, destroy shadows
	virtual ClientShadowHandle_t CreateShadow(ClientEntityHandle_t entity, int nEntIndex, int flags, void** pSplitScreenBits = NULL) = 0;
	virtual void DestroyShadow(ClientShadowHandle_t handle) = 0;

	// Create flashlight.
	// FLASHLIGHTFIXME: need to rename all of the shadow stuff to projectedtexture and have flashlights and shadows as instances.
	virtual ClientShadowHandle_t CreateFlashlight(const FlashlightState_t& lightState) = 0;
	virtual void UpdateFlashlightState(ClientShadowHandle_t shadowHandle, const FlashlightState_t& lightState) = 0;
	virtual void DestroyFlashlight(ClientShadowHandle_t handle) = 0;

	// Create simple projected texture.  it is not a light or a shadow, but this system does most of the work already for it
	virtual ClientShadowHandle_t CreateProjection(const FlashlightState_t& lightState) = 0;
	virtual void UpdateProjectionState(ClientShadowHandle_t shadowHandle, const FlashlightState_t& lightState) = 0;
	virtual void DestroyProjection(ClientShadowHandle_t handle) = 0;

	// Indicate that the shadow should be recomputed due to a change in
	// the client entity
	virtual void UpdateProjectedTexture(ClientShadowHandle_t handle, bool force = false) = 0;

	// Used to cause shadows to be re-projected against the world.
	virtual void AddToDirtyShadowList(ClientShadowHandle_t handle, bool force = false) = 0;
	virtual void AddToDirtyShadowList(IClientRenderable* pRenderable, bool force = false) = 0;

	// deals with shadows being added to shadow receivers
	virtual void AddShadowToReceiver(ClientShadowHandle_t handle,
		IClientRenderable* pRenderable, ShadowReceiver_t type) = 0;

	virtual void RemoveAllShadowsFromReceiver(
		IClientRenderable* pRenderable, ShadowReceiver_t type) = 0;

	// Re-renders all shadow textures for shadow casters that lie in the leaf list
	virtual void ComputeShadowTextures(const CViewSetup& view, int leafCount, WorldListLeafData_t* pLeafList) = 0;

	// Frees shadow depth textures for use in subsequent view/frame
	virtual void UnlockAllShadowDepthTextures() = 0;

	// Renders the shadow texture to screen...
	virtual void RenderShadowTexture(int w, int h) = 0;

	// Sets the shadow direction + color
	virtual void SetShadowDirection(const Vector& dir) = 0;
	virtual const Vector& GetShadowDirection() const = 0;

	virtual void SetShadowColor(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void SetShadowDistance(float flMaxDistance) = 0;
	virtual void SetShadowBlobbyCutoffArea(float flMinArea) = 0;
	virtual void SetFalloffBias(ClientShadowHandle_t handle, unsigned char ucBias) = 0;

	// Marks the Render-to-texture shadow as needing to be re-rendered
	virtual void MarkRenderToTextureShadowDirty(ClientShadowHandle_t handle) = 0;

	// Advance the frame
	virtual void AdvanceFrame() = 0;

	// Set and clear flashlight target renderable
	virtual void SetFlashlightTarget(ClientShadowHandle_t shadowHandle, EHANDLE targetEntity) = 0;

	// Set flashlight light world flag
	virtual void SetFlashlightLightWorld(ClientShadowHandle_t shadowHandle, bool bLightWorld) = 0;

	virtual void SetShadowsDisabled(bool bDisabled) = 0;

	virtual void ComputeShadowDepthTextures(const CViewSetup& pView, bool bSetup = false) = 0;

	virtual void DrawVolumetrics(const CViewSetup& view) = 0;

	// Toggle shadow casting from world light sources
	virtual void SetShadowFromWorldLightsEnabled(bool bEnable) = 0;

	virtual void DrawDeferredShadows(const CViewSetup& view, int leafCount, WorldListLeafData_t* pLeafList) = 0;

	virtual void InitRenderTargets() = 0;

	// Reprojects moved shadows against the world
	virtual void ReprojectShadows() = 0;

	virtual void UpdateSplitscreenLocalPlayerShadowSkip() = 0;

	virtual void GetFrustumExtents(ClientShadowHandle_t handle, Vector& vecMin, Vector& vecMax) = 0;

	virtual void ShutdownRenderTargets(void) = 0;
};

class IMaterialVar
{
public:
	PAD(0xC);
	float flValue;
public:
	void SetVecValue(float r, float g, float b) {
		using original_fn = void(__thiscall*)(IMaterialVar*, float, float, float);
		return (*(original_fn**)this)[11](this, r, g, b);
	}
	void SetVecValue(int val) {
		using original_fn = void(__thiscall*)(IMaterialVar*, int);
		(*(original_fn**)this)[4](this, val);
	}
	void SetVecValue(float val) {
		using original_fn = void(__thiscall*)(IMaterialVar*, float);
		(*(original_fn**)this)[4](this, val);
	}
};

//-----------------------------------------------------------------------------
// Shader state flags can be read from the FLAGS materialvar
// Also can be read or written to with the Set/GetMaterialVarFlags() call
// Also make sure you add/remove a string associated with each flag below to CShaderSystem::ShaderStateString in ShaderSystem.cpp
//-----------------------------------------------------------------------------
enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_PSEUDO_TRANSLUCENT = (1 << 9), // used to mark water materials for rendering after opaques but before translucents (with alpha blending but also with depth writes)
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_AOPREPASS = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_MULTIPLY = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};

//-----------------------------------------------------------------------------
// Preview image return values
//-----------------------------------------------------------------------------
enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum ImageFormat
{
	IMAGE_FORMAT_DEFAULT = -2,	// Use this image format if you want to perform tool operations on the texture
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,	// NOTE: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,			// Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102,	// 10 bit-per component Render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,			// 16 bit FP format

	// Depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,	// Specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW,	// Specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,
	IMAGE_FORMAT_LINEAR_A8,
	IMAGE_FORMAT_LINEAR_DXT1,
	IMAGE_FORMAT_LINEAR_DXT3,
	IMAGE_FORMAT_LINEAR_DXT5,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	// these are used for runtime conversion to DXT1/5
	IMAGE_FORMAT_DXT1_RUNTIME,
	IMAGE_FORMAT_DXT5_RUNTIME,

	// special depth format
	IMAGE_FORMAT_INTZ,

	NUM_IMAGE_FORMATS
};

//-----------------------------------------------------------------------------
// The vertex format type
//-----------------------------------------------------------------------------
typedef unsigned __int64 VertexFormat_t;

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// Vector
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

class IMaterial {
public:
	enum indices : size_t {
		GETNAME = 0,
		GETTEXTUREGROUPNAME = 1,
		FINDVAR = 11,
		INCREMENTREFERENCECOUNT = 12,
		ALPHAMODULATE = 27,
		COLORMODULATE = 28,
		SETFLAG = 29,
		GETFLAG = 30,
		ISERRORMATERIAL = 42
	};

	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char* GetName() const = 0;
	virtual const char* GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to Render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties(int* width, int* height,
		ImageFormat* imageFormat, bool* isTranslucent) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char* data,
		int width, int height,
		ImageFormat imageFormat) const = 0;
	// 
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;

	virtual int				GetNumAnimationFrames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are Normalized to texture.
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float* pOffset) = 0;
	virtual void			GetMaterialScale(float* pScale) = 0;
	virtual IMaterial* GetMaterialPage(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar* FindVar(const char* varName, bool* found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const = 0;

	virtual void			GetLowResColorSample(float s, float t, float* color) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() = 0;

	// Are we translucent?
	virtual bool			IsTranslucent() = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested() = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit() = 0;

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat() const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const = 0;

	virtual bool			UsesEnvCubemap(void) = 0;

	virtual bool			NeedsTangentSpace(void) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector& reflect) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() = 0;

	// Sets the shader associated with the material
	virtual void			SetShader(const char* pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar** GetShaderParams(void) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const = 0;

	// Don't want to mess with the vtable layout
	virtual void			Unused() {}

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float* r, float* g, float* b) = 0;

	// Is this translucent given a particular alpha modulation?
	virtual bool			IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(KeyValues* pKeyValues) = 0;
	virtual const char* GetShaderName() const = 0;

	virtual void			DeleteIfUnreferenced() = 0;

	virtual bool			IsSpriteCard() = 0;

	virtual void			CallBindProxy(void* proxyData, void** pCallQueue) = 0;

	virtual void			RefreshPreservingMaterialVars() = 0;

	virtual bool			WasReloadedFromWhitelist() = 0;

	// when enabled, 0 will be a pure exclude, otherwise the desired maxmip
	virtual bool			SetTempExcluded(bool bSet, int nExcludedDimensionLimit = 0) = 0;

	virtual int				GetReferenceCount() const = 0;
};

class IMaterialSystem {
public:
	enum indices : size_t {
		OVERRIDECONFIG = 21,
		CREATEMATERIAL = 83,
		FINDMATERIAL = 84,
		FIRSTMATERIAL = 86,
		NEXTMATERIAL = 87,
		INVALIDMATERIAL = 88,
		GETMATERIAL = 89
	};

	//__forceinline IMaterial* CreateMaterial( const char* name, KeyValues* kv ) {
	//	return util::get_method< IMaterial*( __thiscall* )( void*, const char*, KeyValues* ) >( this, CREATEMATERIAL )( this, name, kv );
	//}

	__forceinline IMaterial* FindMaterial(const char* name, char* type) {
		return  util::get_method< IMaterial* (__thiscall*)(void*, const char*, char*, bool, void*) >(this, FINDMATERIAL)(this, name, type, 1, 0);
	}

	__forceinline uint16_t FirstMaterial() {
		return util::get_method< uint16_t(__thiscall*)(void*) >(this, FIRSTMATERIAL)(this);
	}

	__forceinline uint16_t NextMaterial(uint16_t handle) {
		return util::get_method< uint16_t(__thiscall*)(void*, uint16_t) >(this, NEXTMATERIAL)(this, handle);
	}

	__forceinline uint16_t InvalidMaterial() {
		return util::get_method< uint16_t(__thiscall*)(void*) >(this, INVALIDMATERIAL)(this);
	}

	__forceinline IMaterial* GetMaterial(uint16_t handle) {
		return util::get_method< IMaterial* (__thiscall*)(void*, uint16_t) >(this, GETMATERIAL)(this, handle);
	}

	// find material by hash.
	__forceinline IMaterial* FindMaterial(hash_t hash) {
		for (uint16_t h = FirstMaterial(); h != InvalidMaterial(); h = NextMaterial(h)) {
			IMaterial* mat = GetMaterial(h);
			if (!mat)
				continue;

			if (RT_HASH(mat->GetName()) == hash)
				return mat;
		}

		return nullptr;
	}
};

class CStudioRenderContext {
public:
	enum indices : size_t {
		DRAWMODEL = 29,
		FORCEDMATERIALOVERRIDE = 33
	};

public:
	PAD(0x250);
	IMaterial* m_pForcedMaterial;

public:
	__forceinline void ForcedMaterialOverride(IMaterial* mat) {
		return util::get_method< void(__thiscall*)(void*, IMaterial*, int, int) >(this, FORCEDMATERIALOVERRIDE)(this, mat, 0, 0);
	}
};

struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;
struct LightDesc_t;

struct StaticPropRenderInfo_t
{
	const matrix3x4_t* pModelToWorld;
	const model_t* pModel;
	IClientRenderable* pRenderable;
	Vector* pLightingOrigin;
	short                   skin;
	ModelInstanceHandle_t   instance;
};

typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;

class C_StudioHDR;
class ITexture;
class IMatRenderContext;
class DataCacheHandle_t;

struct LightingQuery_t
{
	Vector                  m_LightingOrigin;
	ModelInstanceHandle_t   m_InstanceHandle;
	bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
	IClientRenderable* m_pRenderable;
};

struct MaterialLightingState_t
{
	Vector m_vecAmbientCube[6]; // ambient, and lights that aren't in locallight[]
	Vector m_vecLightingOrigin; // The position from which lighting state was computed
	int m_nLocalLightCount;
	LightDesc_t m_LightDescription[4];
};


class IVModelRender {
public:
	enum indices : size_t {
		FORCEDMATERIALOVERRIDE = 1,
		ISFORCEDMATERIALOVERRIDE = 2,
		DRAWMODELEXECUTE = 21
	};

	virtual int                     DrawModel(int flags, IClientRenderable* pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t* model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t* modelToWorld = NULL, const matrix3x4_t* pLightingOffset = NULL) = 0;
	virtual void                    ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
	virtual bool                    IsForcedMaterialOverride(void) = 0;
	virtual void                    SetViewTarget(const C_StudioHDR* pStudioHdr, int nBodyIndex, const Vector& target) = 0;
	virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable* pRenderable, LightCacheHandle_t* pCache = NULL) = 0;
	virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
	virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
	virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable* pRenderable) = 0;
	virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
	virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
	virtual void                    RemoveAllDecalsFromAllModels() = 0;
	virtual matrix3x4_t*			DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, DrawModelInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void                    DrawModelShadow(IClientRenderable* pRenderable, const DrawModelInfo_t& info, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual void                    ReleaseAllStaticPropColorData(void) = 0;
	virtual void                    RestoreAllStaticPropColorData(void) = 0;
	virtual int                     DrawModelEx(ModelRenderInfo_t& pInfo) = 0;
	virtual int                     DrawModelExStaticProp(ModelRenderInfo_t& pInfo) = 0;
	virtual bool                    DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelState_t* pState, matrix3x4_t** ppBoneToWorldOut) = 0;
	virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void                    SetupLighting(const Vector& vecCenter) = 0;
	virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) = 0;
	virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
	virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
	virtual void                    SetupLightingEx(const Vector& vecCenter, ModelInstanceHandle_t handle) = 0;
	virtual bool                    GetBrightestShadowingLightSource(const Vector& vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
	virtual void                    ComputeLightingState(int nCount, const LightingQuery_t* pQuery, MaterialLightingState_t* pState, ITexture** ppEnvCubemapTexture) = 0;
	virtual void                    GetModelDecalHandles(StudioDecalHandle_t* pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t* pHandles) = 0;
	virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t* pQuery, MaterialLightingState_t* pState, MaterialLightingState_t* pDecalState, ColorMeshInfo_t** ppStaticLighting, ITexture** ppEnvCubemapTexture, DataCacheHandle_t* pColorMeshHandles) = 0;
	virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t* pColorMeshHandles) = 0;
};

class IVRenderView {
public:
	enum indices : size_t {
		SETBLEND = 4,
		SETCOLORMODULATION = 6,
		GETCOLORMODULATION = 7,
		SCENEEND = 9,
		DRAWWORLDLISTS = 14,
	};

	__forceinline void SetBlend(float blend) {
		return util::get_method< void(__thiscall*)(void*, float)>(this, SETBLEND)(this, blend);
	}

	__forceinline void SetColorModulation(Color col) {
		float color[] = { (float)col.r() / 255,  (float)col.g() / 255, (float)col.b() / 255,  1.f };
		return util::get_method< void(__thiscall*)(void*, float*) >(this, SETCOLORMODULATION)(this, color);
	}

	__forceinline Color GetColorModulation() {
		float color[4]{};
		util::get_method< void(__thiscall*)(void*, float*) >(this, GETCOLORMODULATION)(this, color);

		return Color(
			(uint8_t)color[0] * 255,
			(uint8_t)color[1] * 255,
			(uint8_t)color[2] * 255,
			(uint8_t)color[3] * 255
		);
	}
};

class CViewRender {
public:
	enum indices : size_t {
		ONRENDERSTART = 4,
		RENDERVIEW = 6,
		GETPLAYERVIEWSETUP = 12,
		RENDER2DEFFECTSPOSTHUD = 39,
		RENDERSMOKEOVERLAY = 40,
	};

public:
	// virtuals.
	__forceinline const CViewSetup* GetPlayerViewSetup(int slot = -1) {
		return util::get_method< const CViewSetup* (__thiscall*)(decltype(this), int) >(this, GETPLAYERVIEWSETUP)(this, slot);
	}

public:
	// members.
	PAD(0x4);
	CViewSetup m_view;
};

struct MaterialVideoMode_t {
	int m_Width;
	int m_Height;
	int m_Format;
	int m_RefreshRate;
};

struct MaterialSystem_Config_t {
	MaterialVideoMode_t m_VideoMode;
	float               m_fMonitorGamma;
	float               m_fGammaTVRangeMin;
	float               m_fGammaTVRangeMax;
	float               m_fGammaTVExponent;
	bool                m_bGammaTVEnabled;
	bool                m_bTripleBuffered;
	int                 m_nAASamples;
	int                 m_nForceAnisotropicLevel;
	int                 m_nSkipMipLevels;
	int                 m_nDxSupportLevel;
	int                 m_nFlags;
	bool                m_bEditMode;
	char                m_nProxiesTestMode;
	bool                m_bCompressedTextures;
	bool                m_bFilterLightmaps;
	bool                m_bFilterTextures;
	bool                m_bReverseDepth;
	bool                m_bBufferPrimitives;
	bool                m_bDrawFlat;
	bool                m_bMeasureFillRate;
	bool                m_bVisualizeFillRate;
	bool                m_bNoTransparency;
	bool                m_bSoftwareLighting;
	bool                m_bAllowCheats;
	char                m_nShowMipLevels;
	bool                m_bShowLowResImage;
	bool                m_bShowNormalMap;
	bool                m_bMipMapTextures;
	char                m_nFullbright;
	bool                m_bFastNoBump;
	bool                m_bSuppressRendering;
	bool                m_bDrawGray;
	bool                m_bShowSpecular;
	bool                m_bShowDiffuse;
	int                 m_nWindowedSizeLimitWidth;
	int                 m_nWindowedSizeLimitHeight;
	int                 m_nAAQuality;
	bool                m_bShadowDepthTexture;
	bool                m_bMotionBlur;
	bool                m_bSupportFlashlight;
	bool                m_bPaintEnabled;
	PAD(0xC);
};