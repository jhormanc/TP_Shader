#pragma once
#include <list>
#include "..\Shapes\Shapes.h"
#include "..\Shapes\Triangle.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\Cube.h"
#include "..\Shapes\Mesh.h"
#include "..\Global\Global.h"
#include "..\Geometry\Ray.h"
#include "..\Terrain\TerrainFractal.h"

struct Light
{
	Point o;
	int influence;
};
class Scene
{
	std::vector<Shapes *> objects;
public:
	std::list<Light> lights;
	Scene();
	Shapes * intersect(const Ray &r, float & t);
	BBox getBound() const;
	void placeLightHalfCircle(const Point& o, const float rayon, const unsigned int nbLight, const Point& sunshinePos, const int sunshinePower);
	~Scene();
};

