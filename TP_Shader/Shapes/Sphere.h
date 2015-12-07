#pragma once
#include "Shapes.h"
#include "..\Global\Global.h"
#include "..\Geometry\Geometry.h"

class Sphere : public Shapes
{
	
public:
	Point origin;
	float radius;
	Sphere();
	Sphere(float radius_, Point origin_);
	Sphere(const Sphere & s);

	bool intersect(const Ray &ray, float * tHit) const;
	BBox getBound() const;
	Normals getNormal(Point p) const;
	bool overlap(const Sphere& s) const;
	~Sphere();
};

