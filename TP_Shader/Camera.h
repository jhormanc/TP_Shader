#pragma once
#include "Geometry\Geometry.h"
#include "Global\Global.h"

class Camera
{
	Vector u, v, w;
	float lu, lv, lw;
	Vector up;
	Point origin;
	Point pt_view;
	
	void Init();

public:
	
	Camera(Point o_, Point d, float dw, Vector up);

	Vector PtScreen(int i, int j, int width, int height);

	//void Init(Point o_, Point d, double dw);

	void Move(const bool& move_target, const int& x = 0, const int& y = 0, const int& z = 0);
	void Rotate(const Vector& pt_screen);

	Point getOrigin();

	Vector Forward() const;
};