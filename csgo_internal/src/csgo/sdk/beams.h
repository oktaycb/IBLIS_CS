#pragma once

#define MAX_BEAM_ENTS	10
#define NOISE_DIVISIONS	128

//class Beam_t {
//public:
//    PAD( 52 );       // 0x0
//	int flags;       // 0x34
//	PAD( 144 );      
//	float die;       // 0xC8
//	PAD( 20 );       
//	float r;         // 0xE0
//	float g;         // 0xE4
//	float b;         // 0xE8
//	PAD( 8 );
//	float frameRate; // 0xF4
//	float frame;     // 0xF8
//}; 

class Beam_t {
public:
    void    *unk_1; // probably 2-3 vmts?
    void    *unk_2;
    void    *unk_3;
	Vector	m_Mins;
	Vector	m_Maxs;
	void    *m_queryHandleHalo;
	float	m_haloProxySize;
	Beam_t  *next;
	int		type;
	int		flags;
	int		numAttachments;
	Vector	attachment[ MAX_BEAM_ENTS ];
	Vector	Delta;
	float	t;		
	float	freq;
	float	die;
	float	width;
	float	endWidth;
	float	fadeLength;
	float	amplitude;
	float	life;
	float	r, g, b;
	float	brightness;
	float	speed;
	float	frameRate;
	float	frame;
	int		segments;
	EHANDLE	entity[ MAX_BEAM_ENTS ];
	int		attachmentIndex[ MAX_BEAM_ENTS ];
	int		modelIndex;
	int		haloIndex;
	float	haloScale;
	int		frameCount;
	float	rgNoise[ NOISE_DIVISIONS + 1 ];
	void    *trail;
	float	start_radius;
	float	end_radius;
	bool	m_bCalculatedNoise;
	float	m_flHDRColorScale;
};

struct BeamInfo_t {
    int		   m_nType;
	Entity     *m_pStartEnt;
	int		   m_nStartAttachment;
	Entity     *m_pEndEnt;
	int		   m_nEndAttachment;
	Vector	   m_vecStart;
	Vector	   m_vecEnd;
	int		   m_nModelIndex;
	const char *m_pszModelName;
	int		   m_nHaloIndex;
	const char *m_pszHaloName;
	float	   m_flHaloScale;
	float	   m_flLife;
	float	   m_flWidth;
	float	   m_flEndWidth;
	float	   m_flFadeLength;
	float	   m_flAmplitude;
	float	   m_flBrightness;
	float	   m_flSpeed;
	int		   m_nStartFrame;
	float	   m_flFrameRate;
	float	   m_flRed;
	float	   m_flGreen;
	float	   m_flBlue;
	bool	   m_bRenderable;
	int		   m_nSegments;
	int		   m_nFlags;
	Vector	   m_vecCenter;
	float	   m_flStartRadius;
	float	   m_flEndRadius;

	__forceinline BeamInfo_t() { 
		m_nType        = 0; // TE_BEAMPOINTS;
		m_nSegments    = -1;
		m_pszModelName = 0;
		m_pszHaloName  = 0;
		m_nModelIndex  = -1;
		m_nHaloIndex   = -1;
		m_bRenderable  = true;
		m_nFlags       = 0;
	}
};
class C_Beam;

class IViewRenderBeams
{
public:

	virtual void	InitBeams(void) = 0;
	virtual void	ShutdownBeams(void) = 0;
	virtual void	ClearBeams(void) = 0;

	// Updates the state of the temp ent beams
	virtual void	UpdateTempEntBeams() = 0;

	virtual void	DrawBeam(C_Beam* pbeam, ITraceFilter* pEntityBeamTraceFilter = nullptr) = 0;
	virtual void	DrawBeam(Beam_t* pbeam) = 0;

	virtual void	KillDeadBeams(Player* pEnt) = 0;

	// New interfaces!
	virtual Beam_t* CreateBeamEnts(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamEntPoint(BeamInfo_t& beamInfo) = 0;
	virtual	Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRing(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRingPoint(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamCirclePoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamFollow(BeamInfo_t& beamInfo) = 0;

	virtual void	FreeBeam(Beam_t* pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t* pBeam, BeamInfo_t& beamInfo) = 0;

	// These will go away!
	virtual void	CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1) = 0;
	virtual void	CreateBeamEntPoint(int	nStartEntity, const Vector* pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamPoints(Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;
	virtual void	CreateBeamRingPoint(const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;
	virtual void	CreateBeamCirclePoints(int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float m_nEndWidth, float m_nFadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float r, float g, float b,
		float brightness) = 0;
};