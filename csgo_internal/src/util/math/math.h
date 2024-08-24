#pragma once
#include "../../csgo/sdk/vector2.h"

#define fsel(c,x,y) ( (c) >= 0 ? (x) : (y) )

namespace Math {
	// pi constants.
	constexpr float pi = 3.1415926535897932384f; // pi
	constexpr float pi_2 = pi * 2.f;               // pi * 2
	constexpr float invtwopi = 0.1591549f;
	constexpr float threehalfpi = 4.7123889f;
	constexpr float halfpi = 1.570796f;
	static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	static unsigned g_nRandomValues[256] = { 238,	164,	191,	168,	115,	 16,	142,	 11,	213,	214,	 57,	151,	248,	252,	 26,	198,13,	105,	102,	 25,	 43,	 42,	227,	107,	210,	251,	 86,	 66,	 83,	193,	126,	108,131,	  3,	 64,	186,	192,	 81,	 37,	158,	 39,	244,	 14,	254,	 75,	 30,	  2,	 88,172,	176,	255,	 69,	  0,	 45,	116,	139,	 23,	 65,	183,	148,	 33,	 46,	203,	 20,143,	205,	 60,	197,	118,	  9,	171,	 51,	233,	135,	220,	 49,	 71,	184,	 82,	109,36,	161,	169,	150,	 63,	 96,	173,	125,	113,	 67,	224,	 78,	232,	215,	 35,	219,79,	181,	 41,	229,	149,	153,	111,	217,	 21,	 72,	120,	163,	133,	 40,	122,	140,208,	231,	211,	200,	160,	182,	104,	110,	178,	237,	 15,	101,	 27,	 50,	 24,	189,177,	130,	187,	 92,	253,	136,	100,	212,	 19,	174,	 70,	 22,	170,	206,	162,	 74,247,	  5,	 47,	 32,	179,	117,	132,	195,	124,	123,	245,	128,	236,	223,	 12,	 84,54,	218,	146,	228,	157,	 94,	106,	 31,	 17,	 29,	194,	 34,	 56,	134,	239,	246,241,	216,	127,	 98,	  7,	204,	154,	152,	209,	188,	 48,	 61,	 87,	 97,	225,	 85,90,	167,	155,	112,	145,	114,	141,	 93,	250,	  4,	201,	156,	 38,	 89,	226,	196,1,	235,	 44,	180,	159,	121,	119,	166,	190,	144,	 10,	 91,	 76,	230,	221,	 80,207,	 55,	 58,	 53,	175,	  8,	  6,	 52,	 68,	242,	 18,	222,	103,	249,	147,	129,138,	243,	 28,	185,	 62,	 59,	240,	202,	234,	 99,	 77,	 73,	199,	137,	 95,	165, };

	typedef unsigned char uint8;

	unsigned __fastcall HashInt(const int n);
	unsigned __fastcall HashString(const char* pszKey);
	unsigned __fastcall HashStringCaseless(const char* pszKey);

	__forceinline unsigned long const& FloatBits(float const& f)
	{
		return *reinterpret_cast<unsigned long const*>(&f);
	}

	__forceinline bool IsFinite(const float& f)
	{
		return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
	}

	// degrees to radians.
	__forceinline constexpr float DEG2RAD(float val) {
		return val * (pi / 180.f);
	}

	// radians to degrees.
	__forceinline constexpr float RAD2DEG(float val) {
		return val * (180.f / pi);
	}

	__forceinline static float fInterpolate(const float from, const float to, const float percent)
	{
		if (from == to)
			return from;

		return to * percent + from * (1.f - percent);
	}

	// angle mod ( shitty Normalize ).
	__forceinline float AngleMod(float angle) {
		return (360.f / 65536) * ((int)(angle * (65536.f / 360.f)) & 65535);
	}

	typedef __declspec(align(16)) union {
		float f[4];
		__m128 v;
	} m128;

	inline __m128 sqrt_ps(const __m128 squared) {
		return _mm_sqrt_ps(squared);
	}

