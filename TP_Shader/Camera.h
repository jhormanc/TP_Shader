#pragma once
#include "Geometry\Geometry.h"
#include "Transform\Transform.h"
#include "Global\Global.h"

class Camera
{
	Vector u, v, w;
	double lu, lv, lw;
	Vector up;
public:
	Point o;
	Camera(Point o_, Point d, double dw, Vector up);

	Vector PtScreen(int i, int j, int width, int height);

	void Init(Point o_, Point d, double dw);

	Point getOrigin()
	{
		return o;
	}
};