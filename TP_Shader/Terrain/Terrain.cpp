#include "Terrain.h"

// Fonction pour trouver la hauteur max et min
void Terrain::MaxMin ( double x ) {
	high = std::max ( high, x );
	low = std::min ( low, x );
}

// Renvoie vrai si le point p est en dehors du terrain, faux sinon.
bool Terrain::inside ( const Point &  p ) const {
	return ( p.z > getPoint ( p.x, p.y ).z );
}

// calcul la distance en hauteur entre le point p et le terrain
double Terrain::distance ( const Point &  p ) const {
	Point pointTerrain = getPoint ( p.x, p.y );

	if ( (pointTerrain != noIntersectPoint) ) // Car distance en z en attendant box.
		return ( p.z - pointTerrain.z ) * k; // 0.5 = pente maximale

	return noIntersect;
}

ColorRGB Terrain::getColor(const Point & p) {
	ColorRGB roche = { 150.f, 110.f, 40.f };
	ColorRGB herbe = { 40.f, 150.f, 74.f };
	ColorRGB neige = { 255.f, 255.f, 255.f };

	//float slope = dot ( getNormal ( Point ( p.x, p.y, p.z ) ), Normals ( .0f, .0f, .1f ) );
	return herbe;
	ColorRGB c = neige * abs(p.z / low) + herbe * abs(p.z / high);// + roche * slope;
	
	return c;
}

// Calcul la pente maximale du terrain
void Terrain::calcK ( ) {
	k = std::abs ( getPoint ( 0., 0. ).z - getPoint ( 0., 1. ).z );
	for ( unsigned int j = 0; j < terrain_height - 1; j++ ) {
		for ( unsigned int i = 0; i < terrain_width - 1; i++ ) {
			k = std::max (
				std::max (
				std::max ( k, double ( std::abs ( getPoint ( i, j ).z - getPoint ( i, j + 1 ).z ) ) ),
				std::abs ( double ( getPoint ( i, j ).z - getPoint ( i + 1, j ).z ) ) ),
				std::abs ( double ( getPoint ( i, j ).z - getPoint ( i + 1, j + 1 ).z ) ) );
		}
	}
	k /= high;
}

// Renvoie True si le Ray r touche le terrain
bool Terrain::intersect(const Ray& r, float *tHit) const
{
	*tHit = 0.f;
	Point res;
	for (int i = 0; i < 256; i++)
	{
		res = r.o + (r.d * *tHit);
		Point tmp = getPoint(res.x, res.y);
		if (tmp != noIntersectPoint)
		{
			double h = res.z - tmp.z;
			if (h < (0.001 * *tHit))
				return true;
			*tHit += k * h;
		}
		else
			*tHit += 10.;

	}
	*tHit = noIntersect;
	return false;
}


ColorRGB Terrain::getColor ( const Vector & p ) const 
{
	ColorRGB roche = { 150.f, 110.f, 40.f };
	ColorRGB herbe = { 40.f, 150.f, 74.f };
	ColorRGB neige = { 255.f, 255.f, 255.f };

	float slope = abs ( dot ( getNormal ( Point ( p.x, p.y, p.z ) ), Normals ( .0f, .0f, .1f ) ) );

	printf ( "%f", slope );

	if ( slope > .35f )
		return roche;
	if ( p.z < 3 * high * .25f )
		return herbe;
	else 
		return neige;
}