	__forceinline __m128 cos_52s_ps(const __m128 x)
	{
		const auto c1 = _mm_set1_ps(0.9999932946f);
		const auto c2 = _mm_set1_ps(-0.4999124376f);
		const auto c3 = _mm_set1_ps(0.0414877472f);
		const auto c4 = _mm_set1_ps(-0.0012712095f);
		const auto x2 = _mm_mul_ps(x, x);
		return _mm_add_ps(c1, _mm_mul_ps(x2, _mm_add_ps(c2, _mm_mul_ps(x2, _mm_add_ps(c3, _mm_mul_ps(c4, x2))))));
	}

	__forceinline __m128 cos_ps(__m128 angle)
	{
		angle = _mm_andnot_ps(signmask, angle);
		angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(pi_2)));

		auto cosangle = angle;
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi_2), angle))));

		auto result = cos_52s_ps(cosangle);
		result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
		return result;
	}

	__forceinline __m128 sin_ps(const __m128 angle)
	{
		return cos_ps(_mm_sub_ps(_mm_set1_ps(halfpi), angle));
	}

	__forceinline void sincos_ps(__m128 angle, __m128* sin, __m128* cos) {
		const auto anglesign = _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(signmask, angle));
		angle = _mm_andnot_ps(signmask, angle);
		angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(pi_2)));

		auto cosangle = angle;
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
		cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi_2), angle))));

		auto result = cos_52s_ps(cosangle);
		result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
		*cos = result;

		const auto sinmultiplier = _mm_mul_ps(anglesign, _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(_mm_cmpgt_ps(angle, _mm_set1_ps(pi)), signmask)));
		*sin = _mm_mul_ps(sinmultiplier, sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(result, result))));
	}

	matrix3x4_t AngleMatrix(const QAngle angles);

	void AngleMatrix(const QAngle& ang, const Vector& pos, matrix3x4_t& out);

	float ClampCycle(float flCycleIn);

	float DotProduct(const Vector& a, const Vector& b);
	float DotProduct(const Vector* a, const Vector* b);

	float DistanceToRay(const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along = nullptr, Vector* pointOnRay = nullptr);

	float AngleNormalize(float angle);
	float NormalizeYaw(float flAngle);

	// normalizes an angle.
	void NormalizeAngle(float& angle);

	void RotatePoint(Vector2D& point, Vector2D origin, bool clockwise, float angle);

	__forceinline float NormalizedAngle(float angle) {
		NormalizeAngle(angle);
		return angle;
	}

	QAngle CalcAngle(const Vector& vecSource, const Vector& vecDestination);
	float AngleDiff(float destAngle, float srcAngle);
	float Approach(float target, float value, float speed);
	Vector Approach(Vector target, Vector value, float speed);
	float ApproachAngle(float target, float value, float speed);
	void  NormalizeVector(Vector& vec);
	void  VectorAngles(const Vector& forward, const Vector& pseudoup, QAngle& angles);
	void  VectorAngles(const Vector& forward, QAngle& ang, Vector* up = nullptr);
	void  AngleVectors(const QAngle& angles, Vector* forward, Vector* right = nullptr, Vector* up = nullptr);
	void  SinCos(const float x, float& sin, float& cos) noexcept;
	float GetFOV(const QAngle& aimAngle, const QAngle& viewAngle);
	void  VectorTransform(const Vector& in, const matrix3x4_t& matrix, Vector& out);
	float SegmentToSegment(const Vector s1, const Vector s2, const Vector k1, const Vector k2);
	void  VectorITransform(const Vector& in, const matrix3x4_t& matrix, Vector& out);
	void  MatrixAngles(const matrix3x4_t& matrix, QAngle& angles);
	void MatrixPosition(const matrix3x4_t& matrix, Vector& position);
	void MatrixAngles(const matrix3x4_t& matrix, QAngle& angles, Vector& position);
	void  MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out);
	void  ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);

	// computes the intersection of a ray with a box ( AABB ).
	bool IntersectRayWithBox(const Vector& start, const Vector& Delta, const Vector& mins, const Vector& maxs, float tolerance, BoxTraceInfo_t* out_info);
	bool IntersectRayWithBox(const Vector& start, const Vector& Delta, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr, float* fraction_left_solid = nullptr);

	// computes the intersection of a ray with a oriented box ( OBB ).
	bool IntersectRayWithOBB(const Vector& start, const Vector& Delta, const matrix3x4_t& obb_to_world, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr);
	bool IntersectRayWithOBB(const Vector& start, const Vector& Delta, const Vector& box_origin, const QAngle& box_rotation, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr);

	// returns whether or not there was an intersection of a sphere against an infinitely extending ray. 
	// returns the two intersection points.
	bool IntersectInfiniteRayWithSphere(const Vector& start, const Vector& Delta, const Vector& sphere_center, float radius, float* out_t1, float* out_t2);
	void VectorYawRotate(const Vector& in, float flYaw, Vector& out);

	// returns whether or not there was an intersection, also returns the two intersection points ( clamped 0.f to 1.f. ).
	// note: the point of closest approach can be found at the average t value.
	bool IntersectRayWithSphere(const Vector& start, const Vector& Delta, const Vector& sphere_center, float radius, float* out_t1, float* out_t2);
	bool IntersectLineWithBB(Vector& start, Vector& end, Vector& min, Vector& max);

	Vector Interpolate(const Vector from, const Vector to, const float percent);
	Vector VectorRotate(Vector& in1, matrix3x4_t& in2);
	Vector VectorRotate(Vector& in1, QAngle& in2);

	bool WorldToScreen(const Vector& in, Vector2D& out);

	template <class T>
	__forceinline T LerpVec(Vector flPercent, T const& A, T const& B)
	{
		return A + (B - A) * flPercent;
	}

	template <class T>
	__forceinline T Lerp(float flPercent, T const& A, T const& B)
	{
		return A + (B - A) * flPercent;
	}

	template < typename t >
	__forceinline void clamp(t& n, const t& lower, const t& upper) {
		n = std::max(lower, std::min(n, upper));
	}

	template<class T, class U>
	__forceinline static T clampf(const T& in, const U& low, const U& high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}

	template< class T >
	__forceinline void Normalize(T& vecVector)
	{
		vecVector[0] = Math::clampf(vecVector[0], -89.0f, 89.0f);
		vecVector[1] = Math::clampf(Math::NormalizeYaw(vecVector[1]), -180.0f, 180.0f);
		vecVector[2] = 0.0f;
	}

	__forceinline float Bias(float x, float biasAmt) {
		static float lastAmt = -1;
		static float lastExponent = 0;
		if (lastAmt != biasAmt)
		{
			lastExponent = log(biasAmt) * -1.4427f;
		}
		return pow(x, lastExponent);
	}

	__forceinline float smoothstep_bounds(float edge0, float edge1, float x)
	{
		x = clampf((x - edge0) / (edge1 - edge0), 0.f, 1.f);
		return x * x * (3.f - 2.f * x);
	}

	__forceinline float clamp_cycle(float flCycleIn)
	{
		flCycleIn -= int(flCycleIn);

		if (flCycleIn < 0)
		{
			flCycleIn += 1;
		}
		else if (flCycleIn > 1)
		{
			flCycleIn -= 1;
		}

		return flCycleIn;
	}

	static float normalize_float(float angle)
	{
		auto revolutions = angle / 360.f;
		if (angle > 180.f || angle < -180.f)
		{
			revolutions = round(abs(revolutions));
			if (angle < 0.f)
				angle = (angle + 360.f * revolutions);
			else
				angle = (angle - 360.f * revolutions);
		}
		return angle;
	}

	__forceinline float SimpleSpline(float value) {
		float valueSquared = value * value;

		// Nice little ease-in, ease-out spline-like curve
		return (3 * valueSquared - 2 * valueSquared * value);
	}

	__forceinline float SimpleSplineRemapValClamped(float val, float A, float B, float C, float D) {
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = std::clamp(cVal, 0.0f, 1.0f);
		return C + (D - C) * SimpleSpline(cVal);
	}

	// Remap a value in the range [A,B] to [C,D].
	__forceinline float RemapVal(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return fsel(val - B, D, C);
		return C + (D - C) * (val - A) / (B - A);
	}

	__forceinline float RemapValClamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return fsel(val - B, D, C);
		float cVal = (val - A) / (B - A);
		cVal = clampf(cVal, 0.0f, 1.0f);

		return C + (D - C) * cVal;
	}
}