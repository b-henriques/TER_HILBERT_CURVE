#include "HilbertCurve2D.h"
#include <iostream>

int main(int argc, char* argv[])
{
	// TODO: implement tests => main is temporary
	HilbertCurve2D hc(14);

	unsigned int x{ 47 };
	unsigned int y{ 63 };
	std::cout << hc.coords_to_index(x, y) << std::endl;

	unsigned int x_unknown{ 10 };
	unsigned int y_unknown{ 15 };
	hc.index_to_coord(54, x_unknown, y_unknown);
	std::cout << "x_unknown = " << x_unknown << ", y_unknown = " << y_unknown << std::endl;

}