#pragma once

#define STUDIO_ENABLE_PERF_COUNTERS
#define STUDIO_SEQUENCE_ACTIVITY_LOOKUPS_ARE_SLOW 0 

class IMaterial;
class IMesh;
class IMorph;
struct virtualmodel_t;
struct vertexFileHeader_t;
struct thinModelVertices_t;

namespace OptimizedModel
{
	struct StripHeader_t;
}

#define fsel(c,x,y) ( (c) >= 0 ? (x) : (y) )
#define Assert( _exp ) ((void)0)

#define Plat_FastMemset memset
#define Plat_FastMemcpy memcpy

/*
==============================================================================
STUDIO MODELS
Studio models are position independent, so the cache manager can move them.
==============================================================================
*/

#define STUDIO_VERSION		49

struct studiohdr_t;

#ifdef _GAMECONSOLE
#define MAXSTUDIOTRIANGLES	65536	// 
#define MAXSTUDIOVERTS		32768	// These numbers save memory in CCachedRenderData, but restrict usable model sizes on 360
#define	MAXSTUDIOFLEXVERTS	4096	// 
#else
#define MAXSTUDIOTRIANGLES	65536	// TODO: tune this
#define MAXSTUDIOVERTS		65536	// TODO: tune this
#define	MAXSTUDIOFLEXVERTS	10000	// max number of verts that can be flexed per mesh.  TODO: tune this
#endif
#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		256		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

#define MAXSTUDIOBONEBITS	8		// NOTE: MUST MATCH MAXSTUDIOBONES

// NOTE!!! : Changing this number also changes the vtx file format!!!!!
#define MAX_NUM_BONES_PER_VERT 3

//Adrian - Remove this when we completely phase out the old event system.
#define NEW_EVENT_STYLE ( 1 << 10 )

struct mstudiodata_t
{
	int		count;
	int		offset;
};

#define STUDIO_PROC_AXISINTERP	1
#define STUDIO_PROC_QUATINTERP	2
#define STUDIO_PROC_AIMATBONE	3
#define STUDIO_PROC_AIMATATTACH 4
#define STUDIO_PROC_JIGGLE		5
#define STUDIO_PROC_TWIST_MASTER 6
#define STUDIO_PROC_TWIST_SLAVE 7	// Multiple twist bones are computed at once for the same parent/child combo so TWIST_NULL do nothing
#define STUDIO_PROC_POINT_CONSTRAINT	8
#define STUDIO_PROC_ORIENT_CONSTRAINT	9
#define STUDIO_PROC_AIM_CONSTRAINT		10
#define STUDIO_PROC_IK_CONSTRAINT		11
#define STUDIO_PROC_PARENT_CONSTRAINT	12
#define STUDIO_PROC_SOFTBODY			16

#define DECL_ALIGN(x) __declspec( align( x ) )

#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)


//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef BITVEC_H
#define BITVEC_H
#ifdef _WIN32
#pragma once
#endif

#include <limits.h>

typedef __int16					int16;
typedef unsigned __int16		uint16;
typedef __int32					int32;
typedef unsigned __int32		uint32;
typedef __int64					int64;
typedef unsigned __int64		uint64;

class CBitVecAccessor
{
public:
	CBitVecAccessor(uint32* pDWords, int iBit);

	void		operator=(int val);
	operator uint32();

private:
	uint32* m_pDWords;
	int	m_iBit;
};


//-----------------------------------------------------------------------------
// Support functions
//-----------------------------------------------------------------------------

#define LOG2_BITS_PER_INT	5
#define BITS_PER_INT		32

#if _WIN32 && !defined(_X360)
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif

inline int FirstBitInWord(unsigned int elem, int offset)
{
#if _WIN32
	if (!elem)
		return -1;
#if defined( _X360 )
	// this implements CountTrailingZeros() / BitScanForward()
	unsigned int mask = elem - 1;
	unsigned int comp = ~elem;
	elem = mask & comp;
	return (32 - _CountLeadingZeros(elem)) + offset;
#else
	unsigned long out;
	_BitScanForward(&out, elem);
	return out + offset;
#endif

#else
	static unsigned firstBitLUT[256] =
	{
		0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
	};
	unsigned elemByte;

	elemByte = (elem & 0xFF);
	if (elemByte)
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if (elemByte)
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if (elemByte)
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if (elemByte)
		return offset + firstBitLUT[elemByte];

	return -1;
#endif
}

//-------------------------------------

inline unsigned GetEndMask(int numBits)
{
	static unsigned bitStringEndMasks[] =
	{
		0xffffffff,
		0x00000001,
		0x00000003,
		0x00000007,
		0x0000000f,
		0x0000001f,
		0x0000003f,
		0x0000007f,
		0x000000ff,
		0x000001ff,
		0x000003ff,
		0x000007ff,
		0x00000fff,
		0x00001fff,
		0x00003fff,
		0x00007fff,
		0x0000ffff,
		0x0001ffff,
		0x0003ffff,
		0x0007ffff,
		0x000fffff,
		0x001fffff,
		0x003fffff,
		0x007fffff,
		0x00ffffff,
		0x01ffffff,
		0x03ffffff,
		0x07ffffff,
		0x0fffffff,
		0x1fffffff,
		0x3fffffff,
		0x7fffffff,
	};

	return bitStringEndMasks[numBits % BITS_PER_INT];
}


inline int GetBitForBitnum(int bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
		(1 << 8),
		(1 << 9),
		(1 << 10),
		(1 << 11),
		(1 << 12),
		(1 << 13),
		(1 << 14),
		(1 << 15),
		(1 << 16),
		(1 << 17),
		(1 << 18),
		(1 << 19),
		(1 << 20),
		(1 << 21),
		(1 << 22),
		(1 << 23),
		(1 << 24),
		(1 << 25),
		(1 << 26),
		(1 << 27),
		(1 << 28),
		(1 << 29),
		(1 << 30),
		(1 << 31),
	};

	return bitsForBitnum[(bitNum) & (BITS_PER_INT - 1)];
}

inline int GetBitForBitnumByte(int bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
	};

	return bitsForBitnum[bitNum & 7];
}

inline int CalcNumIntsForBits(int numBits) { return (numBits + (BITS_PER_INT - 1)) / BITS_PER_INT; }

typedef unsigned int		uint;

