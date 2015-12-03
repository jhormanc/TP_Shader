#pragma once
#include "Point.h"
#include "Ray.h"
#include <algorithm>
class BBox
{
	
public:
	Point pMin, pMax;
	BBox();
	BBox(const Point & p);
	BBox(const Point & p1, const Point & p2);
	BBox(const BBox & b);
	bool ovelap(const BBox &b) const;
	bool inside(const Point &p) const;
	bool intersect(const Ray &ray, float * hitt0, float * hitt1) const;
	~BBox();
};

