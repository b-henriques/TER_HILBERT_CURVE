#pragma once
#include "Point2D.h"
#include "HilbertCurve2D.h"
#include <memory> /*unique_ptr*/




class HilbertCurve2D_Adaptive : public HilbertCurve2D
{
public:
	HilbertCurve2D_Adaptive(std::vector<Point2D> &_points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads = 1);

	Point2D get_mappedPoint(Point2D point);
	uint64_t get_MortonIndex(Point2D point);
	uint64_t get_HilbertIndex(Point2D point);
	std::vector<Point2D> get_points_from_hilbertindex(uint64_t hilbertindex);




private:


	struct Node {
		bool leaf = false;
		double value;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;
		uint64_t quadrant_index;

	};

	std::unique_ptr<Node> tree;

	


	Node* genTree(std::vector<Point2D>& points, uint32_t start, uint32_t end, uint32_t level, uint64_t zindex);
	uint32_t select_median_x(std::vector<Point2D>& points, uint32_t start, uint32_t end);
	uint32_t select_median_y(std::vector<Point2D>& points, uint32_t start, uint32_t end);
	
	uint64_t get_quadrant_index(Point2D point);

};

