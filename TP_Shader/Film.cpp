#include "Film.h"
Film::Film(int xres, int yres, const char * filename_, ColorRGB backgroundColor_ = black) :xResolution(xres), yResolution(yres), filename(filename_), backgroundColor(backgroundColor_)
{
	colors = std::vector<std::vector<ColorRGB>>();
	colors.resize(xres);
	for (int i = 0; i < xres; ++i)
		colors[i].resize(yres);

	for (int j = 0; j < yres; ++j)
		for (int i = 0; i < xres; ++i)
			colors[i][j] = backgroundColor_;
}

void Film::writePpm()
{
	std::ofstream of(filename);
	const unsigned int maxColors = 255;

	of << "P3" << std::endl << xResolution << " " << yResolution << std::endl << maxColors << std::endl;
	for (int j = 0; j < yResolution; ++j)
		for (int i = 0; i < xResolution; ++i)
		{
			of << (int)colors[i][j].x << " " << (int)colors[i][j].y << " " << (int)colors[i][j].z << " ";
		}

	of.close();
}
Film::~Film()
{
}
