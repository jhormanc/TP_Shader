#pragma once

#include "..\Geometry\Vector.h"
#include "Terrain.h"
#include "..\Noise\Noise.h"

class TerrainFractal : public Terrain 
{
	Vector ** pointList;
public:
	// Renvoi un terrain géneré aléatoirement
	TerrainFractal (unsigned terrain_width_, unsigned int terrain_height_);

	Point getPoint (float x, float y) const;

	// Renvoi la normal du terrain au point p
	Normals getNormal(Point p) const;

	//Vector getColor ( const Vector & p ) const;
};

