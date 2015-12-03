#include "Mesh.h"


Mesh::Mesh() : faces(std::vector<Triangle>())
{
}

bool Mesh::intersect(const Ray &ray, float * tHit) const
{
	float newt = INFINITY;
	float tmpt = INFINITY;;
	for (int i = 0; i < faces.size(); ++i)
	{
		if (faces[i].intersect(ray, &tmpt))
			newt = std::fminf(newt, tmpt);
	}
	*tHit = newt;
	return newt < INFINITY;
}
BBox Mesh::getBound() const
{
	BBox res;
	for (int i = 0; i < faces.size(); ++i)
	{
		res = unionBBox(res, faces[i].getBound());
	}
	return res;
}
Normals Mesh::getNormal(Point p) const
{
	for (int i = 0; i < faces.size(); ++i)
	{
		if (faces[i].isIn(p))
			return faces[i].getNormal(p);
	}
	return Normals();

}



Mesh::~Mesh()
{
}
