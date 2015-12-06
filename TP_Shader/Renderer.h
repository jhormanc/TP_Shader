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
	float CameraX, CameraY, CameraZ;

	Camera cam;
	Scene scene;
	Film film;
	ColorRGB ambiant = ColorRGB{ 00.F, 00.f, 0.f };

public:
	Renderer(QObject *parent = 0);
	void render();
	ColorRGB radiance(Ray r);
	float V(Point collide, Point l);
	ColorRGB shade(Point p, Normals n, Point eye, Point l, ColorRGB color);

	void CameraUp(float up){
		CameraZ += up;
	}
	void CameraRotation(float rot);

	~Renderer();

signals:
	void renderedImage(const QImage &image, float scaleFactor);

protected:
	void run() Q_DECL_OVERRIDE;

};

