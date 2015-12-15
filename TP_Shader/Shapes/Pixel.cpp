#include "Pixel.h"

Pixel::Pixel() : Point(0.f), color(black), alpha(1.f)
{

}

Pixel::Pixel(const Point& _position) : Point(_position), color(black), alpha(1.f)
{
	
}

Pixel::Pixel(const Point& _position, const ColorRGB& _color) : Point(_position), color(_color), alpha(1.f)
{

}
