#pragma once
#include "Point2D.h"
#include "HilbertCurve2D.h"
#include "Quadrant.h"
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
	std::vector<double> tree;
	
	void genTree(std::vector<Point2D> &points, uint32_t start, uint32_t end, uint32_t level, uint64_t zindex);
	uint32_t select_median_x(std::vector<Point2D>& points, uint32_t start, uint32_t end);
	uint32_t select_median_y(std::vector<Point2D>& points, uint32_t start, uint32_t end);
	static uint32_t pickPivot(uint32_t start, uint32_t end);
	void swap(std::vector<Point2D>& points, uint32_t i, uint32_t j);

	uint64_t get_quadrant_index(Point2D point);

};

