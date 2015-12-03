#pragma once
#include "..\Shapes\Shapes.h"
#include "..\Shapes\Triangle.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\Cube.h"
#include "..\Shapes\Mesh.h"
#include "..\Global\Global.h"
#include "..\Geometry\Ray.h"
#include "..\Terrain\TerrainFractal.h"

class Scene
{
	std::vector<Shapes *> objects;
public:
	Point light;
	Scene();
	Shapes * intersect(const Ray &r, float & t);
	BBox getBound() const;
	~Scene();
};

