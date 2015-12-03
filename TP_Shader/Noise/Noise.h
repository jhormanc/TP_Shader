#pragma once

#include <stdlib.h>
#include <math.h>
#include "..\Geometry\Vector.h"

class Noise {

public:
	static double noise ( double x, double y );
	static double ridge ( const double z, const double zr );

private:
	static int fastfloor ( double x );
	static double dot ( int g[], double x, double y );

	// Renvoi une valeur aleatoire entre 0 et 1 a partir de x.
	static double hash ( double x );
	static Vector warp ( const Vector p, const double c, const double f );
	static double smooth ( const double z, const double zmin, const double zmax );

	static double perlin2D ( double x, double y, const double res_x, const double res_y );
	static double simplex ( double xin, double yin );

};

