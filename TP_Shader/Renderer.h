#pragma once
#include <iostream>
#include "Camera.h"
#include "Scene\Scene.h"
#include "SamplerPoisson.h"
#include <QSize>
#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <QImage>

class Renderer : public QThread
{
	Q_OBJECT

private:
	QMutex mutex;
	QWaitCondition condition;
	bool restart;
	bool abort;
	float CameraX, CameraY, CameraZ;

	Camera cam;
	Scene scene;
	Film film;
	ColorRGB ambiant = ColorRGB{ 00.F, 00.f, 0.f };
	SamplerPoisson samplerPoisson;

public:
	Renderer(QObject *parent = 0);
	void render();
	ColorRGB radiance(Ray r);
	float V(Point collide, Point l);
	float delta(Point collide, Point l, float r);
	ColorRGB shade(Point p, Normals n, Point eye, Point l, ColorRGB color);

	void CameraRotation(float rot);

	// Déplace la caméra
	void MoveCam(const int& x = 0, const int& y = 0, const int& z = 0);

	// Rotate la caméra
	void RotateCam(const Point& pt_screen);

	~Renderer();

signals:
	void renderedImage(const QImage &image);

protected:
	bool changes;
	void run() Q_DECL_OVERRIDE;

};

