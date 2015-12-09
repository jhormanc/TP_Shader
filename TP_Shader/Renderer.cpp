#include "Renderer.h"

Renderer::Renderer(QObject *parent) : QThread(parent), cam(Point(-20., 500., 700.), Point(500., 500., 0.), 1., Vector(0., 0., -1.)),
film(Film(768, 768, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), samplerPoisson(BBox(Point(0.f, 0.f, 0.f),Point(1000.f, 1000.f, 500.f)), 10), scene(Scene())
{
	CameraX = -20;
	CameraY = 500;
	CameraZ = 700;
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
			int n = 0;
			ColorRGB acc = ColorRGB{ 0, 0, 0 };
			float accli = 0.f;
			Point sunshine(500, 500, 500); // midi
			for (int i = 0; i < nbEchantillon; ++i)
			{
				Light l = { samplerPoisson.next(), 1 }; // 2 eme param a enlever
				float cosLiS = std::abs(dot(normalize(l.o - Point(0)), normalize(sunshine - Point(0))));
				float li = 0.2f + 0.8f * cosLiS * cosLiS * cosLiS * cosLiS;
			//	 li = 1.f;
				const float epsilon = 0.1f;
				accli += li;
				acc = acc + shade(p, obj->getNormal(p), r.o, l.o, obj->getColor(p)).cclamp(0.f, 255.f) * li;
			}
			return acc * (1.0f / accli);
		}
	}
	return ColorRGB{ 0.f, 0.f, 0.f };
}

ColorRGB Renderer::shade(Point p, Normals n, Point eye, Point l, ColorRGB color)
{
	return ambiant + (color * clamp(dot(normalize(l - p), n), 0.f, 1.f) + color * std::pow(clamp(dot(reflect(normalize(l - p), n), normalize(eye - p)), 0.f, 1.f), 40)) * delta(p, l, r_delta);
}

float Renderer::delta(Point collide, Point l, float r)
{
	const float epsilon = 0.1f;
	float t;
	Vector lightVec = normalize(l - collide);
	Ray lightRay = Ray(collide + epsilon * lightVec, lightVec);
	Shapes * obj;

	if ((obj = scene.intersectSegment(lightRay, t, r)) != nullptr)
	{
		return 0.f;
	}
	return 1.f;
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
		start(HighestPriority);
	}
	else 
	{
		restart = true;
		condition.wakeOne();
	}
}

void Renderer::run()
{
	if (!abort)
	{
		samplerPoisson.genAleatoire();
		int h = film.yResolution;
		int w = film.xResolution;
		QImage image(w, h, QImage::Format_RGB32);
		ColorRGB c;

		mutex.lock();
		Point cam_pt(cam.getOrigin());
		Vector cam_vec(cam_pt.x, cam_pt.y, cam_pt.z);


		#pragma omp parallel for schedule(static)
		for (int y = 0; y < h; y++)
		{
		//	std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
			for (int x = 0; x < w; x++)
			{
				Vector cam_dir = normalize(cam.PtScreen(x, y, w, h) - cam_vec);
				Ray r = Ray(cam_pt, cam_dir);
				c = radiance(r);
				image.setPixel(x, y, qRgb(c.x, c.y, c.z));
				//film.colors[x][y] = c;
			}
		}

		//film.writePpm();
		mutex.unlock();
		if (!restart)
			emit renderedImage(image, 1.f);


		mutex.lock();
		if (!restart)
			condition.wait(&mutex);
		restart = false;
		mutex.unlock();
	}
}

void Renderer::MoveCam(const int& x, const int& y, const int& z)
{
	mutex.lock();
	cam.Move(true, x, y, z);
	mutex.unlock();
}

void Renderer::RotateCam(const Point& pt)
{
	mutex.lock();

	Vector pt_screen = cam.PtScreen(pt.x, pt.y, film.xResolution, film.yResolution);
	Point org = cam.getOrigin();
	Vector dir = normalize(pt_screen - Vector(org.x, org.y, org.z));
	float rot = dot(dir, cam.Forward());

	cam.Rotate(dir, rot);

	mutex.unlock();
}
