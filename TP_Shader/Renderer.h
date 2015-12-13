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
	
	float CameraX, CameraY, CameraZ;
	Camera cam;
	Terrain * terrain;
	Film film;
	ColorRGB ambiant = ColorRGB{ 00.F, 00.f, 0.f };
	SamplerPoisson samplerPoisson;
	float lastRenderTime;

public:
	bool calledPrecalc = false;
	bool changes; // Pour savoir quand il y a une modif (cam�ra, rendu, nb samples)
	bool precalculed; // Rendu pr�calcul� ou pas
	bool abort; // Exit
	Renderer(QObject *parent = 0);
	void render();
	ColorRGB radiance(Ray r);
	ColorRGB radiance(Point p, Point o);
	float V(Point collide, Point l);
	float delta(Point collide, Point l, float r);
	ColorRGB shade(Point p, Normals n, Point eye, Point l, ColorRGB color);
	void precalc();
	
	ColorRGB radiancePrecalculed(Ray r);
	void CameraRotation(float rot);

	// D�place la cam�ra
	void MoveCam(const int& x = 0, const int& y = 0, const int& z = 0);

	// Rotate la cam�ra
	void RotateCam(const Point& pt_screen);

	bool changeNbSamples(const int& nbToAdd);
	void changeRenderMode();
	void UpdatePrecalc();
	// diffus : diffus ou sp�culaire
	// coefToAdd : entre 0.f et 1.f
	bool AddCoeff(const bool& diffus, const float& coefToAdd);
	// intensityToAdd : 0.f < globalIntensity + sunIntensity < 1.f
	bool AddIntensity(const float& intensityToAdd);
	// sun : soleil ou sp�culaire
	// influenceToAdd : 10 < influenceSpec < 40
	bool AddInfluence(const bool& sun, const int& influenceToAdd);


	Point GetSunPoint();
	void MoveSun(Vector dir);
	float GetRenderTime();
	bool AddDeltaR(const float& delta);
	void ChangeRenderColor();

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

	~Renderer();

signals:
	void renderedImage(const QImage &image);

protected:
	void run();

};

