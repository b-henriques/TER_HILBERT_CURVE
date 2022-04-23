#include "HilbertCurve2D.h"
#include "Point2D.h"
#include "HilbertCurve2D_Adaptive.h"
#include "HilbertCurve2D_Fixed.h"
#include <iostream>

int main(int argc, char* argv[])
{
	//16777216 -- order 12 pow(4,2)
	std::vector<Point2D> points = Point2D::generatePoints(16777216, 0, 0, 10, 10);

	//std::vector<Point2D> points;
	/*points.push_back(Point2D(1, 1));
	points.push_back(Point2D(1, 2));
	points.push_back(Point2D(2, 4));
	points.push_back(Point2D(3, 4));
	points.push_back(Point2D(1, 4));
	points.push_back(Point2D(0, 4));*/

	/*points.push_back(Point2D(1, 1));
	points.push_back(Point2D(1, 2));
	points.push_back(Point2D(1, 3));
	points.push_back(Point2D(1, 4));
	points.push_back(Point2D(2, 1));
	points.push_back(Point2D(2, 2));
	points.push_back(Point2D(2, 3));
	points.push_back(Point2D(2, 4));
	points.push_back(Point2D(3, 1));
	points.push_back(Point2D(3, 2));
	points.push_back(Point2D(3, 3));
	points.push_back(Point2D(3, 4));
	points.push_back(Point2D(4, 1));
	points.push_back(Point2D(4, 2));
	points.push_back(Point2D(4, 3));
	points.push_back(Point2D(4, 4));*/

	/*for (auto p : points)
	{
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	}
	std::cout << "========================" << std::endl;*/

	HilbertCurve2D_Fixed hc(points, 12, Point2D(0,0), Point2D(10,10), 8);
	//HilbertCurve2D_Adaptive hc(points, 1, Point2D(0, 0), Point2D(10, 10));


	std::cout << std::endl << std::endl;
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

}