#include "Renderer.h"

bool Renderer::renderPrecalculed(false);
int Renderer::nbSamples(nbEchantillon);
float Renderer::coefDiffus(1.f);
float Renderer::coefSpec(0.5f);
int Renderer::specInfluence(40);
int Renderer::sunInfluence(4);
float Renderer::sunIntensity(0.f);
float Renderer::globalIntensity(1.f); 
Point Renderer::sunPoint(11000.f, -500.f, 500.f); //terrainWidth * 0.5f, terrainHeight * 0.5f, 1000.f
float Renderer::rDelta(r_delta);
bool Renderer::renderGrey(false);
bool Renderer::renderNbIter(false);
bool Renderer::refreshAuto(false);

Renderer::Renderer(QObject *parent) : QThread(parent),
cam(camOrigin, camTarget, 1., Vector(0.f, 0.f, -1.f), 45.f),
	film(Film(windowWidth, windowHeight, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), 
	samplerPoisson(BBox(Point(0.f, 0.f, 0.f), Point(terrainWidth, terrainHeight, 1000.f)), 1.f), 
	terrain(new TerrainFractal(terrainWidth, terrainHeight, stepsTerrain))
{
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

ColorRGB Renderer::radiance(Ray r, float &z, int *nbIter)
{
	float acc = 0.f;
	float accli = 0.f;
	*nbIter = 0;
	float t;

	if (terrain->intersect(r, &t, nbIter))
	{
		Point p(r.o + r.d * t);

		// Fix trou noir
		Pixel pt = terrain->getPoint(p.x, p.y);
		z = Point::distance(r.o, pt);

		#pragma omp parallel for schedule(static)
		for (int i = 0; i < nbSamples; ++i)
		{
			Point l = samplerPoisson.next();

			float cosLiS = dot(normalize(l - Point(0)), normalize(sunPoint - Point(0)));
			
			float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
			accli += li;
			acc = acc + delta(pt, l, rDelta) * li;
		}

		if (!renderNbIter)
			return shade(pt, terrain->getNormal(pt), r.o, sunPoint).cclamp(0.f, 255.f) * clamp(acc / accli, 0.f, 1.f);
	}
	else
		z = distMax;

	if (!renderNbIter)
		return sky;

	float n = clamp(*nbIter, 0.f, 255.f);
	return ColorRGB{ n, n, n };
}

ColorRGB Renderer::radiance(Pixel p, Point o)
{
	float acc = 0.f;
	float accli = 0.f;
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < nbSamples; ++i)
	{
		Point l = samplerPoisson.next(); // 2 eme param a enlever
		float cosLiS = std::abs(dot(normalize(l - Point(0)), normalize(sunPoint - Point(0))));
		float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
		accli += li;

		acc = acc + delta(p, l, rDelta) * li;
	}
	return shade(p, terrain->getNormal(p), o, sunPoint).cclamp(0.f, 255.f) * clamp(acc / (accli), 0.f, 1.f);
}


ColorRGB Renderer::radiancePrecalculed(Ray r, float &z)
{
	float t;
	int nbIter = 0;

	if (terrain->intersect(r, &t, &nbIter))
	{
		Point p(r.o + r.d * t);
		z = Point::distance(r.o, p);
		ColorRGB res = terrain->getColorPrecalculed(p);
		if(!renderNbIter)
			return res;
	}
	else
		z = distMax;

	if (!renderNbIter)
		return sky;

	nbIter = clamp(nbIter, 0, 255);
	return ColorRGB{ nbIter, nbIter, nbIter };
}

ColorRGB Renderer::shade(Pixel p, Normals n, Point eye, Point l)
{
	return ambiant + terrain->getColor(p) * clamp(
		(dot(normalize(l - p), n) * coefDiffus // diffus  
		+ std::pow(dot(reflect(normalize(l - p), n), normalize(eye - p)), specInfluence) * coefSpec)  // speculaire
		, 0.f, 1.f);
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

	if (terrain->intersect(lightRay, &t, nullptr))
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
	int w = terrain->getPointsWidth();
	int h = terrain->getPointsHeight();
	int steps = terrain->getSteps();
	auto start = std::chrono::high_resolution_clock::now();

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			
		/*	Vector cam_dir = normalize( - precalcCam.getOrigin());
			Ray r = Ray(precalcCam.getOrigin(), cam_dir);*/
			terrain->precalc[i][j] = radiance(terrain->getPoint((float)(i * steps), (float)(j * steps)), cam.getOrigin());
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

void Renderer::postprocess_lightning(const float &x, const float &y, const float &z, const int &nb_iter, ColorRGB &c, Sphere &sun, const float &invDistMax, const Vector &cam_pt, const Vector &dir)
{
	float t = z / distMax;

	Vector w = Noise::warp(Vector(x, y, 0.f), 1.f, 1.f / 500.f, false);
	float u = Noise::simplex(w.x / 500.f, w.y / 500.f)
		+ Noise::simplex(w.x / 1000.f, w.y / 1000.f)
		+ Noise::simplex(w.x / 2000.f, w.y / 2000.f)
		+ Noise::simplex(w.x / 5000.f, w.y / 5000.f);
	u = (u + 4.f) * 0.125f;
	ColorRGB c2 = ColorRGB(red) * u + ColorRGB(sunset_bright) * (1.f - u);

	float r = Noise::simplex(w.x / 500.f, w.y / 500.f)
		+ Noise::simplex(w.x / 100.f, w.y / 100.f) 
		+ Noise::simplex(w.x / 500.f, w.y / 500.f)
		+ Noise::simplex(w.x / 700.f, w.y / 700.f);
	r = (r + 4.f) * 0.125f;

	ColorRGB sun_color = ColorRGB(sun_yellow) * r + ColorRGB(sun_bright) * (1.f - r);

	Vector pt = cam_pt + dir * distMax;
	Ray ray = Ray(Point(cam_pt.x, cam_pt.y, cam_pt.z), dir);
	float hit;
	float dist = sun.distanceToPoint(Point(pt.x, pt.y, pt.z));
	bool sun_intersect = sun.intersect(ray, &hit) && z == distMax;
	float s = std::max(0.f, dist * invDistMax);
	ColorRGB c3;

	if (sun_intersect)
		c3 = sun_color;// *distance(sunPoint, pt);
	else
		c3 = ColorRGB(c2) * s + ColorRGB(orange) * (1.f - s);

	float v = clamp(nb_iter / 255.f, 0.f, 1.f);
	ColorRGB c4;

	if (sun_intersect)
		c4 = c3;
	else if (z == distMax)
		c4 = ColorRGB(sunset) * v + c3 * (1.f - v);
	else
		c4 = c2;

	c = c * (1 - t) + c4 * t;
}

void Renderer::postprocess_shadowing(const float &z, ColorRGB &c) 
{
	float t = z / distMax;

	ColorRGB c2 = c  * shadowFactor;
	c = c * (1 - t) + c2 * t;
}

void Renderer::postprocess_fog(const float &z, ColorRGB &c) 
{
	float t = exp(-z / (distMax * fogFactor));
	ColorRGB c2 = grey_light;
	c = c2 * (1 - t) + c * t;
}

void Renderer::run()
{
	qDebug("start render ");
	std::ofstream mesureFile("mesureFile.txt", std::ios::out | std::ios::app);

	while (!abort)
	{
		if (changes || refreshAuto) // changes
		{
			auto start = std::chrono::high_resolution_clock::now();
			int h = film.yResolution;
			int w = film.xResolution;
			QImage image(w, h, QImage::Format_RGB32);

			mutex.lock();
			Camera camera(cam);
			bool p(renderPrecalculed);

			Point cam_pt(camera.getOrigin());
			Vector cam_vec(cam_pt.x, cam_pt.y, cam_pt.z);
			Sphere sun(250.f, sunPoint);
			Vector pt = cam_vec + normalize(camera.PtScreen(0, 0, w, h) - cam_vec) * distMax;
			float invDistMax = 1.f / sun.distanceToPoint(Point(pt.x, pt.y, pt.z));

			#pragma omp parallel for schedule(static)
			for (int x = 0; x < w; x++)
			{
				//	std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
				for (int y = 0; y < h; y++)
				{
					int nbIter;
					Vector cam_dir = normalize(camera.PtScreen(x, y, w, h) - cam_vec);
					Ray r = Ray(cam_pt, cam_dir);
					float z;

					ColorRGB c = p ? radiancePrecalculed(r, z) : radiance(r, z, &nbIter);

					postprocess_lightning((float)x, (float)y, z, nbIter, c, sun, invDistMax, cam_vec, cam_dir);
					//postprocess_shadowing(z, c);
					//postprocess_fog(z, c);

					image.setPixel(x, y, qRgb(c.x, c.y, c.z));
					//film.colors[x][y] = c;
				}
			}

			mutex.unlock();

			emit renderedImage(image);
			auto end = std::chrono::high_resolution_clock::now();
			float sec = std::chrono::duration<float, std::milli>(end - start).count() * 0.001;
			mesureFile << "render : " << sec << " s" << std::endl;
			lastRenderTime = sec;

			if (!refreshAuto)
			{
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
	if (nbSamples > 1 || nbToAdd > 0)
	{
		mutex.lock();
		nbSamples += nbToAdd;
		if (nbSamples < 1)
			nbSamples = 1;
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
		if ((coefDiffus > 0.f && coefToAdd < 0.f) || (coefDiffus < 1.f && coefToAdd > 0.f))
		{
			mutex.lock();
			coefDiffus = clamp(coefDiffus + coefToAdd, 0.f, 1.f);
			changes = true;
			mutex.unlock();
			return true;
		}
	}
	else
	{
		if ((coefSpec > 0.f && coefToAdd < 0.f) || (coefSpec < 1.f && coefToAdd > 0.f))
		{
			mutex.lock();			
			coefSpec = clamp(coefSpec + coefToAdd, 0.f, 1.f);
			changes = true;
			mutex.unlock();
			return true;
		}
	}

	return false;
}

bool Renderer::AddIntensity(const float& intensityToAdd)
{
	if ((sunIntensity > 0.f && intensityToAdd < 0.f) || (sunIntensity < 1.f && intensityToAdd > 0.f))
	{
		mutex.lock();
		sunIntensity = clamp(sunIntensity + intensityToAdd, 0.f, 1.f);
		globalIntensity = clamp(globalIntensity - intensityToAdd, 0.f, 1.f);
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
		if (sunInfluence > 0 || influenceToAdd > 0)
		{
			mutex.lock();
			sunInfluence += influenceToAdd;
			if (sunInfluence < 0)
				sunInfluence = 0;
			changes = true;
			mutex.unlock();
			return true;
		}
	}
	else
	{
		if ((specInfluence > 10 && influenceToAdd < 0) || (specInfluence < 40 && influenceToAdd > 0))
		{
			mutex.lock();
			specInfluence = (int)clamp((float)(specInfluence + influenceToAdd), 10.f, 40.f);
			changes = true;
			mutex.unlock();
			return true;
		}
	}

	return false;
}

bool Renderer::AddDeltaR(const float& delta)
{
	if (rDelta > 0.f || delta > 0.f)
	{
		mutex.lock();
		rDelta += delta;
		if (rDelta < 0.f)
			rDelta = 0.f;
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

void Renderer::ChangeRenderIter()
{
	mutex.lock();
	renderNbIter = !renderNbIter;
	changes = true;
	mutex.unlock();
}

void Renderer::ChangeRenderAuto()
{
	mutex.lock();
	refreshAuto = !refreshAuto;
	changes = true;
	mutex.unlock();
}