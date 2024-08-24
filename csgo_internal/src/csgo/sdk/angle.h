#pragma once

class QAngle {
public:
	// data member variables.
	float x, y, z;

public:
	// constructors.
	__forceinline QAngle( ) : x{}, y{}, z{} {}
	__forceinline QAngle( float x, float y, float z ) : x{ x }, y{ y }, z{ z } {}

	// at-accesors.
	__forceinline float& at( const size_t index ) {
		return ( ( float* )this )[ index ];
	}

	__forceinline float& at( const size_t index ) const {
		return ( ( float* )this )[ index ];
	}

	// index operators.
	__forceinline float& operator( )( const size_t index ) {
		return at( index );
	}

	__forceinline const float& operator( )( const size_t index ) const {
		return at( index );
	}

	__forceinline float& operator[ ]( const size_t index ) {
		return at( index );
	}

	__forceinline const float& operator[ ]( const size_t index ) const {
		return at( index );
	}

	// equality operators.
	__forceinline bool operator==( const QAngle& v ) const {
		return v.x == x && v.y == y && v.z == z;
	}

	__forceinline bool operator!=( const QAngle& v ) const {
		return v.x != x || v.y != y || v.z != z;
	}

	__forceinline bool operator !( ) const {
		return !x && !y && !z;
	}

	// copy assignment.
	__forceinline QAngle& operator=( const QAngle& v ) {
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	// negation-operator.
	__forceinline QAngle operator-( ) const {
		return QAngle( -x, -y, -z );
	}

	// arithmetic operators.
	__forceinline QAngle operator+( const QAngle& v ) const {
		return {
			x + v.x,
			y + v.y,
			z + v.z
		};
	}

	__forceinline QAngle operator-( const QAngle& v ) const {
		return {
			x - v.x,
			y - v.y,
			z - v.z
		};
	}

	__forceinline QAngle operator*( const QAngle& v ) const {
		return {
			x * v.x,
			y * v.y,
			z * v.z
		};
	}

	__forceinline QAngle operator/( const QAngle& v ) const {
		return {
			x / v.x,
			y / v.y,
			z / v.z
		};
	}

	// compound assignment operators.
	__forceinline QAngle& operator+=( const QAngle& v ) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__forceinline QAngle& operator-=( const QAngle& v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	__forceinline QAngle& operator*=( const QAngle& v ) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	__forceinline QAngle& operator/=( const QAngle& v ) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// arithmetic operators w/ float.
	__forceinline QAngle operator+( float f ) const {
		return {
			x + f,
			y + f,
			z + f
		};
	}

	__forceinline QAngle operator-( float f ) const {
		return {
			x - f,
			y - f,
			z - f
		};
	}

	__forceinline QAngle operator*( float f ) const {
		return {
			x * f,
			y * f,
			z * f
		};
	}

	__forceinline QAngle operator/( float f ) const {
		return {
			x / f,
			y / f,
			z / f
		};
	}

	// compound assignment operators w/ float.
	__forceinline QAngle& operator+=( float f ) {
		x += f;
		y += f;
		z += f;
		return *this;
	}

	__forceinline QAngle& operator-=( float f ) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	__forceinline QAngle& operator*=( float f ) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	__forceinline QAngle& operator/=( float f ) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	// methods.
	__forceinline void clear( ) {
		x = y = z = 0.f;
	}

	__forceinline float LengthSqr() const {
		return ((x * x) + (y * y) + (z * z));
	}

	__forceinline float Length2DSqr() const {
		return ((x * x) + (y * y));
	}

	__forceinline float Length() const {
		return std::sqrt(LengthSqr());
	}

	__forceinline float Length2D() const {
		return std::sqrt(Length2DSqr());
	}

	__forceinline float Delta(const QAngle& vOther) const {
		QAngle Delta;

		Delta.x = x - vOther.x;
		Delta.y = y - vOther.y;
		Delta.z = z - vOther.z;

		return Delta.Length2D();
	}

	__forceinline void Normalize( ) {
		Math::NormalizeAngle( x );
		Math::NormalizeAngle( y );
		Math::NormalizeAngle( z );
	}

	__forceinline QAngle Normalized( ) const {
		auto vec = *this;
		vec.Normalize( );
		return vec;
	}

	__forceinline void clamp( ) {
		Math::clamp( x, -89.f, 89.f );
		Math::clamp( y, -180.f, 180.f );
		Math::clamp( z, -90.f, 90.f );
	}

	__forceinline void SanitizeAngle( ) {
		Math::NormalizeAngle( y );
		clamp( );
	}

	__forceinline bool IsZero() const {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}
};