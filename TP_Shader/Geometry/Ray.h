#pragma once
#include "Vector.h"
#include "Point.h"
#include "..\Global\Constants.h"
class Ray
{

public:
	Point o;
	Vector d;
	mutable float mint, maxt;
	int depth;
	Ray();
	Ray(const Point& o_, const Vector& d_, float startt = 0.f, float endt = INFINITY, int d = 0);
	Ray(const Point& o_, const Vector& d_, const Ray & parent, float startt = 0.f, float endt = INFINITY);
	Ray(const Ray & r);
	
	Point operator()(float t) const;
	~Ray();
};

