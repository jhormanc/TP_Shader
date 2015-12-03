#include "Renderer.h"

Renderer::Renderer() :c(Camera(Point(50.f, 52.f, 300.6f), normalize(Vector(0.f, -0.042612f, -1.f)), Vector(0, 1, 0), 768, 768, 1.f, 300.f, 64.5)),
f(Film(768, 768, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), s(Scene())
{
}

ColorRGB Renderer::radiance(Ray r)
{
	float t;
	Shapes * obj = nullptr;
	if (s.getBound().intersect(r, nullptr, nullptr))
	{
		if ((obj = s.intersect(r, t)) != nullptr)
		{
			Point p = r.o + (r.d * t);
		
			return shade ( p, obj->getNormal ( p ), r.o, obj->getColor ( p ) ).cclamp ( 0.f, 255.f );
		}
	}
	return ColorRGB{ 0.f, 0.f, 0.f };
}

ColorRGB Renderer::shade(Point p, Normals n, Point eye, ColorRGB color)
{
	return ambiant + (color * clamp(dot(n, normalize(s.light - p)), 0.f, 1.f) + color *  std::pow(clamp(dot(reflect(normalize(s.light - p), n), normalize(eye - p)), 0.f, 1.f), 40)) * V(p, s.light);
}

float Renderer::V(Point collide, Point l)
{
	const float epsilon = 0.1f;
	float t;
	Vector lightVec = normalize(l - collide);
	Ray lightRay = Ray(l, -lightVec);
	Shapes * obj;
	if ((obj = s.intersect(lightRay, t)) != nullptr)
	{
		float distance1 = distance(lightRay.o, lightRay.o + lightRay.d * t);

		float distance2 = distance(lightRay.o, collide);
		if (!(distance1 + epsilon < distance2))
			return 1.f;
	}
	return 0.f;
}

void Renderer::render()
{
	int h = f.yResolution;
	int w = f.xResolution;

	#pragma omp parallel for
	for (int y = 0; y < h; y++)
	{
		//std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
		for (int x = 0; x < w; x++)
		{
			
			f.colors[x][y] = radiance(c.getRay(x, y));
		}
	}
	f.writePpm();

}


Renderer::~Renderer()
{
}
