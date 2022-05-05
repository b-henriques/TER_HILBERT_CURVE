#include "Point2D.h"
#include <random>
#include <chrono> 


double Point2D::getX()
{
	return x;
}

double Point2D::getY()
{
	return y;
}

void Point2D::setX(double _x)
{
	x = _x;
}

void Point2D::setY(double _y)
{
	y = _y;
}

double Point2D::dist(Point2D point)
{
	return sqrt(
		pow(x - point.getX(), 2) +
		pow(y - point.getY(), 2)
		* 1.0 /*implicit conversion to double*/
	);
}
