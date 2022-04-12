#include "Quadrant.h"

uint32_t Quadrant::getX()
{
	return x;
}

uint32_t Quadrant::getY()
{
	return y;
}

uint64_t Quadrant::getHilbertIndex()
{
	return hilbert_index;
}

uint64_t Quadrant::getMortonIndex()
{
	return morton_index;
}

std::vector<Point2D> Quadrant::getPoints()
{
	return points;
}