#include "HilbertCurve2D.h"
#include <stdexcept> /*runtime_error*/
#include <immintrin.h>
#include <math.h>
#include <string> 

HilbertCurve2D::HilbertCurve2D(uint32_t _order, Point2D _bl, Point2D _tr)
{
	if (_bl.getX() >= _tr.getX() || _bl.getY() >= _tr.getY())
	{
		throw std::runtime_error("illformed bounding-box: bottomleft point shoud be < to topright point");
	}
	order = _order;
	quadrants = std::vector<Quadrant>(pow(4, order));
	for (int i = 0; i < quadrants.size(); i++) {
		quadrants[i].setHilbertIndex(i);
		uint32_t x, y;
		hilbertindex_to_coord(i, x, y);
		quadrants[i].setX(x);
		quadrants[i].setY(y);
		quadrants[i].setMortonIndex(coords_to_mortonindex(x, y));
	}
	bottomLeft = _bl;
	topRight = _tr;
}

void HilbertCurve2D::checkXY(uint32_t x, uint32_t y)
{
	uint32_t colspam = pow(2, order) - 1;
	if (x > colspam || y > colspam)
	{
		throw std::runtime_error("x or y not associated with curve order" + std::to_string(order));
	}
}

void HilbertCurve2D::checkHilberIndex(uint64_t hi)
{
	if (hi > pow(4, order) - 1)
	{
		throw std::runtime_error("hilbertindex associated with curve order " + std::to_string(order));
	}
}


uint64_t HilbertCurve2D::coords_to_hilbertindex(uint32_t x, uint32_t y)
{
	checkXY(x, y);

	uint64_t index = 0;

	for (int i = order - 1; i >= 0; i--)
	{
		index = index << 2;
		uint32_t half = 1 << i;

		// quadrant = x_i.y_i => quadrant in {00,01,10,11}
		uint32_t quadrant = 0;
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
		{uint32_t tmp = x;
		x = y;
		y = tmp;
		}
		break;
		case 1: //quadrant = 1
			break;
		case 2:
		{uint32_t tmp = x;
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

void HilbertCurve2D::hilbertindex_to_coord(uint64_t index, uint32_t& x, uint32_t& y)
{
	checkHilberIndex(index);
	x = 0;
	y = 0;

	for (int i = 0; i <= order - 1; i++)
	{
		// quadrant = index1.index0 => quadrant in {00,01,10,11}
		uint32_t quadrant = 0;
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
		{uint32_t tmp = x;
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
			{uint32_t tmp = x;
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

uint64_t HilbertCurve2D::coords_to_mortonindex(uint32_t x, uint32_t y)
{
	return _pdep_u64(x, UINT64_C(0x5555555555555555) << 1) | _pdep_u64(y, UINT64_C(0x5555555555555555));
}

void HilbertCurve2D::mortonindex_to_coord(uint64_t index, uint32_t& x, uint32_t& y)
{
	x = 0;
	y = 0;
	x = _pext_u64(index, UINT64_C(0x5555555555555555) << 1);
	y = _pext_u64(index, UINT64_C(0x5555555555555555));

}

uint64_t HilbertCurve2D::mortonToHilbert(uint64_t zorder)
{
	uint32_t config = 0;
	uint64_t hilbert = 0;
	for (int i = order - 1; i >= 0; i--) {
		uint32_t quadrant = zorder >> (2 * i) & 3;
		hilbert = hilbert << 2 | base_pattern[config][quadrant];
		config = configuration[config][quadrant];
	}
	return hilbert;
}

