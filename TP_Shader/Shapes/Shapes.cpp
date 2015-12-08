#include "Shapes.h"

Shapes::Shapes() : c(white)
{

}

Shapes::Shapes(ColorRGB c_) : c(c_)
{

}

ColorRGB Shapes::getColor(const Point& p)
{
	return c;
}

bool Shapes::intersectSegment(const Ray &ray, float * tHit, float tMax) const
{
	bool res = intersect(ray, tHit);
	if (*tHit < tMax)
		return res;
	*tHit = noIntersect;
	return false;
}
bool Shapes::canIntersect()
{
	return true;
}

Shapes::~Shapes()
{
}
