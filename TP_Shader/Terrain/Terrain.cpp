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
		return (p.z - pointTerrain.z) * k; // 0.5 = pente maximale

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
	k *= 0.5;
}

// Renvoie True si le Ray r touche le terrain
bool Terrain::intersect(const Ray& r, float *tHit) const
{
	// a decommenter quand k sera = max des d�riv�es
	if (r.d.z > k)
	{
		return false;
	}
	float k2 = 1.f / (k - r.d.z);
	BBox box = getBound();
	Point res = r.o + (r.d * *tHit);
	float tmin = std::min((res - box.pMin).length(), (res - box.pMax).length());
	float tmax = std::max((res - box.pMin).length(), (res - box.pMax).length());
	*tHit = tmin;
	while (*tHit >= tmin && *tHit <= tmax)
	{
		res = r.o + (r.d * *tHit);
		Point tmp = getPoint(res.x, res.y);
		if (tmp != noIntersectPoint)
		{
			double h = res.z - tmp.z;
			if (h < (0.0001 * *tHit))
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


ColorRGB Terrain::getColor ( const Point & p ) {
	//ColorRGB roche = { 150.f, 110.f, 40.f };
	//ColorRGB herbe = { 40.f, 150.f, 74.f };
	//ColorRGB neige = { 255.f, 255.f, 255.f };
	////return herbe;
	//float slope = abs ( dot ( getNormal ( Point ( p.x, p.y, p.z ) ), Normals ( .0f, .0f, .1f ) ) );

	//printf ( "%f", slope );

	//if ( slope > .35f )
	//	return roche;
	//if ( p.z < 3 * high * .25f )
	//	return herbe;
	//else 
	//	return neige;


	double z = getPoint(p.x, p.y).z;
	ColorRGB color;

	ColorRGB white = { 230.f, 230.f, 230.f };
	ColorRGB brown = { 51.f, 25.f, 0.f };
	ColorRGB green = { 0.f, 102.f, 10.f };
	ColorRGB blue = { 0.f, 128.f, 220.f };
	ColorRGB grey = { 64.f, 64.f, 64.f };

	double max = high - low;

	// Noise

	if (z >= low + (max * (70. / 100.)))
		color = white;
	else if (z >= low + (max * (50. / 100.)))
		color = grey;
	else if (z >= low + (max * (10. / 100.)))
		color = brown;
	else if (z >= 0.f)
		color = green;
	else
		color = blue;

	return color;
}
