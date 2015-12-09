#pragma once
#include "..\Geometry\Ray.h"
#include "..\Geometry\BBox.h"
#include "..\Global\Global.h"
#include "..\Film.h"

#include "..\Global\Constants.h"

class Shapes
{
	ColorRGB c;

public:
	Shapes();
	Shapes(ColorRGB c_);
	virtual bool canIntersect();
	virtual bool intersect(const Ray &ray, float * tHit) const = 0;
	virtual bool intersectSegment(const Ray &ray, float * tHit, float tMax) const;
	virtual BBox getBound() const = 0;
	virtual Normals getNormal(Point p) const = 0;
	virtual ColorRGB getColor(const Point & p);
	~Shapes();
};