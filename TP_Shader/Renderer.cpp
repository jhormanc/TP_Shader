#include "Renderer.h"


Renderer::Renderer(QObject *parent) : QThread(parent), cam(Camera(Point(50.f, 52.f, 300.6f), normalize(Vector(0.f, -0.042612f, -1.f)), Vector(0, 1, 0), 768, 768, 1.f, 300.f, 64.5)),
film(Film(768, 768, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), scene(Scene())

{
	restart = false;
	abort = false;
}

Renderer::~Renderer()
{
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();

	wait();
}

ColorRGB Renderer::radiance(Ray r)
{
	float t;
	Shapes * obj = nullptr;
	if (scene.getBound().intersect(r, nullptr, nullptr))
	{
		if ((obj = scene.intersect(r, t)) != nullptr)
		{
			Point p = r.o + (r.d * t);
		
			return shade(p, obj->getNormal(p), r.o, obj->getColor(p)).cclamp(0.f, 255.f);
		}
	}
	return ColorRGB{ 0.f, 0.f, 0.f };
}

ColorRGB Renderer::shade(Point p, Normals n, Point eye, ColorRGB color)
{
	return ambiant + (color * clamp(dot(n, normalize(scene.light - p)), 0.f, 1.f) + color * std::pow(clamp(dot(reflect(normalize(scene.light - p), n), normalize(eye - p)), 0.f, 1.f), 40)) * V(p, scene.light);
}

float Renderer::V(Point collide, Point l)
{
	const float epsilon = 0.1f;
	float t;
	Vector lightVec = normalize(l - collide);
	Ray lightRay = Ray(l, -lightVec);
	Shapes * obj;

	if ((obj = scene.intersect(lightRay, t)) != nullptr)
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
	QMutexLocker locker(&mutex);

	if (!isRunning()) 
	{
		start(LowPriority);
	}
	else 
	{
		restart = true;
		condition.wakeOne();
	}
}

void Renderer::run()
{
	forever
	{
		int h = film.yResolution;
		int w = film.xResolution;
		QImage image(w, h, QImage::Format_RGB32);
		ColorRGB c;

		#pragma omp parallel for
		for (int y = 0; y < h; y++)
		{
			std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
			for (int x = 0; x < w; x++)
			{
				c = radiance(cam.getRay(x, y));
				image.setPixel(x, y, qRgb(c.x, c.y, c.z));
				//film.colors[x][y] = c;
			}
		}

		//film.writePpm();

		if (!restart)
			emit renderedImage(image, 1.f);

		mutex.lock();
		if (!restart)
			condition.wait(&mutex);
		restart = false;
		mutex.unlock();
	}
}


