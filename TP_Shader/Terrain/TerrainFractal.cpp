#include "TerrainFractal.h"


// Renvoi un terrain généré aléatoirement
TerrainFractal::TerrainFractal (unsigned int terrain_width_, unsigned int terrain_height_) : Terrain () 
{
	terrain_width = terrain_width_;
	terrain_height = terrain_height_;

	high = (low = Noise::noise(0., 0.));

	pointList = new Vector *[terrain_width];
	for (int i = 0; i < terrain_width; i++)
		pointList[i] = new Vector[terrain_height];

	// Pour récuperer le Low and Height
	for (unsigned int j = 0; j < terrain_height; j++) 
	{
		for (unsigned int i = 0; i < terrain_width; i++) 
		{
			float z = Noise::noise((float)i, (float)j);
			pointList[i][j] = Vector((float)i, (float)j, z);
			MaxMin(z);
		}
	}

	calcK();
}

Point TerrainFractal::getPoint(float x, float y) const 
{
	return x > 0 && x < terrain_width && y > 0 && y < terrain_height ? Point ( x, y, Noise::noise ( x, y ) ) : noIntersectPoint;

	/*int tmpI = (int)x;
	int tmpJ = (int)y;

	if (!(tmpI < terrain_width && tmpJ < terrain_height))
		return noIntersectPoint;

	Vector & a(pointList[tmpI < terrain_width - 1 ? tmpI + 1 : tmpI][tmpJ]);
	Vector & b(pointList[tmpI][tmpJ < terrain_width - 1 ? tmpJ + 1 : tmpJ]);
	Vector & c(pointList[tmpI < terrain_width - 1 ? tmpI + 1 : tmpI][tmpJ < terrain_width - 1 ? tmpJ + 1 : tmpJ]);

	double x2 = x - (double)tmpI;
	double y2 = y - (double)tmpJ;
	double z = (1 - x2) * (1 - y2) * pointList[tmpI][tmpJ].z
		+ x2 * (1 - y2) * a.z
		+ (1 - x2) * y2 * b.z
		+ x2 * y2 * c.z;

	return Point(x, y, z);*/
}

// Renvoi la normal du terrain au point p
Normals TerrainFractal::getNormal(Point p) const 
{
	float eps = .1f;
	return Normals ( getPoint ( p.x - eps, p.y - eps ) + getPoint ( p.x + eps, p.y + eps ) ) / ( 2 * eps );
}
//
//Vector TerrainFractal::getColor ( const Vector & p ) const { 
//	return Vector ( 1., 1., 1. ); 
//}
