#include "Triangle.h"


Triangle::Triangle() : p1(Point(0.f)), p2(Point(0.f)), p3(Point(0.f))
{
}
Triangle::Triangle(Point p1_, Point p2_, Point p3_) : p1(p1_), p2(p2_), p3(p3_)
{
}
Triangle::Triangle(const Triangle & t) : p1(t.p1), p2(t.p2), p3(t.p3)
{
	
}

bool Triangle::intersect(const Ray &ray, float * tHit) const
{
	Vector e1 = p2 - p1;
	Vector e2 = p3 - p1;
	Vector h = cross(ray.d, e2);
	float divisor = dot(e1, h);
	if (std::abs(divisor) < 0.00001)
		return false;
	float invDivisor = 1.f / divisor;

	Vector s = ray.o - p1;

	auto u = invDivisor * dot(s, h);
	auto q = cross(s, e1);
	auto v = invDivisor * dot(ray.d, q);
	auto t = invDivisor * dot(e2, q);

	if (u < 0.f || u > 1.f)
		return false;

	if (v < 0.f || v + u > 1.f)
		return false;

	if (t < ray.mint || t > ray.maxt)
		return false;
	
	*tHit = t;
	return true;
}

BBox Triangle::getBound() const
{
	return unionBBox(BBox(p1, p2), p3);
}

Normals Triangle::getNormal(Point p) const
{
	return Normals(normalize(cross((p2 - p1), (p3 - p1))));
}

bool Triangle::isIn(Point p) const
{
	Vector v0 = p2 - p1;
	Vector v1 = p3 - p1;
	Vector v2 = p - p1;

	// Compute dot products
	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}
Triangle::~Triangle()
{
}
