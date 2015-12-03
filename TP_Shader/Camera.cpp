#include "Camera.h"


Camera::Camera(Point o, Vector d, Vector up, int w, int h, float near, float far, float angle)
{
	width = w;
	height = h;
	Transform camera =
		Transform::scale(float(w), float(h), 1.f) * Transform::translate(Vector(0.5, 0.5, 0.f)) *
		Transform::perspective(float(angle * M_PI / 180.f), near, far) * Transform::lookAt(o, o + d, up); // OK
	screenToRay = Mat4x4::inverse(camera.m);
}

Ray Camera::getRay(float x, float y)
{

	Point p0 = screenToRay * Point(x, (float)height - y, 0.f);
	Point p1 = screenToRay * Point(x, (float)height - y, 1.f);

	Vector d = normalize(p1 - p0);

	return Ray(p0, d);
}


Camera::~Camera()
{
}
