#pragma once

#include <math.h>
#include <qdebug.h>
#include "..\Geometry\Geometry.h"
#include "..\Geometry\Vector.h"
#include "..\Geometry\Normals.h"
#include "..\Shapes\Pixel.h"
#include "Global\Constants.h"
#include "..\Noise\Noise.h"

class Terrain
{

protected:
	
	bool renderGrey;
	float k;						// Pente maximale
	float high, low;				// Paramètre pour connaitre la hauteur max et min de la map
	Pixel ** pointList;
	int steps;
	int terrain_width, terrain_height;
	int points_width, points_height;
public:

	ColorRGB ** precalc;
	Terrain();
	Terrain(const int& terrain_width, const int& terrain_height, const int& _teps);
	Terrain(const Terrain&);
	Terrain & operator=(const Terrain&);
	//Pour definir un max et un min
	void MaxMin(float);

	// Renvoie vrai si le point p est en dehors du terrain, faux sinon.
	virtual bool inside(const Point & p) const;

	// calcul la distance en hauteur entre le point p et le terrain
	virtual float distance(const Point & p) const;

	//	virtual Vector getColor ( const Vector & p ) const = 0;
	 ColorRGB getColor(const Point & p);
	 void initColor(Pixel &p);

	 ColorRGB getColorPrecalculed(const Point & p);
	 ColorRGB ColorFadeHight(ColorRGB, ColorRGB, ColorRGB, float, float, float);

	// Renvoi la normal du terrain au point p
	 virtual Normals getNormal(Pixel p) const = 0;

	// Renvoie le point x, y, z appartenant a pointList a partir du x, y (recherche matrice + interpolation).
	virtual Pixel getPoint(float x, float y) const = 0;
	virtual float getZ(float x, float y) const = 0;
	virtual float getSlope(Pixel p) const = 0;

	Point getOrigin () const 
	{
		return Point (0.); // TODO
	}

	virtual bool intersect(const Ray &ray, float * tHit, int * nbIter) const;
	virtual BBox getBound() const
	{
		return BBox(getOrigin() + Point(0., 0., low), Point(terrain_width, terrain_height, high));
	}
	bool intersectSegment(const Ray& r, float * tHit, float tMax) const;

	// Renvoie vrai si le Ray r touche le terrain.
	//bool intersection(Ray r, double &t) const;

	inline int getPointsWidth() const
	{
		return points_width;
	}

	inline int getPointsHeight() const 
	{
		return points_height;
	}

	inline int getSteps() const
	{
		return steps;
	}
		

	float getLow() const
	{
		return low;
	}

	float getHigh() const
	{
		return high;
	}


	
	// Calcul la pente maximale du terrain
	void calcK();
	virtual ~Terrain();
	//Mesh* GetMesh ( );

	void ChangeRenderColor(const bool& render_grey);
	
};

