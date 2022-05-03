#pragma once
#include "Point2D.h"
#include "HilbertCurve2D.h"
#include <memory> /*unique_ptr*/



class HilbertCurve2D_Adaptive : public HilbertCurve2D
{
public:
	HilbertCurve2D_Adaptive(std::vector<Point2D> &_points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads = 1);

	virtual std::pair<uint32_t, uint32_t> get_mappedPoint(Point2D point);
	virtual uint64_t get_MortonIndex(Point2D point);
	virtual uint64_t get_HilbertIndex(Point2D point);

	virtual std::vector<Point2D> get_n_closest(Point2D point, uint32_t n);
	virtual std::vector<Point2D> get_points_in_range(Point2D point, double dist_max);




private:


	struct Node {
		bool leaf = false;
		double value;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;
		uint64_t quadrant_index;

	};

	std::unique_ptr<Node> tree;

	
	Node* generateTree();

	void genTreePar(uint32_t start, uint32_t end, uint32_t level, uint64_t zindex, Node* output, uint32_t nbthreads);

	uint32_t select_median_x(uint32_t start, uint32_t end);
	uint32_t select_median_y(uint32_t start, uint32_t end);
	
	uint64_t get_quadrant_index(Point2D point);

};

