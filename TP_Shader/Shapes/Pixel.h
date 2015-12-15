#pragma once
#include "Geometry\Point.h"
#include "Global\ColorRGB.h"
#include "Global\Constants.h"

class Pixel : public Point
{
private:

public:
	ColorRGB color;
	float alpha;

	Pixel();
	Pixel(const Point& _position);
	Pixel(const Point& _position, const ColorRGB& _color);
};

