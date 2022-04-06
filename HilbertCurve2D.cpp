#include "HilbertCurve2D.h"
#include <stdexcept> /*runtime_error*/

HilbertCurve2D::HilbertCurve2D(int _order)
{
	// Todo verify order < 15: if not throw error?
	order = _order;
}


unsigned int HilbertCurve2D::coords_to_index(unsigned int x, unsigned int y)
{
	//TODO: drawing explaining algorithm for ter presentation


	// verify if x, y are valid coords: if not throw error?

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
		* 01->01
		* 10->11
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
		case 1: //quadrant = 1
			break;
		case 2:
			{unsigned int tmp = x;
			x = ((1 << (i + 1)) - 1) - y;
			y = ((1 << (i + 1)) - 1) - tmp;
			}
			quadrant = 3;
			break;
		case 3:
			quadrant = 2;
			break;
		default:
			throw std::runtime_error("unexpected behavior");
		} // endswitch
		index = index | quadrant;
	} //endfor
	return index;
}

void HilbertCurve2D::index_to_coord(unsigned int index, unsigned int& x, unsigned int& y)
{
	x = 0;
	y = 0;

	for (int i = 0; i <= order; i++)
	{
		// quadrant = index1.index0 => quadrant in {00,01,10,11}
		unsigned int quadrant = 0;
		quadrant = index & 3;

		/* Transformations:
		* 00->00
		* 01->01
		* 11->10
		* 10->11
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
			y = y | (1 << i);
			break;
		case 2:
			x = x | (1 << i);
			y = y | (1 << i);
			break;
		case 3:
			x = x | (1 << i);
			{unsigned int tmp = x;
			x = ((1 << (i + 1)) - 1) - y;
			y = ((1 << (i + 1)) - 1) - tmp;
			}
			break;
		default:
			throw std::runtime_error("unexpected behavior");
		} // endswitch
		index = index >> 2;
	} //endfor
}

unsigned int morton_Hilbert(unsigned int zorder, unsigned k){
	unsigned config = 0;
	unsigned int hilbert = 0;
	for (int i=k-1; i>=0; i--){
		unsigned quadrant = zorder>>(2*i) & 3;
		hilbert  = hilbert<<2 | base_pattern[config][quadrant];
		config = configuration[config][quadrant];
	}
	return hilbert;
}
