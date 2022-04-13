#include "HilbertCurve2D_Adaptive.h"
#include <math.h> 
#include <random>
#include <chrono> 
#include <string>  
#include <iostream>

HilbertCurve2D_Adaptive::HilbertCurve2D_Adaptive(const std::vector<Point2D> _points, uint32_t _order, Point2D _bl, Point2D _tr) : HilbertCurve2D(_order, _bl, _tr)
{
	std::vector<Point2D> points = _points;
	tree = std::vector<double>(2 * pow(4, order) - 1);
	genTree(points, 0, points.size() - 1, 0, 0);
	for (auto q : quadrants)
	{
		std::cout << q.getPoints().size() << "-";
		/*std::cout << "Quadrant: " << q.getHilbertIndex() << std::endl;
		std::cout << "x,y: " << q.getX() << "," << q.getY() << std::endl;
		std::cout << "Morton: " << q.getMortonIndex() << std::endl;
		for (auto p : q.getPoints())
		{
			std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
		}
		std::cout << "========================" << std::endl;*/
	}
	/*std::cout << std::endl;
	for (auto d : tree){ std::cout << d << ","; }
	std::cout << std::endl;*/
}

Point2D HilbertCurve2D_Adaptive::get_mappedPoint(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return Point2D(q.getX(), q.getY());
}

uint64_t HilbertCurve2D_Adaptive::get_MortonIndex(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return q.getMortonIndex();
}

uint64_t HilbertCurve2D_Adaptive::get_HilbertIndex(Point2D point)
{
	uint64_t qindex = get_quadrant_index(point);
	Quadrant q = quadrants[qindex];
	return q.getHilbertIndex();
}

std::vector<Point2D> HilbertCurve2D_Adaptive::get_points_from_hilbertindex(uint64_t hilbertindex)
{
	checkHilberIndex(hilbertindex);
	return quadrants[hilbertindex].getPoints();
}

uint32_t HilbertCurve2D_Adaptive::pickPivot(uint32_t start, uint32_t end)
{
	//pivot tire aleatirement uniforme
	uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<uint32_t> distribution(start, end);
	return distribution(generator);
}

void HilbertCurve2D_Adaptive::swap(std::vector<Point2D>& points, uint32_t i, uint32_t j)
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

void HilbertCurve2D_Adaptive::genTree(std::vector<Point2D>& points, uint32_t start, uint32_t end, uint32_t level, uint64_t zindex)
{
	//TODO: only median split if 2 points or more, else even split
	/*if (start > end) {
		return;
	}*/
	//position in tree vector
	uint32_t pos = pow(2, level) - 1 + zindex;
	//if leaf
	if (level == 2 * order)
	{
		uint64_t hi = mortonToHilbert(zindex);
		//uint32_t x, y;
		std::vector<Point2D>::const_iterator first = points.begin() + start;
		std::vector<Point2D>::const_iterator last = points.begin() + end + 1;
		std::vector<Point2D> pts(first, last);
		//mortonindex_to_coord(zindex, x, y);
		//Quadrant q = Quadrant(zindex, x, y, hi, pts);
		//quadrants[hi] = q;
		quadrants[hi].addPoints(pts);
		tree[pos] = hi;
		return;
	}
	//if level even split by x, else by y
	uint32_t pivot_index;
	if (!(level % 2))
	{
		pivot_index = select_median_x(points, start, end, start + ((end - start) / 2));
		//add split_value to tree
		tree[pos] = points[pivot_index].getX();
	}
	else {
		pivot_index = select_median_y(points, start, end, start + ((end - start) / 2));
		tree[pos] = points[pivot_index].getY();
	}

	uint32_t z_left, z_right;
	z_left = zindex << 1;
	z_right = zindex << 1 | 1;
	genTree(points, start, pivot_index, level + 1, z_left);
	genTree(points, pivot_index + 1, end, level + 1, z_right);

}

uint32_t HilbertCurve2D_Adaptive::select_median_x(std::vector<Point2D>& points, uint32_t start, uint32_t end, uint32_t m)
{ 
	//pick random uniform pivot
	uint32_t pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(points, pivot_index, start);
	pivot_index = start;

	// A pivot B with point in A if point.x < pivot.x ...
	for (uint32_t k = start + 1; k <= end; k++)
	{
		if (points[k].getX() <= points[pivot_index].getX())
		{
			swap(points, k, pivot_index + 1);
			swap(points, pivot_index, pivot_index + 1);
			pivot_index++;
		}
	}

	//if median
	if (pivot_index == m) return pivot_index;

	//if median in A
	if (pivot_index < m)
	{
		return select_median_x(points, pivot_index + 1, end, m);
	}//if in B
	else {
		return select_median_x(points, start, pivot_index - 1, m);
	}
}

uint32_t HilbertCurve2D_Adaptive::select_median_y(std::vector<Point2D>& points, uint32_t start, uint32_t end, uint32_t m)
{
	//pick random uniform pivot
	uint32_t pivot_index = pickPivot(start, end);
	//place pivot at start
	swap(points, pivot_index, start);
	pivot_index = start;


	// A pivot B with point in A if point.y < pivot.y ...
	for (uint32_t k = start + 1; k <= end; k++)
	{
		if (points[k].getY() <= points[pivot_index].getY())
		{
			swap(points, k, pivot_index + 1);
			swap(points, pivot_index, pivot_index + 1);
			pivot_index++;
		}
	}

	//if median
	if (pivot_index == m) return pivot_index;

	//if median in A
	if (pivot_index < m)
	{
		return select_median_y(points, pivot_index + 1, end, m);
	}//if in B
	else {
		return select_median_y(points, start, pivot_index - 1, m);
	}

}