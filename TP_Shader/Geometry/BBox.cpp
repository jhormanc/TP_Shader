#include "BBox.h"


BBox::BBox() : pMin(Point(INFINITY)), pMax(Point(-INFINITY))
{
}

BBox::BBox(const Point & p) : pMin(p), pMax(p)
{
}

BBox::BBox(const Point & p1, const Point & p2) : pMin(Point(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z))),
pMax(Point(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)))
{
}
BBox::BBox(const BBox & b) : pMin(b.pMin), pMax(b.pMax)
{
}

bool BBox::ovelap(const BBox &b) const
{
	return (pMax.x >= b.pMin.x) && (pMin.x <= b.pMax.x) &&
		(pMax.y >= b.pMin.y) && (pMin.y <= b.pMax.y) &&
		(pMax.z >= b.pMin.z) && (pMin.z <= b.pMax.z);
}

bool BBox::inside(const Point &p) const
{
	return (p.x >= pMin.x) && (p.x <= pMax.x) &&
		(p.y >= pMin.y) && (p.y <= pMax.y) &&
		(p.z >= pMin.z) && (p.z <= pMax.z);
}
//
//bool BBox::intersect(const Ray &ray, float * hitt0, float * hitt1) const
//{
//	float t0 = -INFINITY;
//	float t1 = INFINITY;
//
//	float invRayDir = 1.f / ray.d.x;
//	float tNear = (pMin.x - ray.o.x) * invRayDir;
//	float tFar = (pMax.x - ray.o.x) * invRayDir;
//	if (tNear > tFar) std::swap(tNear, tFar);
//	t0 = tNear > t0 ? tNear : t0;
//	t1 = tFar < t1 ? tFar : t1 ;
//	if (t0 > t1) return false;
//
//	 t0 = -INFINITY;
//	t1 = INFINITY;
//	invRayDir = 1.f / ray.d.y;
//	tNear = (pMin.y - ray.o.y) * invRayDir;
//	tFar = (pMax.y - ray.o.y) * invRayDir;
//	if (tNear > tFar) std::swap(tNear, tFar);
//	t0 = tNear > t0 ? tNear : t0;
//	t1 = tFar < t1 ? tFar : t1;
//	if (t0 > t1) return false;
//
//	t0 = -INFINITY;
//	t1 = INFINITY;
//	invRayDir = 1.f / ray.d.z;
//	tNear = (pMin.z - ray.o.z) * invRayDir;
//	tFar = (pMax.z - ray.o.z) * invRayDir;
//	if (tNear > tFar) std::swap(tNear, tFar);
//	t0 = tNear > t0 ? tNear : t0;
//	t1 = tFar < t1 ? tFar : t1;
//	if (t0 > t1) return false;
//
//	if (hitt0) *hitt0 = t0;
//	if (hitt1) *hitt1 = t1;
//	return true;
//} 
bool BBox::intersect(const Ray &ray, float * hitt0, float * hitt1) const
{
	float tmin, tmax, tymin, tymax, tzmin, 	tzmax;
	if(ray.d.x >= 0.f)
	{
		tmin = (pMin.x - ray.o.x) / ray.d.x;
		tmax = 	(pMax.x - ray.o.x) / ray.d.x;
	}
	else
	{
		tmin = (pMax.x - ray.o.x) / ray.d.x;
		tmax = (pMin.x - ray.o.x) / ray.d.x;
	}
	if (ray.d.y >=	0.f)
	{
		tymin=(pMin.y -	ray.o.y)/ ray.d.y;
		tymax = (pMax.y - ray.o.y) / ray.d.y;
	}
	else
	{
		tymin = (pMax.y - ray.o.y) / ray.d.y;
		tymax = (pMin.y - ray.o.y) / ray.d.y;
	}
	if((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if(tymax<tmax)
		tmax=tymax;
	if(ray.d.z >= 0.f)
	{
		tzmin = (pMin.z - ray.o.z) / ray.d.z;
		tzmax	=(pMax.z - ray.o.z)	/ ray.d.z;
	}
	else
	{
		tzmin = (pMax.z - ray.o.z) / ray.d.z;
		tzmax = (pMin.z - ray.o.z) / ray.d.z;
	}
	if((tmin>	tzmax)||(tzmin>tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	*hitt0 = tmin;
	*hitt1 = tmax;
	return true;
}

BBox::~BBox()
{
}
