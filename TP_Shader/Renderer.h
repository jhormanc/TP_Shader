#pragma once
#include <iostream>
#include "Camera.h"
#include "Scene\Scene.h"

class Renderer
{
	Camera c;
	Scene s;
	Film f;
	ColorRGB ambiant = ColorRGB{ 100.F, 100.f, 100.f };
public:
	Renderer();
	void render();
	ColorRGB radiance(Ray r);
	float V(Point collide, Point l);
	ColorRGB shade(Point p, Normals n, Point eye, ColorRGB color);
	~Renderer();
};

