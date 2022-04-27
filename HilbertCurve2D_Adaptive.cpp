#include "HilbertCurve2D_Adaptive.h"
#include <string> 
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

HilbertCurve2D_Adaptive::HilbertCurve2D_Adaptive(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads) : HilbertCurve2D(_order, _bl, _tr, _points, _nb_threads)
{
	tree = std::unique_ptr<Node>(genTree(points, 0, points.size() - 1, 0, 0));


	//auto l = tree.get();
	//while (l) {
	//	std::cout << "leaf? " << l->leaf << "value " << l->value << std::endl;
	//	l = l->left.get();
	//}
	//std::cout << "\t hi \t start \t end\n";
	//for (auto itr = quads.begin(); itr != quads.end(); ++itr) {
	//	std::cout << '\t' << itr->first << '\t' << (itr->second.start) << '\t' << (itr->second.end) << '\n';
	//	for (int i = itr->second.start; i <= itr->second.end; i++) {
	//		auto p = points[i];
	//		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	//	}
	//}

	//for (auto q : quadrants)
	//{
	//	std::cout << q.getPoints().size() << "-";
	//	/*std::cout << "Quadrant: " << q.getHilbertIndex() << std::endl;
	//	std::cout << "x,y: " << q.getX() << "," << q.getY() << std::endl;
	//	std::cout << "Morton: " << q.getMortonIndex() << std::endl;
	//	for (auto p : q.getPoints())
	//	{
	//		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	//	}
	//	std::cout << "========================" << std::endl;*/
	//}
	//std::cout << std::endl;
}

Point2D HilbertCurve2D_Adaptive::get_mappedPoint(Point2D point)
{
	return Point2D();

}

uint64_t HilbertCurve2D_Adaptive::get_MortonIndex(Point2D point)
{
	return 0;
}

uint64_t HilbertCurve2D_Adaptive::get_HilbertIndex(Point2D point)
{
	return 0;

}

std::vector<Point2D> HilbertCurve2D_Adaptive::get_points_from_hilbertindex(uint64_t hilbertindex)
{
	checkHilberIndex(hilbertindex);
	return std::vector<Point2D>();
}

//TODO
uint64_t HilbertCurve2D_Adaptive::get_quadrant_index(Point2D point)
{

	return 0;
}

HilbertCurve2D_Adaptive::Node* HilbertCurve2D_Adaptive::genTree(std::vector<Point2D>& points, uint32_t start, uint32_t end, uint32_t level, uint64_t zindex)
{
	Node* node = new Node();
	//if number of points = 1 or level of detail achieved
	if (end - start == 0 || level == 2 * order)
	{
		if (level != 2 * order) {
			zindex = zindex << 2 * order - level;
		}
		uint64_t hi = mortonToHilbert(zindex);
		//create leaf
		node->leaf = true;
		node->quadrant_index = hi;


		//create quadrant
		quadrants.insert(
			std::pair<uint64_t, HilbertCurve2D::Quadrant>(
				hi,
				HilbertCurve2D::Quadrant(start, end)
				)
		);


		//return node
		return node;

	}
	else { //number of points >= 2
		uint32_t pivot_index;
		//if level even split by x, else by y
		if (!(level % 2))
		{
			pivot_index = select_median_x(points, start, end);

			//check for points with same coord => as they should belong to same quadrant
			while (pivot_index < end && points[pivot_index].getX() == points[pivot_index + 1].getX())
				pivot_index++;

			//add split_value to tree
			node->value = points[pivot_index].getX();
		}
		else {
			pivot_index = select_median_y(points, start, end);

			//check for points with same coord => as they should belong to same quadrant
			while (pivot_index < end && points[pivot_index].getY() == points[pivot_index + 1].getY())
				pivot_index++;

			//add split_value to tree
			node->value = points[pivot_index].getY();
		}

		uint32_t z_left, z_right;
		z_left = zindex << 1;
		z_right = zindex << 1 | 1;
		node->left = std::unique_ptr<Node>(genTree(points, start, pivot_index, level + 1, z_left));
		if (pivot_index < end)
			node->right = std::unique_ptr<Node>(genTree(points, pivot_index + 1, end, level + 1, z_right));
		return node;

	}
}

uint32_t HilbertCurve2D_Adaptive::select_median_x(std::vector<Point2D>& points, uint32_t start, uint32_t end)
{
	uint32_t m = start + ((end - start) / 2);
	std::nth_element(points.begin() + start, points.begin() + m, points.begin() + end + 1, [](Point2D a, Point2D b) {return a.getX() < b.getX(); });
	return m;
}

uint32_t HilbertCurve2D_Adaptive::select_median_y(std::vector<Point2D>& points, uint32_t start, uint32_t end)
{
	uint32_t m = start + ((end - start) / 2);
	std::nth_element(points.begin() + start, points.begin() + m, points.begin() + end + 1, [](Point2D a, Point2D b) {return a.getY() < b.getY(); });
	return m;
}