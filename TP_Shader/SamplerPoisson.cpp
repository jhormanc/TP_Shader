#include "SamplerPoisson.h"


SamplerPoisson::SamplerPoisson(BBox size_, float r_) : size(size_), randArray(std::vector<Sphere>()), r(r_)
{
}
void SamplerPoisson::genAleatoire()
{	
	randArray.clear();
	for (int i = 0; i < maxSample; ++i)
	{
		bool addSphere = true;
		Sphere tmp(r, Point(randf(size.pMin.x, size.pMax.x), randf(size.pMin.y, size.pMax.y), randf(size.pMin.z, size.pMax.z)));
		for (int j = 0; j < randArray.size(); ++j)
		{
			if (tmp.overlap(randArray[j]))
			{
				addSphere = false;
				break;
			}
		}
		if (addSphere)
		{
			randArray.push_back(tmp);
		}
	}
}
Point SamplerPoisson::next()
{
	if (++indice >= randArray.size() - 1)
		indice = 0;
	return randArray[indice].origin;
}

