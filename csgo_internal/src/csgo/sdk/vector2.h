#pragma once

// 2d vector for screen coordinates, maybe make it use integers since we cant have half pixels
class Vector2D {
public:
	float x, y;

public:
	// ctors
	__forceinline Vector2D( void ) {}
	__forceinline Vector2D( float x, float y ) : x{ x }, y{ y } {}
	__forceinline Vector2D( int x, int y ) : x{ ( float )x }, y{ float( y ) } {}

	// at-accesors.
	__forceinline float& at( const size_t index ) {
		return ( ( float* )this )[ index ];
	}
	__forceinline float& at( const size_t index ) const {
		return ( ( float* )this )[ index ];
	}

	// index operators.
	__forceinline float& operator()( const size_t index ) {
		return at( index );
	}
	__forceinline const float& operator()( const size_t index ) const {
		return at( index );
	}
	__forceinline float& operator[]( const size_t index ) {
		return at( index );
	}
	__forceinline const float& operator[]( const size_t index ) const {
		return at( index );
	}

	// equality operators.
	__forceinline bool operator==( const Vector2D& v ) const {
		return v.x == x && v.y == y;
	}
	__forceinline bool operator!=( const Vector2D& v ) const {
		return v.x != x || v.y != y;
	}

	// copy assignment.
	__forceinline Vector2D& operator=( const Vector2D& v ) {
		x = v.x;
		y = v.y;
		return *this;
	}

	// negation-operator.
	__forceinline Vector2D operator-( ) const {
		return Vector2D( -x, -y );
	}

	// arithmetic operators.
	__forceinline Vector2D operator+( const Vector2D& v ) const {
		return {
			x + v.x,
			y + v.y
		};
	}
	__forceinline Vector2D operator-( const Vector2D& v ) const {
		return {
			x - v.x,
			y - v.y
		};
	}
	__forceinline Vector2D operator*( const Vector2D& v ) const {
		return {
			x * v.x,
			y * v.y
		};
	}
	__forceinline Vector2D operator/( const Vector2D& v ) const {
		return {
			x / v.x,
			y / v.y
		};
	}

	// compound assignment operators.
	__forceinline Vector2D& operator+=( const Vector2D& v ) {
		x += v.x;
		y += v.y;
		return *this;
	}
	__forceinline Vector2D& operator-=( const Vector2D& v ) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	__forceinline Vector2D& operator*=( const Vector2D& v ) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	__forceinline Vector2D& operator/=( const Vector2D& v ) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	// arithmetic operators w/ float
	__forceinline Vector2D operator+( float f ) const {
		return {
			x + f,
			y + f
		};
	}
	__forceinline Vector2D operator-( float f ) const {
		return {
			x - f,
			y - f
		};
	}
	__forceinline Vector2D operator*( float f ) const {
		return {
			x * f,
			y * f
		};
	}
	__forceinline Vector2D operator/( float f ) const {
		return {
			x / f,
			y / f
		};
	}

	// compound assignment operators w/ float
	__forceinline Vector2D& operator+=( float f ) {
		x += f;
		y += f;
		return *this;
	}
	__forceinline Vector2D& operator-=( float f ) {
		x -= f;
		y -= f;
		return *this;
	}
	__forceinline Vector2D& operator*=( float f ) {
		x *= f;
		y *= f;
		return *this;
	}
	__forceinline Vector2D& operator/=( float f ) {
		x /= f;
		y /= f;
		return *this;
	}

	// methods.
	__forceinline float LengthSqr( ) const {
		return ( x * x + y * y );
	}

	__forceinline float Length( ) const {
		return std::sqrt( LengthSqr( ) );
	}

	__forceinline void clear( ) {
		x = y = 0.f;
	}

	__forceinline bool valid() const {
		return x > 1 && y > 1;
	}

	__forceinline bool zero() const {
		return x == 0.0f && y == 0.0f;
	}
};