#include "Camera.h"

Camera::Camera(Point o_, Point a_, float dw, Vector _up)
{
	origin = o_;
	pt_view = a_;
	up = _up;

	Init();

	lu = windowWidth / windowHeight;
	lv = 1.f;
	lw = dw;
}

Vector Camera::PtScreen(int i, int j, int width, int height)
{
	float tu = (float)i / (float)(width - 1);
	float tv = (float)j / (float)(height - 1);

	Vector res = Vector(origin.x, origin.y, origin.z) + (w * lw)
		+ u * (-lu * (1 - tu) + tu * lu)
		+ v * (-lv * (1 - tv) + tv * lv);

	return res;
}

void Camera::Init()
{
	w = normalize(pt_view - origin);
	u = normalize(-cross(w, up));
	v = cross(w, u);
	u = -u;
}

// D�place l'origine dans la direction de la cam�ra
void Camera::Move(const bool& move_target, const int& x, const int& y, const int& z)
{
	Point dir(0.f);

	if (x != 0)
		dir += u * (float)x;
	if (y != 0)
		dir += w * (float)y;
	if (z != 0)
		dir += v * (float)z;

	if (move_target)
		pt_view = pt_view + dir;

	origin = origin + dir;
	Init();
}

// TODO dir
// D�place le point de vue de la cam�ra d'un certain angle
// dir : axe(s) de rotation
// rot : angle de rotation en degr�
void Camera::Rotate(const Vector& pt_screen)
{
	pt_view = Point(pt_screen.x, pt_screen.y, pt_screen.z);

	Init();
}