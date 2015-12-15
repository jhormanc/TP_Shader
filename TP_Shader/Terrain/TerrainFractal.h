#pragma once

#include "..\Geometry\Vector.h"
#include "Terrain.h"
#include "..\Noise\Noise.h"

class TerrainFractal : public Terrain 
{
	Vector ** pointList;
public:
	TerrainFractal();
	// Renvoi un terrain g�ner� al�atoirement
	TerrainFractal (unsigned terrain_width_, unsigned int terrain_height_);
	TerrainFractal(const TerrainFractal &);
	TerrainFractal & operator=(const TerrainFractal &);
	Point getPoint (float x, float y) const;
	double getZ(float x, float y) const;
	double getSlope(Point p) const;

	// Renvoi la normal du terrain au point p
	Normals getNormal(Point p) const;
	~TerrainFractal();
	//Vector getColor ( const Vector & p ) const;
};

