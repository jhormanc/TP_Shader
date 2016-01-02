#pragma once
#include <iostream>
#include "Camera.h"
#include "SamplerPoisson.h"
#include <QSize>
#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <QImage>
#include <chrono>
#include "Terrain\Terrain.h"
#include "Terrain\TerrainFractal.h"
#include "Film.h"

class Renderer : public QThread
{
	Q_OBJECT

private:
	QMutex mutex;
	QWaitCondition condition;
	bool restart;
	Camera cam;
	Terrain * terrain;
	Film film;
	ColorRGB ambiant = ColorRGB { 0.F, 0.f, 0.f };
	SamplerPoisson samplerPoisson;
	float lastRenderTime;

public:
	bool calledPrecalc = false;
	bool changes; // Pour savoir quand il y a une modif (caméra, rendu, nb samples)
	bool precalculed; // Rendu précalculé ou pas
	bool abort; // Exit
	Renderer(QObject *parent = 0);
	void render();
	ColorRGB radiance(Ray r, float &z, int *nbIter);
	ColorRGB radiance(Pixel p, Point o);
	float V(Point collide, Point l);
	float delta(Point collide, Point l, float r);
	ColorRGB shade(Pixel p, Normals n, Point eye, Point l);
	void precalc();
	
	ColorRGB radiancePrecalculed ( Ray r, float &z );
	void CameraRotation(float rot);

	inline Point GetCamOrigin()
	{
		return cam.getOrigin();
	}

	inline Point GetCamTarget()
	{
		return cam.getTarget();
	}

	// Déplace la caméra
	void MoveCam(const int& x = 0, const int& y = 0, const int& z = 0);

	// Rotate la caméra
	void RotateCam(const Point& pt_screen);

	bool changeNbSamples(const int& nbToAdd);
	void changeRenderMode();
	void UpdatePrecalc();
	// diffus : diffus ou spéculaire
	// coefToAdd : entre 0.f et 1.f
	bool AddCoeff(const bool& diffus, const float& coefToAdd);
	// intensityToAdd : 0.f < globalIntensity + sunIntensity < 1.f
	bool AddIntensity(const float& intensityToAdd);
	// sun : soleil ou spéculaire
	// influenceToAdd : 10 < influenceSpec < 40
	bool AddInfluence(const bool& sun, const int& influenceToAdd);

	void postprocess_shadowing(const float &z, ColorRGB &c);
	void postprocess_lightning(const float &x, const float &y, const float &z, const int &nb_iter, ColorRGB &c);
	void postprocess_fog(const float &z, ColorRGB &c);

	void MoveSun(Vector dir);
	float GetRenderTime();
	bool AddDeltaR(const float& delta);
	void ChangeRenderColor();
	void ChangeRenderIter();
	void ChangeRenderAuto();

	static bool renderPrecalculed;
	static int nbSamples;
	static float coefDiffus;
	static float coefSpec;
	static int specInfluence;
	static int sunInfluence;
	static float sunIntensity;
	static float globalIntensity;
	static Point sunPoint;
	static float rDelta;
	static bool renderGrey;
	static bool renderNbIter;
	static bool refreshAuto;

	~Renderer();

signals:
	void renderedImage(const QImage &image);

protected:
	void run();

};

