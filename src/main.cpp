#include <HilbertCurve2D_Adaptive.h>
#include <HilbertCurve2D_Fixed.h>
#include <Point2D.h>
#include <PointsGeneratorBuilder.h>
#include <iostream>

int main(int argc, char* argv[])
{
	//16777216 -- order 12 pow(4,2)
	std::vector<Point2D> points;
	PointsGeneratorBuilder genb;
	/*std::pair<std::vector<double>, std::vector<double>> x_params({ 0.0, 2.0, 4.0, 10.0 }, { 10.0, 1.0, 10.0 });
	std::pair<std::vector<double>, std::vector<double>> y_params({ 0.0, 2.0, 4.0, 10.0 }, { 10.0, 1.0, 10.0 });
	auto gen = genb.mk_piecewise_constant(x_params, y_params);*/
	auto gen = genb.mk_uniform(Point2D(10, 10));
	//UniformPointsGenerator gen(10, 10);
	//GammaPointsGenerator gen(10, 10, 15, 1.0, 4.0, 1.0);
	/*PiecewiseConstantPointsGenerator gen(
		{ 0.0, 2.0, 4.0, 10.0 }, { 10.0, 1.0, 10.0 },
		{ 0.0, 2.0, 4.0, 10.0 }, { 10.0, 1.0, 10.0 }
	);*/
	//points = gen.generatePoints(16777216, 8);
	std::cout << "Points generated\n";
	//std::vector<Point2D> points = Point2D::generatePoints(167772160, 0, 0, 10, 10);
	
	//std::vector<Point2D> points;
	points.push_back(Point2D(1, 1));
	points.push_back(Point2D(3, 1));
	points.push_back(Point2D(1, 3));
	points.push_back(Point2D(3, 3));
	/*points.push_back(Point2D(1, 4));
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
	points.push_back(Point2D(4, 4));
	points.push_back(Point2D(10, 10));
	points.push_back(Point2D(10, 0));
	points.push_back(Point2D(10, 1));*/
	std::vector<Point2D> points2(points);
	for (auto p : points)
	{
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	}
	std::cout << "========================" << std::endl;
	//return 0;

	HilbertCurve2D_Fixed hc(points2, 1, 4, 4, 8);
	HilbertCurve2D_Adaptive hc2(points, 1, 4, 4, 8);
	for (auto p : points)
	{
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	}
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

	std::cout << hc2.get_MortonIndex(Point2D(3, 3)) << std::endl;
	std::cout << hc2.get_HilbertIndex(Point2D(3, 3)) << std::endl;
	std::pair<uint64_t, uint64_t> range = hc2.get_points_from_hilbertindex(3);
	std::cout << range.first << "," << range.second << std::endl;

	std::pair<uint64_t, uint64_t> coords = hc.get_mappedPoint(Point2D(1.0000001, 2));
	std::cout << coords.first << "," << coords.second << std::endl;


	auto ret = hc.get_n_closest(Point2D(1.5, 1.5), 2);
	//auto ret = hc.get_points_in_range(Point2D(9.5, 3.5), 0.05);
	for (auto p : ret) {
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << p.dist(Point2D(1.5, 1.5)) << std::endl;
	}
	std::cout << ret.size() << "\n";

	//ret = hc2.get_points_in_range(Point2D(9.5, 3.5), 0.05);
	ret = hc2.get_n_closest(Point2D(1.5, 1.5), 2);
	for (auto p : ret) {
		std::cout << "(" << p.getX() << "," << p.getY() << ")" << p.dist(Point2D(1.5, 1.5)) << std::endl;
	}
	std::cout << ret.size() << "\n";

}