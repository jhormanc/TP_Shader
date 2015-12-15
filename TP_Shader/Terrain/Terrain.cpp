#include "Terrain.h"

Terrain::Terrain()
{
	renderGrey = false;
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

	renderGrey = false;
}

Terrain::Terrain(unsigned int terrain_width_, unsigned int terrain_height_) : terrain_width(terrain_width_), terrain_height(terrain_height_)
{
	precalc = new ColorRGB *[terrain_width_];
	for (unsigned int i = 0; i < terrain_width_; i++)
		precalc[i] = new ColorRGB[terrain_height_];

	renderGrey = false;
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
bool Terrain::intersect(const Ray& r, float *tHit, int * nbIter) const
{
	int nbStep = 0;
	// a decommenter quand k sera = max des derivees
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
	if (tmax < tmin)
		return false;
	*tHit = tmin;
	Point res = r.o + (r.d * *tHit);

	while (*tHit >= tmin && *tHit <= tmax)
	{
		++nbStep;
		res = r.o + (r.d * *tHit);
		Point tmp = getPoint(res.x, res.y);
		if (tmp != noIntersectPoint)
		{
			double h = res.z - tmp.z;
			if (h < 0.001f)
			{
				if (nbIter != nullptr) *nbIter = nbStep;
				return true;
			}
			*tHit +=  h * k2;
		}
		else
		{
			*tHit = noIntersect;
			if (nbIter != nullptr) *nbIter = nbStep;
			return false;
		}

	}
	*tHit = noIntersect;
	if (nbIter != nullptr) *nbIter = nbStep;
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
	tMax = std::min(tMax, t2);
	if (tmin >= tMax)
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

ColorRGB Terrain::getColorPrecalculed(const Point & p) 
{
	//Point intersect = getPoint(p.x, p.y);
	return precalc[(int)p.x][(int)p.y];
}

ColorRGB Terrain::getColor(const Point & p) 
{
	if (!renderGrey)
	{
		double z = getPoint(p.x, p.y).z;

		double slope = std::max(std::max(std::max(
			std::abs(double(z - getPoint(p.x - 1, p.y).z)),
			std::abs(double(z - getPoint(p.x + 1, p.y).z))),
			std::abs(double(z - getPoint(p.x, p.y - 1).z))),
			std::abs(double(z - getPoint(p.x, p.y + 1).z)));
		slope *= .5f;

		ColorRGB color;

		double max = high - low;

		float steps = low + (max * (20. / 100.));

		if (z >= low + (max * (80. / 100.))) 
		{
			color = neige;
		}
		else if (z >= low + (max * (60. / 100.))) 
		{
			if (slope <= .1f)
				color = neige;
			else
				color = ColorFadeHight(roche, neige, terre_claire, z - 3. * steps, steps, slope);
		}
		else if (z >= low + (max * (40. / 100.))) 
		{
			color = ColorFadeHight(terre, roche, terre_claire, z - 2. * steps, steps, slope);
		}
		else if (z >= 0.f) 
		{
			color = ColorFadeHight(herbe, terre, terre_claire, z, 2.f * steps, slope);
		}
		else 
		{
			color = bleue;
		}

		return color;
		//double z = getPoint(p.x, p.y).z;
		//double slope = getSlope(p);

		////float n = Noise::noise(p.x,p.y);
		////float v = (n*(z / 1000) + 0.2*n*(z / 500) + 0.1*n*(z / 200));
		//float v = (Noise::noise2(p.x / 1000.f, p.y / 1000.f) + 0.7*Noise::noise2(p.x / 500.f, p.y / 500.f) + 0.5*Noise::noise2(p.x / 100.f, p.y / 100.f)) / 2.2;
		////v = (1.0 + sin(p.x / 100.0))*0.5;
		////float v = 1.f;
		//ColorRGB color;

		//if (slope<0.2)
		//	color = ColorRGB{ 91, 60, 17 } *v + ColorRGB{ 205	,133	,63 } *(1 - v);
		//else
		//	color = ColorRGB{ 246,	220,	18 } *v + ColorRGB{ 254, 248, 108 } *(1 - v);
		////color hori = brun * v + brun clair * (1 - v);

		//return color;
	}
	else
		return grey;
}

/*
*c1 and c2 are the color to do the fading
*c3 is for the part where the slope is to hight.
*/
ColorRGB Terrain::ColorFadeHight(ColorRGB c1, ColorRGB c2, ColorRGB c3, double z, double nb_step, double slope){
	ColorRGB colorDiff = c2 - c1;
	ColorRGB c = c1 + ((colorDiff * z) / nb_step);
	c = c + c3 *slope;

	return c;
}

void Terrain::ChangeRenderColor(const bool& render_grey)
{
	renderGrey = render_grey;
}



Terrain::~Terrain()
{
	for (unsigned int i = 0; i < terrain_width; i++)
		delete[] precalc[i];
	delete[] precalc;
}
