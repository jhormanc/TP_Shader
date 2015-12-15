#pragma once

#include "Terrain.h"
#include "..\Noise\Noise.h"
#include "Shapes\Pixel.h"

class TerrainFractal : public Terrain 
{

public:
	TerrainFractal();
	// Renvoi un terrain g�ner� al�atoirement
	TerrainFractal (unsigned terrain_width_, unsigned int terrain_height_);
	TerrainFractal(const TerrainFractal &);
	TerrainFractal & operator=(const TerrainFractal &);
	Pixel getPoint(float x, float y) const;
	double getZ(float x, float y) const;
	double getSlope(Pixel p) const;

	// Renvoi la normal du terrain au point p
	Normals getNormal(Pixel p) const;
	~TerrainFractal();
	//Vector getColor ( const Vector & p ) const;
};

