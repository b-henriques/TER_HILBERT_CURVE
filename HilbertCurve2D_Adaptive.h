#pragma once
#include "Point2D.h"
#include "HilbertCurve2D.h"
#include <vector>



class HilbertCurve2D_Adaptive : public HilbertCurve2D
{
public:
	HilbertCurve2D_Adaptive(const std::vector<Point2D> _points, uint32_t _order);

	Point2D get_mappedPoint(Point2D point);
	uint64_t get_MortonIndex(Point2D point);
	uint64_t get_HilbertIndex(Point2D point);
	std::vector<Point2D> get_points_from_hilbertindex(uint64_t hilbertindex);




private:
	class Quadrant {
	public:

		uint32_t x{ 0 }, y{ 0 };         // mapped coordinates in grid
		uint32_t start{ 0 }, end{ 0 };   // index start, end of bloc in points
		uint64_t hilbert_index{ 0 };
		uint64_t morton_index{ 0 };
		Quadrant() {};
		Quadrant(uint32_t _s, uint32_t _e, uint64_t _mi, uint32_t _x, uint32_t _y, uint64_t _hi)
			: start(_s), end(_e), morton_index(_mi), x(_x), y(_y), hilbert_index(_hi) {}
	};


	std::vector<Point2D> points;
	std::vector<double> tree;
	std::vector<Quadrant> quadrants;


	void genTree(uint32_t start, uint32_t end, uint32_t level, uint64_t zindex);
	uint32_t select_median_x(uint32_t start, uint32_t end);
	uint32_t select_median_y(uint32_t start, uint32_t end);
	static uint32_t pickPivot(uint32_t start, uint32_t end);
	void swap(uint32_t i, uint32_t j);

	uint64_t get_quadrant_index(Point2D point);

};

