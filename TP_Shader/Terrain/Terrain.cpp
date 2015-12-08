#include "Terrain.h"

// Fonction pour trouver la hauteur max et min
void Terrain::MaxMin(double x) 
{
	high = std::max(high, x);
	low = std::min(low, x);
}

// Renvoie vrai si le point p est en dehors du terrain, faux sinon.
bool Terrain::inside(const Point& p) const 
{
	return (p.z > getPoint(p.x, p.y).z);
}

// calcul la distance en hauteur entre le point p et le terrain
double Terrain::distance ( const Point &  p ) const 
{
	Point pointTerrain = getPoint ( p.x, p.y );

	if (pointTerrain != noIntersectPoint) // Car distance en z en attendant box.
		return (p.z - pointTerrain.z); // 0.5 = pente maximale

	return noIntersect;
}

// Calcul la pente maximale du terrain
void Terrain::calcK() 
{
	k = std::abs(getPoint(0., 0.).z - getPoint(0., 1.).z);

	#pragma omp parallel for schedule(static)
	for (int j = 0; j < terrain_height - 1; j++) 
	{
		for (int i = 0; i < terrain_width - 1; i++) 
		{
			k = std::max(std::max(std::max(k, 
				std::abs(double(getPoint(i, j).z - getPoint(i, j + 1).z))),
				std::abs(double(getPoint(i, j).z - getPoint(i + 1, j).z) )),
				std::abs(double(getPoint(i, j).z - getPoint(i + 1, j + 1).z)));
		}
	}
	k *= 0.5f;
}

// Renvoie True si le Ray r touche le terrain
bool Terrain::intersect(const Ray& r, float *tHit) const
{
	// a decommenter quand k sera = max des dérivées
	if (r.d.z > k)
	{
		return false;
	}
	float k2 = 1.f / (k - r.d.z);
	BBox box = getBound();
	Point res = r.o + (r.d * *tHit);
	float t1, t2;
	box.intersect(r, &t1, &t2);
	float tmin = std::max(0.f, std::min(t1, t2));
	float tmax = std::max(t1, t2);
	//tmin = std::max(0.f, tmin);
	*tHit = tmin;
	//*tHit = 0.f;
	//Point res;
	//for (int i = 0; i < 1024; i++)
	while (*tHit >= tmin && *tHit <= tmax)
	{
		res = r.o + (r.d * *tHit);
		Point tmp = getPoint(res.x, res.y);
		if (tmp != noIntersectPoint)
		{
			double h = res.z - tmp.z;
			if (h < 0.001 * *tHit)
				return true;
			*tHit +=  h * k2;
		}
		else
		{
			*tHit = noIntersect;
			return false;
		}

	}
	*tHit = noIntersect;
	return false;
}

bool Terrain::intersectSegment(const Ray& r, float *tHit, float tMax) const
{
	float tmin = 0.f;
	if (r.d.z > k)
	{
		return false;
	}
	float k2 = 1.f / (k - r.d.z);
	while (*tHit >= tmin && *tHit <= tMax)
	{
		Point res = r.o + (r.d * *tHit);
		Point tmp = getPoint(res.x, res.y);
		if (tmp != noIntersectPoint)
		{
			double h = res.z - tmp.z;
			if (h < 0.001)
				return true;
			*tHit += h * k2;
		}
		else
		{
			*tHit = noIntersect;
			return false;
		}

	}
	*tHit = noIntersect;
	return false;
}
ColorRGB Terrain::getColor ( const Point & p ) {
//	return ColorRGB{ 255.f, 255.f, 255.f };
	ColorRGB roche = { 100.f, 100.f, 100.f };
	ColorRGB roche_claire = { 200.f, 200.f, 200.f };
	ColorRGB terre = { 95.f, 62.f, 5.f };
	ColorRGB terre_claire = { 195.f, 162.f, 105.f };
	ColorRGB herbe = { 40.f, 150.f, 74.f };
	ColorRGB neige = { 255.f, 255.f, 255.f };
	ColorRGB bleue = { 0.f, 128.f, 220.f };

	double z = getPoint ( p.x, p.y ).z;

	double slope = std::max ( std::max ( std::max (
		std::abs ( double ( z - getPoint ( p.x - 1, p.y ).z ) ),
		std::abs ( double ( z - getPoint ( p.x + 1, p.y ).z ) ) ),
		std::abs ( double ( z - getPoint ( p.x, p.y - 1 ).z ) ) ),
		std::abs ( double ( z - getPoint ( p.x, p.y + 1 ).z ) ) );		
	slope *= .5f;
	
	ColorRGB color;

	double max = high - low;

	float rng1 = 60.0f + rand ( ) * 10;
	float rng2 = 40.0f + rand ( ) * 10;
	float rng3 = .0f + rand ( ) * 10;
		
	if ( z >= low + ( max * ( 80 / 100. ) ) ) {
		if ( slope <= .8f )
			color = neige;
		else
			color = roche_claire;
	}		
	else if ( z >= low + ( max * ( 60 / 100. ) ) ) {
		if (slope <= .1f )
			color = neige;
		else if ( slope <= .3f )
			color = roche_claire;
		else
			color = roche;
	}	
	else if ( z >= low + ( max * ( 40 / 100. ) ) ) {
		if ( slope <= .25f )
			color = terre;
		else
			color = terre_claire;
	}
	else if ( z >= low + ( max * ( 20 / 100. ) ) ) {
		if ( slope <= .15f )
			color = herbe;
		else
			color = terre;
	}		
	else if ( z >= 0.f ) {
		if ( slope <= .85f )
			color = herbe;
		else
			color = terre;
	}		
	else {
		color = bleue;
	}		
		
	return color;
}
