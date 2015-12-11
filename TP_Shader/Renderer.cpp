#include "Renderer.h"

Renderer::Renderer(QObject *parent) : QThread(parent), cam(Point(-20., 2500., 1000.), Point(2500., 2500., 100.), 1., Vector(0., 0., -1.)),
film(Film(768, 768, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), samplerPoisson(BBox(Point(0.f, 0.f, 0.f),Point(5000.f, 5000.f, 500.f)), 10), terrain(new TerrainFractal(5000, 5000))
{
	CameraX = -20;
	CameraY = 500;
	CameraZ = 700;
	restart = false;
	abort = false;
	changes = true;
}

Renderer::~Renderer()
{
	mutex.lock();
	changes = false;
	abort = true;
	condition.wakeOne();
	mutex.unlock();

	wait();
}

ColorRGB Renderer::radiance(Point p, Point o)
{
	float t;
	//if (terrain->getBound().intersect(r, nullptr, nullptr))
	//{
	//	if (terrain->intersect(r, &t))
	//	{
			//Point p = r.o + (r.d * t);
			int n = 0;
			ColorRGB acc = ColorRGB{ 0, 0, 0 };
			float accli = 0.f;
			Point sunshine(2500, 2500, 1000); // midi
			for (int i = 0; i < nbEchantillon; ++i)
			{
				Light l = { samplerPoisson.next(), 1 }; // 2 eme param a enlever
				float cosLiS = std::abs(dot(normalize(l.o - Point(0)), normalize(sunshine - Point(0))));
				float li = 0.2f + 0.8f * cosLiS * cosLiS * cosLiS * cosLiS;
			//	 li = 1.f;
				const float epsilon = 0.1f;
				accli += li;
				acc = acc + shade(p, terrain->getNormal(p), o, l.o, terrain->getColor(p)).cclamp(0.f, 255.f) * li;
			}
			return acc * (1.0f / accli);
	//	}
	//}
	//return ColorRGB{ 0.f, 0.f, 0.f };
}


ColorRGB Renderer::radiancePrecalculed(Ray r)
{
	float t;
	Shapes * obj = nullptr;
	if (terrain->getBound().intersect(r, nullptr, nullptr))
	{
		if (terrain->intersect(r, &t))
		{
			Point p(r.o + r.d * t);
			//qDebug(" p : %f, %f, %f", p.x, p.y, p.z);
			ColorRGB res =  terrain->getColorPrecalculed(p);
			//qDebug(" res : %f, %f, %f", res.x, res.y, res.z);
			return res;
		}
	}
	return ColorRGB{ 0.f, 0.f, 0.f };
}
ColorRGB Renderer::shade(Point p, Normals n, Point eye, Point l, ColorRGB color)
{
	return ambiant + color * clamp(
		(dot(normalize(l - p), n) // diffus  
		+ std::pow(dot(reflect(normalize(l - p), n), normalize(eye - p)), 40)) // speculaire
		, 0.f, 1.f);
}

float Renderer::delta(Point collide, Point l, float r)
{
	const float epsilon = 0.1f;
	float t;
	Vector lightVec = normalize(l - collide);
	Ray lightRay = Ray(collide + epsilon * lightVec, lightVec);
	Shapes * obj;

	if (terrain->intersectSegment(lightRay, &t, r))
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

	if (terrain->intersect(lightRay, &t))
	{
		float distance1 = distance(lightRay.o, lightRay.o + lightRay.d * t);

		float distance2 = distance(lightRay.o, collide);
		if (!(distance1 + epsilon < distance2))
			return 1.f;
	}
	return 0.f;
}

void Renderer::precalc()
{
	Camera precalcCam(Point(2500., 2500., 1000.), Point(2500., 2500., 0.), 1., Vector(0., 0., -1.));
	//BBox sceneSize = terrain->getBound();
	int w = terrain->terrain_width;
	int h = terrain->terrain_height;
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
		/*	Vector cam_dir = normalize( - precalcCam.getOrigin());
			Ray r = Ray(precalcCam.getOrigin(), cam_dir);*/
			terrain->precalc[i][j] = radiance(terrain->getPoint(i, j), precalcCam.getOrigin());
		}
	}
}

void Renderer::render()
{
	
	QMutexLocker locker(&mutex);
	if (!isRunning()) 
	{
		samplerPoisson.genAleatoire();
		if (!calledPrecalc)
		{
			precalc();
			calledPrecalc = true;
		}
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
	forever
	{
		while (!abort)
		{
			if (changes)
			{
				
				int h = film.yResolution;
				int w = film.xResolution;
				QImage image(w, h, QImage::Format_RGB32);
				ColorRGB c;

				mutex.lock();
				Camera camera(cam);
				mutex.unlock();

				Point cam_pt(camera.getOrigin());
				Vector cam_vec(cam_pt.x, cam_pt.y, cam_pt.z);

				#pragma omp parallel for schedule(static)
				for (int y = 0; y < h; y++)
				{
					//	std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
					for (int x = 0; x < w; x++)
					{
						Vector cam_dir = normalize(camera.PtScreen(x, y, w, h) - cam_vec);
						Ray r = Ray(cam_pt, cam_dir);
						c = radiancePrecalculed(r);
						image.setPixel(x, y, qRgb(c.x, c.y, c.z));
						//film.colors[x][y] = c;
					}
				}

				//film.writePpm();

				if (!restart)
					emit renderedImage(image);

				mutex.lock();

				if (!restart)
				{
					changes = false;
					condition.wait(&mutex);
				}
				restart = false;

				mutex.unlock();
			}
		}
	}
}

void Renderer::MoveCam(const int& x, const int& y, const int& z)
{
	mutex.lock();

	cam.Move(true, x, y, z);
	changes = true;

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
	changes = true;

	mutex.unlock();
}
