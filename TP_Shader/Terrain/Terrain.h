#pragma once

#include <math.h>
#include "..\Geometry\Geometry.h"
#include "..\Geometry\Vector.h"
#include "..\Geometry\Normals.h"
#include "..\Global\Global.h"
#include "Global\Constants.h"
#include <qdebug.h>
class Terrain
{

protected:
	
	
	double k;						// Pente maximale
	double high, low;				// Paramètre pour connaitre la hauteur max et min de la map

public:
	unsigned int terrain_width;
	unsigned int terrain_height;
	ColorRGB ** precalc;
	Terrain();
	Terrain(unsigned int terrain_width, unsigned int terrain_height);
	Terrain(const Terrain&);
	Terrain & operator=(const Terrain&);
	//Pour definir un max et un min
	void MaxMin(double);

	// Renvoie vrai si le point p est en dehors du terrain, faux sinon.
	virtual bool inside(const Point & p) const;

	// calcul la distance en hauteur entre le point p et le terrain
	virtual double distance (const Point & p) const;

	//	virtual Vector getColor ( const Vector & p ) const = 0;
	 ColorRGB getColor(const Point & p);

	 ColorRGB getColorPrecalculed(const Point & p);
	 ColorRGB ColorFadeHight(ColorRGB,ColorRGB,ColorRGB,double,double,double);

	// Renvoi la normal du terrain au point p
	virtual Normals getNormal(Point p) const = 0;

	// Renvoie le point x, y, z appartenant a pointList a partir du x, y (recherche matrice + interpolation).
	virtual Point getPoint(float x, float y) const = 0;

	Point getOrigin () const 
	{
		return Point (0.); // TODO
	}

	virtual bool intersect(const Ray &ray, float * tHit) const;
	virtual BBox getBound() const
	{
		return BBox(getOrigin() + Point(0., 0., low), Point(terrain_width, terrain_height, high));
	}
	bool intersectSegment(const Ray& r, float * tHit, float tMax) const;

	// Renvoie vrai si le Ray r touche le terrain.
	//bool intersection(Ray r, double &t) const;

	// DEBUG
	unsigned int getWidth() const 
	{
		return terrain_width;
	}

	unsigned int getHeight() const 
	{
		return terrain_height;
	}

	double getLow() const 
	{
		return low;
	}

	double getHigh() const 
	{
		return high;
	}
	
	// Calcul la pente maximale du terrain
	void calcK();
	virtual ~Terrain();
	//Mesh* GetMesh ( );
	
};

