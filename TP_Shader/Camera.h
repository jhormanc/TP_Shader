#pragma once
#include "Geometry\Geometry.h"
#include "Transform\Transform.h"
#include "Global\Global.h"

class Camera
{
	Vector u, v, w;
	double lu, lv, lw;
public:
	Point o;
	Camera(Point o_, Point d, double dw, Vector up);

	Vector PtScreen(int i, int j, int width, int height);

	Point getOrigin()
	{
		return o;
	}
};