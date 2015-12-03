#pragma once
#include <math.h>
class Normals;
class Vector
{
public:
	float x, y, z;
	Vector() : x(0.f), y(0.f), z(0.f){};
	Vector(float f) : x(f), y(f), z(f){};
	Vector(float x_, float y_, float z_) : x(x_), y(y_), z(z_){};
	Vector(const Vector &v) : x(v.x), y(v.y), z(v.z){};
	explicit Vector(const Normals &n);

	inline Vector operator+(const Vector &v) const { return Vector(x + v.x, y + v.y, z + v.z); };

	inline Vector & operator+=(const Vector &v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	inline Vector operator-(const Vector &v) const { return Vector(x - v.x, y - v.y, z - v.z); };

	inline Vector operator-() const { return Vector(-x, -y, -z); };
	inline Vector operator-(float f) const { return Vector(x - f, y - f, z - f); };

	inline Vector & operator-=(const Vector &v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	inline Vector operator*(float f) const { return Vector(x * f, y * f, z * f); };

	inline Vector & operator*=(float f)
	{
		x *= f; y *= f; z *= f;
		return *this;
	}

	inline Vector operator/(float f) const 
	{ 
		const float inv = 1.f / f;
		return Vector(x * inv, y * inv, z * inv); 
	}

	inline Vector & operator/=(float f)
	{
		const float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}

	inline bool & operator!=( const Vector &v ) {
		bool res = ( x != v.x || y != v.y || z != v.z );
		return res;
	}

	inline float operator[](unsigned int i) const
	{
		return(&x)[i];
	}

	inline float & operator[](unsigned int i)
	{
		return(&x)[i];
	}

	inline float lengthSquared() const 
	{ 
		return x*x + y*y + z*z; 
	};
	inline float length() const 
	{ 
		return sqrtf(lengthSquared()); 
	}

	~Vector(){};
};
