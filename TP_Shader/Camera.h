#pragma once
#include "Geometry\Geometry.h"
#include "Transform\Transform.h"
class Camera
{
	Mat4x4 screenToRay;
	int width, height;
public:
	Camera(Point o, Vector d, Vector up, int w, int h, float near, float far, float angle);
	Ray getRay(float x, float y);
	~Camera();
};

