#include "Renderer.h"

bool Renderer::renderPrecalculed(false);
int  Renderer::nbSamples(nbEchantillon);
float  Renderer::coefDiffus(1.f);
float  Renderer::coefSpec(1.f);
int  Renderer::specInfluence(40);
int  Renderer::sunInfluence(4);
float  Renderer::sunIntensity(0.8f);
float  Renderer::globalIntensity(0.2f);
Point  Renderer::sunPoint(2500.f, 2500.f, 1000.f);
float  Renderer::rDelta(r_delta);
bool  Renderer::renderGrey(false);

Renderer::Renderer(QObject *parent) : QThread(parent), cam(Point(-20.f, 2500.f, 1000.f), Point(2500.f, 2500.f, 100.f), 1., Vector(0.f, 0.f, -1.f)),
film(Film(768, 768, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), samplerPoisson(BBox(Point(0.f, 0.f, 0.f),Point(5000.f, 5000.f, 500.f)), 10.f), terrain(new TerrainFractal(5000, 5000))
{
	CameraX = -20;
	CameraY = 500;
	CameraZ = 700;
	restart = false;
	abort = false;
	changes = true;
	lastRenderTime = 0.f;
}

Renderer::~Renderer()
{
	delete terrain;
	mutex.lock();
	changes = false;
	abort = true;
	condition.wakeOne();
	mutex.unlock();

	wait();
}

ColorRGB Renderer::radiance(Ray r)
{
	ColorRGB acc = ColorRGB{ 0.f, 0.f, 0.f };
	float accli = 0.f;

	float t;
	if (terrain->intersect(r, &t))
	{
		Point p(r.o + r.d * t);

		#pragma omp parallel for schedule(static)
		for (int i = 0; i < nbSamples; ++i)
		{
			Point l = samplerPoisson.next(); // 2 eme param a enlever
			float cosLiS = std::abs(dot(normalize(l - Point(0)), normalize(sunPoint - Point(0))));
			float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
			accli += li;
			acc = acc + shade(p, terrain->getNormal(p), r.o, l, terrain->getColor(p)).cclamp(0.f, 255.f) * li;
		}
		return acc * (1.0f / accli);
	}
	return sky;
}

ColorRGB Renderer::radiance(Point p, Point o)
{
	ColorRGB acc = ColorRGB{ 0.f, 0.f, 0.f };
	float accli = 0.f;

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < nbSamples; ++i)
	{
		Point l = samplerPoisson.next(); // 2 eme param a enlever
		float cosLiS = std::abs(dot(normalize(l - Point(0)), normalize(sunPoint - Point(0))));
		float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
		accli += li;
		acc = acc + shade(p, terrain->getNormal(p), o, l, terrain->getColor(p)).cclamp(0.f, 255.f) * li;
	}
	return acc * (1.0f / accli);
}


ColorRGB Renderer::radiancePrecalculed(Ray r)
{
	float t;
	if (terrain->intersect(r, &t))
	{
		Point p(r.o + r.d * t);
		ColorRGB res = terrain->getColorPrecalculed(p);
		return res;
	}
	return sky;
}
ColorRGB Renderer::shade(Point p, Normals n, Point eye, Point l, ColorRGB color)
{
	return ambiant + color * clamp(
		(dot(normalize(l - p), n) * coefDiffus // diffus  
		+ std::pow(dot(reflect(normalize(l - p), n), normalize(eye - p)), specInfluence) * coefSpec)  // speculaire
		, 0.f, 1.f) * delta(p, l, rDelta);
}

float Renderer::delta(Point collide, Point l, float r)
{
	const float epsilon = 0.1f;
	float t;
	Vector lightVec = normalize(l - collide);
	Ray lightRay = Ray(collide + epsilon * lightVec, lightVec);

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
	qDebug("start preprocessing ");
	std::ofstream mesureFile("mesureFile.txt", std::ios::out | std::ios::app);

	//BBox sceneSize = terrain->getBound();
	int w = terrain->terrain_width;
	int h = terrain->terrain_height;
	auto start = std::chrono::high_resolution_clock::now();

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			
		/*	Vector cam_dir = normalize( - precalcCam.getOrigin());
			Ray r = Ray(precalcCam.getOrigin(), cam_dir);*/
			terrain->precalc[i][j] = radiance(terrain->getPoint(i, j), cam.getOrigin());
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	mesureFile << "preprocess : " << std::chrono::duration<float, std::milli>(end - start).count() * 0.001 << " s" << std::endl;
	
	mesureFile.close();
}

