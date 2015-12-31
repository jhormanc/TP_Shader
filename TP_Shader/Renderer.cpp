#include "Renderer.h"

bool Renderer::renderPrecalculed(false);
int Renderer::nbSamples(nbEchantillon);
float Renderer::coefDiffus(1.f);
float Renderer::coefSpec(1.f);
int Renderer::specInfluence(40);
int Renderer::sunInfluence(4);
float Renderer::sunIntensity(0.8f);
float Renderer::globalIntensity(0.2f);
Point Renderer::sunPoint(windowWidth * 0.5f, windowHeight * 0.5f, 1000.f);
float Renderer::rDelta(r_delta);
bool Renderer::renderGrey(false);
bool Renderer::renderNbIter(false);
bool Renderer::refreshAuto(false);

Renderer::Renderer(QObject *parent) : QThread(parent), cam(Point(-20.f, terrainHeight * 0.5f, 1000.f), Point(terrainWidth * 0.5f, terrainHeight * 0.5f, 100.f), 1., Vector(0.f, 0.f, -1.f)),
film(Film(windowWidth, windowHeight, "test.ppm", ColorRGB{ 0.0f, 0.0f, 0.0f })), samplerPoisson(BBox(Point(0.f, 0.f, 0.f), Point(terrainWidth, terrainHeight, 1000.f)), 1.f), terrain(new TerrainFractal(terrainWidth, terrainHeight, stepsTerrain))
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

ColorRGB Renderer::radiance(Ray r, float &z)
{
	ColorRGB acc = ColorRGB{ 0.f, 0.f, 0.f };
	float accli = 0.f;
	int nbIter = 0;
	float t;

	if (terrain->intersect(r, &t, &nbIter))
	{
		Point p(r.o + r.d * t);

		// Fix trou noir
		Pixel pt = terrain->getPoint(p.x, p.y);
		z = Point::distance(r.o, pt);
			
		ColorRGB shading = shade(pt, terrain->getNormal(pt), r.o, sunPoint);
		ColorRGB colorTerrain = terrain->getColor(pt) / 255.f;

		#pragma omp parallel for schedule(static)
		for (int i = 0; i < nbSamples; ++i)
		{
			Point l = samplerPoisson.next();

			float cosLiS = dot(normalize(l - Point(0)), normalize(sunPoint - Point(0)));
			
			float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
			accli += li;
			acc = acc + colorTerrain * delta(pt, l, rDelta) * li;
		}

		if (!renderNbIter)
			return ((shading / 255.f).cclamp(0.f, 255.f) * (acc / (accli)).cclamp(0.f, 1.f)) * 255.f;
	}
	else
		z = distMax;

	if (!renderNbIter)
		return sky;

	nbIter = clamp(nbIter, 0.f, 255.f);
	return ColorRGB{ nbIter, nbIter, nbIter };
}

ColorRGB Renderer::radiance(Pixel p, Point o)
{
	ColorRGB acc = ColorRGB{ 0.f, 0.f, 0.f };
	float accli = 0.f;
	ColorRGB shading = shade(p, terrain->getNormal(p), o, sunPoint);
	ColorRGB colorTerrain = terrain->getColor(p) / 255.f;
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < nbSamples; ++i)
	{
		Point l = samplerPoisson.next(); // 2 eme param a enlever
		float cosLiS = std::abs(dot(normalize(l - Point(0)), normalize(sunPoint - Point(0))));
		float li = globalIntensity + sunIntensity * std::pow(cosLiS, sunInfluence);
		accli += li;

		acc = acc + colorTerrain * delta(p, l, rDelta) * li;
	}

	return ((shading / 255.f).cclamp(0.f, 255.f) * (acc / (accli)).cclamp(0.f, 1.f)) * 255.f;
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

void Renderer::postprocess_lightning ( const float &z, ColorRGB &c ) {
	float t = z / distMax;
	ColorRGB c2 = orange;
	c = c * ( 1 - t ) + c2 * t;
}


void Renderer::postprocess_shadowing ( const float &z, ColorRGB &c ) {
	float t = z / distMax;
	ColorRGB c2 = c  * shadowFactor;
	c = c * ( 1 - t ) + c2 * t;
}

void Renderer::postprocess_fog ( const float &z, ColorRGB &c ) 
{
	float t = exp ( -z / ( distMax * fogFactor ) );
	ColorRGB c2 = grey_light;
	c = c2 * ( 1 - t ) + c * t;
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
			
			#pragma omp parallel for schedule(static)
			for (int x = 0; x < w; x++)
			{
				//	std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";
				for (int y = 0; y < h; y++)
				{
					Vector cam_dir = normalize(camera.PtScreen(x, y, w, h) - cam_vec);
					Ray r = Ray(cam_pt, cam_dir);
					float z;
					ColorRGB c = p ? radiancePrecalculed(r, z) : radiance(r, z);
					//postprocess_lightning ( z, c );
					//postprocess_shadowing ( z, c );
					//postprocess_fog ( z, c );

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