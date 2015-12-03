#include "Cube.h"


Cube::Cube(const Point & p1, const Point & p2) : BBox(p1, p2)
{

}

bool Cube::intersect(const Ray &ray, float * tHit) const
{
	float t0 = INFINITY, t1 = INFINITY;
	bool res = BBox::intersect(ray, &t0, &t1);
	*tHit = std::fminf(std::fabsf(t0), t1);
	return res;
}

BBox Cube::getBound() const
{
	return *this;
} 

Normals Cube::getNormal(Point p) const
{
	Point o((pMin.x + pMax.x) * 0.5,
		(pMin.y + pMax.y) * 0.5,
		(pMin.z + pMax.z) * 0.5);
	return Normals(normalize(p - o));
}

Cube::~Cube()
{
}
