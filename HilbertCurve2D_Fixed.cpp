#include "HilbertCurve2D_Fixed.h"
#include <thread>
#include <math.h> /*ceil*/
#include <functional> /*ref*/
#include<utility> /*make_pair*/

HilbertCurve2D_Fixed::HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads)
	: HilbertCurve2D(_order, _bl, _tr, _points, _nb_threads)
{
	x_factor = calulateFactor(bottomLeft.getX(), topRight.getX(), order);
	y_factor = calulateFactor(bottomLeft.getY(), topRight.getY(), order);

	generateCurve();
}

std::pair<uint32_t, uint32_t> HilbertCurve2D_Fixed::get_mappedPoint(Point2D point)
{
	checkPoint(point);
	uint32_t x, y;
	mapCellXY(point, x, y);
	return std::pair<uint32_t, uint32_t>(x, y);
}

uint64_t HilbertCurve2D_Fixed::get_MortonIndex(Point2D point)
{
	checkPoint(point);
	std::pair<uint64_t, uint64_t> coords = get_mappedPoint(point);
	return coords_to_mortonindex(coords.first, coords.second);
}

uint64_t HilbertCurve2D_Fixed::get_HilbertIndex(Point2D point)
{
	checkPoint(point);
	uint64_t zindex = get_MortonIndex(point);
	return mortonToHilbert(zindex);
}

std::vector<Point2D> HilbertCurve2D_Fixed::get_n_closest(Point2D point, uint32_t n)
{
	checkPoint(point);

	return std::vector<Point2D>();
}

std::vector<Point2D> HilbertCurve2D_Fixed::get_points_in_range(Point2D point, double dist_max)
{
	checkPoint(point);

	Point2D top, bottom, left, right;


	return std::vector<Point2D>();
}

double HilbertCurve2D_Fixed::calulateFactor(double vmin, double vmax, int k)
{
	double vwidth = vmax - vmin;
	uint32_t n = 1U << k; // 2^k
	double f = n / vwidth;

	while (vwidth * f >= (double)n)
	{
		f = nextafter(f, 0.0);
	}
	return f;
}

void HilbertCurve2D_Fixed::mapCellXY(Point2D point, uint32_t& x, uint32_t& y)
{
	x = (point.getX() - bottomLeft.getX()) * x_factor;
	y = (point.getY() - bottomLeft.getY()) * y_factor;
}

void HilbertCurve2D_Fixed::mapPoints2HilbertIndex(uint64_t start, uint64_t end, std::vector<Quadrants_info>& quadrants_info, std::vector<uint64_t>& hi_map)
{
	uint32_t x;
	uint32_t y;
	uint64_t hi;
	std::vector<uint64_t> localQuadrantStart(pow(4, order), 0);
	std::vector<uint64_t> local_nbPointsPerQuadrant(pow(4, order), 0);

	for (uint64_t i = start; i < end; i++) {

		mapCellXY(points[i], x, y);
		hi = mortonToHilbert(coords_to_mortonindex(x, y));
		hi_map[i] = hi;
		local_nbPointsPerQuadrant[hi]++;
	}
	uint64_t cnt = 0;
	for (uint64_t q = 0; q < local_nbPointsPerQuadrant.size(); q++) {
		localQuadrantStart[q] += cnt;
		cnt += local_nbPointsPerQuadrant[q];
	}
	for (uint64_t i = 0; i < quadrants_info.size(); i++) {
		quadrants_info[i].nbpoints += local_nbPointsPerQuadrant[i];
		quadrants_info[i].start_pos += localQuadrantStart[i];
	}
}

void HilbertCurve2D_Fixed::generateQuadrants(uint64_t hi_start, uint64_t hi_end, std::vector<Quadrants_info>& quadrants_info)
{
	uint64_t i;
	for (i = hi_start; i < hi_end; i++) {
		if (quadrants_info[i].nbpoints)
		{
			quadrants.insert(
				std::make_pair(
					i,
					HilbertCurve2D::Quadrant(
						quadrants_info[i].start_pos,
						quadrants_info[i].start_pos + quadrants_info[i].nbpoints - 1
					)
				)
			);
		}
	}
}

void HilbertCurve2D_Fixed::orderPoints(uint64_t start, uint64_t end, std::vector<Quadrants_info>& quadrants_info, const std::vector<uint64_t>& hi_map, const std::vector<Point2D>& tmpPoints)
{
	for (uint64_t i = start; i < end; i++)
	{
		uint64_t q = hi_map[i];
		uint64_t pos = quadrants_info[q].start_pos++;
		points[pos] = tmpPoints[i];
	}
}

void HilbertCurve2D_Fixed::generateCurve()
{
	std::vector<Point2D> tmpPoints = points;
	std::vector<uint64_t> hi_map(points.size());
	uint32_t t;
	std::vector<std::thread> threads;
	uint64_t points_interval = points.size() / nb_threads;
	uint64_t hi_interval = pow(4, order) / nb_threads;

	std::vector<HilbertCurve2D_Fixed::Quadrants_info> quadrantsInfo(pow(4, order));

	//map2HI
	for (t = 0; t < nb_threads; t++)
	{
		uint64_t start = t * points_interval;
		uint64_t end = (t + 1) * points_interval;
		if (t == nb_threads - 1)
		{
			end = points.size();
		}
		threads.push_back(
			std::thread(&HilbertCurve2D_Fixed::mapPoints2HilbertIndex, this, start, end,
				std::ref(quadrantsInfo),
				std::ref(hi_map)
			)
		);
	}
	for (t = 0; t < nb_threads; t++)
	{
		threads[t].join();
	}


	//createQuadrants
	for (t = 0; t < nb_threads; t++)
	{
		uint64_t hi_start = t * hi_interval;
		uint64_t hi_end = (t + 1) * hi_interval;
		if (t == nb_threads - 1)
		{
			hi_end = pow(4, order);
		}
		threads[t] = std::thread(
			&HilbertCurve2D_Fixed::generateQuadrants, this,
			hi_start, hi_end,
			std::ref(quadrantsInfo)
		);
	}
	for (t = 0; t < nb_threads; t++)
	{
		threads[t].join();
	}


	//reorder points
	for (t = 0; t < nb_threads; t++)
	{
		uint64_t start = t * points_interval;
		uint64_t end = (t + 1) * points_interval;
		if (t == nb_threads - 1)
		{
			end = points.size();
		}
		threads[t] = std::thread(
			&HilbertCurve2D_Fixed::orderPoints, this,
			start, end,
			std::ref(quadrantsInfo),
			std::cref(hi_map),
			std::cref(tmpPoints)
		);
	}
	for (t = 0; t < nb_threads; t++)
	{
		threads[t].join();
	}
}
