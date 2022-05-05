#include "HilbertCurve2D_Fixed.h"
#include <thread>
#include <math.h> /*ceil*/
#include <functional> /*ref*/
#include<utility> /*make_pair*/
#include <queue>
#include <set>
#include<iostream>

HilbertCurve2D_Fixed::HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, double _x_max, double _y_max, uint32_t _nb_threads)
	: HilbertCurve2D(_order, _x_max, _y_max, _points, _nb_threads)
{
	x_factor = calulateFactor(0, _x_max, order);
	y_factor = calulateFactor(0, _y_max, order);

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

	//maxheap
	auto cmp = [this, point](uint64_t left, uint64_t right) { return (points[left].dist(point)) < (points[right].dist(point)); };
	std::priority_queue<uint64_t, std::vector<uint64_t>, decltype(cmp)> closest_points(cmp);
	std::set<uint64_t> closest_points_set;

	uint64_t hi = get_HilbertIndex(point);
	auto indexes = get_points_from_hilbertindex(hi);
	uint64_t i;

	//initialize maxheap with points in quadrant or neighboring quadrants
	for (i = indexes.first; i < (indexes.second + 1) && closest_points.size() < n; i++)
	{
		closest_points.push(i);
		closest_points_set.insert(i);
	}
	bool after = true;
	while (closest_points.size() < n)
	{
		if (indexes.second == (points.size() - 1))
		{
			if (indexes.first == 0) return std::vector<Point2D>(points);
			indexes.first--;
			closest_points.push(indexes.first);
			closest_points_set.insert(indexes.first);
		}
		else {
			if (indexes.first == 0 || after) {
				indexes.second++;
				closest_points.push(indexes.second);
				closest_points_set.insert(indexes.second);
				after = false;
				continue;
			}
			indexes.first--;
			closest_points.push(indexes.first);
			closest_points_set.insert(indexes.first);
			after = true;
		}
	}
	if (indexes.second == (points.size() - 1) && indexes.first == 0) return std::vector<Point2D>(points);


	double x_width = x_max;
	double y_width = y_max;

	struct to_search {
		uint64_t zindex;
		uint32_t level;
		bool x_axis;
	};
	std::vector<to_search> heap;
	heap.push_back(to_search{ 0, 2, true });

	//uint64_t ii = 0;
	while (!heap.empty()) {
		//ii++;
		to_search current_element = heap.back();
		heap.pop_back();


		if (current_element.level == pow(2, order + 1)) {
			uint64_t hi = mortonToHilbert(current_element.zindex);
			auto indexes = get_points_from_hilbertindex(hi);
			if (indexes == std::pair<uint64_t, uint64_t>(1, 0)) continue;

			for (uint64_t i = indexes.first; i <= indexes.second; i++)
			{
				if (points[i].dist(point) < points[closest_points.top()].dist(point))
				{
					if (closest_points_set.find(i) == closest_points_set.end())
					{
						closest_points.pop();
						closest_points.push(i);
					}
				}
			}
			continue;
		}

		double dist;
		uint32_t next_level = current_element.level;
		uint32_t x, y;
		uint32_t pos = 1;
		if (current_element.x_axis)
		{
			x = _pext_u64(current_element.zindex, UINT64_C(0x5555555555555555) << 1);
			if (x) pos = 1 + x * 2;
			dist = (x_width * pos / current_element.level) - point.getX();

		}
		else
		{
			y = _pext_u64(current_element.zindex, UINT64_C(0x5555555555555555) << 1);
			if (y) pos = 1 + y * 2;
			next_level *= 2;
			dist = (y_width * pos / current_element.level) - point.getY();
		}

		if (abs(dist) > points[closest_points.top()].dist(point))
		{

			if (dist < 0)
			{
				heap.push_back(to_search{ (current_element.zindex << 1 | 1), next_level, !current_element.x_axis });
			}
			else
			{
				heap.push_back(to_search{ (current_element.zindex << 1), next_level, !current_element.x_axis });
			}

		}
		else {
			heap.push_back(to_search{ (current_element.zindex << 1 | 1), next_level, !current_element.x_axis });
			heap.push_back(to_search{ (current_element.zindex << 1), next_level,  !current_element.x_axis });
		}

	}
	//std::cout << "ii:" << ii << "\n";

	std::vector<Point2D> ret(n);
	for (i = n; i > 0; i--) {
		//std::cout << closest_points.top() << "\n";
		ret[i - 1] = points[closest_points.top()];
		closest_points.pop();
	}

	return ret;
}

std::vector<Point2D> HilbertCurve2D_Fixed::get_points_in_range(Point2D point, double dist_max)
{
	checkPoint(point);

	std::vector<Point2D> ret;

	double x_width = x_max;
	double y_width = y_max;

	struct to_search {
		uint64_t zindex;
		uint32_t level;
		bool x_axis;
	};
	std::vector<to_search> heap;
	heap.push_back(to_search{ 0, 2, true });
	//uint64_t ii = 0;
	while (!heap.empty()) {
		//ii++;
		to_search current_element = heap.back();
		heap.pop_back();
		if (current_element.level == pow(2, order + 1)) {
			uint64_t hi = mortonToHilbert(current_element.zindex);
			auto indexes = get_points_from_hilbertindex(hi);
			if (indexes == std::pair<uint64_t, uint64_t>(1, 0)) continue;

			for (uint64_t i = indexes.first; i <= indexes.second; i++)
			{
				if (points[i].dist(point) < dist_max) ret.push_back(points[i]);
			}
			continue;
		}

		double dist;
		uint32_t next_level = current_element.level;
		uint32_t x, y;
		uint32_t pos = 1;
		if (current_element.x_axis)
		{
			x = _pext_u64(current_element.zindex, UINT64_C(0x5555555555555555) << 1);
			if (x) pos = 1 + x * 2;
			dist = (x_width * pos / current_element.level) - point.getX();

		}
		else
		{
			y = _pext_u64(current_element.zindex, UINT64_C(0x5555555555555555) << 1);
			if (y) pos = 1 + y * 2;
			next_level *= 2;
			dist = (y_width * pos / current_element.level) - point.getY();
		}

		if (abs(dist) > dist_max)
		{

			if (dist < 0)
			{
				heap.push_back(to_search{ (current_element.zindex << 1 | 1), next_level, !current_element.x_axis });
			}
			else
			{
				heap.push_back(to_search{ (current_element.zindex << 1), next_level, !current_element.x_axis });
			}

		}
		else {
			heap.push_back(to_search{ (current_element.zindex << 1 | 1), next_level, !current_element.x_axis });
			heap.push_back(to_search{ (current_element.zindex << 1), next_level,  !current_element.x_axis });
		}

	}
	//std::cout << "ii:" << ii << "\n";
	return ret;
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
	x = (point.getX()) * x_factor;
	y = (point.getY()) * y_factor;
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
