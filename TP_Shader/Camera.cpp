#include "Camera.h"

Camera::Camera(Point o_, Point a_, float dw, Vector _up, float FoV)
{
	origin = o_;
	pt_view = a_;
	up = _up;

	Init();

	lw = dw;

	SetFov(FoV);
	
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

void Camera::SetFov(float FoV)
{
	Vector tmp = (w * lw);
	float c = tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
	lu = std::sqrt((-cos(FoV) + c) / (u.x * u.x + u.y * u.y + u.z * u.z));
	lv = lu * (float)(windowHeight / (float)windowWidth);
}

void Camera::Init()
{
	w = normalize(pt_view - origin);
	u = normalize(-cross(w, up));
	v = cross(w, u);
	u = -u;
}

// Déplace l'origine dans la direction de la caméra
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
// Déplace le point de vue de la caméra d'un certain angle
// dir : axe(s) de rotation
// rot : angle de rotation en degré
void Camera::Rotate(const Vector& pt_screen)
{
	pt_view = Point(pt_screen.x, pt_screen.y, pt_screen.z);

	Init();
}