#include "TerrainFractal.h"

TerrainFractal::TerrainFractal()
{

}

TerrainFractal::TerrainFractal(const TerrainFractal &terrain) : Terrain(terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	pointList = new Vector *[terrain.terrain_width];
	for (int i = 0; i < terrain.terrain_width; i++)
		pointList[i] = new Vector [terrain.terrain_height];

	for (int i = 0; i < terrain.terrain_width; ++i)
		for (int j = 0; j < terrain.terrain_height; ++j)
			pointList[i][j] = terrain.pointList[i][j];
}

TerrainFractal & TerrainFractal::operator=(const TerrainFractal & terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	precalc = new ColorRGB *[terrain.terrain_width];
	for (int i = 0; i < terrain.terrain_width; i++)
		precalc[i] = new ColorRGB[terrain.terrain_height];
	for (int i = 0; i < terrain.terrain_width; ++i)
		for (int j = 0; j < terrain.terrain_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];
	return *this;
}
// Renvoi un terrain généré aléatoirement
TerrainFractal::TerrainFractal(unsigned int terrain_width_, unsigned int terrain_height_) : Terrain(terrain_width_, terrain_height_)
{
	high = (low = Noise::noise(0., 0.));

	pointList = new Vector *[terrain_width_];
	for (int i = 0; i < terrain_width_; i++)
		pointList[i] = new Vector[terrain_height_];

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < terrain_width_; i++)
	{
		for (int j = 0; j < terrain_height_; j++)
		{
			float z = Noise::noise(i, j);
			pointList[i][j] = Vector(i, j, z);
			MaxMin(z);
		}
	}

	calcK();
}

Point TerrainFractal::getPoint(float x, float y) const 
{
	//return Point(x, y, 0.f);
	//return x > 0 && x < terrain_width && y > 0 && y < terrain_height ? Point ( x, y, 0.f ) : noIntersectPoint;

	int tmpI = (int)x;
	int tmpJ = (int)y;

	if (!(tmpI >= 0 && tmpI < terrain_width && tmpJ >= 0 && tmpJ < terrain_height))
		return noIntersectPoint;

	Vector & a(pointList[tmpI < terrain_width - 1 ? tmpI + 1 : tmpI][tmpJ]);
	Vector & b(pointList[tmpI][tmpJ < terrain_height - 1 ? tmpJ + 1 : tmpJ]);
	Vector & c(pointList[tmpI < terrain_width - 1 ? tmpI + 1 : tmpI][tmpJ < terrain_height - 1 ? tmpJ + 1 : tmpJ]);
	Vector & d(pointList[tmpI][tmpJ]);

	float x2 = x - (float)tmpI;
	float y2 = y - (float)tmpJ;
	float z = (1.f - x2) * (1.f - y2) * d.z
		+ x2 * (1.f - y2) * a.z
		+ (1.f - x2) * y2 * b.z
		+ x2 * y2 * c.z;

	return Point(x, y, z);
}

// Renvoi la normal du terrain au point p
Normals TerrainFractal::getNormal(Point p) const 
{
	float eps = .1f;
	return Normals( normalize(Vector( getPoint ( p.x - eps, p.y - eps ) + getPoint ( p.x + eps, p.y + eps ) - Point(0.f)) / ( 2 * eps )));
}

TerrainFractal::~TerrainFractal()
{
	for (int i = 0; i < terrain_width; i++)
		delete[] pointList[i];
	delete[] pointList;
}
//
//Vector TerrainFractal::getColor ( const Vector & p ) const { 
//	return Vector ( 1., 1., 1. ); 
//}
