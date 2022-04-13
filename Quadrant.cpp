#include "Quadrant.h"

uint32_t Quadrant::getX()
{
	return x;
}

void Quadrant::setX(uint32_t _x)
{
	x = _x;
}

uint32_t Quadrant::getY()
{
	return y;
}

void Quadrant::setY(uint32_t _y)
{
	y = _y;
}

uint64_t Quadrant::getHilbertIndex()
{
	return hilbert_index;
}

void Quadrant::setHilbertIndex(uint64_t _hi)
{
	hilbert_index = _hi;
}

uint64_t Quadrant::getMortonIndex()
{
	return morton_index;
}

void Quadrant::setMortonIndex(uint64_t _mi)
{
	morton_index = _mi;
}

std::vector<Point2D> Quadrant::getPoints()
{
	return points;
}

void Quadrant::reserve(int n)
{
	points.reserve(n);
}

void Quadrant::addPoints(std::vector<Point2D> _points)
{
	points.insert(points.end(), _points.begin(), _points.end());
}

void Quadrant::addPoint(Point2D _point)
{
	points.push_back(_point);
}