// http://bits.stephan-brumme.com/PopulationCount.html
// http://graphics.stanford.edu/~seander/bithacks.html#PopulationCountSetParallel
inline uint PopulationCount(uint32 v)
{
	uint32 const w = v - ((v >> 1) & 0x55555555);
	uint32 const x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
	return ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

inline uint PopulationCount(uint64 v)
{
	uint64 const w = v - ((v >> 1) & 0x5555555555555555ull);
	uint64 const x = (w & 0x3333333333333333ull) + ((w >> 2) & 0x3333333333333333ull);
	return ((x + (x >> 4) & 0x0F0F0F0F0F0F0F0Full) * 0x0101010101010101ull) >> 56; // [Sergiy] I'm not sure if it's faster to multiply here to reduce the bit sum further first, so feel free to optimize, please
}

inline uint PopulationCount(uint16 v)
{
	uint16 const w = v - ((v >> 1) & 0x5555);
	uint16 const x = (w & 0x3333) + ((w >> 2) & 0x3333);
	return ((x + (x >> 4) & 0x0F0F) * 0x101) >> 8;
}

typedef unsigned char uint8;

inline uint PopulationCount(uint8 v)
{
	uint8 const w = v - ((v >> 1) & 0x55);
	uint8 const x = (w & 0x33) + ((w >> 2) & 0x33);
	return x + (x >> 4) & 0x0F;
}


#ifdef _X360
#define BitVec_Bit( bitNum ) GetBitForBitnum( bitNum )
#define BitVec_BitInByte( bitNum ) GetBitForBitnumByte( bitNum )
#else
#define BitVec_Bit( bitNum ) ( 1 << ( (bitNum) & (BITS_PER_INT-1) ) )
#define BitVec_BitInByte( bitNum ) ( 1 << ( (bitNum) & 7 ) )
#endif
#define BitVec_Int( bitNum ) ( (bitNum) >> LOG2_BITS_PER_INT )


//-----------------------------------------------------------------------------
// template CBitVecT
//
// Defines the operations relevant to any bit array. Simply requires a base
// class that implements GetNumBits(), Base(), GetNumDWords() & ValidateOperand()
//
// CVarBitVec and CBitVec<int> are the actual classes generally used
// by clients
//

template <class BASE_OPS>
class CBitVecT : public BASE_OPS
{
public:
	CBitVecT();
	explicit CBitVecT(int numBits);			// Must be initialized with the number of bits

	void	Init(int val = 0);

	// Access the bits like an array.
	CBitVecAccessor	operator[](int i);
	uint32 operator[](int i)const;

	// Do NOT override bitwise operators (see note in header)
	void	And(const CBitVecT& andStr, CBitVecT* out) const;
	void	Or(const CBitVecT& orStr, CBitVecT* out) const;
	void	Xor(const CBitVecT& orStr, CBitVecT* out) const;

	void	Not(CBitVecT* out) const;

	void	CopyTo(CBitVecT* out) const;
	void	Copy(const CBitVecT<BASE_OPS>& other, int nBits = -1);
	bool	Compare(const CBitVecT<BASE_OPS>& other, int nBits = -1) const;

	bool	IsAllClear(void) const;		// Are all bits zero?
	bool	IsAllSet(void) const;		// Are all bits one?

	uint32	Get(uint32 bitNum) const;
	bool 	IsBitSet(int bitNum) const;
	void 	Set(int bitNum);
	void 	Set(int bitNum, bool bNewVal);
	void 	Clear(int bitNum);

	bool	TestAndSet(int bitNum);

	void 	Set(uint32 offset, uint32 mask);
	void 	Clear(uint32 offset, uint32 mask);
	uint32	Get(uint32 offset, uint32 mask);

	void	SetAll(void);			// Sets all bits
	void	ClearAll(void);			// Clears all bits

	uint32	GetDWord(int i) const;
	void	SetDWord(int i, uint32 val);
	uint    PopulationCount()const;

	CBitVecT<BASE_OPS>& operator=(const CBitVecT<BASE_OPS>& other) { other.CopyTo(this); return *this; }
	bool			operator==(const CBitVecT<BASE_OPS>& other) { return Compare(other); }
	bool			operator!=(const CBitVecT<BASE_OPS>& other) { return !operator==(other); }

	static void GetOffsetMaskForBit(uint32 bitNum, uint32* pOffset, uint32* pMask) { *pOffset = BitVec_Int(bitNum); *pMask = BitVec_Bit(bitNum); }
};

//-----------------------------------------------------------------------------
// class CVarBitVecBase
//
// Defines the operations necessary for a variable sized bit array
template <typename BITCOUNTTYPE>
class CVarBitVecBase
{
public:
	bool	IsFixedSize() const { return false; }
	int		GetNumBits(void) const { return m_numBits; }
	void	Resize(int numBits, bool bClearAll = false);		// resizes bit array

	int 	GetNumDWords() const { return m_numInts; }
	uint32* Base() { return m_pInt; }
	const uint32* Base() const { return m_pInt; }

	void Attach(uint32* pBits, int numBits);
	bool Detach(uint32** ppBits, int* pNumBits);

	int		FindNextSetBit(int iStartBit) const; // returns -1 if no set bit was found

protected:
	CVarBitVecBase();
	explicit CVarBitVecBase(int numBits);
	CVarBitVecBase(const CVarBitVecBase<BITCOUNTTYPE>& from);
	CVarBitVecBase& operator=(const CVarBitVecBase<BITCOUNTTYPE>& from);
	~CVarBitVecBase(void);

	void 		ValidateOperand(const CVarBitVecBase<BITCOUNTTYPE>& operand) const { Assert(GetNumBits() == operand.GetNumBits()); }

	unsigned	GetEndMask() const { return ::GetEndMask(GetNumBits()); }

private:

	BITCOUNTTYPE	m_numBits;					// Number of bits in the bitstring
	BITCOUNTTYPE	m_numInts;					// Number of ints to needed to store bitstring
	uint32			m_iBitStringStorage;		// If the bit string fits in one int, it goes here
	uint32* m_pInt;					// Array of ints containing the bitstring

	void	AllocInts(int numInts);	// Free the allocated bits
	void	ReallocInts(int numInts);
	void	FreeInts(void);			// Free the allocated bits
};

//-----------------------------------------------------------------------------
// class CFixedBitVecBase
//
// Defines the operations necessary for a fixed sized bit array. 
// 

template <int bits> struct BitCountToEndMask_t { };
template <> struct BitCountToEndMask_t< 0> { enum { MASK = 0xffffffff }; };
template <> struct BitCountToEndMask_t< 1> { enum { MASK = 0x00000001 }; };
template <> struct BitCountToEndMask_t< 2> { enum { MASK = 0x00000003 }; };
template <> struct BitCountToEndMask_t< 3> { enum { MASK = 0x00000007 }; };
template <> struct BitCountToEndMask_t< 4> { enum { MASK = 0x0000000f }; };
template <> struct BitCountToEndMask_t< 5> { enum { MASK = 0x0000001f }; };
template <> struct BitCountToEndMask_t< 6> { enum { MASK = 0x0000003f }; };
template <> struct BitCountToEndMask_t< 7> { enum { MASK = 0x0000007f }; };
template <> struct BitCountToEndMask_t< 8> { enum { MASK = 0x000000ff }; };
template <> struct BitCountToEndMask_t< 9> { enum { MASK = 0x000001ff }; };
template <> struct BitCountToEndMask_t<10> { enum { MASK = 0x000003ff }; };
template <> struct BitCountToEndMask_t<11> { enum { MASK = 0x000007ff }; };
template <> struct BitCountToEndMask_t<12> { enum { MASK = 0x00000fff }; };
template <> struct BitCountToEndMask_t<13> { enum { MASK = 0x00001fff }; };
template <> struct BitCountToEndMask_t<14> { enum { MASK = 0x00003fff }; };
template <> struct BitCountToEndMask_t<15> { enum { MASK = 0x00007fff }; };
template <> struct BitCountToEndMask_t<16> { enum { MASK = 0x0000ffff }; };
template <> struct BitCountToEndMask_t<17> { enum { MASK = 0x0001ffff }; };
template <> struct BitCountToEndMask_t<18> { enum { MASK = 0x0003ffff }; };
template <> struct BitCountToEndMask_t<19> { enum { MASK = 0x0007ffff }; };
template <> struct BitCountToEndMask_t<20> { enum { MASK = 0x000fffff }; };
template <> struct BitCountToEndMask_t<21> { enum { MASK = 0x001fffff }; };
template <> struct BitCountToEndMask_t<22> { enum { MASK = 0x003fffff }; };
template <> struct BitCountToEndMask_t<23> { enum { MASK = 0x007fffff }; };
template <> struct BitCountToEndMask_t<24> { enum { MASK = 0x00ffffff }; };
template <> struct BitCountToEndMask_t<25> { enum { MASK = 0x01ffffff }; };
template <> struct BitCountToEndMask_t<26> { enum { MASK = 0x03ffffff }; };
template <> struct BitCountToEndMask_t<27> { enum { MASK = 0x07ffffff }; };
template <> struct BitCountToEndMask_t<28> { enum { MASK = 0x0fffffff }; };
template <> struct BitCountToEndMask_t<29> { enum { MASK = 0x1fffffff }; };
template <> struct BitCountToEndMask_t<30> { enum { MASK = 0x3fffffff }; };
template <> struct BitCountToEndMask_t<31> { enum { MASK = 0x7fffffff }; };

//-------------------------------------

template <int NUM_BITS>
class CFixedBitVecBase
{
public:
	bool	IsFixedSize() const { return true; }
	int		GetNumBits(void) const { return NUM_BITS; }
	void	Resize(int numBits, bool bClearAll = false) { Assert(numBits == NUM_BITS); if (bClearAll) Plat_FastMemset(m_Ints, 0, NUM_INTS * sizeof(uint32)); }// for syntatic consistency (for when using templates)

	int 			GetNumDWords() const { return NUM_INTS; }
	uint32* Base() { return m_Ints; }
	const uint32* Base() const { return m_Ints; }

	int		FindNextSetBit(int iStartBit) const; // returns -1 if no set bit was found

protected:
	CFixedBitVecBase() {}
	explicit CFixedBitVecBase(int numBits)
	{
		// doesn't make sense, really. Supported to simplify templates & allow easy replacement of variable 
		Assert(numBits == NUM_BITS);
	}

	void 		ValidateOperand(const CFixedBitVecBase<NUM_BITS>& operand) const { } // no need, compiler does so statically

public: // for test code
	unsigned	GetEndMask() const { return static_cast<unsigned>(BitCountToEndMask_t<NUM_BITS% BITS_PER_INT>::MASK); }

private:
	enum
	{
		NUM_INTS = (NUM_BITS + (BITS_PER_INT - 1)) / BITS_PER_INT
	};

	uint32 m_Ints[(NUM_BITS + (BITS_PER_INT - 1)) / BITS_PER_INT];
};

//-----------------------------------------------------------------------------
//
// The actual classes used
//

// inheritance instead of typedef to allow forward declarations
template< class CountType = unsigned short >
class CVarBitVecT : public CBitVecT< CVarBitVecBase< CountType > >
{
public:
	CVarBitVecT()
	{
	}

	explicit CVarBitVecT(int numBits)
		: CBitVecT<CVarBitVecBase< CountType > >(numBits)
	{
	}
};

class CVarBitVec : public CVarBitVecT<unsigned short>
{
public:
	CVarBitVec() : CVarBitVecT<unsigned short>() {}
	explicit CVarBitVec(int nBitCount) : CVarBitVecT<unsigned short>(nBitCount) {}
};

class CLargeVarBitVec : public CBitVecT< CVarBitVecBase<int> >
{
public:
	CLargeVarBitVec()
	{
	}

	explicit CLargeVarBitVec(int numBits)
		: CBitVecT< CVarBitVecBase<int> >(numBits)
	{
	}
};

//-----------------------------------------------------------------------------

template < int NUM_BITS >
class CBitVec : public CBitVecT< CFixedBitVecBase<NUM_BITS> >
{
public:
	CBitVec()
	{
	}

	explicit CBitVec(int numBits)
		: CBitVecT< CFixedBitVecBase<NUM_BITS> >(numBits)
	{
	}
};


//-----------------------------------------------------------------------------

typedef CBitVec<32> CDWordBitVec;

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline CVarBitVecBase<BITCOUNTTYPE>::CVarBitVecBase()
{
	Plat_FastMemset(this, 0, sizeof(*this));
}

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline CVarBitVecBase<BITCOUNTTYPE>::CVarBitVecBase(int numBits)
{
	Assert(numBits);
	m_numBits = numBits;

	// Figure out how many ints are needed
	m_numInts = CalcNumIntsForBits(numBits);
	m_pInt = NULL;
	AllocInts(m_numInts);
}

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline CVarBitVecBase<BITCOUNTTYPE>::CVarBitVecBase(const CVarBitVecBase<BITCOUNTTYPE>& from)
{
	if (from.m_numInts)
	{
		m_numBits = from.m_numBits;
		m_numInts = from.m_numInts;
		m_pInt = NULL;
		AllocInts(m_numInts);
		memcpy(m_pInt, from.m_pInt, m_numInts * sizeof(int));
	}
	else
		memset(this, 0, sizeof(*this));
}

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline CVarBitVecBase<BITCOUNTTYPE>& CVarBitVecBase<BITCOUNTTYPE>::operator=(const CVarBitVecBase<BITCOUNTTYPE>& from)
{
	Resize(from.GetNumBits());
	if (m_pInt)
		memcpy(m_pInt, from.m_pInt, m_numInts * sizeof(int));
	return (*this);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
// Input  :
// Output :
//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline CVarBitVecBase<BITCOUNTTYPE>::~CVarBitVecBase(void)
{
	FreeInts();
}

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline void CVarBitVecBase<BITCOUNTTYPE>::Attach(uint32* pBits, int numBits)
{
	FreeInts();
	m_numBits = numBits;
	m_numInts = CalcNumIntsForBits(numBits);
	if (m_numInts > 1)
	{
		m_pInt = pBits;
	}
	else
	{
		m_iBitStringStorage = *pBits;
		m_pInt = &m_iBitStringStorage;
		free(pBits);
	}
}

//-----------------------------------------------------------------------------

template <typename BITCOUNTTYPE>
inline bool CVarBitVecBase<BITCOUNTTYPE>::Detach(uint32** ppBits, int* pNumBits)
{
	if (!m_numBits)
	{
		return false;
	}

	*pNumBits = m_numBits;
	if (m_numInts > 1)
	{
		*ppBits = m_pInt;
	}
	else
	{
		*ppBits = (uint32*)malloc(sizeof(uint32));
		**ppBits = m_iBitStringStorage;
		free(m_pInt);
	}

	memset(this, 0, sizeof(*this));
	return true;
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline CBitVecT<BASE_OPS>::CBitVecT()
{
	// undef this is ints are not 4 bytes
	// generate a compile error if sizeof(int) is not 4 (HACK: can't use the preprocessor so use the compiler)

	COMPILE_TIME_ASSERT(sizeof(int) == 4);

	// Initialize bitstring by clearing all bits
	ClearAll();
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline CBitVecT<BASE_OPS>::CBitVecT(int numBits)
	: BASE_OPS(numBits)
{
	// undef this is ints are not 4 bytes
	// generate a compile error if sizeof(int) is not 4 (HACK: can't use the preprocessor so use the compiler)

	COMPILE_TIME_ASSERT(sizeof(int) == 4);

	// Initialize bitstring by clearing all bits
	ClearAll();
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline CBitVecAccessor CBitVecT<BASE_OPS>::operator[](int i)
{
	Assert(i >= 0 && i < this->GetNumBits());
	return CBitVecAccessor(this->Base(), i);
}

template <class BASE_OPS>
inline uint32 CBitVecT<BASE_OPS>::operator[](int i)const
{
	Assert(i >= 0 && i < this->GetNumBits());
	return this->Base()[i >> 5] & (1 << (i & 31));
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Init(int val)
{
	if (this->Base())
		Plat_FastMemset(this->Base(), (val) ? 0xff : 0, this->GetNumDWords() * sizeof(int));
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline uint32 CBitVecT<BASE_OPS>::Get(uint32 bitNum) const
{
	Assert(bitNum < (uint32)this->GetNumBits());
	const uint32* pInt = this->Base() + BitVec_Int(bitNum);
	return (*pInt & BitVec_Bit(bitNum));
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsBitSet(int bitNum) const
{
	Assert(bitNum >= 0 && bitNum < this->GetNumBits());
	const uint32* pInt = this->Base() + BitVec_Int(bitNum);
	return ((*pInt & BitVec_Bit(bitNum)) != 0);
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set(int bitNum)
{
	Assert(bitNum >= 0 && bitNum < this->GetNumBits());
	uint32* pInt = this->Base() + BitVec_Int(bitNum);
	*pInt |= BitVec_Bit(bitNum);
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::TestAndSet(int bitNum)
{
	Assert(bitNum >= 0 && bitNum < this->GetNumBits());
	uint32 bitVecBit = BitVec_Bit(bitNum);
	uint32* pInt = this->Base() + BitVec_Int(bitNum);
	bool bResult = ((*pInt & bitVecBit) != 0);
	*pInt |= bitVecBit;
	return bResult;
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Clear(int bitNum)
{
	Assert(bitNum >= 0 && bitNum < this->GetNumBits());
	uint32* pInt = this->Base() + BitVec_Int(bitNum);
	*pInt &= ~BitVec_Bit(bitNum);
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set(int bitNum, bool bNewVal)
{
	uint32* pInt = this->Base() + BitVec_Int(bitNum);
	uint32 bitMask = BitVec_Bit(bitNum);
	if (bNewVal)
	{
		*pInt |= bitMask;
	}
	else
	{
		*pInt &= ~bitMask;
	}
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set(uint32 offset, uint32 mask)
{
	uint32* pInt = this->Base() + offset;
	*pInt |= mask;
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Clear(uint32 offset, uint32 mask)
{
	uint32* pInt = this->Base() + offset;
	*pInt &= ~mask;
}

//-----------------------------------------------------------------------------

template <class BASE_OPS>
inline uint32 CBitVecT<BASE_OPS>::Get(uint32 offset, uint32 mask)
{
	uint32* pInt = this->Base() + offset;
	return (*pInt & mask);
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::And(const CBitVecT& addStr, CBitVecT* out) const
{
	ValidateOperand(addStr);
	ValidateOperand(*out);

	uint32* pDest = out->Base();
	const uint32* pOperand1 = this->Base();
	const uint32* pOperand2 = addStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		pDest[i] = pOperand1[i] & pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Or(const CBitVecT& orStr, CBitVecT* out) const
{
	ValidateOperand(orStr);
	ValidateOperand(*out);

	uint32* pDest = out->Base();
	const uint32* pOperand1 = this->Base();
	const uint32* pOperand2 = orStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		pDest[i] = pOperand1[i] | pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Xor(const CBitVecT& xorStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = this->Base();
	const uint32* pOperand2 = xorStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		pDest[i] = pOperand1[i] ^ pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Not(CBitVecT* out) const
{
	this->ValidateOperand(*out);

	uint32* pDest = out->Base();
	const uint32* pOperand = this->Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		pDest[i] = ~(pOperand[i]);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Copy a bit string
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::CopyTo(CBitVecT* out) const
{
	out->Resize(this->GetNumBits());

	this->ValidateOperand(*out);
	Assert(out != this);

	memcpy(out->Base(), this->Base(), this->GetNumDWords() * sizeof(int));
}

//-----------------------------------------------------------------------------
// Purpose: Are all bits zero?
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsAllClear(void) const
{
	// Number of available bits may be more than the number
	// actually used, so make sure to mask out unused bits
	// before testing for zero
	(const_cast<CBitVecT*>(this))->Base()[this->GetNumDWords() - 1] &= CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		if (this->Base()[i] != 0)
		{
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Are all bits set?
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsAllSet(void) const
{
	// Number of available bits may be more than the number
	// actually used, so make sure to mask out unused bits
	// before testing for set bits
	(const_cast<CBitVecT*>(this))->Base()[this->GetNumDWords() - 1] |= ~CBitVecT<BASE_OPS>::GetEndMask();  // external semantics of const retained

	for (int i = this->GetNumDWords() - 1; i >= 0; --i)
	{
		if (this->Base()[i] != ~0)
		{
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets all bits
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::SetAll(void)
{
	if (this->Base())
		Plat_FastMemset(this->Base(), 0xff, this->GetNumDWords() * sizeof(int));
}

//-----------------------------------------------------------------------------
// Purpose: Clears all bits
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::ClearAll(void)
{
	if (this->Base())
		Plat_FastMemset(this->Base(), 0, this->GetNumDWords() * sizeof(int));
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Copy(const CBitVecT<BASE_OPS>& other, int nBits)
{
	if (nBits == -1)
	{
		nBits = other.GetNumBits();
	}

	this->Resize(nBits);

	this->ValidateOperand(other);
	Assert(&other != this);

	memcpy(this->Base(), other.Base(), this->GetNumDWords() * sizeof(uint32));
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::Compare(const CBitVecT<BASE_OPS>& other, int nBits) const
{
	if (nBits == -1)
	{
		if (other.GetNumBits() != this->GetNumBits())
		{
			return false;
		}

		nBits = other.GetNumBits();
	}

	if (nBits > other.GetNumBits() || nBits > this->GetNumBits())
	{
		return false;
	}

	(const_cast<CBitVecT*>(this))->Base()[this->GetNumDWords() - 1] &= CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained
	(const_cast<CBitVecT*>(&other))->Base()[this->GetNumDWords() - 1] &= other.CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained

	int nBytes = PAD_NUMBER(nBits, 8) >> 3;

	return (memcmp(this->Base(), other.Base(), nBytes) == 0);
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline uint32 CBitVecT<BASE_OPS>::GetDWord(int i) const
{
	Assert(i >= 0 && i < this->GetNumDWords());
	return this->Base()[i];
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::SetDWord(int i, uint32 val)
{
	Assert(i >= 0 && i < this->GetNumDWords());
	this->Base()[i] = val;
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline uint32 CBitVecT<BASE_OPS>::PopulationCount() const
{
	int nDwordCount = this->GetNumDWords();
	const uint32* pBase = this->Base();
	uint32 nCount = 0;
	for (int i = 0; i < nDwordCount; ++i)
	{
		nCount += ::PopulationCount(pBase[i]);
	}
	return nCount;
}

//-----------------------------------------------------------------------------

inline unsigned GetStartBitMask(int startBit)
{
	static unsigned int g_StartMask[32] =
	{
		0xffffffff,
		0xfffffffe,
		0xfffffffc,
		0xfffffff8,
		0xfffffff0,
		0xffffffe0,
		0xffffffc0,
		0xffffff80,
		0xffffff00,
		0xfffffe00,
		0xfffffc00,
		0xfffff800,
		0xfffff000,
		0xffffe000,
		0xffffc000,
		0xffff8000,
		0xffff0000,
		0xfffe0000,
		0xfffc0000,
		0xfff80000,
		0xfff00000,
		0xffe00000,
		0xffc00000,
		0xff800000,
		0xff000000,
		0xfe000000,
		0xfc000000,
		0xf8000000,
		0xf0000000,
		0xe0000000,
		0xc0000000,
		0x80000000,
	};

	return g_StartMask[startBit & 31];
}


#ifdef _X360
#define BitVec_Bit( bitNum ) GetBitForBitnum( bitNum )
#define BitVec_BitInByte( bitNum ) GetBitForBitnumByte( bitNum )
#else
#define BitVec_Bit( bitNum ) ( 1 << ( (bitNum) & (BITS_PER_INT-1) ) )
#define BitVec_BitInByte( bitNum ) ( 1 << ( (bitNum) & 7 ) )
#endif
#define BitVec_Int( bitNum ) ( (bitNum) >> LOG2_BITS_PER_INT )

inline bool BitVec_IsBitSet(const uint32* pBase, int nBitNum)
{
	const uint32* pInt = pBase + BitVec_Int(nBitNum);
	return ((*pInt & BitVec_Bit(nBitNum)) != 0);
}

inline void Bitvec_Set(uint32* pBits, int nBitNum)
{
	uint32* pInt = pBits + BitVec_Int(nBitNum);
	*pInt |= BitVec_Bit(nBitNum);
}

inline void Bitvec_Unset(uint32* pBits, int nBitNum)
{
	uint32* pInt = pBits + BitVec_Int(nBitNum);
	*pInt &= ~BitVec_Bit(nBitNum);
}

inline bool BitVec_TestAndSetBit(uint32* pBase, int nBitNum)
{
	uint32* pInt = pBase + BitVec_Int(nBitNum);
	uint32 nBits = *pInt;
	uint32 nSetMask = BitVec_Bit(nBitNum);
	*pInt = nBits | nSetMask;
	return (nBits & nSetMask) ? true : false;
}

inline void BitVec_ClearAll(uint32* pDst, int nDWords, uint32 nEndMask)
{
	if (nDWords > 1)
	{
		memset(pDst, 0, (nDWords - 1) * sizeof(uint32));
	}
	pDst[nDWords - 1] &= ~nEndMask;
}

inline void BitVec_ClearAll(uint32* pDst, int nDWords)
{
	memset(pDst, 0, nDWords * sizeof(uint32));
}

inline void BitVec_SetAll(uint32* pDst, int nDWords, uint32 nEndMask)
{
	if (nDWords > 1)
	{
		memset(pDst, 0xff, (nDWords - 1) * sizeof(uint32));
	}
	pDst[nDWords - 1] |= nEndMask;
}

inline void BitVec_SetAll(uint32* pDst, int nDWords)
{
	memset(pDst, 0xff, nDWords * sizeof(uint32));
}

inline void BitVec_Or(uint32* pDst, const uint32* pSrc, int nDWords, uint32 nEndMask)
{
	uint32* pEnd = pDst + nDWords - 1;
	for (; pDst < pEnd; ++pSrc, ++pDst)
	{
		*pDst = *pDst | *pSrc;
	}
	*pDst = *pDst | (*pSrc & nEndMask);
}

inline void BitVec_Or(uint32* pDst, const uint32* pSrc, int nDWords)
{
	uint32* pEnd = pDst + nDWords;
	for (; pDst < pEnd; ++pSrc, ++pDst)
	{
		*pDst = *pDst | *pSrc;
	}
}

inline void BitVec_AndNot(uint32* pDst, const uint32* pSrc, const uint32* pAndNot, int nDWords)
{
	uint32* pEnd = pDst + nDWords;
	for (; pDst < pEnd; ++pSrc, ++pDst, ++pAndNot)
	{
		*pDst = *pSrc & ~(*pAndNot);
	}
}

inline bool BitVec_IsAnySet(const uint32* pDst, int nDWords)
{
	const uint32* pEnd = pDst + nDWords;
	for (; pDst < pEnd; ++pDst)
	{
		if (*pDst != 0)
			return true;
	}
	return false;
}

inline int BitVec_CountNewBits(const uint32* pOld, const uint32* pNew, int nDWords, uint32 nEndMask)
{
	// NOTE - this assumes that any unused bits are unchanged between pOld and pNew

	int nNewBits = 0;

	const uint32* pEnd = pNew + nDWords - 1;
	for (; pNew < pEnd; ++pOld, ++pNew)
	{
		uint32 diff = *pNew & ~*pOld;
		nNewBits += PopulationCount(diff);
	}
	nNewBits += PopulationCount((*pNew & ~*pOld) & nEndMask);

	return nNewBits;
}


template <typename BITCOUNTTYPE>
inline int CVarBitVecBase<BITCOUNTTYPE>::FindNextSetBit(int startBit) const
{
	if (startBit < GetNumBits())
	{
		int wordIndex = BitVec_Int(startBit);
		unsigned int startMask = GetStartBitMask(startBit);
		int lastWord = GetNumDWords() - 1;

		// handle non dword lengths
		if ((GetNumBits() % BITS_PER_INT) != 0)
		{
			unsigned int elem = Base()[wordIndex];
			elem &= startMask;
			if (wordIndex == lastWord)
			{
				elem &= (GetEndMask());
				// there's a bit remaining in this word
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);
			}
			else
			{
				// there's a bit remaining in this word
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);

				// iterate the words
				for (int i = wordIndex + 1; i < lastWord; i++)
				{
					elem = Base()[i];
					if (elem)
						return FirstBitInWord(elem, i << 5);
				}
				elem = Base()[lastWord] & GetEndMask();
				if (elem)
					return FirstBitInWord(elem, lastWord << 5);
			}
		}
		else
		{
			const uint32* RESTRICT pCurElem = Base() + wordIndex;
			unsigned int elem = *pCurElem;
			elem &= startMask;
			do
			{
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);
				++pCurElem;
				elem = *pCurElem;
				++wordIndex;
			} while (wordIndex <= lastWord);
		}

	}

	return -1;
}

template <int NUM_BITS>
inline int CFixedBitVecBase<NUM_BITS>::FindNextSetBit(int startBit) const
{
	if (startBit < NUM_BITS)
	{
		int wordIndex = BitVec_Int(startBit);
		unsigned int startMask = GetStartBitMask(startBit);

		// handle non dword lengths
		if ((NUM_BITS % BITS_PER_INT) != 0)
		{
			unsigned int elem = Base()[wordIndex];
			elem &= startMask;
			if (wordIndex == NUM_INTS - 1)
			{
				elem &= (GetEndMask());
				// there's a bit remaining in this word
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);
			}
			else
			{
				// there's a bit remaining in this word
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);

				// iterate the words
				for (int i = wordIndex + 1; i < NUM_INTS - 1; i++)
				{
					elem = Base()[i];
					if (elem)
						return FirstBitInWord(elem, i << 5);
				}
				elem = Base()[NUM_INTS - 1] & GetEndMask();
				if (elem)
					return FirstBitInWord(elem, (NUM_INTS - 1) << 5);
			}
		}
		else
		{
			const uint32* RESTRICT pCurElem = Base() + wordIndex;
			unsigned int elem = *pCurElem;
			elem &= startMask;
			do
			{
				if (elem)
					return FirstBitInWord(elem, wordIndex << 5);
				++pCurElem;
				elem = *pCurElem;
				++wordIndex;
			} while (wordIndex <= NUM_INTS - 1);
		}

	}

	return -1;
}

//-----------------------------------------------------------------------------
// Unrolled loops for some common sizes

template<>
__forceinline void CBitVecT< CFixedBitVecBase<256> >::And(const CBitVecT& addStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = Base();
	const uint32* pOperand2 = addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
	pDest[3] = pOperand1[3] & pOperand2[3];
	pDest[4] = pOperand1[4] & pOperand2[4];
	pDest[5] = pOperand1[5] & pOperand2[5];
	pDest[6] = pOperand1[6] & pOperand2[6];
	pDest[7] = pOperand1[7] & pOperand2[7];
}

template<>
__forceinline  bool CBitVecT< CFixedBitVecBase<256> >::IsAllClear(void) const
{
	const uint32* pInts = Base();
	return (pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0 && pInts[3] == 0 && pInts[4] == 0 && pInts[5] == 0 && pInts[6] == 0 && pInts[7] == 0);
}

template<>
__forceinline  void CBitVecT< CFixedBitVecBase<256> >::CopyTo(CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
	pDest[3] = pInts[3];
	pDest[4] = pInts[4];
	pDest[5] = pInts[5];
	pDest[6] = pInts[6];
	pDest[7] = pInts[7];
}

template<>
__forceinline  void CBitVecT< CFixedBitVecBase<128> >::And(const CBitVecT& addStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = Base();
	const uint32* pOperand2 = addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
	pDest[3] = pOperand1[3] & pOperand2[3];
}

template<>
__forceinline  bool CBitVecT< CFixedBitVecBase<128> >::IsAllClear(void) const
{
	const uint32* pInts = Base();
	return (pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0 && pInts[3] == 0);
}

template<>
__forceinline  void CBitVecT< CFixedBitVecBase<128> >::CopyTo(CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
	pDest[3] = pInts[3];
}

template<>
inline void CBitVecT< CFixedBitVecBase<96> >::And(const CBitVecT& addStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = Base();
	const uint32* pOperand2 = addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
}

template<>
inline bool CBitVecT< CFixedBitVecBase<96> >::IsAllClear(void) const
{
	const uint32* pInts = Base();
	return (pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0);
}

template<>
inline void CBitVecT< CFixedBitVecBase<96> >::CopyTo(CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
}

template<>
inline void CBitVecT< CFixedBitVecBase<64> >::And(const CBitVecT& addStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = Base();
	const uint32* pOperand2 = addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
}

template<>
inline bool CBitVecT< CFixedBitVecBase<64> >::IsAllClear(void) const
{
	const uint32* pInts = Base();
	return (pInts[0] == 0 && pInts[1] == 0);
}

template<>
inline void CBitVecT< CFixedBitVecBase<64> >::CopyTo(CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
}

template<>
inline void CBitVecT< CFixedBitVecBase<32> >::And(const CBitVecT& addStr, CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pOperand1 = Base();
	const uint32* pOperand2 = addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
}

template<>
inline bool CBitVecT< CFixedBitVecBase<32> >::IsAllClear(void) const
{
	const uint32* pInts = Base();

	return (pInts[0] == 0);
}

template<>
inline void CBitVecT< CFixedBitVecBase<32> >::CopyTo(CBitVecT* out) const
{
	uint32* pDest = out->Base();
	const uint32* pInts = Base();

	pDest[0] = pInts[0];
}

//-----------------------------------------------------------------------------

template <>
inline uint32 CBitVecT< CFixedBitVecBase<32> >::Get(uint32 bitNum) const
{
	return (*Base() & BitVec_Bit(bitNum));
}

//-----------------------------------------------------------------------------

template <>
inline bool CBitVecT< CFixedBitVecBase<32> >::IsBitSet(int bitNum) const
{
	return ((*Base() & BitVec_Bit(bitNum)) != 0);
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Set(int bitNum)
{
	*Base() |= BitVec_Bit(bitNum);
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Clear(int bitNum)
{
	*Base() &= ~BitVec_Bit(bitNum);
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Set(int bitNum, bool bNewVal)
{
	uint32 bitMask = BitVec_Bit(bitNum);
	if (bNewVal)
	{
		*Base() |= bitMask;
	}
	else
	{
		*Base() &= ~bitMask;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Resizes the bit string to a new number of bits
// Input  : resizeNumBits - 
//-----------------------------------------------------------------------------
template <typename BITCOUNTTYPE>
inline void CVarBitVecBase<BITCOUNTTYPE>::Resize(int resizeNumBits, bool bClearAll)
{
	Assert(resizeNumBits >= 0 && ((BITCOUNTTYPE)resizeNumBits == resizeNumBits));

	int newIntCount = CalcNumIntsForBits(resizeNumBits);
	if (newIntCount != GetNumDWords())
	{
		if (Base())
		{
			ReallocInts(newIntCount);
			if (!bClearAll && resizeNumBits >= GetNumBits())
			{
				Base()[GetNumDWords() - 1] &= GetEndMask();
				Plat_FastMemset(Base() + GetNumDWords(), 0, (newIntCount - GetNumDWords()) * sizeof(int));
			}
		}
		else
		{
			// Figure out how many ints are needed
			AllocInts(newIntCount);
			// Initialize bitstring by clearing all bits
			bClearAll = true;
		}

		m_numInts = newIntCount;
	}
	else if (!bClearAll && resizeNumBits >= GetNumBits() && Base())
	{
		Base()[GetNumDWords() - 1] &= GetEndMask();
	}

	if (bClearAll && Base())
	{
		Plat_FastMemset(Base(), 0, newIntCount * sizeof(int));
	}

	// store the new size and end mask
	m_numBits = resizeNumBits;
}

//-----------------------------------------------------------------------------
// Purpose: Allocate the storage for the ints
// Input  : numInts - 
//-----------------------------------------------------------------------------
template <typename BITCOUNTTYPE>
inline void CVarBitVecBase<BITCOUNTTYPE>::AllocInts(int numInts)
{
	Assert(!m_pInt);

	if (numInts == 0)
		return;

	if (numInts == 1)
	{
		m_pInt = &m_iBitStringStorage;
		return;
	}

	m_pInt = (uint32*)malloc(numInts * sizeof(int));
}


//-----------------------------------------------------------------------------
// Purpose: Reallocate the storage for the ints
// Input  : numInts - 
//-----------------------------------------------------------------------------
template <typename BITCOUNTTYPE>
inline void CVarBitVecBase<BITCOUNTTYPE>::ReallocInts(int numInts)
{
	Assert(Base());
	if (numInts == 0)
	{
		FreeInts();
		return;
	}

	if (m_pInt == &m_iBitStringStorage)
	{
		if (numInts != 1)
		{
			m_pInt = ((uint32*)malloc(numInts * sizeof(int)));
			*m_pInt = m_iBitStringStorage;
		}

		return;
	}

	if (numInts == 1)
	{
		m_iBitStringStorage = *m_pInt;
		free(m_pInt);
		m_pInt = &m_iBitStringStorage;
		return;
	}

	m_pInt = (uint32*)realloc(m_pInt, numInts * sizeof(int));
}


//-----------------------------------------------------------------------------
// Purpose: Free storage allocated with AllocInts
//-----------------------------------------------------------------------------
template <typename BITCOUNTTYPE>
inline void CVarBitVecBase<BITCOUNTTYPE>::FreeInts(void)
{
	if (m_numInts > 1)
	{
		free(m_pInt);
	}
	m_pInt = NULL;
}

// ------------------------------------------------------------------------ //
// CBitVecAccessor inlines.
// ------------------------------------------------------------------------ //

inline CBitVecAccessor::CBitVecAccessor(uint32* pDWords, int iBit)
{
	m_pDWords = pDWords;
	m_iBit = iBit;
}


inline void CBitVecAccessor::operator=(int val)
{
	if (val)
		m_pDWords[m_iBit >> 5] |= (1 << (m_iBit & 31));
	else
		m_pDWords[m_iBit >> 5] &= ~(unsigned long)(1 << (m_iBit & 31));
}

inline CBitVecAccessor::operator uint32()
{
	return m_pDWords[m_iBit >> 5] & (1 << (m_iBit & 31));
}


//=============================================================================

#endif // BITVEC_H

class CBoneBitList : public CBitVec<MAXSTUDIOBONES>
{
public:
	inline void MarkBone(int iBone)
	{
		Set(iBone);
	}
	inline bool IsBoneMarked(int iBone)
	{
		return Get(iBone) != 0 ? true : false;
	}
};

class RadianEuler;
class DegreeEuler;
class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a float[4]
public:
	inline Quaternion(void) {

		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline explicit Quaternion(RadianEuler const& angle);
	inline explicit Quaternion(DegreeEuler const& angle);

	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }
	inline void Init(const Vector& vImaginaryPart, float flRealPart) { x = vImaginaryPart.x; y = vImaginaryPart.y; z = vImaginaryPart.z; w = flRealPart; }

	bool IsValid() const;
	void Invalidate();

	bool operator==(const Quaternion& src) const;
	bool operator!=(const Quaternion& src) const;

	inline Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

	// 
	const Vector GetForward()const;
	const Vector GetLeft()const;
	const Vector GetUp()const;

	float* Base() { return (float*)this; }
	const float* Base() const { return (float*)this; }

	// convenience for debugging
	inline void Print() const;

	// Imaginary part
	Vector& ImaginaryPart() { return *(Vector*)this; }
	const Vector& ImaginaryPart() const { return *(Vector*)this; }
	float& RealPart() { return w; }
	float RealPart() const { return w; }
	inline QAngle ToQAngle() const;
	inline /*struct*/ class matrix3x4_t ToMatrix() const;

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	inline Quaternion operator+(void) const { return *this; }
	inline Quaternion operator-(void) const { return Quaternion(-x, -y, -z, -w); }

	float x, y, z, w;
};


class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	inline QuaternionAligned(void) {};
	inline QuaternionAligned(float X, float Y, float Z, float W)
	{
		Init(X, Y, Z, W);
	}
public:
	explicit QuaternionAligned(const Quaternion& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
	}

	QuaternionAligned& operator=(const Quaternion& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
		return *this;
	}
};

struct mstudioaxisinterpbone_t
{
	int				control;// local transformation of this bone used to calc 3 point blend
	int				axis;	// axis to check
	Vector			pos[6];	// X+, X-, Y+, Y-, Z+, Z-
	Quaternion		quat[6];// X+, X-, Y+, Y-, Z+, Z-

	mstudioaxisinterpbone_t() {}
private:
	// No copy constructors allowed
	mstudioaxisinterpbone_t(const mstudioaxisinterpbone_t& vOther);
};


struct mstudioquatinterpinfo_t
{

	float			inv_tolerance;	// 1 / radian angle of trigger influence
	Quaternion		trigger;	// angle to match
	Vector			pos;		// new position
	Quaternion		quat;		// new angle

	mstudioquatinterpinfo_t() {}
private:
	// No copy constructors allowed
	mstudioquatinterpinfo_t(const mstudioquatinterpinfo_t& vOther);
};

struct mstudioquatinterpbone_t
{

	int				control;// local transformation to check
	int				numtriggers;
	int				triggerindex;
	inline mstudioquatinterpinfo_t* pTrigger(int i) const { return  (mstudioquatinterpinfo_t*)(((byte*)this) + triggerindex) + i; };

	mstudioquatinterpbone_t() {}
private:
	// No copy constructors allowed
	mstudioquatinterpbone_t(const mstudioquatinterpbone_t& vOther);
};


#define JIGGLE_IS_FLEXIBLE				0x01
#define JIGGLE_IS_RIGID					0x02
#define JIGGLE_HAS_YAW_CONSTRAINT		0x04
#define JIGGLE_HAS_PITCH_CONSTRAINT		0x08
#define JIGGLE_HAS_ANGLE_CONSTRAINT		0x10
#define JIGGLE_HAS_LENGTH_CONSTRAINT	0x20
#define JIGGLE_HAS_BASE_SPRING			0x40

struct mstudiojigglebone_t
{


	int				flags;

	// general params
	float			Length;					// how from from bone base, along bone, is tip
	float			tipMass;

	// flexible params
	float			yawStiffness;
	float			yawDamping;
	float			pitchStiffness;
	float			pitchDamping;
	float			alongStiffness;
	float			alongDamping;

	// angle constraint
	float			angleLimit;				// maximum deflection of tip in radians

	// yaw constraint
	float			minYaw;					// in radians
	float			maxYaw;					// in radians
	float			yawFriction;
	float			yawBounce;

	// pitch constraint
	float			minPitch;				// in radians
	float			maxPitch;				// in radians
	float			pitchFriction;
	float			pitchBounce;

	// base spring
	float			baseMass;
	float			baseStiffness;
	float			baseDamping;
	float			baseMinLeft;
	float			baseMaxLeft;
	float			baseLeftFriction;
	float			baseMinUp;
	float			baseMaxUp;
	float			baseUpFriction;
	float			baseMinForward;
	float			baseMaxForward;
	float			baseForwardFriction;

private:
	// No copy constructors allowed
	//mstudiojigglebone_t(const mstudiojigglebone_t& vOther);
};

struct mstudioaimatbone_t
{


	int				parent;
	int				aim;		// Might be bone or attach
	Vector			aimvector;
	Vector			upvector;
	Vector			basepos;

	mstudioaimatbone_t() {}
private:
	// No copy constructors allowed
	mstudioaimatbone_t(const mstudioaimatbone_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudiotwistbonetarget_t
{


	int				m_nBone;
	float			m_flWeight;
	Vector			m_vBaseTranslate;
	Quaternion		m_qBaseRotation;

	mstudiotwistbonetarget_t() {}
private:
	// No copy constructors allowed
	mstudiotwistbonetarget_t(const mstudiotwistbonetarget_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudiotwistbone_t
{


	bool			m_bInverse;				// False: Apply child rotation to twist targets True: Apply parent rotation to twist targets
	Vector			m_vUpVector;			// In parent space, projected into plane defined by vector between parent & child
	int				m_nParentBone;
	Quaternion		m_qBaseInv;	// The base rotation of the parent, used if m_bInverse is true
	int				m_nChildBone;

	int				m_nTargetCount;
	int				m_nTargetIndex;
	inline mstudiotwistbonetarget_t* pTarget(int i) const { return (mstudiotwistbonetarget_t*)(((byte*)this) + m_nTargetIndex) + i; }

	mstudiotwistbone_t() {}
private:
	// No copy constructors allowed
	mstudiotwistbone_t(const mstudiotwistbone_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudioconstraintslave_t
{


	int				m_nBone;
	Vector			m_vBasePosition;
	Quaternion		m_qBaseOrientation;

	mstudioconstraintslave_t() {}
private:
	// No copy constructors allowed
	mstudioconstraintslave_t(const mstudioconstraintslave_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudioconstrainttarget_t
{


	int				m_nBone;
	float			m_flWeight;
	Vector			m_vOffset;
	Quaternion		m_qOffset;

	mstudioconstrainttarget_t() {}
private:
	// No copy constructors allowed
	mstudioconstrainttarget_t(const mstudioconstrainttarget_t& vOther);
};


//-----------------------------------------------------------------------------
// Point constraint, slave position matches target position
//-----------------------------------------------------------------------------
struct mstudiopointconstraint_t
{


	mstudioconstraintslave_t m_slave;	// DEFINE_EMBEDDED

	int				m_nTargetCount;
	int				m_nTargetIndex;
	inline mstudioconstrainttarget_t* pTarget(int i) const { return (mstudioconstrainttarget_t*)(((byte*)this) + m_nTargetIndex) + i; }

	mstudiopointconstraint_t() {}
private:
	// No copy constructors allowed
	mstudiopointconstraint_t(const mstudiopointconstraint_t& vOther);
};


//-----------------------------------------------------------------------------
// Orient constraint, slave orientation matches target orientation
//-----------------------------------------------------------------------------
struct mstudioorientconstraint_t
{


	mstudioconstraintslave_t m_slave;	// DEFINE_EMBEDDED

	int				m_nTargetCount;
	int				m_nTargetIndex;
	inline mstudioconstrainttarget_t* pTarget(int i) const { return (mstudioconstrainttarget_t*)(((byte*)this) + m_nTargetIndex) + i; }

	mstudioorientconstraint_t() {}
private:
	// No copy constructors allowed
	mstudioorientconstraint_t(const mstudioorientconstraint_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudioaimconstraint_t
{


	mstudioconstraintslave_t m_slave;	// DEFINE_EMBEDDED

	int				m_nTargetCount;
	int				m_nTargetIndex;
	inline mstudioconstrainttarget_t* pTarget(int i) const { return (mstudioconstrainttarget_t*)(((byte*)this) + m_nTargetIndex) + i; }

	Quaternion		m_qAimOffset;
	Vector			m_vUp;
	short			m_nUpSpaceTarget;
	unsigned char	m_nUpType;
	unsigned char	m_unused;

	mstudioaimconstraint_t() {}
private:
	// No copy constructors allowed
	mstudioaimconstraint_t(const mstudioaimconstraint_t& vOther);
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct mstudioikconstraint_t
{


	mstudioikconstraint_t() {}
private:
	// No copy constructors allowed
	mstudioikconstraint_t(const mstudioaimconstraint_t& vOther);
};


//-----------------------------------------------------------------------------
// Parent constraint, slave position and orientation are updated to behave as children of the target
//-----------------------------------------------------------------------------
struct mstudioparentconstraint_t
{


	mstudioconstraintslave_t m_slave;	// DEFINE_EMBEDDED

	int				m_nTargetCount;
	int				m_nTargetIndex;
	inline mstudioconstrainttarget_t* pTarget(int i) const { return (mstudioconstrainttarget_t*)(((byte*)this) + m_nTargetIndex) + i; }

	mstudioparentconstraint_t() {}
private:
	// No copy constructors allowed
	mstudioparentconstraint_t(const mstudioparentconstraint_t& vOther);
};

class DegreeEuler;
class RadianEuler
{
public:
	inline RadianEuler(void) { }
	inline RadianEuler(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline explicit RadianEuler(Quaternion const& q);
	inline explicit RadianEuler(Vector const& angles);
	inline explicit RadianEuler(DegreeEuler const& angles);

	// Initialization
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

	//	conversion to qangle
	Vector ToQAngle(void) const;
	bool IsValid() const;
	void Invalidate();

	inline float* Base() { return &x; }
	inline const float* Base() const { return &x; }

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	float x, y, z;
};
//-----------------------------------------------------------------------------
// Degree Euler angle aligned to axis (NOT ROLL/PITCH/YAW)
//-----------------------------------------------------------------------------
class DegreeEuler
{
public:
	///\name Initialization 
	//@{
	inline DegreeEuler(void) ///< Create with un-initialized components. If VECTOR_PARANOIA is set, will Init with NANS.
	{
		// Initialize to NAN to catch errors
#ifdef VECTOR_PARANOIA
		x = y = z = VEC_T_NAN;
#endif
	}
	inline DegreeEuler(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline explicit DegreeEuler(Quaternion const& q);
	inline explicit DegreeEuler(Vector const& angles);
	inline explicit DegreeEuler(RadianEuler const& angles);

	// Initialization
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

	inline Vector ToQAngle() const;

	//	conversion to qangle
	bool IsValid() const;
	void Invalidate();

	inline float* Base() { return &x; }
	inline const float* Base() const { return &x; }

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	float x, y, z;
};

class matrix3x4_t;
class Quaternion;

// bones
struct mstudiobone_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void* pProcedure() const { if (procindex == 0) return NULL; else return  (void*)(((byte*)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropidx; }
	inline int			GetSurfaceProp(void) const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};

struct mstudiolinearbone_t
{


	int numbones;

	int flagsindex;
	inline int flags(int i) const { Assert(i >= 0 && i < numbones); return *((int*)(((byte*)this) + flagsindex) + i); };
	inline int* pflags(int i) { Assert(i >= 0 && i < numbones); return ((int*)(((byte*)this) + flagsindex) + i); };

	int	parentindex;
	inline int parent(int i) const { Assert(i >= 0 && i < numbones); return *((int*)(((byte*)this) + parentindex) + i); };

	int	posindex;
	inline const Vector& pos(int i) const { Assert(i >= 0 && i < numbones); return *((Vector*)(((byte*)this) + posindex) + i); };

	int quatindex;
	inline const Quaternion& quat(int i) const { Assert(i >= 0 && i < numbones); return *((Quaternion*)(((byte*)this) + quatindex) + i); };

	int rotindex;
	inline const RadianEuler& rot(int i) const { Assert(i >= 0 && i < numbones); return *((RadianEuler*)(((byte*)this) + rotindex) + i); };

	int posetoboneindex;
	inline const matrix3x4_t& poseToBone(int i) const { Assert(i >= 0 && i < numbones); return *((matrix3x4_t*)(((byte*)this) + posetoboneindex) + i); };

	int	posscaleindex;
	inline const Vector& posscale(int i) const { Assert(i >= 0 && i < numbones); return *((Vector*)(((byte*)this) + posscaleindex) + i); };

	int	rotscaleindex;
	inline const Vector& rotscale(int i) const { Assert(i >= 0 && i < numbones); return *((Vector*)(((byte*)this) + rotscaleindex) + i); };

	int	qalignmentindex;
	inline const Quaternion& qalignment(int i) const { Assert(i >= 0 && i < numbones); return *((Quaternion*)(((byte*)this) + qalignmentindex) + i); };

	int unused[6];

	mstudiolinearbone_t() {}
private:
	// No copy constructors allowed
	mstudiolinearbone_t(const mstudiolinearbone_t& vOther);
};


//-----------------------------------------------------------------------------
// The component of the bone used by mstudioboneflexdriver_t
//-----------------------------------------------------------------------------
enum StudioBoneFlexComponent_t
{
	STUDIO_BONE_FLEX_INVALID = -1,	// Invalid
	STUDIO_BONE_FLEX_TX = 0,		// Translate X
	STUDIO_BONE_FLEX_TY = 1,		// Translate Y
	STUDIO_BONE_FLEX_TZ = 2			// Translate Z
};


//-----------------------------------------------------------------------------
// Component is one of Translate X, Y or Z [0,2] (StudioBoneFlexComponent_t)
//-----------------------------------------------------------------------------
struct mstudioboneflexdrivercontrol_t
{


	int m_nBoneComponent;		// Bone component that drives flex, StudioBoneFlexComponent_t
	int m_nFlexControllerIndex;	// Flex controller to drive
	float m_flMin;				// Min value of bone component mapped to 0 on flex controller
	float m_flMax;				// Max value of bone component mapped to 1 on flex controller

	mstudioboneflexdrivercontrol_t() {}
private:
	// No copy constructors allowed
	mstudioboneflexdrivercontrol_t(const mstudioboneflexdrivercontrol_t& vOther);
};


//-----------------------------------------------------------------------------
// Drive flex controllers from bone components
//-----------------------------------------------------------------------------
struct mstudioboneflexdriver_t
{


	int m_nBoneIndex;			// Bone to drive flex controller
	int m_nControlCount;		// Number of flex controllers being driven
	int m_nControlIndex;		// Index into data where controllers are (relative to this)

	inline mstudioboneflexdrivercontrol_t* pBoneFlexDriverControl(int i) const
	{
		Assert(i >= 0 && i < m_nControlCount);
		return (mstudioboneflexdrivercontrol_t*)(((byte*)this) + m_nControlIndex) + i;
	}

	int unused[3];

	mstudioboneflexdriver_t() {}
private:
	// No copy constructors allowed
	mstudioboneflexdriver_t(const mstudioboneflexdriver_t& vOther);
};

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000	// bone can't spin 360 degrees, all interpolation is Normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS		0x00200000	// Vector48
#define BONE_HAS_SAVEFRAME_ROT64	0x00400000	// Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32	0x00800000	// Quaternion32

// bone controllers
struct mstudiobonecontroller_t
{

	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};

// intersection boxes
struct mstudiobbox_t
{

	int					bone;
	int					group;				// intersection group
	Vector				vecMin;				// bounding box, or the ends of the capsule if flCapsuleRadius > 0 
	Vector				vecMax;
	int					szhitboxnameindex;	// offset to the name of the Hitbox.
	QAngle				angOffsetOrientation;
	float				flCapsuleRadius;
	int				    unused[0x4];

	const char* pszHitboxName() const
	{
		if (szhitboxnameindex == 0)
			return "";

		return ((const char*)this) + szhitboxnameindex;
	}

	mstudiobbox_t() {}

private:
	// No copy constructors allowed
	mstudiobbox_t(const mstudiobbox_t& vOther);
};

// demand loaded sequence groups
struct mstudiomodelgroup_t
{

	int					szlabelindex;	// textual name
	inline char* const pszLabel(void) const { return ((char*)this) + szlabelindex; }
	int					sznameindex;	// file name
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
};

struct mstudiomodelgrouplookup_t
{
	int					modelgroup;
	int					indexwithingroup;
};

// animtags
struct mstudioanimtag_t
{

	int					tag;
	float				cycle;

	int					sztagindex;
	inline char* const pszTagName(void) const { return ((char*)this) + sztagindex; }
};

// events
// NOTE: If you modify this struct you MUST also modify mstudioevent_for_client_server_t in npcevent.h!!!
struct mstudioevent_t
{

	float				cycle;
	int					event;
	int					type;
	inline const char* pszOptions(void) const { return options; }
	char				options[64];

	int					szeventindex;
	inline char* const pszEventName(void) const { return ((char*)this) + szeventindex; }
};

#define	ATTACHMENT_FLAG_WORLD_ALIGN 0x10000

// attachment
struct mstudioattachment_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix3x4_t			local; // attachment point
	int					unused[8];
};

#define IK_SELF 1
#define IK_WORLD 2
#define IK_GROUND 3
#define IK_RELEASE 4
#define IK_ATTACHMENT 5
#define IK_UNLATCH 6

struct mstudioikerror_t
{

	Vector		pos;
	Quaternion	q;

	mstudioikerror_t() {}

private:
	// No copy constructors allowed
	mstudioikerror_t(const mstudioikerror_t& vOther);
};

union mstudioanimvalue_t;

struct mstudiocompressedikerror_t
{

	float	scale[6];
	short	offset[6];
	inline mstudioanimvalue_t* pAnimvalue(int i) const { if (offset[i] > 0) return  (mstudioanimvalue_t*)(((byte*)this) + offset[i]); else return NULL; };
	mstudiocompressedikerror_t() {}

private:
	// No copy constructors allowed
	mstudiocompressedikerror_t(const mstudiocompressedikerror_t& vOther);
};

struct mstudioikrule_t
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
	Quaternion	q;

	int			compressedikerrorindex;
	inline mstudiocompressedikerror_t* pCompressedError() const { return (mstudiocompressedikerror_t*)(((byte*)this) + compressedikerrorindex); };
	int			unused2;

	int			iStart;
	int			ikerrorindex;
	inline mstudioikerror_t* pError(int i) const { return  (ikerrorindex) ? (mstudioikerror_t*)(((byte*)this) + ikerrorindex) + (i - iStart) : NULL; };

	float		start;	// beginning of influence
	float		peak;	// start of full influence
	float		tail;	// end of full influence
	float		end;	// end of all influence

	float		unused3;	// 
	float		contact;	// frame footstep makes ground concact
	float		drop;		// how far down the foot should drop when reaching for IK
	float		top;		// top of the foot box

	int			unused6;
	int			unused7;
	int			unused8;

	int			szattachmentindex;		// name of world attachment
	inline char* const pszAttachment(void) const { return ((char*)this) + szattachmentindex; }

	int			unused[7];

	mstudioikrule_t() {}

private:
	// No copy constructors allowed
	mstudioikrule_t(const mstudioikrule_t& vOther);
};

//=========================================================
// 16 bit float
//=========================================================
const int float32bias = 127;
const int float16bias = 15;

const float maxfloat16bits = 65504.0f;

class float16
{
public:
	// float16() {};
	//float16( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }
	float16& operator=(const unsigned short& other) { m_storage.rawWord = other; return *this; };

	void Init() { m_storage.rawWord = 0; }
	//	float16& operator=(const float16 &other) { m_storage.rawWord = other.m_storage.rawWord; return *this; }
		//	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
	//	operator unsigned short () { return m_storage.rawWord; }
	//	operator float () { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
	unsigned short GetBits() const
	{
		return m_storage.rawWord;
	}
	float GetFloat() const
	{
		return Convert16bitFloatTo32bits(m_storage.rawWord);
	}
	void SetFloat(float in)
	{
		m_storage.rawWord = ConvertFloatTo16bits(in);
	}

	bool IsInfinity() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa == 0;
	}
	bool IsNaN() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa != 0;
	}

	bool operator==(const float16 other) const { return m_storage.rawWord == other.m_storage.rawWord; }
	bool operator!=(const float16 other) const { return m_storage.rawWord != other.m_storage.rawWord; }

	//	bool operator< (const float other) const	   { return GetFloat() < other; }
	//	bool operator> (const float other) const	   { return GetFloat() > other; }

	template< bool BRANCHLESS > // allows you to force branchy/branchless implementation regardless of the current platform
	static unsigned short ConvertFloatTo16bitsNonDefault(float input);
	static float Convert16bitFloatTo32bits(unsigned short input);

	// unfortunately, function templates can't have default template parameters in 2010-era C++ 
	inline static unsigned short ConvertFloatTo16bits(float input)
	{	// default to branchless on ppc and branchy on x86
		return ConvertFloatTo16bitsNonDefault<false>(input);
	}

protected:
	union float32bits
	{
		float rawFloat;
		unsigned __int32 rawAsInt;
		struct
		{
			unsigned int mantissa : 23;
			unsigned int biased_exponent : 8;
			unsigned int sign : 1;
		} bits;
	};

	union float16bits
	{
		unsigned short rawWord;
		struct
		{
			unsigned short mantissa : 10;
			unsigned short biased_exponent : 5;
			unsigned short sign : 1;
		} bits;
	};

	static bool IsNaN(float16bits in)
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa != 0;
	}
	static bool IsInfinity(float16bits in)
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa == 0;
	}

	// 0x0001 - 0x03ff
	float16bits m_storage;
};

struct mstudioikrulezeroframe_t
{
	short		chain;
	short		slot;
	float16		start;	// beginning of influence
	float16		peak;	// start of full influence
	float16		tail;	// end of full influence
	float16		end;	// end of all influence
};


struct mstudioiklock_t
{

	int			chain;
	float		flPosWeight;
	float		flLocalQWeight;
	int			flags;

	int			unused[4];
};


struct mstudiolocalhierarchy_t
{

	int			iBone;			// bone being adjusted
	int			iNewParent;		// the bones new parent

	float		start;			// beginning of influence
	float		peak;			// start of full influence
	float		tail;			// end of full influence
	float		end;			// end of all influence

	int			iStart;			// first frame 

	int			localanimindex;
	inline mstudiocompressedikerror_t* pLocalAnim() const { return (mstudiocompressedikerror_t*)(((byte*)this) + localanimindex); };

	int			unused[4];
};



// animation frames
union mstudioanimvalue_t
{
	struct
	{
		byte	valid;
		byte	total;
	} num;
	short		value;
};

struct mstudioanim_valueptr_t
{

	short	offset[3];
	inline mstudioanimvalue_t* pAnimvalue(int i) const { if (offset[i] > 0) return  (mstudioanimvalue_t*)(((byte*)this) + offset[i]); else return NULL; };
};

#define STUDIO_ANIM_RAWPOS	0x01 // Vector48
#define STUDIO_ANIM_RAWROT	0x02 // Quaternion48
#define STUDIO_ANIM_ANIMPOS	0x04 // mstudioanim_valueptr_t
#define STUDIO_ANIM_ANIMROT	0x08 // mstudioanim_valueptr_t
#define STUDIO_ANIM_DELTA	0x10
#define STUDIO_ANIM_RAWROT2	0x20 // Quaternion64

class Quaternion48
{
public:
	// Construction/destruction:
	Quaternion48(void);
	Quaternion48(float X, float Y, float Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion48& operator=(const Quaternion& vOther);
	operator Quaternion () const;

	//private:
	unsigned short x : 16;
	unsigned short y : 16;
	unsigned short z : 15;
	unsigned short wneg : 1;
};

// per bone per animation DOF and weight pointers, RLE encoded
struct mstudio_rle_anim_t
{

	byte				bone;
	byte				flags;		// weighing options

	// valid for animating data only
	inline byte* pData(void) const { return (((byte*)this) + sizeof(struct mstudio_rle_anim_t)); };
	inline mstudioanim_valueptr_t* pRotV(void) const { return (mstudioanim_valueptr_t*)(pData()); };
	inline mstudioanim_valueptr_t* pPosV(void) const { return (mstudioanim_valueptr_t*)(pData()) + ((flags & STUDIO_ANIM_ANIMROT) != 0); };

	// points to next bone in the list
	short				nextoffset;
	inline mstudio_rle_anim_t* pNext(void) const { if (nextoffset != 0) return  (mstudio_rle_anim_t*)(((byte*)this) + nextoffset); else return NULL; };
};


#define STUDIO_FRAME_CONST_POS	0x01 // Vector48 in constants
#define STUDIO_FRAME_CONST_ROT	0x02 // Quaternion48 in constants
#define STUDIO_FRAME_ANIM_POS	0x04 // Vector48 in framedata
#define STUDIO_FRAME_ANIM_ROT	0x08 // Quaternion48 in framedata
#define STUDIO_FRAME_ANIM_POS2	0x10 // Vector in framedata
#define STUDIO_FRAME_CONST_POS2	0x20 // Vector in constants
#define STUDIO_FRAME_CONST_ROT2	0x40 // Quaternion48S in constants
#define STUDIO_FRAME_ANIM_ROT2	0x80 // Quaternion48S in framedata

struct mstudio_frame_anim_t
{


	inline byte* pBoneFlags(void) const { return (((byte*)this) + sizeof(struct mstudio_frame_anim_t)); };

	int				constantsoffset;
	inline byte* pConstantData(void) const { return (((byte*)this) + constantsoffset); };

	int				frameoffset;
	int 			framelength;
	inline byte* pFrameData(int iFrame) const { return (((byte*)this) + frameoffset + iFrame * framelength); };

	int				unused[3];
};



struct mstudiomovement_t
{

	int					endframe;
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	Vector				vector;		// movement vector relative to this blocks initial angle
	Vector				position;	// relative to start of animation???

	mstudiomovement_t() {}
private:
	// No copy constructors allowed
	mstudiomovement_t(const mstudiomovement_t& vOther);
};

// used for piecewise loading of animation data
struct mstudioanimblock_t
{

	int					datastart;
	int					dataend;
};

struct mstudioanimsections_t
{

	int					animblock;
	int					animindex;
};

struct mstudioanimdesc_t
{

	int					baseptr;
	inline studiohdr_t* pStudiohdr(void) const { return (studiohdr_t*)(((byte*)this) + baseptr); }

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					numframes;

	// piecewise movement
	int					nummovements;
	int					movementindex;
	inline mstudiomovement_t* const pMovement(int i) const { return (mstudiomovement_t*)(((byte*)this) + movementindex) + i; };

	int					ikrulezeroframeindex;
	mstudioikrulezeroframe_t* pIKRuleZeroFrame(int i) const { if (ikrulezeroframeindex) return (mstudioikrulezeroframe_t*)(((byte*)this) + ikrulezeroframeindex) + i; else return NULL; };

	int					unused1[5];			// remove as appropriate (and zero if loading older versions)	

	int					animblock;
	int					animindex;	 // non-zero when anim data isn't in sections
	byte* pAnimBlock(int block, int index, bool preloadIfMissing = true) const; // returns pointer to a specific anim block (local or external)
	bool hasAnimBlockBeenPreloaded(int block) const;
	byte* pAnim(int* piFrame, float& flStall) const; // returns pointer to data and new frame index
	byte* pAnim(int* piFrame) const; // returns pointer to data and new frame index

	int					numikrules;
	int					ikruleindex;	// non-zero when IK rule is stored in the mdl
	int					animblockikruleindex; // non-zero when IK data is stored in animblock file
	mstudioikrule_t* pIKRule(int i) const;

	int					numlocalhierarchy;
	int					localhierarchyindex;
	mstudiolocalhierarchy_t* pHierarchy(int i) const;

	int					sectionindex;
	int					sectionframes; // number of frames used in each fast lookup section, zero if not used
	inline mstudioanimsections_t* const pSection(int i) const { return (mstudioanimsections_t*)(((byte*)this) + sectionindex) + i; }

	short				zeroframespan;	// frames per span
	short				zeroframecount; // number of spans
	int					zeroframeindex;
	byte* pZeroFrameData() const { if (zeroframeindex) return (((byte*)this) + zeroframeindex); else return NULL; };
	mutable float		zeroframestalltime;		// saved during read stalls

	mstudioanimdesc_t() {}
private:
	// No copy constructors allowed
	mstudioanimdesc_t(const mstudioanimdesc_t& vOther);
};

struct mstudioikrule_t;

struct mstudioautolayer_t
{

	//private:
	short				iSequence;
	short				iPose;
	//public:
	int					flags;
	float				start;	// beginning of influence
	float				peak;	// start of full influence
	float				tail;	// end of full influence
	float				end;	// end of all influence
};

struct mstudioactivitymodifier_t
{
	int					sznameindex;
	inline char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : NULL; }
};

// sequence descriptions
struct mstudioseqdesc_t
{

	int					baseptr;
	inline studiohdr_t* pStudiohdr(void) const { return (studiohdr_t*)(((byte*)this) + baseptr); }

	int					szlabelindex;
	inline char* const pszLabel(void) const { return ((char*)this) + szlabelindex; }

	int					szactivitynameindex;
	inline char* const pszActivityName(void) const { return ((char*)this) + szactivitynameindex; }

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;
	inline mstudioevent_t* pEvent(int i) const { Assert(i >= 0 && i < numevents); return (mstudioevent_t*)(((byte*)this) + eventindex) + i; };

	Vector				vecMin;		// per sequence bounding box
	Vector				vecMax;

	int					numblends;

	// Index into array of shorts which is groupsize[0] x groupsize[1] in Length
	int					animindexindex;

	inline int			anim(int x, int y) const
	{
		if (x >= groupsize[0])
		{
			x = groupsize[0] - 1;
		}

		if (y >= groupsize[1])
		{
			y = groupsize[1] - 1;
		}

		int offset = y * groupsize[0] + x;
		short* blends = (short*)(((byte*)this) + animindexindex);
		int value = (int)blends[offset];
		return value;
	}

	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[2];
	int					paramindex[2];	// X, Y, Z, XR, YR, ZR
	float				paramstart[2];	// local (0..1) starting value
	float				paramend[2];	// local (0..1) ending value
	int					paramparent;

	float				fadeintime;		// ideal Cross fate in time (0.2 default)
	float				fadeouttime;	// ideal Cross fade out time (0.2 default)

	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait

	float				lastframe;		// frame that should generation EndOfSequence

	int					nextseq;		// auto advancing sequences
	int					pose;			// index of Delta animation between end and nextseq

	int					numikrules;

	int					numautolayers;	//
	int					autolayerindex;
	inline mstudioautolayer_t* pAutolayer(int i) const { Assert(i >= 0 && i < numautolayers); return (mstudioautolayer_t*)(((byte*)this) + autolayerindex) + i; };

	int					weightlistindex;
	inline float* pBoneweight(int i) const { return ((float*)(((byte*)this) + weightlistindex) + i); };
	inline float		weight(int i) const { return *(pBoneweight(i)); };

	// FIXME: make this 2D instead of 2x1D arrays
	int					posekeyindex;
	float* pPoseKey(int iParam, int iAnim) const { return (float*)(((byte*)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey(int iParam, int iAnim) const { return *(pPoseKey(iParam, iAnim)); }

	int					numiklocks;
	int					iklockindex;
	inline mstudioiklock_t* pIKLock(int i) const { Assert(i >= 0 && i < numiklocks); return (mstudioiklock_t*)(((byte*)this) + iklockindex) + i; };

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					cycleposeindex;		// index of pose parameter to use as cycle index

	int					activitymodifierindex;
	int					numactivitymodifiers;
	inline mstudioactivitymodifier_t* pActivityModifier(int i) const { Assert(i >= 0 && i < numactivitymodifiers); return activitymodifierindex != 0 ? (mstudioactivitymodifier_t*)(((byte*)this) + activitymodifierindex) + i : NULL; };

	int					animtagindex;
	int					numanimtags;
	inline mstudioanimtag_t* pAnimTag(int i) const { Assert(i >= 0 && i < numanimtags); return (mstudioanimtag_t*)(((byte*)this) + animtagindex) + i; };

	int					rootDriverIndex;

	int					unused[2];		// remove/add as appropriate (grow back to 8 ints on version change!)

	mstudioseqdesc_t() {}
private:
	// No copy constructors allowed
	mstudioseqdesc_t(const mstudioseqdesc_t& vOther);
};

struct mstudioposeparamdesc_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int					flags;	// ????
	float				start;	// starting value
	float				end;	// ending value
	float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

struct mstudioflexdesc_t
{

	int					szFACSindex;
	inline char* const pszFACS(void) const { return ((char*)this) + szFACSindex; }
};

struct mstudioflexcontroller_t
{

	int					sztypeindex;
	inline char* const pszType(void) const { return ((char*)this) + sztypeindex; }
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	mutable int			localToGlobal;	// remapped at load time to master list
	float				min;
	float				max;
};

enum FlexControllerRemapType_t
{
	FLEXCONTROLLER_REMAP_PASSTHRU = 0,
	FLEXCONTROLLER_REMAP_2WAY,	// Control 0 -> ramps from 1-0 from 0->0.5. Control 1 -> ramps from 0-1 from 0.5->1
	FLEXCONTROLLER_REMAP_NWAY,	// StepSize = 1 / (control count-1) Control n -> ramps from 0-1-0 from (n-1)*StepSize to n*StepSize to (n+1)*StepSize. A second control is needed to specify amount to use 
	FLEXCONTROLLER_REMAP_EYELID
};

class CStudioHdr;
struct mstudioflexcontrollerui_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }

	// These are used like a union to save space
	// Here are the possible configurations for a UI controller
	//
	// SIMPLE NON-STEREO:	0: control	1: unused	2: unused
	// STEREO:				0: left		1: right	2: unused
	// NWAY NON-STEREO:		0: control	1: unused	2: value
	// NWAY STEREO:			0: left		1: right	2: value

	int					szindex0;
	int					szindex1;
	int					szindex2;

	inline const mstudioflexcontroller_t* pController(void) const
	{
		return !stereo ? (mstudioflexcontroller_t*)((char*)this + szindex0) : NULL;
	}
	inline char* const	pszControllerName(void) const { return !stereo ? pController()->pszName() : NULL; }
	inline int			controllerIndex(const CStudioHdr& cStudioHdr) const;

	inline const mstudioflexcontroller_t* pLeftController(void) const
	{
		return stereo ? (mstudioflexcontroller_t*)((char*)this + szindex0) : NULL;
	}
	inline char* const	pszLeftName(void) const { return stereo ? pLeftController()->pszName() : NULL; }
	inline int			leftIndex(const CStudioHdr& cStudioHdr) const;

	inline const mstudioflexcontroller_t* pRightController(void) const
	{
		return stereo ? (mstudioflexcontroller_t*)((char*)this + szindex1) : NULL;
	}
	inline char* const	pszRightName(void) const { return stereo ? pRightController()->pszName() : NULL; }
	inline int			rightIndex(const CStudioHdr& cStudioHdr) const;

	inline const mstudioflexcontroller_t* pNWayValueController(void) const
	{
		return remaptype == FLEXCONTROLLER_REMAP_NWAY ? (mstudioflexcontroller_t*)((char*)this + szindex2) : NULL;
	}
	inline char* const	pszNWayValueName(void) const { return remaptype == FLEXCONTROLLER_REMAP_NWAY ? pNWayValueController()->pszName() : NULL; }
	inline int			nWayValueIndex(const CStudioHdr& cStudioHdr) const;

	// Number of controllers this ui description contains, 1, 2 or 3
	inline int			Count() const { return (stereo ? 2 : 1) + (remaptype == FLEXCONTROLLER_REMAP_NWAY ? 1 : 0); }
	inline const mstudioflexcontroller_t* pController(int index) const;

	unsigned char		remaptype;	// See the FlexControllerRemapType_t enum
	bool				stereo;		// Is this a stereo control?
	byte				unused[2];
};

// this is the memory image of vertex anims (16-bit fixed point)
struct mstudiovertanim_t
{

	unsigned short		index;
	byte				speed;	// 255/max_length_in_flex
	byte				side;	// 255/left_right

protected:
	union
	{
		short			Delta[3];
		float16			flDelta[3];
	};

	union
	{
		short			ndelta[3];
		float16			flNDelta[3];
	};

public:
	inline void ConvertToFixed(float flVertAnimFixedPointScale)
	{
		Delta[0] = (short)(flDelta[0].GetFloat() / flVertAnimFixedPointScale);
		Delta[1] = (short)(flDelta[1].GetFloat() / flVertAnimFixedPointScale);
		Delta[2] = (short)(flDelta[2].GetFloat() / flVertAnimFixedPointScale);
		ndelta[0] = (short)(flNDelta[0].GetFloat() / flVertAnimFixedPointScale);
		ndelta[1] = (short)(flNDelta[1].GetFloat() / flVertAnimFixedPointScale);
		ndelta[2] = (short)(flNDelta[2].GetFloat() / flVertAnimFixedPointScale);
	}

	inline Vector GetDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(Delta[0] * flVertAnimFixedPointScale, Delta[1] * flVertAnimFixedPointScale, Delta[2] * flVertAnimFixedPointScale);
	}
	inline Vector GetNDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(ndelta[0] * flVertAnimFixedPointScale, ndelta[1] * flVertAnimFixedPointScale, ndelta[2] * flVertAnimFixedPointScale);
	}
	inline Vector GetDeltaFloat()
	{
		return Vector(flDelta[0].GetFloat(), flDelta[1].GetFloat(), flDelta[2].GetFloat());
	}
	inline Vector GetNDeltaFloat()
	{
		return Vector(flNDelta[0].GetFloat(), flNDelta[1].GetFloat(), flNDelta[2].GetFloat());
	}
	inline void SetDeltaFixed(const Vector& vInput, float flVertAnimFixedPointScale)
	{
		Delta[0] = (short)(vInput.x / flVertAnimFixedPointScale);
		Delta[1] = (short)(vInput.y / flVertAnimFixedPointScale);
		Delta[2] = (short)(vInput.z / flVertAnimFixedPointScale);
	}
	inline void SetNDeltaFixed(const Vector& vInputNormal, float flVertAnimFixedPointScale)
	{
		ndelta[0] = (short)(vInputNormal.x / flVertAnimFixedPointScale);
		ndelta[1] = (short)(vInputNormal.y / flVertAnimFixedPointScale);
		ndelta[2] = (short)(vInputNormal.z / flVertAnimFixedPointScale);
	}

	// Ick...can also force fp16 data into this structure for writing to file in legacy format...
	inline void SetDeltaFloat(const Vector& vInput)
	{
		flDelta[0].SetFloat(vInput.x);
		flDelta[1].SetFloat(vInput.y);
		flDelta[2].SetFloat(vInput.z);
	}
	inline void SetNDeltaFloat(const Vector& vInputNormal)
	{
		flNDelta[0].SetFloat(vInputNormal.x);
		flNDelta[1].SetFloat(vInputNormal.y);
		flNDelta[2].SetFloat(vInputNormal.z);
	}

	class CSortByIndex
	{
	public:
		bool operator()(const mstudiovertanim_t& left, const mstudiovertanim_t& right)const
		{
			return left.index < right.index;
		}
	};
	friend class CSortByIndex;

	mstudiovertanim_t() {}
	//private:
	// No copy constructors allowed, but it's needed for std::sort()
	//	mstudiovertanim_t(const mstudiovertanim_t& vOther);
};

// this is the memory image of vertex anims (16-bit fixed point)
struct mstudiovertanim_wrinkle_t : public mstudiovertanim_t
{
	short	wrinkledelta;

	inline float GetWrinkleDeltaFixed(float flVertAnimFixedPointScale)
	{
		return wrinkledelta * flVertAnimFixedPointScale;
	}
};

enum StudioVertAnimType_t
{
	STUDIO_VERT_ANIM_NORMAL = 0,
	STUDIO_VERT_ANIM_WRINKLE,
};

struct mstudioflex_t
{

	int					flexdesc;	// input value

	float				target0;	// zero
	float				target1;	// one
	float				target2;	// one
	float				target3;	// zero

	int					numverts;
	int					vertindex;

	inline	mstudiovertanim_t* pVertanim(int i) const { Assert(vertanimtype == STUDIO_VERT_ANIM_NORMAL); return (mstudiovertanim_t*)(((byte*)this) + vertindex) + i; };
	inline	mstudiovertanim_wrinkle_t* pVertanimWrinkle(int i) const { Assert(vertanimtype == STUDIO_VERT_ANIM_WRINKLE); return  (mstudiovertanim_wrinkle_t*)(((byte*)this) + vertindex) + i; };

	inline	byte* pBaseVertanim() const { return ((byte*)this) + vertindex; };
	inline	int	VertAnimSizeBytes() const { return (vertanimtype == STUDIO_VERT_ANIM_NORMAL) ? sizeof(mstudiovertanim_t) : sizeof(mstudiovertanim_wrinkle_t); }

	int					flexpair;	// second flex desc
	unsigned char		vertanimtype;	// See StudioVertAnimType_t
	unsigned char		unusedchar[3];
	int					unused[6];

};

struct mstudioflexop_t
{

	int		op;
	union
	{
		int		index;
		float	value;
	} d;
};

struct mstudioflexrule_t
{

	int					flex;
	int					numops;
	int					opindex;
	inline mstudioflexop_t* iFlexOp(int i) const { return  (mstudioflexop_t*)(((byte*)this) + opindex) + i; };
};

// 16 bytes
struct mstudioboneweight_t
{

	float	weight[MAX_NUM_BONES_PER_VERT];
	byte	bone[MAX_NUM_BONES_PER_VERT];
	byte	numbones;

	//	byte	material;
	//	short	firstref;
	//	short	lastref;
};

// NOTE: This is exactly 48 bytes
struct mstudiovertex_t
{

	mstudioboneweight_t	m_BoneWeights;
	Vector				m_vecPosition;
	Vector				m_vecNormal;
	Vector2D			m_vecTexCoord;

	mstudiovertex_t() {}

private:
	// No copy constructors allowed
	mstudiovertex_t(const mstudiovertex_t& vOther);
};

// skin info
struct mstudiotexture_t
{

	int						sznameindex;
	inline char* const		pszName(void) const { return ((char*)this) + sznameindex; }
	int						flags;
	int						used;
	int						unused1;

#ifdef PLATFORM_64BITS
	//Having pointers in here really messes up 64 bit. these are only used
	//on by studiomdl though, will need to figure this out if we want
	//to port studiomdl to 64 bit.
	int						unused[12];
#else
	mutable IMaterial* material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
	mutable void* clientmaterial;	// gary, replace with client material pointer if used
	int						unused[10];
#endif

};

// eyeball
struct mstudioeyeball_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int		bone;
	Vector	org;
	float	zoffset;
	float	radius;
	Vector	up;
	Vector	forward;
	int		texture;

	int		unused1;
	float	iris_scale;
	int		unused2;

	int		upperflexdesc[3];	// index of raiser, neutral, and lowerer flexdesc that is set by flex controllers
	int		lowerflexdesc[3];
	float	uppertarget[3];		// angle (radians) of raised, neutral, and lowered lid positions
	float	lowertarget[3];

	int		upperlidflexdesc;	// index of flex desc that actual lid flexes look to
	int		lowerlidflexdesc;
	int		unused[4];			// These were used before, so not guaranteed to be 0
	bool	m_bNonFACS;			// Never used before version 44
	char	unused3[3];
	int		unused4[7];

	mstudioeyeball_t() {}
private:
	// No copy constructors allowed
	mstudioeyeball_t(const mstudioeyeball_t& vOther);
};

// ikinfo
struct mstudioiklink_t
{

	int		bone;
	Vector	kneeDir;	// ideal bending direction (per link, if applicable)
	Vector	unused0;	// unused

	mstudioiklink_t() {}
private:
	// No copy constructors allowed
	mstudioiklink_t(const mstudioiklink_t& vOther);
};

struct mstudioikchain_t
{

	int				sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int				linktype;
	int				numlinks;
	int				linkindex;
	inline mstudioiklink_t* pLink(int i) const { return (mstudioiklink_t*)(((byte*)this) + linkindex) + i; };
	// FIXME: add unused entries
};

struct mstudioiface_t
{
	mstudioiface_t()
	{
		a = b = c = d = 0xFFFF;
	}

	unsigned short a, b, c, d;		// Indices to vertices (If d is 0xFFFF, this is a Triangle, else it's a quad)
};

enum ExtraVertexAttributeType_t
{
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD0 = 0,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD1,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD2,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD3,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD4,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD5,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD6,
	STUDIO_EXTRA_ATTRIBUTE_TEXCOORD7
};

struct ExtraVertexAttributesHeader_t
{
	int		m_count; // Number of individual extra attribute chunks
	int		m_totalbytes; // Total size of extra attribute data (all chunks plus header and index)
};

struct ExtraVertexAttributeIndex_t
{
	ExtraVertexAttributeType_t	m_Type;
	int							m_offset;
	int							m_bytes; //bytes per vertex
};

struct mstudiomodel_t;

struct mstudio_modelvertexdata_t
{

	Vector* Position(int i) const;
	Vector* Normal(int i) const;
	Vector* TangentS(int i) const;
	void* ExtraData(ExtraVertexAttributeType_t type) const;
	Vector2D* Texcoord(int i) const;
	mstudioboneweight_t* BoneWeights(int i) const;
	mstudiovertex_t* Vertex(int i) const;
	bool				HasTangentData(void) const;
	bool				HasExtraData(void) const;
	int					GetGlobalVertexIndex(int i) const;
	int					GetGlobalTangentIndex(int i) const;

	// base of external vertex data stores
	const void* pVertexData;
	const void* pTangentData;
	const void* pExtraData;
};

#ifdef PLATFORM_64BITS
// 64b - match 32-bit packing
#pragma pack( push, 4 )
#endif

// If you want to embed a pointer into one of the structures that is serialized, use this class! It will ensure that the pointers consume the 
// right amount of space and work correctly across 32 and 64 bit. It also makes sure that there is no surprise about how large the structure
// is when placed in the middle of another structure, and supports Intel's desired behavior on 64-bit that pointers are always 8-byte aligned.
#pragma pack( push, 4 )
template < class T >
struct serializedstudioptr_t
{
	T* m_Data;
#ifndef PLATFORM_64BITS
	int padding;
#endif

	serializedstudioptr_t()
	{
		m_Data = nullptr;
#if _DEBUG && !defined( PLATFORM_64BITS )
		padding = 0;
#endif
	}

	inline operator T* () { return m_Data; }
	inline operator const T* () const { return m_Data; }

	inline       T* operator->() { return m_Data; }
	inline const T* operator->() const { return m_Data; }

	inline T* operator=(T* ptr) { return m_Data = ptr; }

};
#pragma pack( pop )

struct mstudio_meshvertexdata_t
{

	Vector* Position(int i) const;
	Vector* Normal(int i) const;
	Vector2D* TangentS(int i) const;
	Vector2D* Texcoord(int i) const;
	mstudioboneweight_t* BoneWeights(int i) const;
	mstudiovertex_t* Vertex(int i) const;
	bool				HasTangentData(void) const;
	int					GetModelVertexIndex(int i) const;
	int					GetGlobalVertexIndex(int i) const;

	// indirection to this mesh's model's vertex data
	int					unused_modelvertexdata; // 64b - Moved to follow num_LOD_Vertexes. 

	// used for fixup calcs when culling top level lods
	// expected number of mesh verts at desired lod
	int					numLODVertexes[MAX_NUM_LODS];

	serializedstudioptr_t< const mstudio_modelvertexdata_t >	modelvertexdata;
};

struct mstudiomesh_t
{

	int					material;

	int					modelindex;
	mstudiomodel_t* pModel() const;

	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexoffset;		// vertex mstudiovertex_t

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
	const mstudio_meshvertexdata_t* GetVertexData(void* pModelData = NULL);
	const thinModelVertices_t* GetThinVertexData(void* pModelData = NULL);

	int					numflexes;			// vertex animation
	int					flexindex;
	inline mstudioflex_t* pFlex(int i) const { return (mstudioflex_t*)(((byte*)this) + flexindex) + i; };

	// special codes for material operations
	int					materialtype;
	int					materialparam;

	// a unique ordinal for this mesh
	int					meshid;

	Vector				center;

	mstudio_meshvertexdata_t vertexdata;

	int					unused[6]; // remove as appropriate

	mstudiomesh_t() {}
private:
	// No copy constructors allowed
	mstudiomesh_t(const mstudiomesh_t& vOther);
};

// studio models
struct mstudiomodel_t
{

	inline const char* pszName(void) const { return name; }
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummeshes;
	int					meshindex;
	inline mstudiomesh_t* pMesh(int i) const { return (mstudiomesh_t*)(((byte*)this) + meshindex) + i; };

	// cache purposes
	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexindex;		// vertex Vector
	int					tangentsindex;		// tangents Vector

	// These functions are defined in application-specific code:
	const vertexFileHeader_t* CacheVertexData(void* pModelData);

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
	const mstudio_modelvertexdata_t* GetVertexData(void* pModelData = NULL);
	const thinModelVertices_t* GetThinVertexData(void* pModelData = NULL);

	int					numattachments;
	int					attachmentindex;

	int					numeyeballs;
	int					eyeballindex;
	inline  mstudioeyeball_t* pEyeball(int i) { return (mstudioeyeball_t*)(((byte*)this) + eyeballindex) + i; };

	mstudio_modelvertexdata_t vertexdata;

#ifdef PLATFORM_64BITS
	int					unused[4];		// 64b - mstudio_modelvertexdata_t has 3 naked pointers. 
#else
	int					unused[7];		// remove as appropriate
#endif
};

#ifdef PLATFORM_64BITS
#pragma pack( pop )
#endif

inline bool mstudio_modelvertexdata_t::HasTangentData(void) const
{
	return pTangentData != NULL;
}

inline bool mstudio_modelvertexdata_t::HasExtraData(void) const
{
	return pExtraData != NULL;
}

inline int mstudio_modelvertexdata_t::GetGlobalVertexIndex(int i) const
{
	mstudiomodel_t* modelptr = (mstudiomodel_t*)((byte*)this - offsetof(mstudiomodel_t, vertexdata));
	Assert((modelptr->vertexindex % sizeof(mstudiovertex_t)) == 0);
	return (i + (modelptr->vertexindex / sizeof(mstudiovertex_t)));
}

inline int mstudio_modelvertexdata_t::GetGlobalTangentIndex(int i) const
{
	mstudiomodel_t* modelptr = (mstudiomodel_t*)((byte*)this - offsetof(mstudiomodel_t, vertexdata));
	Assert((modelptr->tangentsindex % sizeof(Vector2D)) == 0);
	return (i + (modelptr->tangentsindex / sizeof(Vector2D)));
}

inline mstudiovertex_t* mstudio_modelvertexdata_t::Vertex(int i) const
{
	return (mstudiovertex_t*)pVertexData + GetGlobalVertexIndex(i);
}

inline Vector* mstudio_modelvertexdata_t::Position(int i) const
{
	return &Vertex(i)->m_vecPosition;
}

inline Vector* mstudio_modelvertexdata_t::Normal(int i) const
{
	return &Vertex(i)->m_vecNormal;
}

inline void* mstudio_modelvertexdata_t::ExtraData(ExtraVertexAttributeType_t type) const
{
	ExtraVertexAttributesHeader_t* pHeader = (ExtraVertexAttributesHeader_t*)pExtraData;
	ExtraVertexAttributeIndex_t* pIndex = (ExtraVertexAttributeIndex_t*)((byte*)pExtraData + sizeof(ExtraVertexAttributesHeader_t));
	for (int i = 0; i < pHeader->m_count; ++i)
	{
		if (pIndex[i].m_Type == type)
		{
			return (byte*)pExtraData + pIndex[i].m_offset;
		}
	}
	return NULL;
}

inline mstudioboneweight_t* mstudio_modelvertexdata_t::BoneWeights(int i) const
{
	return &Vertex(i)->m_BoneWeights;
}

inline bool mstudio_meshvertexdata_t::HasTangentData(void) const
{
	return modelvertexdata->HasTangentData();
}

inline int mstudio_meshvertexdata_t::GetModelVertexIndex(int i) const
{
	mstudiomesh_t* meshptr = (mstudiomesh_t*)((byte*)this - offsetof(mstudiomesh_t, vertexdata));
	return meshptr->vertexoffset + i;
}

inline int mstudio_meshvertexdata_t::GetGlobalVertexIndex(int i) const
{
	return modelvertexdata->GetGlobalVertexIndex(GetModelVertexIndex(i));
}

inline Vector* mstudio_meshvertexdata_t::Position(int i) const
{
	return modelvertexdata->Position(GetModelVertexIndex(i));
};

inline Vector* mstudio_meshvertexdata_t::Normal(int i) const
{
	return modelvertexdata->Normal(GetModelVertexIndex(i));
};

inline mstudioboneweight_t* mstudio_meshvertexdata_t::BoneWeights(int i) const
{
	return modelvertexdata->BoneWeights(GetModelVertexIndex(i));
};

inline mstudiovertex_t* mstudio_meshvertexdata_t::Vertex(int i) const
{
	return modelvertexdata->Vertex(GetModelVertexIndex(i));
}

// a group of studio model data
enum studiomeshgroupflags_t
{
	MESHGROUP_IS_HWSKINNED = 0x2,
	MESHGROUP_IS_DELTA_FLEXED = 0x4
};


// ----------------------------------------------------------
// Runtime stuff
// ----------------------------------------------------------

struct studiomeshgroup_t
{
	IMesh* m_pMesh;
	int				m_NumStrips;
	int				m_Flags;			// see studiomeshgroupflags_t
	OptimizedModel::StripHeader_t* m_pStripData;
	unsigned short* m_pGroupIndexToMeshIndex;
	int				m_NumVertices;
	int* m_pUniqueFaces;	// for performance measurements
	unsigned short* m_pIndices;
	unsigned short* m_pTopologyIndices;
	bool			m_MeshNeedsRestore;
	short			m_ColorMeshID;
	IMorph* m_pMorph;

	inline unsigned short MeshIndex(int i) const { return m_pGroupIndexToMeshIndex[m_pIndices[i]]; }
};


// studio model data
struct studiomeshdata_t
{
	int					m_NumGroup;
	studiomeshgroup_t* m_pMeshGroup;
};

struct studioloddata_t
{
	// not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
	//int					m_NumMeshes; 
	studiomeshdata_t* m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
	float				m_SwitchPoint;
	// one of these for each lod since we can switch to simpler materials on lower lods.
	int					numMaterials;
	IMaterial** ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
	// hack - this needs to go away.
	int* pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */
#ifndef _CERT
	int					m_NumFaces;	/* Total face count for this LOD */
#endif // !_CERT

	// For decals on hardware morphing, we must actually do hardware skinning
	// For this to work, we have to hope that the total # of bones used by
	// hw flexed verts is < than the max possible for the dx level we're running under
	int* m_pHWMorphDecalBoneRemap;
	int					m_nDecalBoneCount;
};

struct studiohwdata_t
{
	int					m_RootLOD;	// calced and clamped, nonzero for lod culling
	int					m_NumLODs;
	studioloddata_t* m_pLODs;
	int					m_NumStudioMeshes;

	inline float LODMetric(float unitSphereSize) const { return (unitSphereSize != 0.0f) ? (100.0f / unitSphereSize) : 0.0f; }
	inline int GetLODForMetric(float lodMetric) const
	{
		if (!m_NumLODs)
			return 0;

		// On low GPU levels, we pull in the LOD transitions with a scale factor
		// Always slam the LOD transition scale factor to 1.0f in CS:GO. (Not that it should matter, we've disabled model LOD's, but just in case.)
		float flSwitchPointModifier = 1.0f;

		// shadow lod is specified on the last lod with a negative switch
		// never consider shadow lod as viable candidate
		int numLODs = (m_pLODs[m_NumLODs - 1].m_SwitchPoint < 0.0f) ? m_NumLODs - 1 : m_NumLODs;

		for (int i = m_RootLOD; i < numLODs - 1; i++)
		{
			if ((m_pLODs[i + 1].m_SwitchPoint * flSwitchPointModifier) > lodMetric)
				return i;
		}

		return numLODs - 1;
	}
};

// ----------------------------------------------------------
// ----------------------------------------------------------

// body part index
struct mstudiobodyparts_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
	inline mstudiomodel_t* pModel(int i) const { return (mstudiomodel_t*)(((byte*)this) + modelindex) + i; };
};

// body group preset
struct mstudiobodygrouppreset_t
{

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int					iValue;
	int					iMask;
};

struct mstudiomouth_t
{

	int					bone;
	Vector				forward;
	int					flexdesc;

	mstudiomouth_t() {}
private:
	// No copy constructors allowed
	mstudiomouth_t(const mstudiomouth_t& vOther);
};

enum class Hitboxes_t;

struct mstudiohitboxset_t
{

	int					sznameindex;
	inline char* const	pszName(void) const { return ((char*)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t* Hitbox(Hitboxes_t i) const { return (mstudiobbox_t*)(((byte*)this) + hitboxindex) + int(i); };
};


//-----------------------------------------------------------------------------
// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
// NOTE: The operation you should apply is: pretransform * bone transform * posttransform
//-----------------------------------------------------------------------------
struct mstudiosrcbonetransform_t
{


	int			sznameindex;
	inline const char* pszName(void) const { return ((char*)this) + sznameindex; }
	matrix3x4_t	pretransform;
	matrix3x4_t	posttransform;
};


// ----------------------------------------------------------
// Purpose: Load time results on model compositing
// ----------------------------------------------------------

class virtualgroup_t
{
public:
	virtualgroup_t(void) { cache = NULL; };
	// tool dependant.  In engine this is a model_t, in tool it's a direct pointer
	void* cache;
	// converts cache entry into a usable studiohdr_t *
	const studiohdr_t* GetStudioHdr(void) const;

	CUtlVector< int > boneMap;				// maps global bone to local bone
	CUtlVector< int > masterBone;			// maps local bone to global bone
	CUtlVector< int > masterSeq;			// maps local sequence to master sequence
	CUtlVector< int > masterAnim;			// maps local animation to master animation
	CUtlVector< int > masterAttachment;	// maps local attachment to global
	CUtlVector< int > masterPose;			// maps local pose parameter to global
	CUtlVector< int > masterNode;			// maps local transition nodes to global
};

struct virtualsequence_t
{
	int	flags;
	int activity;
	int group;
	int index;
};

struct virtualgeneric_t
{
	int group;
	int index;
};


struct virtualmodel_t
{
	void AppendSequences(int group, const studiohdr_t* pStudioHdr);
	void AppendAnimations(int group, const studiohdr_t* pStudioHdr);
	void AppendAttachments(int ground, const studiohdr_t* pStudioHdr);
	void AppendPoseParameters(int group, const studiohdr_t* pStudioHdr);
	void AppendBonemap(int group, const studiohdr_t* pStudioHdr);
	void AppendNodes(int group, const studiohdr_t* pStudioHdr);
	void AppendTransitions(int group, const studiohdr_t* pStudioHdr);
	void AppendIKLocks(int group, const studiohdr_t* pStudioHdr);
	void AppendModels(int group, const studiohdr_t* pStudioHdr);
	void UpdateAutoplaySequences(const studiohdr_t* pStudioHdr);

	virtualgroup_t* pAnimGroup(int animation) { return &m_group[m_anim[animation].group]; }; // Note: user must manage mutex for this
	virtualgroup_t* pSeqGroup(int sequence) { return &m_group[m_Sequence[sequence].group]; }; // Note: user must manage mutex for this

	char pad_mutex[0x8];
	CUtlVector< virtualsequence_t > m_Sequence;
	CUtlVector< virtualgeneric_t > m_anim;
	CUtlVector< virtualgeneric_t > m_attachment;
	CUtlVector< virtualgeneric_t > m_pose;
	CUtlVector< virtualgroup_t > m_group;
	CUtlVector< virtualgeneric_t > m_node;
	CUtlVector< virtualgeneric_t > m_iklock;
	CUtlVector< unsigned short > m_autoplaySequences;
};

// 'thin' vertex data, used to do model decals (see Studio_CreateThinVertexes())
struct thinModelVertices_t
{
	void Init(int numBoneInfluences, Vector* positions, unsigned short* normals, float* boneWeights, byte* boneIndices)
	{
		Assert(positions != NULL);
		Assert(normals != NULL);
		Assert((numBoneInfluences >= 0) && (numBoneInfluences <= 3));
		Assert(numBoneInfluences > 0 ? !!boneIndices : !boneIndices);
		Assert(numBoneInfluences > 1 ? !!boneWeights : !boneWeights);
		m_numBoneInfluences = numBoneInfluences;
		m_vecPositions = positions;
		m_vecNormals = normals;
		m_boneWeights = boneWeights;
		m_boneIndices = boneIndices;
	}

	void SetPosition(int vertIndex, const Vector& position)
	{
		Assert(m_vecPositions);
		m_vecPositions[vertIndex] = position;
	}

	void SetBoneWeights(int vertIndex, const mstudioboneweight_t& boneWeights)
	{
		Assert((m_numBoneInfluences >= 1) && (m_numBoneInfluences <= 3));
		Assert((boneWeights.numbones >= 1) && (boneWeights.numbones <= m_numBoneInfluences));
		int    numStoredWeights = std::max(0, (m_numBoneInfluences - 1));
		float* pBaseWeight = m_boneWeights + vertIndex * numStoredWeights;
		byte* pBaseIndex = m_boneIndices + vertIndex * m_numBoneInfluences;
		for (int i = 0; i < m_numBoneInfluences; i++)
		{
			pBaseIndex[i] = boneWeights.bone[i];
		}
		for (int i = 0; i < numStoredWeights; i++)
		{
			pBaseWeight[i] = boneWeights.weight[i];
		}
	}

	void GetMeshPosition(mstudiomesh_t* pMesh, int meshIndex, Vector* pPosition) const
	{
		Assert(pMesh);
		GetPosition(pMesh->vertexdata.GetGlobalVertexIndex(meshIndex), pPosition);
	}

	void GetMeshBoneWeights(mstudiomesh_t* pMesh, int meshIndex, mstudioboneweight_t* pBoneWeights) const
	{
		Assert(pMesh);
		GetBoneWeights(pMesh->vertexdata.GetGlobalVertexIndex(meshIndex), pBoneWeights);
	}

	void GetModelPosition(mstudiomodel_t* pModel, int modelIndex, Vector* pPosition) const
	{
		Assert(pModel);
		GetPosition(pModel->vertexdata.GetGlobalVertexIndex(modelIndex), pPosition);
	}

	void GetModelBoneWeights(mstudiomodel_t* pModel, int modelIndex, mstudioboneweight_t* pBoneWeights) const
	{
		Assert(pModel);
		GetBoneWeights(pModel->vertexdata.GetGlobalVertexIndex(modelIndex), pBoneWeights);
	}

private:
	void GetPosition(int vertIndex, Vector* pPosition) const
	{
		Assert(pPosition);
		Assert(m_vecPositions);
		*pPosition = m_vecPositions[vertIndex];
	}

	void GetBoneWeights(int vertIndex, mstudioboneweight_t* pBoneWeights) const
	{
		Assert(pBoneWeights);
		Assert((m_numBoneInfluences <= 1) || (m_boneWeights != NULL));
		Assert((m_numBoneInfluences <= 0) || (m_boneIndices != NULL));
		int    numStoredWeights = std::max(0, (m_numBoneInfluences - 1));
		float* pBaseWeight = m_boneWeights + vertIndex * numStoredWeights;
		byte* pBaseIndex = m_boneIndices + vertIndex * m_numBoneInfluences;
		float  sum = 0.0f;
		// TODO: unroll this loop? It's only three. We could use a switch
		// and code it explicitly for the various possible m_numBoneInfluences
		// which would improve scheduling but bloat code.
		for (int i = 0; i < MAX_NUM_BONES_PER_VERT; i++)
		{
			float weight;
			if (i < (m_numBoneInfluences - 1))
			{
				weight = pBaseWeight[i];
				sum += weight;
			}
			else
			{
				weight = 1.0f - sum;
				sum = 1.0f;
			}

			pBoneWeights->weight[i] = weight;
			pBoneWeights->bone[i] = (i < m_numBoneInfluences) ? pBaseIndex[i] : 0;

			/*
			if ( i < ( m_numBoneInfluences - 1 ) )
				pBoneWeights->weight[i] = pBaseWeight[i];
			else
				pBoneWeights->weight[i] = 1.0f - sum;
			sum += pBoneWeights->weight[i];
			pBoneWeights->bone[i] = ( i < m_numBoneInfluences ) ? pBaseIndex[i] : 0;
			*/
		}

		// Treat 'zero weights' as '100% binding to bone zero':
		pBoneWeights->numbones = m_numBoneInfluences ? m_numBoneInfluences : 1;
	}

	int				m_numBoneInfluences;// Number of bone influences per vertex, N
	float* m_boneWeights;		// This array stores (N-1) weights per vertex (unless N is zero)
	byte* m_boneIndices;		// This array stores N indices per vertex
	Vector* m_vecPositions;
	unsigned short* m_vecNormals;		// Normals are compressed into 16 bits apiece (see PackNormal_UBYTE4() )
};


// ----------------------------------------------------------
// Studio Model Stream Data File
// ----------------------------------------------------------

// little-endian "IDSS"
#define MODEL_STREAM_FILE_ID		(('S'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_STREAM_FILE_VERSION	1

struct vertexStreamFileHeader_t
{

	int		id;								// MODEL_STREAM_FILE_ID
	int		version;						// MODEL_STREAM_FILE_VERSION
	int		checksum;						// same as studiohdr_t, ensures sync
	int		flags;							// flags
	int		numVerts;						// number of vertices
	int		uv2StreamStart;					// offset from base to uv2 stream
	int		uv2ElementSize;					// size of each uv2 element
	int		pad;							// pad

public:

	// Accessor to uv2 stream
	const void* GetStreamUv2() const
	{
		if ((id == MODEL_STREAM_FILE_ID) && (uv2StreamStart != 0))
			return (void*)(uv2StreamStart + (byte*)this);
		else
			return NULL;
	}
};

struct PhysFeModelDesc_t;

// ----------------------------------------------------------
// Studio Model Vertex Data File
// Position independent flat data for cache manager
// ----------------------------------------------------------

// little-endian "IDSV"
#define MODEL_VERTEX_FILE_ID		(('V'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_VERTEX_FILE_VERSION	4
// this id (IDCV) is used once the vertex data has been compressed (see CMDLCache::CreateThinVertexes)
#define MODEL_VERTEX_FILE_THIN_ID	(('V'<<24)+('C'<<16)+('D'<<8)+'I')
// this id (IDDV) is used once the vertex data has been discarded (see CMDLCache::CreateNullVertexes)
#define MODEL_VERTEX_FILE_NULL_ID	(('V'<<24)+('D'<<16)+('D'<<8)+'I')

struct vertexFileHeader_t
{

	int		id;								// MODEL_VERTEX_FILE_ID
	int		version;						// MODEL_VERTEX_FILE_VERSION
	int		checksum;						// same as studiohdr_t, ensures sync
	int		numLODs;						// num of valid lods
	int		numLODVertexes[MAX_NUM_LODS];	// num verts for desired root lod
	int		numFixups;						// num of vertexFileFixup_t
#ifdef _PS3
	union
	{
		int fixupTableStart;
		uint ps3edgeDmaInputDesc;			// [PS3: offset of the EDGE DMA INPUT stream]
	};
	uint inline GetPs3EdgeDmaInputStart() const { return (ps3edgeDmaInputDesc & 0xFFFF) << 8; }
	uint inline GetPs3EdgeDmaInputLength() const { return ((ps3edgeDmaInputDesc >> 16) & 0x7FFF) << 8; }
#else
	int		fixupTableStart;				// offset from base to fixup table
#endif
	int		vertexDataStart;				// offset from base to vertex block
	int		tangentDataStart;				// offset from base to tangent block

public:

	// Accessor to fat vertex data
	const mstudiovertex_t* GetVertexData() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (vertexDataStart != 0))
			return (mstudiovertex_t*)(vertexDataStart + (byte*)this);
		else
			return NULL;
	}
	// Accessor to (fat) tangent vertex data (tangents aren't stored in compressed data)
	const Vector2D* GetTangentData() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (tangentDataStart != 0))
			return (Vector2D*)(tangentDataStart + (byte*)this);
		else
			return NULL;
	}
	// Accessor to extra vertex data
	const void* GetExtraData() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (tangentDataStart != 0))
			return (void*)(tangentDataStart + (byte*)this + numLODVertexes[0] * sizeof(Vector2D));
		else
			return NULL;
	}
	// Accessor to thin vertex data
	const  thinModelVertices_t* GetThinVertexData() const
	{
		if ((id == MODEL_VERTEX_FILE_THIN_ID) && (vertexDataStart != 0))
			return (thinModelVertices_t*)(vertexDataStart + (byte*)this);
		else
			return NULL;
	}

#ifdef _PS3
	// Accessor to EDGE DMA INPUT format
	const byte* GetPs3EdgeDmaInput() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (ps3edgeDmaInputDesc & 0x80000000))
			return (byte*)(GetPs3EdgeDmaInputStart() + (byte*)this);
		else
			return NULL;
	}
#endif
};

// apply sequentially to lod sorted vertex and tangent pools to re-establish mesh order
struct vertexFileFixup_t
{

	int		lod;				// used to skip culled root lod
	int		sourceVertexID;		// absolute index from start of vertex/tangent blocks
	int		numVertexes;
};

// This flag is set if no Hitbox information was specified
#define STUDIOHDR_FLAGS_AUTOGENERATED_HITBOX	( 1 << 0 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_ENV_CUBEMAP		( 1 << 1 )

// Use this when there are translucent parts to the model but we're not going to sort it 
#define STUDIOHDR_FLAGS_FORCE_OPAQUE			( 1 << 2 )

// Use this when we want to Render the opaque parts during the opaque pass
// and the translucent parts during the translucent pass
#define STUDIOHDR_FLAGS_TRANSLUCENT_TWOPASS		( 1 << 3 )

// This is set any time the .qc files has $staticprop in it
// Means there's no bones and no transforms
#define STUDIOHDR_FLAGS_STATIC_PROP				( 1 << 4 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_FB_TEXTURE		    ( 1 << 5 )

// This flag is set by studiomdl.exe if a separate "$shadowlod" entry was present
//  for the .mdl (the shadow lod is the last entry in the lod list if present)
#define STUDIOHDR_FLAGS_HASSHADOWLOD			( 1 << 6 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_BUMPMAPPING		( 1 << 7 )

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_USE_SHADOWLOD_MATERIALS	( 1 << 8 )

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_OBSOLETE				( 1 << 9 )

// NOTE:  This flag is set when we need to draw in the additive stage of the deferred rendering
#define STUDIOHDR_FLAGS_NEEDS_DEFERRED_ADDITIVE	( 1 << 10 )

// NOTE:  This flag is set at mdl build time
#define STUDIOHDR_FLAGS_NO_FORCED_FADE			( 1 << 11 )

// NOTE:  The npc will lengthen the viseme check to always include two phonemes
#define STUDIOHDR_FLAGS_FORCE_PHONEME_CROSSFADE	( 1 << 12 )

// This flag is set when the .qc has $constantdirectionallight in it
// If set, we use constantdirectionallightdot to calculate light intensity
// rather than the normal directional Dot product
// only valid if STUDIOHDR_FLAGS_STATIC_PROP is also set
#define STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT ( 1 << 13 )

// Flag to mark Delta flexes as already converted from disk format to memory format
#define STUDIOHDR_FLAGS_FLEXES_CONVERTED		( 1 << 14 )

// Indicates the studiomdl was built in preview mode
#define STUDIOHDR_FLAGS_BUILT_IN_PREVIEW_MODE	( 1 << 15 )

// Ambient boost (runtime flag)
#define STUDIOHDR_FLAGS_AMBIENT_BOOST			( 1 << 16 )

// Don't cast shadows from this model (useful on first-person models)
#define STUDIOHDR_FLAGS_DO_NOT_CAST_SHADOWS		( 1 << 17 )

// alpha textures should cast shadows in vrad on this model (ONLY prop_static!)
#define STUDIOHDR_FLAGS_CAST_TEXTURE_SHADOWS	( 1 << 18 )

// Model has a quad-only Catmull-Clark SubD cage
#define STUDIOHDR_FLAGS_SUBDIVISION_SURFACE		( 1 << 19 )

// flagged on load to indicate no animation events on this model
#define STUDIOHDR_FLAGS_NO_ANIM_EVENTS			( 1 << 20 )

// If flag is set then studiohdr_t.flVertAnimFixedPointScale contains the
// scale value for fixed point vert anim data, if not set then the
// scale value is the default of 1.0 / 4096.0.  Regardless use
// studiohdr_t::VertAnimFixedPointScale() to always retrieve the scale value
#define STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE	( 1 << 21 )

// If flag is set then model data is processed for EDGE
// the flag is set at tool time when producing PS3-format assets
#define STUDIOHDR_FLAGS_PS3_EDGE_FORMAT			( 1 << 22 )

// this is a specific case to indicate a model is over budget
#define STUDIOHDR_FLAGS_OVER_BUDGET					( 1 << 23 )

// this is a specific case to indicate a model is over budget
#define STUDIOHDR_FLAGS_IGNORE_BUDGETS				( 1 << 24 )

// internally generated combined model
#define STUDIOHDR_FLAGS_COMBINED					( 1 << 25 )

// Model has an additional set of UVs
#define STUDIOHDR_FLAGS_EXTRA_VERTEX_DATA			( 1 << 26 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_BAKED_VERTEX_LIGHTING_IS_INDIRECT_ONLY	( 1 << 27 )

// NOTE! Next time we up the .mdl file format, remove studiohdr2_t
// and insert all fields in this structure into studiohdr_t.
struct studiohdr2_t
{
	// NOTE: For forward compat, make sure any methods in this struct
	// are also available in studiohdr_t so no leaf code ever directly references
	// a studiohdr2_t structure

	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;
	inline int			IllumPositionAttachmentIndex() const { return illumpositionattachmentindex; }

	float flMaxEyeDeflection;
	inline float		MaxEyeDeflection() const { return flMaxEyeDeflection != 0.0f ? flMaxEyeDeflection : 0.866f; } // default to cos(30) if not set

	int linearboneindex;
	inline mstudiolinearbone_t* pLinearBones() const { return (linearboneindex) ? (mstudiolinearbone_t*)(((byte*)this) + linearboneindex) : NULL; }

	int sznameindex;
	inline char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : NULL; }

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;
	inline mstudioboneflexdriver_t* pBoneFlexDriver(int i) const { Assert(i >= 0 && i < m_nBoneFlexDriverCount); return (mstudioboneflexdriver_t*)(((byte*)this) + m_nBoneFlexDriverIndex) + i; }

	void* m_pFeModel; // this is functionally the same as having an index and a function, but more readable.

	int m_nBodyGroupPresetCount;
	int m_nBodyGroupPresetIndex;
	inline mstudiobodygrouppreset_t* pBodyGroupPreset(int i) const { Assert(i >= 0 && i < m_nBodyGroupPresetCount); return (mstudiobodygrouppreset_t*)(((byte*)this) + m_nBodyGroupPresetIndex) + i; };

	int padding_unused; // This hasn't been used before, use it if you need it.

	mutable serializedstudioptr_t< void	> virtualModel;
	mutable serializedstudioptr_t< void	> animblockModel;

	serializedstudioptr_t< void> pVertexBase;
	serializedstudioptr_t< void> pIndexBase;

	int reserved[44];
};

struct studiohdr_t
{

	int					id;
	int					version;

	int				checksum;		// this has to be the same in the phy and vtx files to load!

	inline const char* pszName(void) const { if (studiohdr2index && pStudioHdr2()->pszName()) return pStudioHdr2()->pszName(); else return name; }
	char				name[64];

	int					Length;

	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline const mstudiobone_t* pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t*)(((byte*)this) + boneindex) + i; };
	int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone
	int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	inline mstudiobonecontroller_t* pBonecontroller(int i) const { Assert(i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t*)(((byte*)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up Hitbox set by index
	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		Assert(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t*)(((byte*)this) + hitboxsetindex) + i;
	};

	// Calls through to Hitbox to determine size of specified set
	inline mstudiobbox_t* Hitbox(Hitboxes_t i, int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->Hitbox(i);
	};

	// Calls through to set to get Hitbox count for set
	inline int			iHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	inline mstudioanimdesc_t* pLocalAnimdesc(int i) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t*)(((byte*)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	inline mstudioseqdesc_t* pLocalSeqdesc(int i) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t*)(((byte*)this) + localseqindex) + i; };

	//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq_Internal() const;
	inline int			GetNumSeq() const
	{
		if (numincludemodels == 0)
		{
			return numlocalseq;
		}
		return GetNumSeq_Internal();

	}
	mstudioanimdesc_t& pAnimdesc_Internal(int i) const;
	inline mstudioanimdesc_t& pAnimdesc(int i) const
	{
		if (numincludemodels == 0)
		{
			return *pLocalAnimdesc(i);
		}

		return pAnimdesc_Internal(i);
	}

	mstudioseqdesc_t& pSeqdesc_Internal(int i) const;
	inline mstudioseqdesc_t& pSeqdesc(int i) const
	{
		if (numincludemodels == 0)
		{
			return *pLocalSeqdesc(i);
		}
		return pSeqdesc_Internal(i);
	}
	int			iRelativeAnim_Internal(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	inline int			iRelativeAnim(int baseseq, int relanim) const
	{
		if (numincludemodels == 0)
			return relanim;
		return iRelativeAnim_Internal(baseseq, relanim);
	}
	int					iRelativeSeq_Internal(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index
	inline int			iRelativeSeq(int baseseq, int relseq) const
	{
		if (numincludemodels == 0)
		{
			return relseq;
		}
		return iRelativeSeq_Internal(baseseq, relseq);
	}

	//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:
	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version) const;
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	// raw textures
	int					numtextures;
	int					textureindex;
	inline mstudiotexture_t* pTexture(int i) const { Assert(i >= 0 && i < numtextures); return (mstudiotexture_t*)(((byte*)this) + textureindex) + i; };


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char* pCdtexture(int i) const { return (((char*)this) + *((int*)(((byte*)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short* pSkinref(int i) const { return (short*)(((byte*)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;
	inline mstudiobodyparts_t* pBodypart(int i) const { return (mstudiobodyparts_t*)(((byte*)this) + bodypartindex) + i; };

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	inline mstudioattachment_t* pLocalAttachment(int i) const { Assert(i >= 0 && i < numlocalattachments); return (mstudioattachment_t*)(((byte*)this) + localattachmentindex) + i; };
	//public:
	int					GetNumAttachments(void) const;
	const mstudioattachment_t& pAttachment(int i) const;
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char* pszLocalNodeName(int iNode) const { Assert(iNode >= 0 && iNode < numlocalnodes); return (((char*)this) + *((int*)(((byte*)this) + localnodenameindex) + iNode)); }
	inline byte* pLocalTransition(int i) const { Assert(i >= 0 && i < (numlocalnodes* numlocalnodes)); return (byte*)(((byte*)this) + localnodeindex) + i; };

	//public:
	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char* pszNodeName(int iNode);
	int					GetTransition(int iFrom, int iTo) const;

	int					numflexdesc;
	int					flexdescindex;
	inline mstudioflexdesc_t* pFlexdesc(int i) const { Assert(i >= 0 && i < numflexdesc); return (mstudioflexdesc_t*)(((byte*)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	inline mstudioflexcontroller_t* pFlexcontroller(int i) const { Assert(i >= 0 && i < numflexcontrollers); return (mstudioflexcontroller_t*)(((byte*)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	inline mstudioflexrule_t* pFlexRule(int i) const { Assert(i >= 0 && i < numflexrules); return (mstudioflexrule_t*)(((byte*)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	inline mstudioikchain_t* pIKChain(int i) const { Assert(i >= 0 && i < numikchains); return (mstudioikchain_t*)(((byte*)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	inline mstudiomouth_t* pMouth(int i) const { Assert(i >= 0 && i < nummouths); return (mstudiomouth_t*)(((byte*)this) + mouthindex) + i; };

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	inline mstudioposeparamdesc_t* pLocalPoseParameter(int i) const { Assert(i >= 0 && i < numlocalposeparameters); return (mstudioposeparamdesc_t*)(((byte*)this) + localposeparamindex) + i; };
	//public:
	int					GetNumPoseParameters(void) const;
	const mstudioposeparamdesc_t& pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					surfacepropindex;
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropindex; }
	inline int			GetSurfaceProp() const { return surfacepropLookup; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	inline mstudioiklock_t* pLocalIKAutoplayLock(int i) const { Assert(i >= 0 && i < numlocalikautoplaylocks); return (mstudioiklock_t*)(((byte*)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks(void) const;
	const mstudioiklock_t& pIKAutoplayLock(int i);
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences(unsigned short* pOut, int outCount) const;
	int					GetAutoplayList(unsigned short** pOut) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	inline mstudiomodelgroup_t* pModelGroup(int i) const { Assert(i >= 0 && i < numincludemodels); return (mstudiomodelgroup_t*)(((byte*)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t* FindModel(void** cache, char const* modelname) const;

	// implementation specific back pointer to virtual data. Relocated to studiohdr2_t
	int					unused_virtualModel;

	virtualmodel_t* GetVirtualModel(void) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char* const pszAnimBlockName(void) const { return ((char*)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	inline mstudioanimblock_t* pAnimBlock(int i) const { Assert(i > 0 && i < numanimblocks); return (mstudioanimblock_t*)(((byte*)this) + animblockindex) + i; };

	// Relocated to studiohdr2_t
	int					unused_animblockModel;

	byte* GetAnimBlock(int i, bool preloadIfMissing = true) const;
	bool				hasAnimBlockBeenPreloaded(int i) const;

	int					bonetablebynameindex;
	inline const byte* GetBoneTableSortedByName() const { return (byte*)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	// Relocated to studiohdr2_t
	int                 unused_pVertexBase;
	int                 unused_pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	mstudioflexcontrollerui_t* pFlexControllerUI(int i) const { Assert(i >= 0 && i < numflexcontrollerui); return (mstudioflexcontrollerui_t*)(((byte*)this) + flexcontrolleruiindex) + i; }

	float				flVertAnimFixedPointScale;
	inline float		VertAnimFixedPointScale() const { return (flags & STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE) ? flVertAnimFixedPointScale : 1.0f / 4096.0f; }

	mutable int			surfacepropLookup;	// this index must be cached by the loader, not saved in the file

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;
	studiohdr2_t* pStudioHdr2() const { return (studiohdr2_t*)(((byte*)this) + studiohdr2index); }

	// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
	int					NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }
	const mstudiosrcbonetransform_t* SrcBoneTransform(int i) const { Assert(i >= 0 && i < NumSrcBoneTransforms()); return (mstudiosrcbonetransform_t*)(((byte*)this) + pStudioHdr2()->srcbonetransformindex) + i; }

	inline int			IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

	inline float		MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set

	inline mstudiolinearbone_t* pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : NULL; }

	inline int			BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }
	inline const mstudioboneflexdriver_t* BoneFlexDriver(int i) const { Assert(i >= 0 && i < BoneFlexDriverCount()); return studiohdr2index ? pStudioHdr2()->pBoneFlexDriver(i) : NULL; }

	inline int			BodyGroupPresetCount() const { return studiohdr2index ? pStudioHdr2()->m_nBodyGroupPresetCount : 0; }
	inline const mstudiobodygrouppreset_t* BodyGroupPreset(int i) const { Assert(i >= 0 && i < BodyGroupPresetCount()); return studiohdr2index ? pStudioHdr2()->pBodyGroupPreset(i) : NULL; }

	void* VirtualModel() const { return studiohdr2index ? (void*)(pStudioHdr2()->virtualModel) : nullptr; }
	void				SetVirtualModel(void* ptr) { Assert(studiohdr2index); if (studiohdr2index) { pStudioHdr2()->virtualModel = ptr; } }

	void* VertexBase() const { return studiohdr2index ? (void*)(pStudioHdr2()->pVertexBase) : nullptr; }
	void				SetVertexBase(void* pVertexBase) const { Assert(studiohdr2index); if (studiohdr2index) { pStudioHdr2()->pVertexBase = pVertexBase; } }
	void* IndexBase() const { return studiohdr2index ? (void*)(pStudioHdr2()->pIndexBase) : nullptr; }
	void				SetIndexBase(void* pIndexBase) const { Assert(studiohdr2index); if (studiohdr2index) { pStudioHdr2()->pIndexBase = pIndexBase; } }


	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

class IDataCache;
class IMDLCache;
class CFeModel;
class CSoftbody;
class CSoftbodyEnvironment;


enum LocalFlexController_t
{
	// We cast 0 & -1 into this enum, so we have to define them here to avoid SNC complaining
	DUMMY_INVALID_FLEX_CONTROLLER = -1,
	DUMMY_NULL_FLEX_CONTROLLER = 0,
	// this isn't really an enum - its just a typed int. gcc will not accept it as a fwd decl, so we'll define one value
	DUMMY_FLEX_CONTROLLER = 0x7fffffff						// make take 32 bits
};

inline LocalFlexController_t& operator++(LocalFlexController_t& a) { return a = LocalFlexController_t(int(a) + 1); }
inline LocalFlexController_t& operator--(LocalFlexController_t& a) { return a = LocalFlexController_t(int(a) - 1); }
inline LocalFlexController_t  operator++(LocalFlexController_t& a, int) { LocalFlexController_t t = a; a = LocalFlexController_t(int(a) + 1); return t; }
inline LocalFlexController_t  operator--(LocalFlexController_t& a, int) { LocalFlexController_t t = a; a = LocalFlexController_t(int(a) - 1); return t; }

class CStudioHdr
{
public:
	CStudioHdr(void);
	CStudioHdr(const studiohdr_t* pStudioHdr, IMDLCache* mdlcache = NULL);
	~CStudioHdr() { Term(); }

	void Init(const studiohdr_t* pStudioHdr, IMDLCache* mdlcache = NULL);
	void Term();

public:
	inline bool IsVirtual(void) { return (m_pVModel != NULL); };
	inline bool IsValid(void) { return (m_pStudioHdr != NULL); };
	inline bool IsReadyForAccess(void) const { return (m_pStudioHdr != NULL); };
	inline virtualmodel_t* GetVirtualModel(void) const { return m_pVModel; };
	inline const studiohdr_t* GetRenderHdr(void) const { return m_pStudioHdr; };
	const studiohdr_t* pSeqStudioHdr(int sequence);
	const studiohdr_t* pAnimStudioHdr(int animation);
	//CFeModel *GetFeModel() { return m_pSoftbody? m_pSoftbody->GetFeModel(); }
	CSoftbody* GetSoftbody() const { return m_pSoftbody; }
	void SetSoftbody(CSoftbody* pSoftbody) { m_pSoftbody = pSoftbody; }
	CSoftbody* InitSoftbody(CSoftbodyEnvironment* pSoftbodyEnvironment);
	void FreeSoftbody();
private:
	mutable const studiohdr_t* m_pStudioHdr;
	mutable virtualmodel_t* m_pVModel;
	mutable CSoftbody* m_pSoftbody;

	const virtualmodel_t* ResetVModel(const virtualmodel_t* pVModel) const;
	const studiohdr_t* GroupStudioHdr(int group);
	mutable CUtlVector< const studiohdr_t* > m_pStudioHdrCache;

	mutable int			m_nFrameUnlockCounter;
	int* m_pFrameUnlockCounter;
	char				pad_mutex[0x8]; //0x0028 

public:
	inline int			numbones(void) const { return m_pStudioHdr->numbones; };
	inline const mstudiobone_t* pBone(int i) const { return m_pStudioHdr->pBone(i); };
	int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone
	int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone

	bool				SequencesAvailable() const;
	int					GetNumSeq_Internal(void) const;
	inline int			GetNumSeq(void) const
	{
		if (!m_pVModel)
			return m_pStudioHdr->numlocalseq;
		return GetNumSeq_Internal();
	}

	mstudioanimdesc_t& pAnimdesc_Internal(int i);
	inline mstudioanimdesc_t& pAnimdesc(int i)
	{
		if (!m_pVModel)
			return *m_pStudioHdr->pLocalAnimdesc(i);
		return pAnimdesc_Internal(i);
	}
	mstudioseqdesc_t& pSeqdesc_Internal(int iSequence);
	inline mstudioseqdesc_t& pSeqdesc(int iSequence)
	{
		if (!m_pVModel)
			return *m_pStudioHdr->pLocalSeqdesc(iSequence);

		return pSeqdesc_Internal(iSequence);
	}

	int					iRelativeAnim_Internal(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	inline int			iRelativeAnim(int baseseq, int relanim) const
	{
		if (!m_pVModel)
			return relanim;
		return iRelativeAnim_Internal(baseseq, relanim);
	}

	int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version);
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	int					GetNumAttachments(void) const;
	const mstudioattachment_t& pAttachment(int i);
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char* pszNodeName(int iNode);
	// FIXME: where should this one be?
	int					GetTransition(int iFrom, int iTo) const;

	int					GetNumPoseParameters(void) const;
	const mstudioposeparamdesc_t& pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					GetNumIKAutoplayLocks(void) const;
	const mstudioiklock_t& pIKAutoplayLock(int i);

	inline int			CountAutoplaySequences() const { return m_pStudioHdr->CountAutoplaySequences(); };
	inline int			CopyAutoplaySequences(unsigned short* pOut, int outCount) const { return m_pStudioHdr->CopyAutoplaySequences(pOut, outCount); };
	inline int			GetAutoplayList(unsigned short** pOut) const { return m_pStudioHdr->GetAutoplayList(pOut); };

	inline int			GetNumBoneControllers(void) const { return m_pStudioHdr->numbonecontrollers; };
	inline mstudiobonecontroller_t* pBonecontroller(int i) const { return m_pStudioHdr->pBonecontroller(i); };

	inline int			numikchains() const { return m_pStudioHdr->numikchains; };
	inline int			GetNumIKChains(void) const { return m_pStudioHdr->numikchains; };
	inline mstudioikchain_t* pIKChain(int i) const { return m_pStudioHdr->pIKChain(i); };

	inline int			numflexrules() const { return m_pStudioHdr->numflexrules; };
	inline mstudioflexrule_t* pFlexRule(int i) const { return m_pStudioHdr->pFlexRule(i); };

	inline int			numflexdesc() const { return m_pStudioHdr->numflexdesc; };
	inline mstudioflexdesc_t* pFlexdesc(int i) const { return m_pStudioHdr->pFlexdesc(i); };

	inline LocalFlexController_t			numflexcontrollers() const { return (LocalFlexController_t)m_pStudioHdr->numflexcontrollers; };
	inline mstudioflexcontroller_t* pFlexcontroller(LocalFlexController_t i) const { return m_pStudioHdr->pFlexcontroller(i); };

	inline int			numflexcontrollerui() const { return m_pStudioHdr->numflexcontrollerui; };
	inline mstudioflexcontrollerui_t* pFlexcontrollerUI(int i) const { return m_pStudioHdr->pFlexControllerUI(i); };

	inline const char* name() const { return m_pStudioHdr->pszName(); }; // deprecated -- remove after full xbox merge
	inline const char* pszName() const { return m_pStudioHdr->pszName(); };

	inline int			numbonecontrollers() const { return m_pStudioHdr->numbonecontrollers; };

	inline int			numhitboxsets() const { return m_pStudioHdr->numhitboxsets; };
	inline mstudiohitboxset_t* pHitboxSet(int i) const { return m_pStudioHdr->pHitboxSet(i); };

	inline mstudiobbox_t* Hitbox(Hitboxes_t i, int set) const { return m_pStudioHdr->Hitbox(i, set); };
	inline int			iHitboxCount(int set) const { return m_pStudioHdr->iHitboxCount(set); };

	inline int			numbodyparts() const { return m_pStudioHdr->numbodyparts; };
	inline mstudiobodyparts_t* pBodypart(int i) const { return m_pStudioHdr->pBodypart(i); };

	inline int			numskinfamilies() const { return m_pStudioHdr->numskinfamilies; }

	inline Vector		eyeposition() const { return m_pStudioHdr->eyeposition; };

	inline int			flags() const { return m_pStudioHdr->flags; };

	inline char* const pszSurfaceProp(void) const { return m_pStudioHdr->pszSurfaceProp(); };
	inline int			GetSurfaceProp()const { return m_pStudioHdr->surfacepropLookup; }

	inline float		mass() const { return m_pStudioHdr->mass; };
	inline int			contents() const { return m_pStudioHdr->contents; }

	inline const byte* GetBoneTableSortedByName() const { return m_pStudioHdr->GetBoneTableSortedByName(); };

	inline Vector		illumposition() const { return m_pStudioHdr->illumposition; };

	inline Vector		hull_min() const { return m_pStudioHdr->hull_min; };		// ideal movement hull size
	inline Vector		hull_max() const { return m_pStudioHdr->hull_max; };

	inline Vector		view_bbmin() const { return m_pStudioHdr->view_bbmin; };		// clipping bounding box
	inline Vector		view_bbmax() const { return m_pStudioHdr->view_bbmax; };

	inline int			numtextures() const { return m_pStudioHdr->numtextures; };

	inline int			IllumPositionAttachmentIndex() const { return m_pStudioHdr->IllumPositionAttachmentIndex(); }

	inline float		MaxEyeDeflection() const { return m_pStudioHdr->MaxEyeDeflection(); }

	inline mstudiolinearbone_t* pLinearBones() const { return m_pStudioHdr->pLinearBones(); }

	inline int			BoneFlexDriverCount() const { return m_pStudioHdr->BoneFlexDriverCount(); }
	inline const mstudioboneflexdriver_t* BoneFlexDriver(int i) const { return m_pStudioHdr->BoneFlexDriver(i); }

	inline int			GetNumBodyGroupPresets() const { return m_pStudioHdr->BodyGroupPresetCount(); };
	inline const mstudiobodygrouppreset_t* GetBodyGroupPreset(int i) const { return m_pStudioHdr->BodyGroupPreset(i); }

public:
	int IsSequenceLooping(int iSequence);
	float GetSequenceCycleRate(int iSequence);

	void				RunFlexRules(const float* src, float* dest);
	void				RunFlexRulesOld(const float* src, float* dest);
	void				RunFlexRulesNew(const float* src, float* dest);


public:
	inline int boneFlags(int iBone) const { return m_boneFlags[iBone]; }
	void setBoneFlags(int iBone, int flags);
	void clearBoneFlags(int iBone, int flags);
	inline int boneParent(int iBone) const { return m_boneParent[iBone]; }

public:
	CUtlVector< int >  m_boneFlags;
	CUtlVector< int >  m_boneParent;

public:

	// This class maps an activity to sequences allowed for that activity, accelerating the resolution
	// of SelectWeightedSequence(), especially on PowerPC. Iterating through every sequence
	// attached to a model turned out to be a very destructive cache access pattern on 360.
	// 
	// I've encapsulated this behavior inside a nested class for organizational reasons; there is
	// no particular programmatic or efficiency benefit to it. It just makes clearer what particular
	// code in the otherwise very complicated StudioHdr class has to do with this particular
	// optimization, and it lets you collapse the whole definition down to a single line in Visual
	// Studio.
	class CActivityToSequenceMapping /* final */
	{
	public:
		// A tuple of a sequence and its corresponding weight. Lists of these correspond to activities.
		struct SequenceTuple
		{
			short		seqnum;
			short		weight; // the absolute value of the weight from the sequence header
			CUtlSymbol* pActivityModifiers;		// list of activity modifier symbols
			int			iNumActivityModifiers;
			int* iUnknown;
		};

		// The type of the hash's stored data, a composite of both key and value
		// (because that's how CUtlHash works):
		// key: an int, the activity #
		// values: an index into the m_pSequenceTuples array, a count of the
		// total sequences present for an activity, and the sum of their
		// weights.
		// Note this struct is 128-bits wide, exactly coincident to a PowerPC 
		// cache line and VMX register. Please consider very carefully the
		// performance implications before adding any additional fields to this.
		// You could probably do away with totalWeight if you really had to.
		struct HashValueType
		{
			// KEY (hashed)
			int activityIdx;

			// VALUE (not hashed)
			int startingIdx;
			int count;
			int totalWeight;

			HashValueType(int _actIdx, int _stIdx, int _ct, int _tW) :
				activityIdx(_actIdx), startingIdx(_stIdx), count(_ct), totalWeight(_tW) {}

			// default constructor (ought not to be actually used)
			HashValueType() : activityIdx(-1), startingIdx(-1), count(-1), totalWeight(-1) { }

			class HashFuncs
			{
			public:
				// dummy constructor (gndn)
				HashFuncs(int) {}

				// COMPARE
				// compare two entries for uniqueness. We should never have two different
				// entries for the same activity, so we only compare the activity index;
				// this allows us to use the utlhash as a dict by constructing dummy entries
				// as hash lookup keys.
				bool operator()(const HashValueType& lhs, const HashValueType& rhs) const
				{
					return lhs.activityIdx == rhs.activityIdx;
				}

				// HASH
				// We only hash on the activity index; everything else is data.
				unsigned int operator()(const HashValueType& item) const
				{
					return Math::HashInt(item.activityIdx);
				}
			};
		};

		typedef CUtlHash<HashValueType, HashValueType::HashFuncs, HashValueType::HashFuncs> ActivityToValueIdxHash;

		// These must be here because IFM does not compile/link studio.cpp (?!?)

		// ctor
		CActivityToSequenceMapping(void)
			: m_pSequenceTuples(NULL), m_iSequenceTuplesCount(0), m_ActToSeqHash(8, 0, 0), m_pStudioHdr(NULL), m_expectedVModel(NULL)
		{};

		// dtor -- not virtual because this class has no inheritors
		~CActivityToSequenceMapping()
		{
			if (m_pSequenceTuples != NULL)
			{
				if (m_pSequenceTuples->pActivityModifiers != NULL)
				{
					delete[] m_pSequenceTuples->pActivityModifiers;
				}

				delete[] m_pSequenceTuples;
			}
		}

		/// Get the list of sequences for an activity. Returns the pointer to the
		/// first sequence tuple. Output parameters are a count of sequences present,
		/// and the total weight of all the sequences. (it would be more LHS-friendly
		/// to return these on registers, if only C++ offered more than one return 
		/// value....)
		const SequenceTuple* GetSequences(int forActivity, int* outSequenceCount, int* outTotalWeight);

		/// The number of sequences available for an activity.
		int NumSequencesForActivity(int forActivity);

		static CActivityToSequenceMapping* FindMapping(CStudioHdr* pstudiohdr);
		static void ReleaseMapping(CActivityToSequenceMapping* pMap);
		static void ResetMappings();

		void Initialize(const CStudioHdr* pstudiohdr);
		void Reinitialize(CStudioHdr* pstudiohdr);

		// A more efficient version of the old SelectWeightedSequence() function in animation.cpp. 
		int SelectWeightedSequence(CStudioHdr* pstudiohdr, int activity, int curSequence);

		// selects the sequence with the most matching modifiers
		int SelectWeightedSequenceFromModifiers(CStudioHdr* pstudiohdr, int activity, CUtlSymbol* pActivityModifiers, int iModifierCount, Player* parent);

		// Actually a big array, into which the hash values index.
		SequenceTuple* m_pSequenceTuples;
		unsigned int m_iSequenceTuplesCount; // (size of the whole array)
		ActivityToValueIdxHash m_ActToSeqHash;
		const studiohdr_t* m_pStudioHdr;
		const void* m_expectedVModel;

		// double-check that the data I point to hasn't changed
		bool ValidateAgainst(const CStudioHdr* __restrict pstudiohdr);
		void SetValidation(const CStudioHdr* __restrict pstudiohdr);

		friend class CStudioHdr;
	};

	CActivityToSequenceMapping* m_pActivityToSequence;

	void InitActivityToSequence()
	{
		if (!m_pActivityToSequence)
		{
			m_pActivityToSequence = CActivityToSequenceMapping::FindMapping(this);
		}
	}

	/// A more efficient version of the old SelectWeightedSequence() function in animation.cpp. 
	/// Returns -1 on failure to find a sequence
	inline int SelectWeightedSequence(int activity, int curSequence)
	{
		InitActivityToSequence();
		return m_pActivityToSequence->SelectWeightedSequence(this, activity, curSequence);
	}

	inline int SelectWeightedSequenceFromModifiers(int activity, CUtlSymbol* pActivityModifiers, int iModifierCount, Player* player)
	{
		InitActivityToSequence();
		return m_pActivityToSequence->SelectWeightedSequenceFromModifiers(this, activity, pActivityModifiers, iModifierCount, player);
	}

	/// True iff there is at least one sequence for the given activity.
	inline bool HaveSequenceForActivity(int activity)
	{
		InitActivityToSequence();
		return (m_pActivityToSequence->NumSequencesForActivity(activity) > 0);
	}

	// Force this CStudioHdr's activity-to-sequence mapping to be reinitialized
	inline void ReinitializeSequenceMapping(void)
	{
		if (m_pActivityToSequence)
		{
			CActivityToSequenceMapping::ReleaseMapping(m_pActivityToSequence);
			m_pActivityToSequence = NULL;
		}
		m_pActivityToSequence = CActivityToSequenceMapping::FindMapping(this);
	}

public:
	int LookupSequence(const char* pszName);

#ifdef STUDIO_ENABLE_PERF_COUNTERS
public:
	inline void			ClearPerfCounters(void)
	{
		m_nPerfAnimatedBones = 0;
		m_nPerfUsedBones = 0;
		m_nPerfAnimationLayers = 0;
	};

	// timing info
	mutable	int			m_nPerfAnimatedBones;
	mutable	int			m_nPerfUsedBones;
	mutable	int			m_nPerfAnimationLayers;
#endif


};

/*
class CModelAccess
{
public:
	CModelAccess(CStudioHdr *pSemaphore)
	 : m_pStudioHdr(pSemaphore)
	{
		m_pStudioHdr->IncrementAccess();
	}

	~CModelAccess()
	{
		m_pStudioHdr->DecrementAccess();
	}

public:
	CStudioHdr *m_pStudioHdr;
};
#define ENABLE_MODEL_ACCESS( a ) \
	CModelAccess ModelAccess##__LINE__( a->m_pStudioHdr )
*/

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

struct flexweight_t
{

	int					key;
	float				weight;
	float				influence;
};

struct flexsetting_t
{

	int					nameindex;

	inline char* pszName(void) const
	{
		return (char*)(((byte*)this) + nameindex);
	}

	// Leaving this for legacy support
	int					obsolete1;

	// Number of flex settings
	int					numsettings;
	int					index;

	// OBSOLETE:  
	int					obsolete2;

	// Index of start of contiguous array of flexweight_t structures
	int					settingindex;

	//-----------------------------------------------------------------------------
	// Purpose: Retrieves a pointer to the flexweight_t, including resolving
	//  any markov chain hierarchy.  Because of this possibility, we return
	//  the number of settings in the weights array returned.  We'll generally
	//  call this function with i == 0
	// Input  : *base - 
	//			i - 
	//			**weights - 
	// Output : int
	//-----------------------------------------------------------------------------
	inline int psetting(byte* base, int i, flexweight_t** weights) const;
};


struct flexsettinghdr_t
{

	int					id;
	int					version;

	inline const char* pszName(void) const { return name; }
	char				name[64];
	int					Length;

	int					numflexsettings;
	int					flexsettingindex;
	inline flexsetting_t* pSetting(int i) const { return (flexsetting_t*)(((byte*)this) + flexsettingindex) + i; };
	int					nameindex;

	// look up flex settings by "index"
	int					numindexes;
	int					indexindex;

	inline flexsetting_t* pIndexedSetting(int index) const
	{
		if (index < 0 || index >= numindexes)
		{
			return NULL;
		}

		int i = *((int*)(((byte*)this) + indexindex) + index);

		if (i == -1)
		{
			return NULL;
		}

		return pSetting(i);
	}

	// index names of "flexcontrollers"
	int					numkeys;
	int					keynameindex;
	inline char* pLocalName(int i) const { return (char*)(((byte*)this) + *((int*)(((byte*)this) + keynameindex) + i)); };

	int					keymappingindex;
	inline int* pLocalToGlobal(int i) const { return (int*)(((byte*)this) + keymappingindex) + i; };
	inline int			LocalToGlobal(int i) const { return *pLocalToGlobal(i); };
};

//-----------------------------------------------------------------------------
// Purpose: Retrieves a pointer to the flexweight_t.  
// Input  : *base - flexsettinghdr_t * pointer
//			i - index of flex setting to retrieve
//			**weights - destination for weights array starting at index i.
// Output : int
//-----------------------------------------------------------------------------
inline int flexsetting_t::psetting(byte* base, int i, flexweight_t** weights) const
{
	// Grab array pointer
	*weights = (flexweight_t*)(((byte*)this) + settingindex) + i;
	// Return true number of settings
	return numsettings;
};

inline const mstudioflexcontroller_t* mstudioflexcontrollerui_t::pController(int index) const
{
	if (index < 0 || index > Count())
		return NULL;

	if (remaptype == FLEXCONTROLLER_REMAP_NWAY)
	{
		if (stereo)
			return (mstudioflexcontroller_t*)((char*)this) + *(&szindex0 + index);

		if (index == 0)
			return pController();

		if (index == 1)
			return pNWayValueController();

		return NULL;
	}

	if (index > 1)
		return NULL;

	if (stereo)
		return (mstudioflexcontroller_t*)((char*)this) + *(&szindex0 + index);

	if (index > 0)
		return NULL;

	return pController();
}


#define STUDIO_CONST	1	// get float
#define STUDIO_FETCH1	2	// get Flexcontroller value
#define STUDIO_FETCH2	3	// get flex weight
#define STUDIO_ADD		4
#define STUDIO_SUB		5
#define STUDIO_MUL		6
#define STUDIO_DIV		7
#define STUDIO_NEG		8	// not implemented
#define STUDIO_EXP		9	// not implemented
#define STUDIO_OPEN		10	// only used in token parsing
#define STUDIO_CLOSE	11
#define STUDIO_COMMA	12	// only used in token parsing
#define STUDIO_MAX		13
#define STUDIO_MIN		14
#define STUDIO_2WAY_0	15	// Fetch a value from a 2 Way slider for the 1st value RemapVal( 0.0, 0.5, 0.0, 1.0 )
#define STUDIO_2WAY_1	16	// Fetch a value from a 2 Way slider for the 2nd value RemapVal( 0.5, 1.0, 0.0, 1.0 )
#define STUDIO_NWAY		17	// Fetch a value from a 2 Way slider for the 2nd value RemapVal( 0.5, 1.0, 0.0, 1.0 )
#define STUDIO_COMBO	18	// Perform a combo operation (essentially multiply the last N values on the stack)
#define STUDIO_DOMINATE	19	// Performs a combination domination operation
#define STUDIO_DME_LOWER_EYELID 20	// 
#define STUDIO_DME_UPPER_EYELID 21	// 

// motion flags
#define STUDIO_X		0x00000001
#define STUDIO_Y		0x00000002	
#define STUDIO_Z		0x00000004
#define STUDIO_XR		0x00000008
#define STUDIO_YR		0x00000010
#define STUDIO_ZR		0x00000020

#define STUDIO_LX		0x00000040
#define STUDIO_LY		0x00000080
#define STUDIO_LZ		0x00000100
#define STUDIO_LXR		0x00000200
#define STUDIO_LYR		0x00000400
#define STUDIO_LZR		0x00000800

#define STUDIO_LINEAR	0x00001000

#define STUDIO_TYPES	0x0003FFFF
#define STUDIO_RLOOP	0x00040000	// controller that wraps shortest distance

// sequence and autolayer flags
#define STUDIO_LOOPING	0x0001		// ending frame should be the same as the starting frame
#define STUDIO_SNAP		0x0002		// do not interpolate between previous animation and this one
#define STUDIO_DELTA	0x0004		// this sequence "adds" to the base sequences, not slerp blends
#define STUDIO_AUTOPLAY	0x0008		// temporary flag that forces the sequence to always play
#define STUDIO_POST		0x0010		// 
#define STUDIO_ALLZEROS	0x0020		// this animation/sequence has no real animation data
#define STUDIO_FRAMEANIM 0x0040		// animation is encoded as by frame x bone instead of RLE bone x frame
#define STUDIO_CYCLEPOSE 0x0080		// cycle index is taken from a pose parameter index
#define STUDIO_REALTIME	0x0100		// cycle index is taken from a real-time clock, not the animations cycle index
#define STUDIO_LOCAL	0x0200		// sequence has a local context sequence
#define STUDIO_HIDDEN	0x0400		// don't show in default selection views
#define STUDIO_OVERRIDE	0x0800		// a forward declared sequence (empty)
#define STUDIO_ACTIVITY	0x1000		// Has been updated at runtime to activity index
#define STUDIO_EVENT	0x2000		// Has been updated at runtime to event index on server
#define STUDIO_WORLD	0x4000		// sequence blends in worldspace
#define STUDIO_NOFORCELOOP 0x8000	// do not force the animation loop
#define STUDIO_EVENT_CLIENT 0x10000	// Has been updated at runtime to event index on client
#define STUDIO_WORLD_AND_RELATIVE 0x20000 // do worldspace blend, then do normal blend on top
#define STUDIO_ROOTXFORM 0x40000	// sequence wants to derive a root re-xform from a given bone

// autolayer flags
//							0x0001
//							0x0002
//							0x0004
//							0x0008
#define STUDIO_AL_POST		0x0010		// 
//							0x0020
#define STUDIO_AL_SPLINE	0x0040		// convert layer ramp in/out curve is a spline instead of linear
#define STUDIO_AL_XFADE		0x0080		// pre-bias the ramp curve to compense for a non-1 weight, assuming a second layer is also going to accumulate
//							0x0100
#define STUDIO_AL_NOBLEND	0x0200		// animation always blends at 1.0 (ignores weight)
//							0x0400
//							0x0800
#define STUDIO_AL_LOCAL		0x1000		// layer is a local context sequence
//							0x2000
#define STUDIO_AL_POSE		0x4000		// layer blends using a pose parameter instead of parent cycle


// Insert this code anywhere that you need to allow for conversion from an old STUDIO_VERSION to a new one.
// If we only support the current version, this function should be empty.
inline bool Studio_ConvertStudioHdrToNewVersion(studiohdr_t* pStudioHdr)
{
	int version = pStudioHdr->version;
	if (version == STUDIO_VERSION)
		return true;

	bool bResult = true;
	if (version < 46)
	{
		// some of the anim index data is incompatible
		for (int i = 0; i < pStudioHdr->numlocalanim; i++)
		{
			mstudioanimdesc_t* pAnim = (mstudioanimdesc_t*)pStudioHdr->pLocalAnimdesc(i);

			// old ANI files that used sections (v45 only) are not compatible
			if (pAnim->sectionframes != 0)
			{
				// zero most everything out
				memset(&(pAnim->numframes), 0, (byte*)(pAnim + 1) - (byte*)&(pAnim->numframes));

				pAnim->numframes = 1;
				pAnim->animblock = -1; // disable animation fetching
				bResult = false;
			}
		}
	}

	if (version < 47)
	{
		// now used to contain zeroframe cache data, make sure it's empty
		if (pStudioHdr->unused4 != 0)
		{
			pStudioHdr->unused4 = 0;
			bResult = false;
		}
		for (int i = 0; i < pStudioHdr->numlocalanim; i++)
		{
			mstudioanimdesc_t* pAnim = (mstudioanimdesc_t*)pStudioHdr->pLocalAnimdesc(i);
			pAnim->zeroframeindex = 0;
			pAnim->zeroframespan = 0;
		}
	}
	else if (version == 47)
	{
		// clear out stale version of zeroframe cache data
		for (int i = 0; i < pStudioHdr->numlocalanim; i++)
		{
			mstudioanimdesc_t* pAnim = (mstudioanimdesc_t*)pStudioHdr->pLocalAnimdesc(i);
			if (pAnim->zeroframeindex != 0)
			{
				pAnim->zeroframeindex = 0;
				pAnim->zeroframespan = 0;
				bResult = false;
			}
		}
	}

	if (version < 49)
	{
		// remove any frameanim flag settings that might be stale
		for (int i = 0; i < pStudioHdr->numlocalanim; i++)
		{
			mstudioanimdesc_t* pAnim = (mstudioanimdesc_t*)pStudioHdr->pLocalAnimdesc(i);
			if (pAnim->flags & STUDIO_FRAMEANIM)
			{
				pAnim->flags &= ~STUDIO_FRAMEANIM;
				bResult = false;
			}
		}
	}
	// for now, just slam the version number since they're compatible
	pStudioHdr->version = STUDIO_VERSION;

	return bResult;
}

// must be Init to fixup with specified rootLOD
inline void Studio_SetRootLOD(studiohdr_t* pStudioHdr, int rootLOD)
{
	// honor studiohdr restriction of root lod in case requested root lod exceeds restriction.
	if (pStudioHdr->numAllowedRootLODs > 0 &&
		rootLOD >= pStudioHdr->numAllowedRootLODs)
	{
		rootLOD = pStudioHdr->numAllowedRootLODs - 1;
	}

	// init the lod fixups that culls higher detail lods
	// vertexes are external, fixups ensure relative offsets and counts are cognizant of shrinking data
	// indexes are built in lodN..lod0 order so higher detail lod data can be truncated at load
	// the fixup lookup arrays are filled (or replicated) to ensure all slots valid
	int vertexindex = 0;
	int tangentsindex = 0;
	int bodyPartID;
	for (bodyPartID = 0; bodyPartID < pStudioHdr->numbodyparts; bodyPartID++)
	{
		mstudiobodyparts_t* pBodyPart = pStudioHdr->pBodypart(bodyPartID);
		int modelID;
		for (modelID = 0; modelID < pBodyPart->nummodels; modelID++)
		{
			mstudiomodel_t* pModel = pBodyPart->pModel(modelID);
			int totalMeshVertexes = 0;
			int meshID;
			for (meshID = 0; meshID < pModel->nummeshes; meshID++)
			{
				mstudiomesh_t* pMesh = pModel->pMesh(meshID);

				// get the fixup, vertexes are reduced
				pMesh->numvertices = pMesh->vertexdata.numLODVertexes[rootLOD];
				pMesh->vertexoffset = totalMeshVertexes;
				totalMeshVertexes += pMesh->numvertices;
			}

			// stay in sync
			pModel->numvertices = totalMeshVertexes;
			pModel->vertexindex = vertexindex;
			pModel->tangentsindex = tangentsindex;

			vertexindex += totalMeshVertexes * sizeof(mstudiovertex_t);
			tangentsindex += totalMeshVertexes * sizeof(Vector2D);
		}
	}

	// track the set desired configuration
	pStudioHdr->rootLOD = rootLOD;
}

// Determines allocation requirements for vertexes
inline int Studio_VertexDataSize(const vertexFileHeader_t* pVvdHdr, int rootLOD, bool bNeedsTangentS, bool bHasExtraData)
{
	// the quantity of vertexes necessary for root lod and all lower detail lods
	// add one extra vertex to each section
	// the extra vertex allows prefetch hints to read ahead 1 vertex without faulting
	int numVertexes = pVvdHdr->numLODVertexes[rootLOD] + 1;
	int dataLength = pVvdHdr->vertexDataStart + numVertexes * sizeof(mstudiovertex_t);
	if (bNeedsTangentS)
	{
		dataLength += numVertexes * sizeof(Vector2D);
#ifdef _PS3
		if (!pVvdHdr->numFixups && (pVvdHdr->ps3edgeDmaInputDesc & 0x80000000))
		{
			// PS3 does not support root lod, so all vertexes will be used
			dataLength = pVvdHdr->GetPs3EdgeDmaInputStart() + pVvdHdr->GetPs3EdgeDmaInputLength();
		}
#endif
	}

	if (bHasExtraData)
	{
		ExtraVertexAttributesHeader_t* pExtraDataHdr = (ExtraVertexAttributesHeader_t*)((byte*)pVvdHdr + pVvdHdr->tangentDataStart + (numVertexes - 1) * sizeof(Vector2D));
		dataLength += pExtraDataHdr->m_totalbytes;
	}

	// allocate this much
	return dataLength;
}

// Load the minimum quantity of verts and Init fixups
inline int Studio_LoadVertexes(const vertexFileHeader_t* pTempVvdHdr, vertexFileHeader_t* pNewVvdHdr, int rootLOD, bool bNeedsTangentS, bool bHasExtraData)
{
	int					i;
	int					target;
	int					numVertexes;
	vertexFileFixup_t* pFixupTable;
	ExtraVertexAttributeIndex_t* pExtraIndex = NULL;
	ExtraVertexAttributeIndex_t* pNewExtraIndex = NULL;
	ExtraVertexAttributesHeader_t* pExtraHeader = NULL;
	ExtraVertexAttributesHeader_t* pNewExtraHeader = NULL;

	numVertexes = pTempVvdHdr->numLODVertexes[rootLOD];

	// copy all data up to start of vertexes
	memcpy((void*)pNewVvdHdr, (void*)pTempVvdHdr, pTempVvdHdr->vertexDataStart);

	for (i = 0; i < rootLOD; i++)
	{
		pNewVvdHdr->numLODVertexes[i] = pNewVvdHdr->numLODVertexes[rootLOD];
	}

	// fixup data starts
	if (bNeedsTangentS)
	{
		// tangent data follows possibly reduced vertex data
		pNewVvdHdr->tangentDataStart = pNewVvdHdr->vertexDataStart + numVertexes * sizeof(mstudiovertex_t);

#ifdef _PS3
		// PS3 does not support root LOD, so all vertices will be available
		if (!pTempVvdHdr->numFixups && (pTempVvdHdr->ps3edgeDmaInputDesc & 0x80000000))
		{
			pNewVvdHdr->ps3edgeDmaInputDesc = pTempVvdHdr->ps3edgeDmaInputDesc;
		}
		else
		{
			pNewVvdHdr->ps3edgeDmaInputDesc = 0;
		}
#endif
	}
	else
	{
		// no tangent data will be available, mark for identification
		pNewVvdHdr->tangentDataStart = 0;

#ifdef _PS3
		pNewVvdHdr->ps3edgeDmaInputDesc = 0;
#endif
	}

	if (bHasExtraData)
	{
		pExtraHeader = (ExtraVertexAttributesHeader_t*)((byte*)pTempVvdHdr + pTempVvdHdr->tangentDataStart + numVertexes * sizeof(Vector2D));
		pExtraIndex = (ExtraVertexAttributeIndex_t*)(pExtraHeader + 1);

		pNewExtraHeader = (ExtraVertexAttributesHeader_t*)((byte*)pNewVvdHdr + pNewVvdHdr->tangentDataStart + numVertexes * sizeof(Vector2D));
		pNewExtraIndex = (ExtraVertexAttributeIndex_t*)(pNewExtraHeader + 1);
	}

	if (!pNewVvdHdr->numFixups)
	{
		// fixups not required
		// transfer vertex data
		memcpy(
			(byte*)pNewVvdHdr + pNewVvdHdr->vertexDataStart,
			(byte*)pTempVvdHdr + pTempVvdHdr->vertexDataStart,
			numVertexes * sizeof(mstudiovertex_t));

		if (bNeedsTangentS)
		{
			// transfer tangent data to cache memory
			memcpy(
				(byte*)pNewVvdHdr + pNewVvdHdr->tangentDataStart,
				(byte*)pTempVvdHdr + pTempVvdHdr->tangentDataStart,
				numVertexes * sizeof(Vector2D));

#ifdef _PS3
			if (pNewVvdHdr->ps3edgeDmaInputDesc)
			{
				// transfer EDGE DMA INPUT to cache memory
				memcpy(
					const_cast<byte*>(pNewVvdHdr->GetPs3EdgeDmaInput()),
					pTempVvdHdr->GetPs3EdgeDmaInput(),
					pTempVvdHdr->GetPs3EdgeDmaInputLength()
				);
			}
#endif
		}

		if (bHasExtraData)
		{
			// Memcpy whole blob of extra bytes
			memcpy(pNewExtraHeader, pExtraHeader, pExtraHeader->m_totalbytes);
		}

		return numVertexes;
	}

	// fixups required
	// re-establish mesh ordered vertexes into cache memory, according to table
	target = 0;
	pFixupTable = (vertexFileFixup_t*)((byte*)pTempVvdHdr + pTempVvdHdr->fixupTableStart);
	for (i = 0; i < pTempVvdHdr->numFixups; i++)
	{
		if (pFixupTable[i].lod < rootLOD)
		{
			// working bottom up, skip over copying higher detail lods
			continue;
		}

		// copy vertexes
		memcpy(
			(mstudiovertex_t*)((byte*)pNewVvdHdr + pNewVvdHdr->vertexDataStart) + target,
			(mstudiovertex_t*)((byte*)pTempVvdHdr + pTempVvdHdr->vertexDataStart) + pFixupTable[i].sourceVertexID,
			pFixupTable[i].numVertexes * sizeof(mstudiovertex_t));

		if (bNeedsTangentS)
		{
			// copy tangents
			memcpy(
				(Vector2D*)((byte*)pNewVvdHdr + pNewVvdHdr->tangentDataStart) + target,
				(Vector2D*)((byte*)pTempVvdHdr + pTempVvdHdr->tangentDataStart) + pFixupTable[i].sourceVertexID,
				pFixupTable[i].numVertexes * sizeof(Vector2D));
		}

		if (bHasExtraData)
		{
			// Memcpy header and index records
			memcpy(pNewExtraHeader, pExtraHeader, sizeof(ExtraVertexAttributesHeader_t) + sizeof(ExtraVertexAttributeIndex_t) * pExtraHeader->m_count);
			// copy extra data
			for (int e = 0; e < pExtraHeader->m_count; ++e)
			{
				int offset = pExtraIndex[e].m_offset;
				int bytesPerVertex = pExtraIndex[e].m_bytes;
				memcpy((byte*)pNewExtraHeader + offset + target * bytesPerVertex,
					(byte*)pExtraHeader + offset + pFixupTable[i].sourceVertexID * bytesPerVertex,
					pFixupTable[i].numVertexes * bytesPerVertex);
			}
		}

		// data is placed consecutively
		target += pFixupTable[i].numVertexes;
	}

	pNewVvdHdr->numFixups = 0;
	pNewVvdHdr->fixupTableStart = 0; // otherwise PS3 will be confusing it for EDGE DMA INPUT

	return target;
}

class model_t {
public:
	void* m_handle;
	char   m_Name[260];
	int    m_load_flags;
	int    m_server_count;
	int    m_Type;
	int    m_Flags;
	Vector vecMin;
	Vector vecMax;
	float  flCapsuleRadius;

private:
	PAD(0x1C);
};


class KeyValues;
struct vcollide_t;
class CGameTrace;
struct cplane_t;
typedef CGameTrace trace_t;
struct virtualterrainparams_t;
class CPhysCollide;
typedef unsigned short MDLHandle_t;
class IClientRenderable;
class SCombinerModelInput_t;

class CUtlBuffer
{
	// Brian has on his todo list to revisit this as there are issues in some cases with CUtlVector using operator = instead of copy construtor in InsertMultiple, etc.
	// The unsafe case is something like this:
	//  CUtlVector< CUtlBuffer > vecFoo;
	// 
	//  CUtlBuffer buf;
	//  buf.Put( xxx );
	//  vecFoo.Insert( buf );
	//
	//  This will cause memory corruption when vecFoo is cleared
	//
	//private:
	//	// Disallow copying
	//	CUtlBuffer( const CUtlBuffer & );// { Assert( 0 ); }
	//	CUtlBuffer &operator=( const CUtlBuffer & );//  { Assert( 0 ); return *this; }

public:
	enum SeekType_t
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	// flags
	enum BufferFlags_t
	{
		TEXT_BUFFER = 0x1,			// Describes how get + put work (as strings, or binary)
		EXTERNAL_GROWABLE = 0x2,	// This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting.
		CONTAINS_CRLF = 0x4,		// For text buffers only, does this contain \n or \n\r?
		READ_ONLY = 0x8,			// For external buffers; prevents null termination from happening.
		AUTO_TABS_DISABLED = 0x10,	// Used to disable/enable push/pop tabs
	};

	// Overflow functions when a get or put overflows
	typedef bool (CUtlBuffer::* UtlBufferOverflowFunc_t)(int nSize);

	// Constructors for growable + external buffers for serialization/unserialization
	CUtlBuffer(int growSize = 0, int initSize = 0, int nFlags = 0);
	CUtlBuffer(const void* pBuffer, int size, int nFlags = 0);
	// This one isn't actually defined so that we catch contructors that are trying to pass a bool in as the third param.
	CUtlBuffer(const void* pBuffer, int size, bool crap) = delete;

	// UtlBuffer objects should not be copyable; we do a slow copy if you use this but it asserts.
	// (REI: I'd like to delete these but we have some python bindings that currently rely on being able to copy these objects)
	CUtlBuffer(const CUtlBuffer&); // = delete;
	CUtlBuffer& operator= (const CUtlBuffer&); // = delete;

#if VALVE_CPP11
	// UtlBuffer is non-copyable (same as CUtlMemory), but it is moveable.  We would like to declare these with '= default'
	// but unfortunately VS2013 isn't fully C++11 compliant, so we have to manually declare these in the boilerplate way.
	CUtlBuffer(CUtlBuffer&& moveFrom); // = default;
	CUtlBuffer& operator= (CUtlBuffer&& moveFrom); // = default;
#endif

	unsigned char	GetFlags() const;

	// NOTE: This will assert if you attempt to recast it in a way that
	// is not compatible. The only valid conversion is binary-> text w/CRLF
	void			SetBufferType(bool bIsText, bool bContainsCRLF);

	// Makes sure we've got at least this much memory
	void			EnsureCapacity(int num);

	// Access for direct read into buffer
	void* AccessForDirectRead(int nBytes);

	// Attaches the buffer to external memory....
	void			SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags = 0);
	bool			IsExternallyAllocated() const;
	void			AssumeMemory(void* pMemory, int nSize, int nInitialPut, int nFlags = 0);
	void* Detach();
	void* DetachMemory();

	// copies data from another buffer
	void			CopyBuffer(const CUtlBuffer& buffer);
	void			CopyBuffer(const void* pubData, int cubData);

	// Controls endian-ness of binary utlbufs - default matches the current platform
	void			ActivateByteSwapping(bool bActivate);
	void			SetBigEndian(bool bigEndian);
	bool			IsBigEndian(void);

	// Resets the buffer; but doesn't free memory
	void			Clear();

	// Clears out the buffer; frees memory
	void			Purge();

	// Dump the buffer to stdout
	void			Spew();

	// Read stuff out.
	// Binary mode: it'll just read the bits directly in, and characters will be
	//		read for strings until a null character is reached.
	// Text mode: it'll parse the file, turning text #s into real numbers.
	//		GetString will read a string until a space is reached
	char			GetChar();
	unsigned char	GetUnsignedChar();
	short			GetShort();
	unsigned short	GetUnsignedShort();
	int				GetInt();
	unsigned int	GetIntHex();
	unsigned int	GetUnsignedInt();
	float			GetFloat();
	double			GetDouble();
	void* GetPtr();
	void			GetString(char* pString, int nMaxChars);
	bool			Get(void* pMem, int size);
	void			GetLine(char* pLine, int nMaxChars);

	// Used for getting objects that have a byteswap datadesc defined
	template <typename T> void GetObjects(T* dest, int count = 1);

	// This will get at least 1 byte and up to nSize bytes. 
	// It will return the number of bytes actually read.
	int				GetUpTo(void* pMem, int nSize);

	// This will return the # of characters of the string about to be read out
	// NOTE: The count will *include* the terminating 0!!
	// In binary mode, it's the number of characters until the next 0
	// In text mode, it's the number of characters until the next space.
	int				PeekStringLength();

	// Eats white space, advances Get index
	void			EatWhiteSpace();

	// Eats C++ style comments
	bool			EatCPPComment();

	// (For text buffers only)
	// Parse a token from the buffer:
	// Grab all text that lies between a starting delimiter + ending delimiter
	// (skipping whitespace that leads + trails both delimiters).
	// If successful, the get index is advanced and the function returns true,
	// otherwise the index is not advanced and the function returns false.
	bool			ParseToken(const char* pStartingDelim, const char* pEndingDelim, char* pString, int nMaxLen);

	// Advance the get index until after the particular string is found
	// Do not eat whitespace before starting. Return false if it failed
	// String test is case-insensitive.
	bool			GetToken(const char* pToken);

	// Write stuff in
	// Binary mode: it'll just write the bits directly in, and strings will be
	//		written with a null terminating character
	// Text mode: it'll convert the numbers to text versions
	//		PutString will not write a terminating character
	void			PutChar(char c);
	void			PutUnsignedChar(unsigned char uc);
	void			PutShort(short s);
	void			PutUnsignedShort(unsigned short us);
	void			PutInt(int i);
	void			PutUnsignedInt(unsigned int u);
	void			PutFloat(float f);
	void			PutDouble(double d);
	void			PutPtr(void*); // Writes the pointer, not the pointed to
	void			PutString(const char* pString);
	void			Put(const void* pMem, int size);

	// Used for putting objects that have a byteswap datadesc defined
	template <typename T> void PutObjects(T* src, int count = 1);

	// What am I writing (put)/reading (get)?
	void* PeekPut(int offset = 0);
	const void* PeekGet(int offset = 0) const;
	const void* PeekGet(int nMaxSize, int nOffset);

	// Where am I writing (put)/reading (get)?
	int TellPut() const;
	int TellGet() const;

	// What's the most I've ever written?
	int TellMaxPut() const;

	// How many bytes remain to be read?
	// NOTE: This is not accurate for streaming text files; it overshoots
	int GetBytesRemaining() const;

	// Change where I'm writing (put)/reading (get)
	void SeekPut(SeekType_t type, int offset);
	void SeekGet(SeekType_t type, int offset);

	// Buffer base
	const void* Base() const;
	void* Base();

	// memory allocation size, does *not* reflect size written or read,
	//	use TellPut or TellGet for that
	int Size() const;

	// Am I a text buffer?
	bool IsText() const;

	// Can I grow if I'm externally allocated?
	bool IsGrowable() const;

	// Am I valid? (overflow or underflow error), Once invalid it stays invalid
	bool IsValid() const;

	// Do I contain carriage return/linefeeds? 
	bool ContainsCRLF() const;

	// Am I read-only
	bool IsReadOnly() const;

	// Converts a buffer from a CRLF buffer to a CR buffer (and back)
	// Returns false if no conversion was necessary (and outBuf is left untouched)
	// If the conversion occurs, outBuf will be cleared.
	bool ConvertCRLF(CUtlBuffer& outBuf);

	// Push/pop pretty-printing tabs
	void PushTab();
	void PopTab();

	// Temporarily disables pretty print
	void EnableTabs(bool bEnable);

#if !defined( _GAMECONSOLE )
	// Swap my internal memory with another buffer,
	// and copy all of its other members
	void SwapCopy(CUtlBuffer& other);
#endif

protected:
	// error flags
	enum
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
		MAX_ERROR_FLAG = GET_OVERFLOW,
	};

	void SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc);

	bool OnPutOverflow(int nSize);
	bool OnGetOverflow(int nSize);

protected:
	// Checks if a get/put is ok
	bool CheckPut(int size);
	bool CheckGet(int size);

	// NOTE: Pass in nPut here even though it is just a copy of m_Put.  This is almost always called immediately 
	// after modifying m_Put and this lets it stay in a register
	void AddNullTermination(int nPut);

	// Methods to help with pretty-printing
	bool WasLastCharacterCR();
	void PutTabs();

	// Default overflow funcs
	bool PutOverflow(int nSize);
	bool GetOverflow(int nSize);

	// Does the next bytes of the buffer match a pattern?
	bool PeekStringMatch(int nOffset, const char* pString, int nLen);

	// Peek size of line to come, check memory bound
	int	PeekLineLength();

	// How much whitespace should I skip?
	int PeekWhiteSpace(int nOffset);

	// Checks if a peek get is ok
	bool CheckPeekGet(int nOffset, int nSize);

	// Call this to peek arbitrarily long into memory. It doesn't fail unless
	// it can't read *anything* new
	bool CheckArbitraryPeekGet(int nOffset, int& nIncrement);

	template <typename T> void GetType(T& dest);
	template <typename T> void GetTypeBin(T& dest);
	template <typename T> bool GetTypeText(T& value, int nRadix = 10);
	template <typename T> void GetObject(T* src);

	template <typename T> void PutType(T src);
	template <typename T> void PutTypeBin(T src);
	template <typename T> void PutObject(T* src);

	// be sure to also update the copy constructor
	// and SwapCopy() when adding members.
	CUtlMemory<unsigned char> m_Memory;
	int m_Get;
	int m_Put;

	unsigned char m_Error;
	unsigned char m_Flags;
	unsigned char m_Reserved;
#if defined( _GAMECONSOLE )
	unsigned char pad;
#endif

	int m_nTab;
	int m_nMaxPut;
	int m_nOffset;

	UtlBufferOverflowFunc_t m_GetOverflowFunc;
	UtlBufferOverflowFunc_t m_PutOverflowFunc;
};

// ik this is really meant to be here but boo fucking hoo. /monster/
struct RenderableInfo_t {
	IClientRenderable* pRenderable;
	void* alpha_property;
	int enum_count;
	int render_frame;
	unsigned short first_shadow;
	unsigned short leaf_list;
	short area;
	uint16_t nFlags;
	uint16_t nFlags2;
	Vector bloated_abs_mins;
	Vector bloated_abs_maxs;
	Vector abs_mins;
	Vector abs_maxs;
	int pad;
};

//-----------------------------------------------------------------------------
// Purpose: a callback class that is notified when a model has finished loading
//-----------------------------------------------------------------------------
class IModelLoadCallback
{
public:
	virtual void OnModelLoadComplete(const model_t* pModel) = 0;

protected:
	// Protected destructor so that nobody tries to delete via this interface.
	// Automatically unregisters if the callback is destroyed while still pending.
	~IModelLoadCallback();
};

//-----------------------------------------------------------------------------
// Indicates the type of translucency of an unmodulated renderable
//-----------------------------------------------------------------------------
enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
};

//-----------------------------------------------------------------------------
// Combiner
//-----------------------------------------------------------------------------
#define COMBINER_MAX_MODELS						15
#define COMBINER_MAX_LODS						8
#define COMBINER_MAX_TEXTURES_PER_MATERIAL		5
#define COMBINER_MAX_ATLAS_GROUPS				5
#define COMBINER_MAX_MATERIALS_PER_INPUT_MODEL	4

typedef struct SCombinedResults
{
	// individual model info
	int							m_nNumVerts[COMBINER_MAX_MODELS][COMBINER_MAX_LODS];
	int							m_nNumIndexes[COMBINER_MAX_MODELS][COMBINER_MAX_LODS];
	int							m_nBatches[COMBINER_MAX_MODELS][COMBINER_MAX_LODS];
	int							m_nNumLODs[COMBINER_MAX_MODELS];

	// cumulative combined info
	int							m_nCombinedNumVerts[COMBINER_MAX_LODS];
	int							m_nCombinedNumIndexes[COMBINER_MAX_LODS];
	int							m_nCombinedBatches[COMBINER_MAX_LODS];
	int							m_nCombinedNumLODs;
	int							m_nNumTexturePackIterations;
	unsigned int				m_nCombinedResults;
	char						m_szErrorMessage[256];
	char						m_szErrorDetails[4096];
	unsigned int				m_nDetailedError;
	float						m_flModelLoadDuration;
	float						m_flModelCombineDuration;
	float						m_flTextureLoadDuration;
	float						m_flTextureCombineDuration;
	float						m_flEngineProcessingDuration;
} TCombinedResults;

typedef void (*CombinedModelLoadedCallback)(void* pUserData, MDLHandle_t OldHandle, MDLHandle_t NewHandle, TCombinedResults& CombinedResults);

class IVModelInfo {
public:
	// indexes for virtuals and hooks.
	enum indices : size_t {
		GETMODEL = 1,
		GETMODELINDEX = 2,
		GETMODELFRAMECOUNT = 8,
		GETSTUDIOMODEL = 30,
		FINDORLOADMODEL = 43
	};

public:
	virtual							~IVModelInfo(void) { }

	virtual const model_t* get_model(int modelindex) const = 0;
	// Returns index of model by name
	virtual int						GetModelIndex(const char* name) const = 0;

	// Returns name of model
	virtual const char* GetModelName(const model_t* model) const = 0;
	virtual vcollide_t* GetVCollide(const model_t* model) const = 0;
	virtual vcollide_t* GetVCollide(int modelindex) const = 0;
	virtual void					GetModelBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
	virtual	void					GetModelRenderBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
	virtual int						GetModelFrameCount(const model_t* model) const = 0;
	virtual int						GetModelType(const model_t* model) const = 0;
	virtual void* GetModelExtraData(const model_t* model) = 0;
	virtual bool					ModelHasMaterialProxy(const model_t* model) const = 0;
	virtual bool					IsTranslucent(model_t const* model) const = 0;
	virtual bool					IsTranslucentTwoPass(const model_t* model) const = 0;
	virtual void					Unused0() {};
	virtual RenderableTranslucencyType_t ComputeTranslucencyType(const model_t* model, int nSkin, int nBody) = 0;
	virtual int						GetModelMaterialCount(const model_t* model) const = 0;
	virtual int 					GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial) = 0;
	virtual bool					IsModelVertexLit(const model_t* model) const = 0;
	virtual const char* GetModelKeyValueText(const model_t* model) = 0;
	virtual bool					GetModelKeyValue(const model_t* model, CUtlBuffer& buf) = 0; // supports keyvalue blocks in submodels
	virtual float					GetModelRadius(const model_t* model) = 0;

	virtual const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** cache, const char* modelname) const = 0;
	virtual const studiohdr_t* FindModel(void* cache) const = 0;
	virtual	virtualmodel_t* GetVirtualModel(const studiohdr_t* pStudioHdr) const = 0;
	virtual byte* GetAnimBlock(const studiohdr_t* pStudioHdr, int nBlock, bool bPreloadIfMissing) const = 0;
	virtual bool					HasAnimBlockBeenPreloaded(const studiohdr_t* pStudioHdr, int nBlock) const = 0;

	// Available on client only!!!
	virtual void					GetModelMaterialColorAndLighting(const model_t* model, Vector const& origin,
		QAngle const& angles, trace_t* pTrace,
		Vector& lighting, Vector& matColor) = 0;
	virtual void					GetIlluminationPoint(const model_t* model, IClientRenderable* pRenderable, Vector const& origin,
		QAngle const& angles, Vector* pLightingCenter) = 0;

	virtual int						GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t* GetStudioModel(const model_t* mod) = 0;
	virtual int						GetModelSpriteWidth(const model_t* model) const = 0;
	virtual int						GetModelSpriteHeight(const model_t* model) const = 0;

	// Sets/gets a map-specified fade range (client only)
	virtual void					SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void					GetLevelScreenFadeRange(float* pMinArea, float* pMaxArea) const = 0;

	// Sets/gets a map-specified per-view fade range (client only)
	virtual void					SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;

	// Computes fade alpha based on distance fade + screen fade (client only)
	virtual unsigned char			ComputeLevelScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char			ComputeViewScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;

	// both client and server
	virtual int						GetAutoplayList(const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList) const = 0;

	// Gets a virtual terrain collision model (creates if necessary)
	// NOTE: This may return NULL if the terrain model cannot be virtualized
	virtual CPhysCollide* GetCollideForVirtualTerrain(int index) = 0;

	virtual bool					IsUsingFBTexture(const model_t* model, int nSkin, int nBody, void /*IClientRenderable*/* pClientRenderable) const = 0;

	virtual const model_t* FindOrLoadModel(const char* name) const = 0;

	virtual MDLHandle_t				GetCacheHandle(const model_t* model) const = 0;

	// Returns planes of non-nodraw brush model surfaces
	virtual int						GetBrushModelPlaneCount(const model_t* model) const = 0;
	virtual void					GetBrushModelPlane(const model_t* model, int nIndex, cplane_t& plane, Vector* pOrigin) const = 0;
	virtual int						GetSurfacepropsForVirtualTerrain(int index) = 0;
	virtual bool					UsesEnvCubemap(const model_t* model) const = 0;
	virtual bool					UsesStaticLighting(const model_t* model) const = 0;

	// Returns index of model by name, dynamically registered if not already known.
	virtual int						RegisterDynamicModel(const char* name, bool bClientSide) = 0;
	virtual int						RegisterCombinedDynamicModel(const char* pszName, MDLHandle_t Handle) = 0;
	virtual void					UpdateCombinedDynamicModel(int nModelIndex, MDLHandle_t Handle) = 0;

	virtual int						BeginCombinedModel(const char* pszName, bool bReuseExisting) = 0;
	virtual bool					SetCombineModels(int nModelIndex, const CUtlVector< SCombinerModelInput_t >& vecModelsToCombine) = 0;
	virtual bool					FinishCombinedModel(int nModelIndex, CombinedModelLoadedCallback pFunc, void* pUserData = NULL) = 0;
	virtual void					ReleaseCombinedModel(int nModelIndex) = 0;

	virtual bool					IsDynamicModelLoading(int modelIndex) = 0;
	virtual void					AddRefDynamicModel(int modelIndex) = 0;
	virtual void					ReleaseDynamicModel(int modelIndex) = 0;

	// Registers callback for when dynamic model has finished loading.
	virtual bool					RegisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback, bool bCallImmediatelyIfLoaded = true) = 0;
	virtual void					UnregisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback) = 0;

	// Poked by engine
	virtual void					OnLevelChange() = 0;

	virtual KeyValues* GetModelKeyValues(const model_t* pModel) = 0;

	virtual void					UpdateViewWeaponModelCache(const char** ppWeaponModels, int nWeaponModels) = 0;
	virtual void					TouchWorldWeaponModelCache(const char** ppWeaponModels, int nWeaponModels) = 0;

};
