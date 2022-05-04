#include "HilbertCurve2D.h"
#include <stdexcept> /*runtime_error*/
#include <immintrin.h> /*_pdep_u64, _pext_u64*/
#include <math.h> /*pow*/
#include <string> 
#include <utility> /*make_pair*/


HilbertCurve2D::HilbertCurve2D(uint32_t _order, double _x_max, double _y_max, std::vector<Point2D>& _points, uint32_t _nb_threads) : points(_points)
{
	if (_x_max<=0 || _y_max <= 0)
	{
		throw std::runtime_error("illformed bounding-box: bottomleft point shoud be < to topright point");
	}
	order = _order;
	x_max = _x_max;
	y_max = _y_max;
	nb_threads = _nb_threads;
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
		throw std::runtime_error("hilbertindex not associated with curve order " + std::to_string(order));
	}
}

void HilbertCurve2D::checkPoint(Point2D point)
{
	if (point.getX() < 0 ||
		point.getX() > x_max ||
		point.getY() < 0 ||
		point.getY() > y_max
		)
	{
		throw std::runtime_error("point not in bounding box" + std::to_string(order));
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

std::pair<uint64_t, uint64_t> HilbertCurve2D::get_points_from_hilbertindex(uint64_t hilbertindex)
{
	checkHilberIndex(hilbertindex);

	if (quadrants.find(hilbertindex) != quadrants.end())
	{
		HilbertCurve2D::Quadrant q = quadrants.at(hilbertindex);
		return std::make_pair(q.start, q.end);
	}
	return std::make_pair(1, 0);
}

