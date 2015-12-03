#pragma once
#include "Vector.h"
class Point {
public:
	float x, y, z;
	Point() : x(0.f), y(0.f), z(0.f){};
	Point(float f) : x(f), y(f), z(f){};
	Point(float x_, float y_, float z_) : x(x_), y(y_), z(z_){};
	Point(const Point &p) : x(p.x), y(p.y), z(p.z){};

	inline Point operator+(const Vector &p) const { return Point(x + p.x, y + p.y, z + p.z); };
	inline Point operator+(const Point &p) const { return Point(x + p.x, y + p.y, z + p.z); }; // n'existe pas normalement mais sert a faire la derive

	inline bool operator!=(const Point &p) const { return x != p.x || y != p.y || z != p.z; };

	inline Point & operator+=(const Vector &p)
	{
		x += p.x; y += p.y; z += p.z;
		return *this;
	}

	inline Vector operator-(const Point &p) const { return Vector(x - p.x, y - p.y, z - p.z); };

	inline Point operator-(const Vector &p) const { return Point(x - p.x, y - p.y, z - p.z); };

	inline Point & operator-=(const Vector &p) 
	{ 
		x -= p.x; y -= p.y; z -= p.z;
		return *this;
	}

};
