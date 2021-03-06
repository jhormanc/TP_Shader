#include "Terrain.h"

Terrain::Terrain()
{
	renderGrey = false;
}

Terrain::Terrain(const Terrain& terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	points_width = terrain.points_width;
	points_height = terrain.points_height;
	steps = terrain.steps;

	precalc = new ColorRGB *[terrain.points_width];
	for (int i = 0; i < terrain.points_width; i++)
		precalc[i] = new ColorRGB[terrain.points_height];

	for (int i = 0; i < terrain.points_width; ++i)
		for (int j = 0; j < terrain.points_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];

	renderGrey = false;
}

Terrain::Terrain(const int& terrain_width_, const int& terrain_height_, const int& _steps) : terrain_width(terrain_width_), terrain_height(terrain_height_), steps(_steps)
{
	if (steps <= 0)
		steps = 1;

	points_width = (int)(terrain_width / (float)steps);
	points_height = (int)(terrain_height / (float)steps);

	precalc = new ColorRGB *[points_width];
	for (int i = 0; i < points_width; i++)
		precalc[i] = new ColorRGB[points_height];

	renderGrey = false;
}

Terrain & Terrain::operator=(const Terrain& terrain)
{
	terrain_width = terrain.terrain_width;
	terrain_height = terrain.terrain_height;
	points_width = terrain.points_width;
	points_height = terrain.points_height;
	steps = terrain.steps;

	precalc = new ColorRGB *[terrain.points_width];
	for (int i = 0; i < terrain.points_width; i++)
		precalc[i] = new ColorRGB[terrain.points_height];

	for (int i = 0; i < terrain.points_width; ++i)
		for (int j = 0; j < terrain.points_height; ++j)
			precalc[i][j] = terrain.precalc[i][j];
	return *this;
}

// Fonction pour trouver la hauteur max et min
void Terrain::MaxMin(float x)
{
	high = std::fmaxf(high, x);
	low = std::fminf(low, x);
}

// Renvoie vrai si le point p est en dehors du terrain, faux sinon.
bool Terrain::inside(const Point& p) const 
{
	return (p.z > getPoint(p.x, p.y).z);
}

// calcul la distance en hauteur entre le point p et le terrain
float Terrain::distance(const Point &  p) const
{
	Point pointTerrain = getPoint ( p.x, p.y );

	if (pointTerrain != noIntersectPoint) // Car distance en z en attendant box.
		return (p.z - pointTerrain.z); // 0.5 = pente maximale

	return noIntersect;
}

// Calcul la pente maximale du terrain
void Terrain::calcK() 
{
	k = getSlope(getPoint(0.f, 0.f));

	#pragma omp parallel for schedule(static)
	for (int j = 0; j < points_height - 1; j++) 
	{
		for (int i = 0; i < points_width - 1; i++)
		{
			k = std::max(getSlope(getPoint(i * steps, j * steps)), k);
		}
	}

	k *= 1.2f;
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

	float k2 = 1.f / (k * Vector(r.d.x, r.d.y, 0.f).length() - r.d.z);
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
			float h = res.z - tmp.z;
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
	float k2 = 1.f / (k * Vector(r.d.x, r.d.y, 0.f).length() - r.d.z);
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
			float h = res.z - tmp.z;
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
	return precalc[(int)(p.x / steps)][(int)(p.y / steps)];
}

void Terrain::initColor(Pixel& p)
{
	float slope = getSlope(p);
	float h = (high - low);
	h = (p.z - low) / (200.f - low);

	Vector w = Noise::warp(Vector(p.x, p.y, 0.f), 2.f, 1.f / 200.f, false);
	float v = Noise::simplex(w.x / 12.5f, w.y / 12.5f)
		+ Noise::simplex(w.x / 25.f, w.y / 25.f)
		+ Noise::simplex(w.x / 50.f, w.y / 50.f)
		+ Noise::simplex(w.x / 100.f, w.y / 100.f);
	v = (v + 4.f) * 0.125f;

	ColorRGB color;

	if (p.z < 0.f)
	{
		color = ColorRGB(water) * v + ColorRGB(water_bright) * (1.f - v);
		p.alpha = 0.1f;
	}
	else if (p.z < 200.f)
	{
		ColorRGB color_grass = ColorRGB(grass) * v + ColorRGB(grass_bright) * (1.f - v);
		ColorRGB color_mountain = ColorRGB(rock) * v + ColorRGB(rock_bright) * (1.f - v);

		if (slope < 0.05f)
			color = color_grass;
		else if (slope < 0.2f)
			color = color_mountain * slope + color_grass * (1.f - slope);
		else
			color = color_mountain;
	}
	else
	{
		if (slope < 0.3f) // 0.4f
			color = ColorRGB(neige) * v + ColorRGB(neige_dark) * (1.f - v);
		else if (slope < 0.5f) // 0.6f
		{
			ColorRGB color_snow = ColorRGB(neige) * v + ColorRGB(neige_dark) * (1.f - v);
			ColorRGB color_mountain = ColorRGB(rock) * v + ColorRGB(rock_bright) * (1.f - v);

			color = color_mountain * slope + color_snow * (1.f - slope);
		}
		else
			color = ColorRGB(rock) * v + ColorRGB(rock_bright) * (1.f - v);
	}
	
	p.color = color;
}

ColorRGB Terrain::getColor(const Point & p) 
{
	if (!renderGrey)
	{
		int tmpI = (int)(p.x / steps);
		int tmpJ = (int)(p.y / steps);

		Pixel & a(pointList[tmpI < points_width - 1 ? tmpI + 1 : tmpI][tmpJ]);
		Pixel & b(pointList[tmpI][tmpJ < points_height - 1 ? tmpJ + 1 : tmpJ]);
		Pixel & c(pointList[tmpI < points_width - 1 ? tmpI + 1 : tmpI][tmpJ < points_height - 1 ? tmpJ + 1 : tmpJ]);
		Pixel & d(pointList[tmpI][tmpJ]);

		float x2 = (p.x - d.x) / steps;
		float y2 = (p.y - d.y) / steps;
		ColorRGB color = d.color * (1.f - x2) * (1.f - y2)
			+ a.color * x2 * (1.f - y2)
			+ b.color * (1.f - x2) * y2
			+ c.color * y2 * x2;

		return color;

		//return getPoint(p.x, p.y).color;
	}
	else
		return grey;
}

/*
*c1 and c2 are the color to do the fading
*c3 is for the part where the slope is to hight.
*/
ColorRGB Terrain::ColorFadeHight(ColorRGB c1, ColorRGB c2, ColorRGB c3, float z, float nb_step, float slope)
{
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
	for (unsigned int i = 0; i < points_width; i++)
		delete[] precalc[i];
	delete[] precalc;
}
