#include "Terrain.h"

Terrain::Terrain()
{

}

Terrain::Terrain(const Terrain& terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	precalc = new ColorRGB *[terrain.terrain_width];
	for (unsigned int i = 0; i < terrain.terrain_width; i++)
		precalc[i] = new ColorRGB[terrain.terrain_height];

	for (unsigned int i = 0; i < terrain.terrain_width; ++i)
		for (unsigned int j = 0; j < terrain.terrain_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];
}

Terrain::Terrain(unsigned int terrain_width_, unsigned int terrain_height_) : terrain_width(terrain_width_), terrain_height(terrain_height_)
{
	precalc = new ColorRGB *[terrain_width_];
	for (unsigned int i = 0; i < terrain_width_; i++)
		precalc[i] = new ColorRGB[terrain_height_];
}

Terrain & Terrain::operator=(const Terrain& terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	precalc = new ColorRGB *[terrain.terrain_width];
	for (unsigned int i = 0; i < terrain.terrain_width; i++)
		precalc[i] = new ColorRGB[terrain.terrain_height];

	for (unsigned int i = 0; i < terrain.terrain_width; ++i)
		for (unsigned int j = 0; j < terrain.terrain_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];
	return *this;
}
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
	
	float t1, t2;
	if (!box.intersect(r, &t1, &t2))
		return false;
	float tmin = std::max(0.f, std::min(t1, t2));
	float tmax = std::max(t1, t2);
	if (tmax <= tmin)
		return false;
	//qDebug(" min box : %f, %f, %f", box.pMin.x, box.pMin.y, box.pMin.z);
	//qDebug(" max box : %f, %f, %f", box.pMax.x, box.pMax.y, box.pMax.z);
	////qDebug(" res : %f, %f, %f", res.x, res.y, res.z);
	//qDebug(" tmin : %f", tmin);
	//qDebug(" tmax : %f", tmax);
	//tmin = std::max(0.f, tmin);
	*tHit = tmin;
	Point res = r.o + (r.d * *tHit);/*
	qDebug(" res : %f, %f, %f", res.x, res.y, res.z);*/

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

bool Terrain::intersectSegment(const Ray& r, float * tHit, float tMax) const
{
	float t1, t2;
	if (r.d.z > k)
	{
		return false;
	}
	float k2 = 1.f / (k - r.d.z);
	BBox box = getBound();
	if (!box.intersect(r, &t1, &t2))
		return false;
	float tmin = std::max(0.f, std::min(t1, t2));
	if (tmin > tMax)
		return false;
	*tHit = tmin;
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

ColorRGB Terrain::getColorPrecalculed(const Point & p) {
	//Point intersect = getPoint(p.x, p.y);
	return precalc[(int)p.x][(int)p.y];
}
ColorRGB Terrain::getColor ( const Point & p ) {
	return ColorRGB{ 255.f, 255.f, 255.f };
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

	/*float rng1 = 60.0f + rand ( ) * 10;
	float rng2 = 40.0f + rand ( ) * 10;
	float rng3 = .0f + rand ( ) * 10;
*/
	float steps = low + (max * (20 / 100.));
	
	if ( z >= low + ( max * ( 80 / 100. ) ) ) {
		if ( slope <= .8f )
			color = neige;
		else
			color = ColorFade(roche_claire, neige, z - 4. * steps, steps);
	}		
	else if ( z >= low + ( max * ( 60 / 100. ) ) ) {
		if (slope <= .1f)
			color = neige;
		else if (slope <= .3f)
			color = roche_claire;
		else
			color = ColorFade(roche, neige, z - 3. * steps, steps);
	}	
	else if ( z >= low + ( max * ( 40 / 100. ) ) ) {
		if ( slope <= .25f )
			color = ColorFade(terre, roche, z - 2. * steps, steps);
		else
			color = terre_claire;
	}
	else if (z >= low + (max * (20 / 100.))) {
		if ( slope <= .15f )
			color = ColorFade(herbe, terre, z - steps, steps);
		else
			color = terre;
	}		
	else if ( z >= 0.f ) {
		if (slope <= .85f)
			color = ColorFade(herbe, terre, z, steps);
		else
			color = ColorFade(terre, roche, z, steps);;
	}		
	else {
		color = bleue;
	}		
		
	return color;
}

ColorRGB Terrain::ColorFade(ColorRGB c1, ColorRGB c2, double z ,double nb_step){
	ColorRGB colorDiff = c2 - c1;
	return c1 + ((colorDiff * z) / nb_step);
}


Terrain::~Terrain()
{
	for (unsigned int i = 0; i < terrain_width; i++)
		delete[] precalc[i];
	delete[] precalc;
}
