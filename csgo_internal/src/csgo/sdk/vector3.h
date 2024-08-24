#pragma once

class Vector {
public:
	// data member variables
	float x, y, z;

public:
	__forceinline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

	// ctors.
	__forceinline Vector() : x{}, y{}, z{} {}
	__forceinline Vector(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}

	// at-accesors.
	__forceinline float& at(const size_t index) {
		return ((float*)this)[index];
	}
	__forceinline float& at(const size_t index) const {
		return ((float*)this)[index];
	}

	// index operators.
	__forceinline float& operator( )(const size_t index) {
		return at(index);
	}
	__forceinline const float& operator( )(const size_t index) const {
		return at(index);
	}
	__forceinline float& operator[ ](const size_t index) {
		return at(index);
	}
	__forceinline const float& operator[ ](const size_t index) const {
		return at(index);
	}

	// equality operators.
	__forceinline bool operator==(const Vector& v) const {
		return v.x == x && v.y == y && v.z == z;
	}
	__forceinline bool operator!=(const Vector& v) const {
		return v.x != x || v.y != y || v.z != z;
	}

	// copy assignment.
	__forceinline Vector& operator=(const Vector& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// negation-operator.
	__forceinline Vector operator-() const {
		return Vector{ -x, -y, -z };
	}

	// arithmetic operators.
	__forceinline Vector operator+(const Vector& v) const {
		return {
			x + v.x,
			y + v.y,
			z + v.z
		};
	}

	__forceinline Vector operator-(const Vector& v) const {
		return {
			x - v.x,
			y - v.y,
			z - v.z
		};
	}

	__forceinline Vector operator*(const Vector& v) const {
		return {
			x * v.x,
			y * v.y,
			z * v.z
		};
	}

	__forceinline Vector operator/(const Vector& v) const {
		return {
			x / v.x,
			y / v.y,
			z / v.z
		};
	}

	// compound assignment operators.
	__forceinline Vector& operator+=(const Vector& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__forceinline Vector& operator-=(const Vector& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	__forceinline Vector& operator*=(const Vector& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	__forceinline Vector& operator/=(const Vector& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// arithmetic operators w/ float.
	__forceinline Vector operator+(float f) const {
		return {
			x + f,
			y + f,
			z + f
		};
	}

	__forceinline Vector operator-(float f) const {
		return {
			x - f,
			y - f,
			z - f
		};
	}

	__forceinline Vector operator*(float f) const {
		return {
			x * f,
			y * f,
			z * f
		};
	}

	__forceinline Vector operator/(float f) const {
		return {
			x / f,
			y / f,
			z / f
		};
	}

	// compound assignment operators w/ float.
	__forceinline Vector& operator+=(float f) {
		x += f;
		y += f;
		z += f;
		return *this;
	}

	__forceinline Vector& operator-=(float f) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	__forceinline Vector& operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	__forceinline Vector& operator/=(float f) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	// methods.
	__forceinline void clear() {
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

	__forceinline float Dot(const Vector& v) const {
		return (x * v.x + y * v.y + z * v.z);
	}

	__forceinline float Dot(float* v) const {
		return (x * v[0] + y * v[1] + z * v[2]);
	}

	__forceinline Vector Cross(const Vector& v) const {
		return {
			(y * v.z) - (z * v.y),
			(z * v.x) - (x * v.z),
			(x * v.y) - (y * v.x)
		};
	}

	__forceinline float Delta(const Vector& vOther) const {
		Vector Delta;

		Delta.x = x - vOther.x;
		Delta.y = y - vOther.y;
		Delta.z = z - vOther.z;

		return Delta.Length2D();
	}

	__forceinline float Normalize() {
		float len = Length();

		(*this) /= (Length() + std::numeric_limits< float >::epsilon());

		return len;
	}

	__forceinline Vector Normalized() const {
		auto vec = *this;

		vec.Normalize();

		return vec;
	}

	void NormalizeInPlace()
	{
		*this = Normalized();
	}

	__forceinline bool IsZero() const {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	__forceinline bool IsValid() const {
		return !IsZero();
	}
};

__forceinline Vector operator*(float f, const Vector& v) {
	return v * f;
}

class __declspec(align(16)) vec_aligned_t : public Vector {
public:
	__forceinline vec_aligned_t() {}

	__forceinline vec_aligned_t(const Vector& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = 0.f;
	}

	float w;
};

//FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
//{
//	c.x = a.x - b.x;
//	c.y = a.y - b.y;
//	c.z = a.z - b.z;
//}