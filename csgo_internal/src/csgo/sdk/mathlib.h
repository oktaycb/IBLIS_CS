//===== Copyright  1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

typedef float				float32;
typedef double				float64;

// for when we don't care about how many bits we use
typedef unsigned int		uint;

template<class T>
__forceinline T Square(T const& a)
{
	return a * a;
}

__forceinline bool IsPowerOfTwo(uint x)
{
	return (x & (x - 1)) == 0;
}

// return the smallest power of two >= x.
// returns 0 if x == 0 or x > 0x80000000 (ie numbers that would be negative if x was signed)
// NOTE: the old code took an int, and if you pass in an int of 0x80000000 casted to a uint,
//       you'll get 0x80000000, which is correct for uints, instead of 0, which was correct for ints
__forceinline uint SmallestPowerOfTwoGreaterOrEqual(uint x)
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

// return the largest power of two <= x. Will return 0 if passed 0
__forceinline uint LargestPowerOfTwoLessThanOrEqual(uint x)
{
	if (x >= 0x80000000)
		return 0x80000000;

	return SmallestPowerOfTwoGreaterOrEqual(x + 1) >> 1;
}