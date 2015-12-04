#include "Camera.h"

Camera::Camera(Point o_, Point a_, double dw, Vector up)
{
	o = o_;

	w = normalize(a_ - o_);
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

	Vector res = Vector(o.x, o.y, o.z) + (w * lw)
		+ u * (-lu * (1 - tu) + tu * lu)
		+ v * (-lv * (1 - tv) + tv * lv);

	return res;
}