#include "HilbertCurve2D_Adaptive.h"
#include <math.h> 
#include <random>
#include <chrono> 
#include <string>  
#include <iostream>

HilbertCurve2D_Adaptive::HilbertCurve2D_Adaptive(const std::vector<Point2D> _points, uint32_t _order) : HilbertCurve2D(_order)
{
	points = _points;
	tree = std::vector<double>(2 * pow(4, order) - 1);
	quadrants = std::vector<Quadrant>(pow(4, order));
	genTree(0, points.size() - 1, 0, 0);

	for (auto p : points)
	{
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	}
	for (auto q : quadrants)
	{
		std::cout << q.start << " " << q.end << std::endl;
	}
}

Point2D HilbertCurve2D_Adaptive::get_mappedPoint(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return Point2D(q.x, q.y);
}

uint64_t HilbertCurve2D_Adaptive::get_MortonIndex(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return q.morton_index;
}

uint64_t HilbertCurve2D_Adaptive::get_HilbertIndex(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return q.hilbert_index;
}

std::vector<Point2D> HilbertCurve2D_Adaptive::get_points_from_hilbertindex(uint64_t hilbertindex)
{
	checkHilberIndex(hilbertindex);
	Quadrant q = quadrants[hilbertindex];
	std::vector<Point2D>::const_iterator first = points.begin() + q.start;
	std::vector<Point2D>::const_iterator last = points.begin() + q.end+1;
	return std::vector<Point2D>(first, last);
}

uint32_t HilbertCurve2D_Adaptive::pickPivot(uint32_t start, uint32_t end)
{
	//pivot tire aleatirement uniforme
	uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<uint32_t> distribution(start, end);
	return distribution(generator);
}

void HilbertCurve2D_Adaptive::swap(uint32_t i, uint32_t j)
{
	Point2D tmp = points[i];
	points[i] = points[j];
	points[j] = tmp;
}

uint64_t HilbertCurve2D_Adaptive::get_quadrant_index(Point2D point)
{
	bool xsplit = true;
	unsigned int pos = 0;
	while (pos < pow(4, order) - 1)
	{
		if (xsplit)
		{
			if (point.getX() < tree[pos])
			{
				pos = 2 * pos + 1;
				continue;
			}
		}
		else {
			if (point.getY() < tree[pos])
			{
				pos = 2 * pos + 1;
				continue;
			}
		}
		pos = 2 * pos + 2;
	}
	return tree[pos];
}

void HilbertCurve2D_Adaptive::genTree(uint32_t start, uint32_t end, uint32_t level, uint64_t zindex)
{
	//position in tree vector
	uint32_t pos = pow(2, level) - 1 + zindex;
	//if leaf
	if (level == 2 * order)
	{
		//TODO
		uint64_t hi = mortonToHilbert(zindex);
		uint32_t x, y;
		mortonindex_to_coord(zindex, x, y);
		Quadrant q = Quadrant(start, end, zindex, x, y, hi);
		quadrants[hi] = q;
		tree[pos] = hi;
		return;
	}
	//if level even split by x, else by y
	uint32_t pivot_index;
	if (!(level % 2))
	{
		pivot_index = select_median_x(start, end);
		//add split_value to tree
		tree[pos] = points[pivot_index].getX();
	}
	else {
		pivot_index = select_median_y(start, end);
		tree[pos] = points[pivot_index].getY();
	}

	uint32_t z_left, z_rigth;
	z_left = zindex << 1;
	z_rigth = zindex << 1 | 1;
	genTree(start, pivot_index, level + 1, z_left);
	genTree(pivot_index + 1, end, level + 1, z_rigth);

}

uint32_t HilbertCurve2D_Adaptive::select_median_x(uint32_t start, uint32_t end)
{
	//pick random uniform pivot
	uint32_t pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(pivot_index, start);
	pivot_index = start;

	// A pivot B with point in A if point.x < pivot.x ...
	for (uint32_t k = start + 1; k <= end; k++)
	{
		if (points[k].getX() < points[pivot_index].getX())
		{
			swap(k, pivot_index + 1);
			swap(pivot_index, pivot_index + 1);
			pivot_index++;
		}
	}

	//if median
	if (pivot_index == start + ((end - start) / 2)) return pivot_index;

	//if median in A
	if (pivot_index < start + ((end - start) / 2))
	{
		return select_median_x(pivot_index + 1, end);
	}//if in B
	else {
		return select_median_x(start, pivot_index - 1);
	}
}

uint32_t HilbertCurve2D_Adaptive::select_median_y(uint32_t start, uint32_t end)
{
	//pick random uniform pivot
	uint32_t pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(pivot_index, start);
	pivot_index = start;

	// A pivot B with point in A if point.y < pivot.y ...
	for (uint32_t k = start + 1; k <= end; k++)
	{
		if (points[k].getY() < points[pivot_index].getY())
		{
			swap(k, pivot_index + 1);
			swap(pivot_index, pivot_index + 1);
			pivot_index++;
		}
	}

	//if median
	if (pivot_index == start + ((end - start) / 2)) return pivot_index;

	//if median in A
	if (pivot_index < start + ((end - start) / 2))
	{
		return select_median_y(pivot_index + 1, end);
	}//if in B
	else {
		return select_median_y(start, pivot_index - 1);
	}

}