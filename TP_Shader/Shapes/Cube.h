#pragma once
#include "Shapes.h"
#include "..\Geometry\BBox.h"
#include "..\Geometry\Normals.h"
#include "..\Geometry\Geometry.h"
class Cube : public BBox, public Shapes 
{
public:
	Cube(const Point & p1, const Point & p2);
	bool intersect(const Ray &ray, float * tHit) const;
	BBox getBound() const;
	Normals getNormal(Point p) const; 
	~Cube();
};

