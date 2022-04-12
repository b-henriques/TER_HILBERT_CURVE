#include "HilbertCurve2D.h"
#include "Point2D.h"
#include "HilbertCurve2D_Adaptive.h"
#include <iostream>

int main(int argc, char* argv[])
{
	std::vector<Point2D> points = Point2D::generatePoints(100000, 0, 0, 10, 10);

	/*for (auto p : points)
	{
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	}
	std::cout << "========================" << std::endl;*/

	HilbertCurve2D_Adaptive hc(points, 4);

	uint32_t x{ 0 };
	uint32_t y{ 1 };
	std::cout << hc.coords_to_hilbertindex(x, y) << std::endl;

	uint32_t x_unknown{ 1 };
	uint32_t y_unknown{ 1 };
	hc.hilbertindex_to_coord(hc.coords_to_hilbertindex(x, y), x_unknown, y_unknown);
	std::cout << "x_unknown = " << x_unknown << ", y_unknown = " << y_unknown << std::endl;

	std::cout << "mortonToHilbert(uint64_t zorder) = " << hc.mortonToHilbert(hc.coords_to_mortonindex(x, y)) << std::endl;
	

	std::cout << "coords_to_mortonindex(uint32_t x, uint32_t y) = " << hc.coords_to_mortonindex(x, y) << std::endl;
	hc.mortonindex_to_coord(hc.coords_to_mortonindex(x, y), x_unknown, y_unknown);
	std::cout << "mortonindex_to_coord = (" << x_unknown << "," << y_unknown << ")" << std::endl;

	//std::vector<std::vector<Point2D>> quadrants(pow(4, 1));
	//for (uint64_t k = 0; k < quadrants.size(); k++)
	//{
	//	quadrants[k] = hc.get_points_from_hilbertindex(k);
	//}
	//for (uint64_t k = 0; k < quadrants.size(); k++)
	//{
	//	std::cout << "========= " << k << " ===========" << std::endl;
	//	for (Point2D p : quadrants[k])
	//	{
	//		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	//	}
	//}
	

}