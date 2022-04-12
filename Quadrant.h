#pragma once
#include "Point2D.h"
class Quadrant
{
public:
	Quadrant() {};
	Quadrant(uint64_t _mi, uint32_t _x, uint32_t _y, uint64_t _hi, std::vector<Point2D> _points)
		: morton_index(_mi), x(_x), y(_y), hilbert_index(_hi), points(_points) {}

	uint32_t getX();
	uint32_t getY();
	uint64_t getHilbertIndex();
	uint64_t getMortonIndex();
	std::vector<Point2D> getPoints();


private:
	uint32_t x{ 0 }, y{ 0 };         // mapped coordinates in grid
	uint64_t hilbert_index{ 0 };
	uint64_t morton_index{ 0 };
	std::vector<Point2D> points;     // points in quadrant

};
