#pragma once
#include<vector>
#include "Geometry\BBox.h"
#include "Shapes\Sphere.h"
#include <random>
class SamplerPoisson
{
	std::vector<Sphere> randArray;
	BBox size;
	const int maxSample = 10000;
	int indice = 0;
	int r;
public:
	SamplerPoisson(BBox size_, int r_);
	void genAleatoire();
	Point next();
	~SamplerPoisson();
};

