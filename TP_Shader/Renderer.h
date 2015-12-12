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
	bool calledPrecalc = false;
	Camera cam;
	Terrain * terrain;
	Film film;
	ColorRGB ambiant = ColorRGB{ 00.F, 00.f, 0.f };
	SamplerPoisson samplerPoisson;

public:
	bool abort;
	Renderer(QObject *parent = 0);
	void render();
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

	~Renderer();

signals:
	void renderedImage(const QImage &image);

protected:
	bool changes;
	void run();

};

