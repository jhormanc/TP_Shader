#pragma once
#include <vector>
#include "Global\Global.h"
#include <fstream>

class Film
{
	const char * filename;
public:
	std::vector<std::vector<ColorRGB>> colors;
	ColorRGB backgroundColor;
	int xResolution, yResolution;
	Film(int xres, int yres, const char * filename_, ColorRGB backgroundColor_);
	void writePpm();
	~Film();
};