void Renderer::render()
{
	QMutexLocker locker(&mutex);
	if (!isRunning()) 
	{
		samplerPoisson.genAleatoire();
		if (renderPrecalculed)
		{
			if (!calledPrecalc)
			{
				precalc();
				calledPrecalc = true;
			}
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
	qDebug("start render ");
	std::ofstream mesureFile("mesureFile.txt", std::ios::out | std::ios::app);

	while (!abort)
	{
		if (changes)
		{
			auto start = std::chrono::high_resolution_clock::now();
			int h = film.yResolution;
			int w = film.xResolution;
			QImage image(w, h, QImage::Format_RGB32);
			ColorRGB c;

			mutex.lock();
			Camera camera(cam);
			bool p(renderPrecalculed);
			mutex.unlock();

			Point cam_pt(camera.getOrigin());
			Vector cam_vec(cam_pt.x, cam_pt.y, cam_pt.z);

			#pragma omp parallel for schedule(static)
			for (int x = 0; x < w; x++)
			{
				//	std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
				for (int y = 0; y < h; y++)
				{
					Vector cam_dir = normalize(camera.PtScreen(x, y, w, h) - cam_vec);
					Ray r = Ray(cam_pt, cam_dir);
					c = p ? radiancePrecalculed(r) : radiance(r);
					image.setPixel(x, y, qRgb(c.x, c.y, c.z));
					//film.colors[x][y] = c;
				}
			}

			emit renderedImage(image);
			auto end = std::chrono::high_resolution_clock::now();
			float sec = std::chrono::duration<float, std::milli>(end - start).count() * 0.001;
			mesureFile << "render : " << sec << " s" << std::endl;
			lastRenderTime = sec;

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
	mesureFile.close();
}

void Renderer::MoveSun(Vector dir)
{
	mutex.lock();
	sunPoint.x += dir.x;
	sunPoint.y += dir.y;
	sunPoint.z += dir.z;
	changes = true;
	mutex.unlock();
}

Point Renderer::GetSunPoint()
{
	return sunPoint;
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
	cam.Rotate(pt_screen);
	changes = true;

	mutex.unlock();
}

bool Renderer::changeNbSamples(const int& nbToAdd)
{
	if (nbToAdd > 0 || (nbSamples + nbToAdd) >= 1)
	{
		mutex.lock();
		nbSamples += nbToAdd;
		changes = true;
		mutex.unlock();
		return true;
	}
	return false;
}

void Renderer::changeRenderMode()
{
	mutex.lock();
	renderPrecalculed = !renderPrecalculed;
	changes = true;
	mutex.unlock();
}

void Renderer::UpdatePrecalc()
{
	mutex.lock();
	precalc();
	changes = true;
	mutex.unlock();
}

float Renderer::GetRenderTime()
{
	return lastRenderTime;
}

bool Renderer::AddCoeff(const bool& diffus, const float& coefToAdd)
{
	if (diffus)
	{
		if ((coefDiffus + coefToAdd) > 0.f && (coefDiffus + coefToAdd) < 1.f)
		{
			mutex.lock();
			coefDiffus += coefToAdd;
			changes = true;
			mutex.unlock();
			return true;
		}
	}
	else
	{
		if ((coefSpec + coefToAdd) > 0.f && (coefSpec + coefToAdd) < 1.f)
		{
			mutex.lock();			
			coefSpec += coefToAdd;
			changes = true;
			mutex.unlock();
			return true;
		}
	}

	return false;
}

bool Renderer::AddIntensity(const float& intensityToAdd)
{
	if ((sunIntensity + intensityToAdd) > 0.f 
		&& (sunIntensity + intensityToAdd) < 1.f 
		&& (globalIntensity - intensityToAdd) > 0.f 
		&& (globalIntensity - intensityToAdd) < 1.f)
	{
		mutex.lock();
		sunIntensity += intensityToAdd;
		globalIntensity -= intensityToAdd;
		changes = true;
		mutex.unlock();
		return true;
	}

	return false;
}

bool Renderer::AddInfluence(const bool& sun, const int& influenceToAdd)
{
	if (sun)
	{
		if ((sunInfluence + influenceToAdd) > 0)
		{
			mutex.lock();
			sunInfluence += influenceToAdd;
			changes = true;
			mutex.unlock();
			return true;
		}
	}
	else
	{
		if ((specInfluence + influenceToAdd) >= 10 && (specInfluence + influenceToAdd) <= 40)
		{
			mutex.lock();
			specInfluence += influenceToAdd;
			changes = true;
			mutex.unlock();
			return true;
		}
	}

	return false;
}

bool Renderer::AddDeltaR(const float& delta)
{
	if ((rDelta + delta) >= 0.f)
	{
		mutex.lock();
		rDelta += delta;
		changes = true;
		mutex.unlock();
		return true;
	}
	return false;
}

void Renderer::ChangeRenderColor()
{
	mutex.lock();
	renderGrey = !renderGrey;
	terrain->ChangeRenderColor(renderGrey);
	changes = true;
	mutex.unlock();
}