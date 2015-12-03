#include "Shapes.h"

Shapes::Shapes() : c(white)
{

}

Shapes::Shapes(ColorRGB c_) : c(c_)
{

}

ColorRGB Shapes::getColor ( const Point &p ) const {
	return c;
}

bool Shapes::canIntersect()
{
	return true;
}

Shapes::~Shapes()
{
}
