#pragma once

#include <stdlib.h>
#include <math.h>
#include "..\Geometry\Vector.h"

class Noise
{
public:
	static float noise(const float x, const float y);
	static float ridge(const float z, const float zr);
	static float noise1(const float x, const float y);
	static float noise3(const float x, const float y);
	static float noise2(const float x, const float y);
	static float noiseAmbient(const float x, const float y);
	static float noiseMountains(const float x, const float y, const float z_plains);
	static float noisePlains(const float x, const float y);
	static float noiseWater(const float, const float y);
	static Vector warp(const Vector p, const float c, const float f, const bool perlin);
	static float smooth(const float z, const float zmin, const float zmax);

	static float perlin2D(const float x, const float y);
	static float simplex(const float xin, const float yin);

private:

	static int fastfloor(float x);
	static float dot(int g[], float x, float y);

	// Renvoi une valeur aleatoire entre 0 et 1 a partir de x.
	static float hash(float x);
};


