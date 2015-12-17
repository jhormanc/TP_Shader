#include "TerrainFractal.h"

TerrainFractal::TerrainFractal()
{

}

TerrainFractal::TerrainFractal(const TerrainFractal &terrain) : Terrain(terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	pointList = new Pixel *[terrain.points_width];
	points_width = terrain.points_width;
	points_height = terrain.points_height;
	steps = terrain.steps;

	for (int i = 0; i < terrain.points_width; i++)
		pointList[i] = new Pixel[terrain.points_height];

	for (int i = 0; i < terrain.points_width; ++i)
		for (int j = 0; j < terrain.points_height; ++j)
			pointList[i][j] = terrain.pointList[i][j];
}

TerrainFractal & TerrainFractal::operator=(const TerrainFractal & terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	points_width = terrain.points_width;
	points_height = terrain.points_height;
	steps = terrain.steps;

	precalc = new ColorRGB *[terrain.points_width];
	for (int i = 0; i < terrain.points_width; i++)
		precalc[i] = new ColorRGB[terrain.points_height];
	for (int i = 0; i < terrain.points_width; ++i)
		for (int j = 0; j < terrain.points_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];
	return *this;
}
// Renvoi un terrain généré aléatoirement
TerrainFractal::TerrainFractal(const int& terrain_width_, const int& terrain_height_, const int& _steps) : Terrain(terrain_width_, terrain_height_, _steps)
{
	high = (low = Noise::noise(0., 0.));

	pointList = new Pixel *[points_width];
	for (int i = 0; i < points_width; i++)
		pointList[i] = new Pixel[points_height];

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < points_width; i++)
	{
		for (int j = 0; j < points_height; j++)
		{
			float z = Noise::noise(i * steps, j * steps);
			Point p(i * steps, j * steps, z);
			pointList[i][j] = Pixel(p);
			MaxMin(z);
		}
	}

	calcK();

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < points_width; i++)
	{
		for (int j = 0; j < points_height; j++)
		{
			ColorRGB c(initColor(pointList[i][j]));
			pointList[i][j].color = c;
		}
	}
}

/*!
\brief
*/

float TerrainFractal::getZ(float x, float y) const
{
	//return Point(x, y, 0.f);
	//return x > 0 && x < terrain_width && y > 0 && y < terrain_height ? Point ( x, y, 0.f ) : noIntersectPoint;

	int tmpI = (int)(x / steps);
	int tmpJ = (int)(y / steps);

	if (!(tmpI >= 0 && tmpI < points_width && tmpJ >= 0 && tmpJ < points_height))
		return 0.0;

	Pixel & a(pointList[tmpI < points_width - 1 ? tmpI + 1 : tmpI][tmpJ]);
	Pixel & b(pointList[tmpI][tmpJ < points_height - 1 ? tmpJ + 1 : tmpJ]);
	Pixel & c(pointList[tmpI < points_width - 1 ? tmpI + 1 : tmpI][tmpJ < points_height - 1 ? tmpJ + 1 : tmpJ]);
	Pixel & d(pointList[tmpI][tmpJ]);

	float x2 = (x - d.x) / steps;
	float y2 = (y - d.y) / steps;
	float z = (1.f - x2) * (1.f - y2) * d.z
		+ x2 * (1.f - y2) * a.z
		+ (1.f - x2) * y2 * b.z
		+ x2 * y2 * c.z;

	return z;
}

/*!
\brief
*/
Pixel TerrainFractal::getPoint(float x, float y) const 
{
	int tmpI = (int)(x / steps);
	int tmpJ = (int)(y / steps);

	if (!(tmpI >= 0 && tmpI < points_width && tmpJ >= 0 && tmpJ < points_height))
		return noIntersectPoint;

	return Pixel(Point(x, y, getZ(x, y)), pointList[tmpI][tmpJ].color);
}

// Renvoi la normal du terrain au point p
Normals TerrainFractal::getNormal(Pixel p) const
{
	float eps = .1f * steps;
	return Normals(normalize(Vector(-(getPoint(p.x + eps, p.y).z - getPoint(p.x - eps, p.y).z) / eps * 0.5f,
		-(getPoint(p.x, p.y + eps).z - getPoint(p.x, p.y - eps).z) / eps * 0.5f,
		1.f)
		));
}


//Renvoie la pente en un Point /*TotallementFaut*/
float TerrainFractal::getSlope(Pixel p) const
{
	Normals n = getNormal(p);

	return std::fabsf((180.f * std::atanhf(sqrt(n.x * n.x + n.y * n.y)) / M_PI) / 90.f);
	//return std::fabsf((std::atanhf(sqrt(n.x * n.x + n.y * n.y)) * 57.29578f) / 90.f);
}

TerrainFractal::~TerrainFractal()
{
	for (int i = 0; i < points_width; i++)
		delete[] pointList[i];
	delete[] pointList;
}
//
//Vector TerrainFractal::getColor ( const Vector & p ) const { 
//	return Vector ( 1., 1., 1. ); 
//}
