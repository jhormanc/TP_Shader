#include "Ray.h"


Ray::Ray() : mint(0.f), maxt(INFINITY), depth(0)
{
}

Ray::Ray(const Point& o_, const Vector& d_, float startt, float endt, int d) : o(o_), d(d_), mint(startt), maxt(endt), depth(d)
{

}

Ray::Ray(const Point& o_, const Vector& d_, const Ray & parent, float startt, float endt) : o(o_), d(d_), mint(startt), maxt(endt), depth(parent.depth)
{

}

Ray::Ray(const Ray & r) : o(r.o), d(r.d), mint(r.mint), maxt(r.maxt), depth(r.depth)
{

}

Point Ray::operator()(float t) const
{
	return o + d * t;
}

Ray::~Ray()
{
}
