#include "HilbertCurve2D.h"
#include <limits> /*numeric limits*/
#include <stdexcept> /*runtime_error*/

HilbertCurve2D::HilbertCurve2D(int _order)
{
	// Todo verify order < 15: if not throw error?
	order = _order;
}


unsigned int HilbertCurve2D::coords_to_index(unsigned int x, unsigned int y)
{
	//TODO: drawing explaining algorithm for ter presentation

	unsigned int index = 0;

	for (int i = order; i >= 0; i--)
	{
		index = index << 2;
		unsigned int half = 1 << i;

		// quadrant = x_i.y_i => quadrant in {00,01,10,11}
		unsigned int quadrant = 0;
		quadrant = (((x & half) > 0) << 1) | ((y & half) > 0);


		/* Transformations:
		* 00->00
		* 01->11
		* 10->01
		* 11->10
		*/

		switch (quadrant)
		{
		case 0:
			{unsigned int tmp = x;
			x = y;
			y = tmp;
			}
			break;
		case 1:
			quadrant = 1;
			break;
		case 2:
			{unsigned int tmp = x;
			x = ((1 << (order + 1)) - 1) - y;
			y = ((1 << (order + 1)) - 1) - tmp;
			}
			quadrant = 3;
			break;
		case 3:
			quadrant = 2;
			break;
		default:
			throw std::runtime_error("unexpected behavior");
		}
		index = index | quadrant;
	}
	return index;
}

