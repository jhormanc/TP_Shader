#include "Camera.h"

Camera::Camera(Point o_, Point a_, double dw, Vector _up)
{
	origin = o_;
	pt_view = a_;
	up = _up;

	w = normalize(pt_view - origin);
	u = normalize(-cross(w, up));
	v = cross(w, u);
	u = -u;

	lu = 4. / 3.;
	lv = 1.;
	lw = dw;
}

Vector Camera::PtScreen(int i, int j, int width, int height)
{
	double tu = (double)i / (double)(width - 1);
	double tv = (double)j / (double)(height - 1);

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

Point Camera::getOrigin()
{
	return origin;
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
void Camera::Rotate(const Vector& dir, const float& rot)
{
	// origin is the origin of the rotation
	// pt_view is the point we want to rotate
	pt_view.x = cos(rot) * (pt_view.x - origin.x) - sin(rot) * (pt_view.y - origin.y) + origin.x;
	pt_view.y = sin(rot) * (pt_view.x - origin.x) + cos(rot) * (pt_view.y - origin.y) + origin.y;

	Init();
}

Vector Camera::Forward() const
{
	return u;
}