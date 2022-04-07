#include "HilbertTree.h"
#include <math.h> 
#include <random>
#include <chrono> 

HilbertTree::HilbertTree(const std::vector<Point2D> _points, int _order)
{
	order = _order;
	tree = std::vector<double>(pow(4, _order) - 1);

}

Point2D HilbertTree::get_mappedPoint(Point2D point)
{
	return Point2D();
}

unsigned int HilbertTree::get_Zorder(Point2D point)
{
	return 0;
}

unsigned int HilbertTree::pickPivot(unsigned int start, unsigned int end)
{
	//pivot tire aleatirement uniforme
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<unsigned int> distribution(start, end);
	return distribution(generator);
}

void HilbertTree::swap(std::vector<Point2D> points, unsigned int i, unsigned int j)
{
	Point2D tmp = points[i];
	points[i] = points[j];
	points[j] = tmp;
}

void HilbertTree::split4Quadrants(std::vector<Point2D> points, unsigned int level)
{

}

unsigned int HilbertTree::select_median_x(std::vector<Point2D> points, unsigned int start, unsigned int end)
{
	//pick random uniform pivot
	unsigned int pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(points, pivot_index, start);
	pivot_index = start;

	// A pivot B with point in A if point.y < pivot.y ...
	for (unsigned int k = start + 1; k <= end; k++)
	{
		if (points[k].getX() < points[pivot_index].getX())
		{
			swap(points, k, pivot_index + 1);
			swap(points, pivot_index, pivot_index + 1);
		}
	}

	//if median
	if (pivot_index == start + ((end - start) / 2)) return pivot_index;

	//if median in A
	if (pivot_index < start + ((end - start) / 2))
	{
		return select_median_y(points, pivot_index + 1, end);
	}//if in B
	else {
		return select_median_y(points, start, pivot_index - 1);
	}
}

unsigned int HilbertTree::select_median_y(std::vector<Point2D> points, unsigned int start, unsigned int end)
{
	//pick random uniform pivot
	unsigned int pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(points, pivot_index, start);
	pivot_index = start;

	// A pivot B with point in A if point.y < pivot.y ...
	for (unsigned int k = start + 1; k <= end; k++)
	{
		if (points[k].getY() < points[pivot_index].getY())
		{
			swap(points, k, pivot_index + 1);
			swap(points, pivot_index, pivot_index + 1);
		}
	}

	//if median
	if (pivot_index == start + ((end - start) / 2)) return pivot_index;

	//if median in A
	if (pivot_index < start + ((end - start) / 2))
	{
		return select_median_y(points, pivot_index + 1, end);
	}//if in B
	else {
		return select_median_y(points, start, pivot_index - 1);
	}

}