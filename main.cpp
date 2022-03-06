#include "HilbertCurve2D.h"
#include <iostream>

int main(int argc, char* argv[])
{
	// TODO: implement tests => main is temporary
	HilbertCurve2D hc(8);
	unsigned int x{ 5 };
	unsigned int y{ 26 };

	std::cout << hc.coords_to_index(x, y);

}