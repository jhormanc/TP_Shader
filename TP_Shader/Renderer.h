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
	bool changes; // Pour savoir quand il y a une modif (caméra, rendu, nb samples)
	bool precalculed; // Rendu précalculé ou pas
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

	// Déplace la caméra
	void MoveCam(const int& x = 0, const int& y = 0, const int& z = 0);

	// Rotate la caméra
	void RotateCam(const Point& pt_screen);

	bool changeNbSamples(const int& nbToAdd);
	void changeRenderMode();
	bool IsRenderPrecalc();
	int GetNbSamples();
	void UpdatePrecalc();
	float GetRenderTime();
	// diffus : diffus ou spéculaire
	// coefToAdd : entre 0.f et 1.f
	void AddCoeff(const bool& diffus, const float& coefToAdd);
	// intensityToAdd : 0.f < globalIntensity + sunIntensity < 1.f
	void AddIntensity(const float& intensityToAdd);
	// sun : soleil ou spéculaire
	// influenceToAdd : 10 < influenceSpec < 40
	void AddInfluence(const bool& sun, const int& influenceToAdd);

	float GetIntensity(const bool& sun);
	float GetCoeff(const bool& diffus);
	int GetInfluence(const bool& sun);

	~Renderer();

signals:
	void renderedImage(const QImage &image);

protected:
	void run();

};

