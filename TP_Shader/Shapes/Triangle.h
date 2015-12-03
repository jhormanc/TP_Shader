#pragma once
#include "Shapes.h"
#include "..\Global\Global.h"
#include "..\Geometry\Geometry.h"

class Triangle : public Shapes
{
	
public:
	Point p1, p2, p3;
	Triangle();
	Triangle(Point p1_, Point p2_, Point p3_);
	Triangle(const Triangle & t);
	bool intersect(const Ray &ray, float * tHit) const;
	BBox getBound() const;
	Normals getNormal(Point p) const;
	bool isIn(Point p) const;
	~Triangle();
};

