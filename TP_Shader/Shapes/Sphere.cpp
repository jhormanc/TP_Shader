#include "Sphere.h"


Sphere::Sphere() : radius(0.f), origin(Point(0.f))
{
}

Sphere::Sphere(float radius_, Point origin_) : radius(radius_), origin(origin_)
{

}
Sphere::Sphere(const Sphere & s) : radius(s.radius), origin(s.origin)
{

}
//#include <iostream>
bool Sphere::intersect(const Ray &ray, float * tHit) const
{
	/*
	float A = ray.d.x*ray.d.x + ray.d.y*ray.d.y + ray.d.z*ray.d.z;
	float B = 2 * (ray.d.x*ray.o.x + ray.d.y*ray.o.y + ray.d.z*ray.o.z);
	float C = ray.o.x*ray.o.x + ray.o.y*ray.o.y + ray.o.z*ray.o.z - radius*radius;
	float t0, t1;
	if (!quadratic(A, B, C, &t0, &t1))
		return false;
	if (t0 > ray.maxt || t1 < ray.mint)
		return false;
	*tHit = t0;
	if (t0 < ray.mint)
	{
		*tHit = t1;
		if (*tHit > ray.maxt) return false;
	}
	return true; */
				// returns distance, 0 if nohit
		Vector op = origin - ray.o;		// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
		float b = dot(ray.d, op);
		float det = b * b - dot(op, op) + radius * radius;

		if (det < 0)
			return false;
		else
			det = sqrtf(det);
		/*
		if (!(t = b - det) >= 0)
		{
			if (!(t = b + det) >= 0)
				return false;
		}
		if (t > ray.maxt || t < ray.mint)
			return false;
		*tHit = t;
		return true; */
		if (b - det >= 0.f)
		{
			*tHit = b - det;
			return true;
		}
		else
		{
			if (b + det >= 0.f)
			{
				*tHit = b + det;
				return true;
			}
			else
				return false;
		}
}

BBox Sphere::getBound() const
{
	return BBox(Point(-radius + origin.x, -radius + origin.y, -radius + origin.z), Point(radius + origin.x, radius + origin.y, radius + origin.z));
}

Normals Sphere::getNormal(Point p) const
{
	return Normals(normalize(p - origin));
}

Sphere::~Sphere()
{
}
