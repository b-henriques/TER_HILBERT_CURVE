#pragma once
#include "Point2D.h"
#include <vector>



class HilbertTree
{
public:
	HilbertTree(const std::vector<Point2D> _points, int _order);
	Point2D get_mappedPoint(Point2D point);
	unsigned int get_Zorder(Point2D point);


private:
	unsigned int order;
	std::vector<double> tree;

	void split4Quadrants(std::vector<Point2D> points, unsigned int level);
	unsigned int select_median_x(std::vector<Point2D> points, unsigned int start, unsigned int end);
	unsigned int select_median_y(std::vector<Point2D> points, unsigned int start, unsigned int end);
	static unsigned int pickPivot(unsigned int start, unsigned int end);
	static void swap(std::vector<Point2D> points, unsigned int i, unsigned int j);

};

