#pragma once
#include "Vector.h"
class Normals
{
public:
	float x, y, z;
	Normals() : x(0.f), y(0.f), z(0.f){};
	Normals(float f) : x(f), y(f), z(f){};
	Normals(float x_, float y_, float z_) : x(x_), y(y_), z(z_){};
	Normals(const Normals &n) : x(n.x), y(n.y), z(n.z){};
	explicit Normals(const Vector& v) : x(v.x), y(v.y), z(v.z){};
	explicit Normals(const Point& v) : x(v.x), y(v.y), z(v.z){};

	inline Normals operator+(const Normals &n) const { return Normals(x + n.x, y + n.y, z + n.z); };

	inline Normals & operator+=(const Normals &n)
	{
		x += n.x; y += n.y; z += n.z;
		return *this;
	}

	inline Normals operator-(const Normals &n) const { return Normals(x - n.x, y - n.y, z - n.z); };

	inline Normals operator-() const { return Normals(-x, -y, -z); };

	inline Normals & operator-=(const Normals &n)
	{
		x -= n.x; y -= n.y; z -= n.z;
		return *this;
	}

	inline Normals operator*(float f) const { return Normals(x * f, y * f, z * f); };

	inline Normals & operator*=(float f)
	{
		x *= f; y *= f; z *= f;
		return *this;
	}

	inline Normals operator/(float f) const 
	{ 
		const float inv = 1.f / f;
		return Normals(x * inv, y * inv, z * inv); 
	}

	inline Normals & operator/=(float f)
	{
		const float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
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
};