#pragma once
#include <iostream>
#include "Camera.h"
#include "Scene\Scene.h"
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

	Camera cam;
	Scene scene;
	Film film;
	ColorRGB ambiant = ColorRGB{ 100.F, 100.f, 100.f };

public:
	Renderer(QObject *parent = 0);
	void render();
	ColorRGB radiance(Ray r);
	float V(Point collide, Point l);
	ColorRGB shade(Point p, Normals n, Point eye, ColorRGB color);
	~Renderer();

signals:
	void renderedImage(const QImage &image, float scaleFactor);

protected:
	void run() Q_DECL_OVERRIDE;

};

