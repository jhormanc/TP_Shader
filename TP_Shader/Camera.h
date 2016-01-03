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
	void SetFov(float FoV);
public:
	
	Camera(Point o_, Point d, float dw, Vector up, float FoV);

	Vector PtScreen(int i, int j, int width, int height);

	void Move(const bool& move_target, const int& x = 0, const int& y = 0, const int& z = 0);
	void Rotate(const Vector& pt_screen);

	inline Point getOrigin()
	{
		return origin;
	}

	inline Point getTarget() const
	{
		return pt_view;
	}
};