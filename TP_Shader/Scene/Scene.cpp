#include "Scene.h"


Scene::Scene() : objects(std::vector<Shapes *>())
{
	const ColorRGB white = ColorRGB{ 0.75f, 0.75f, 0.75f } * 255.f;
	const ColorRGB blue = ColorRGB{ .25f, .25f, .75f } * 255.f;
	const ColorRGB red = ColorRGB{ .75f, .25f, .25f } * 255.f;
	const ColorRGB yellow = ColorRGB{ .9f, .1f, .9f } * 255.f;
	const ColorRGB purple = ColorRGB{ .9f, .9f, .1f } * 255.f;
	
	//// Left Wall
	//objects.push_back(new Triangle(Point(0.f, 0.f, 0.f), Point(0.f, 100.f, 0.f), Point(0.f, 0.f, 150.f));
	//objects.push_back(new Triangle(Point(0.f, 100.f, 150.f), Point(0.f, 100.f, 0.f), Point(0.f, 0.f, 150.f)), red));

	//// Right Wall
	//objects.push_back(new Shapes (new Triangle(Point(100.f, 0.f, 0.f), Point(100.f, 100.f, 0.f), Point(100.f, 0.f, 150.f)), blue));
	//objects.push_back(new Shapes (new Triangle(Point(100.f, 100.f, 150.f), Point(100.f, 100.f, 0.f), Point(100.f, 0.f, 150.f)), blue));
	//// Back wall
	//objects.push_back(new Shapes (new Triangle(Point(0.f, 0.f, 0.f), Point(100.f, .0f, 0.f), Point(100.f, 100.f, 0.f)), white));
	//objects.push_back(new Shapes (new Triangle(Point(0.f, 0.f, 0.f), Point(.0f, 100.f, 0.f), Point(100.f, 100.f, 0.f)), white));
	//
	//// Bottom Floor
	//objects.push_back(new Shapes (new Triangle(Point(0.f, 0.f, 0.f), Point(100.f, .0f, 0.f), Point(100.f, 0.f, 150.f)), white));
	//objects.push_back(new Shapes (new Triangle(Point(0.f, 0.f, 0.f), Point(.0f, 0.f, 150.f), Point(100.f, 0.f, 150.f)), white));

	//// Top Ceiling
	//objects.push_back(new Shapes (new Triangle(Point(0.f, 100.f, 0.f), Point(100.f, 100.f, 0.f), Point(0.f, 100.f, 150.f)), white));
	//objects.push_back(new Shapes (new Triangle(Point(100.f, 100.f, 150.f), Point(100.f, 100.f, 0.f), Point(0.f, 100.f, 150.f)), white));
	//
	// Sphere
	//objects.push_back(new Shapes (new Sphere(16.5, Point(27, 16.5, 47)), red));
	//objects.push_back(new Shapes(new Sphere(16.5, Point(73, 16.5, 78)), white));
//	objects.push_back(new Shapes(new Cube(Point(72, 15.5, 77), Point(73, 16.5, 78)), new Diffus(white)));	
	Terrain * t = new TerrainFractal(1000, 1000);
	objects.push_back(t);
	/*
	Mesh * m = new Mesh();
	*m = Mesh::readFromObj("test.obj", Vector(50, 16.5, 50));
	objects.push_back(new Shapes (m, new Diffus(white)));
	*/
	lights.push_back(Light{ Point(500.f, 500.f, 500.6f), 1 });
	lights.push_back(Light{ Point(-100.f, 1100.f, 500.6f), 1 });
	lights.push_back(Light{ Point(0.f, 0.f, 500.6f), 1 });

	lights.push_back(Light{ Point(250.f, 250.f, 500.6f), 1 });
	lights.push_back(Light{ Point(1100.f, 1100.f, 500.6f), 1 });
	//placeLightHalfCircle(Point(500.f, 500.f, 0.f), 500.F, 1, Point(0.f, 500.f, 0.f), 4);

}

// Dessine un cercle d'origine o, de couleur color ett de rayon r, avec side cotes.
void Scene::placeLightHalfCircle(const Point& o, const float rayon, const unsigned int nbLight, const Point& sunshinePos, const int sunshinePower)
{
	const float dsize = M_PI / (float)nbLight;
	const float distMax = rayon + rayon;

	for (unsigned int i = 0; i <= nbLight; ++i)
	{
		const float angle = (float)i * dsize;
		const Point point(o.x + rayon * sin(angle), o.y, o.z + rayon * cos(angle));
		const int power = std::max(1, (int)((distance(sunshinePos, point) / distMax) * sunshinePower));
		lights.push_back(Light{ point , power });
	}
}

Shapes * Scene::intersect(const Ray &r, float & t)
{
	Shapes * ret = nullptr;
	t = INFINITY;
	float d;
	for (Shapes * &object : objects)
	{
		if (object->getBound().intersect(r, nullptr, nullptr))
		{
			bool isIntersect = object->intersect(r, &d);
			if (isIntersect && d < t)
			{
				t = d;
				ret = object;
			}
		}
	}

	return ret;
}

Shapes * Scene::intersectSegment(const Ray &ray, float & t, float tMax)
{
	Shapes * ret = nullptr;
	t = INFINITY;
	float d;
	for (Shapes * &object : objects)
	{
		if (object->getBound().intersect(ray, nullptr, nullptr))
		{
			bool isIntersect = object->intersectSegment(ray, &d, tMax);
			if (isIntersect && d < t)
			{
				t = d;
				ret = object;
			}
		}
	}

	return ret;
}

BBox Scene::getBound() const
{
	if (objects.size() <= 0) return BBox();
	BBox res = objects[0]->getBound();
	for (unsigned int i = 1; i < objects.size(); ++i)
	{
		unionBBox(res, objects[i]->getBound());
	}
	return res;
}

Scene::~Scene()
{
//	for (unsigned int i = 0; i < objects.size(); ++i)
//		delete objects[i];
}
