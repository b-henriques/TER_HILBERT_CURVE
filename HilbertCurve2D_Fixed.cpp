#include "HilbertCurve2D_Fixed.h"
#include <thread>
#include <math.h> 
#include <functional> /*ref*/
#include <algorithm> /*sort*/
#include <iterator> /*distance*/
#include <random>
#include <chrono> 
#include<iostream>

HilbertCurve2D_Fixed::HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads)
	: HilbertCurve2D(_order, _bl, _tr, _points, _nb_threads)
{
	x_factor = calulateFactor(bottomLeft.getX(), topRight.getX(), order);
	y_factor = calulateFactor(bottomLeft.getY(), topRight.getY(), order);

	generateCurve();
	//std::cout << "========================" << std::endl;
	//std::cout << quadrants.size() << std::endl;
	//for (auto q : quadrants)
	//{
	//	std::cout << q.first << "(" << q.second.start << "," << q.second.end << ")" << std::endl;
	//	for (int k = q.second.start; k <= q.second.end; k++)
	//	{
	//		std::cout << "---(" << points[k].getX() << "," << points[k].getY() << ")" << std::endl;
	//	}
	//}
}

Point2D HilbertCurve2D_Fixed::get_mappedPoint(Point2D point)
{
	return Point2D();
}

uint64_t HilbertCurve2D_Fixed::get_MortonIndex(Point2D point)
{
	return uint64_t();
}

uint64_t HilbertCurve2D_Fixed::get_HilbertIndex(Point2D point)
{
	return uint64_t();
}

std::vector<Point2D> HilbertCurve2D_Fixed::get_points_from_hilbertindex(uint64_t hilbertindex)
{
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

void HilbertCurve2D_Fixed::mapPoints2HilbertIndex(uint64_t start, uint64_t end, std::vector<std::atomic<std::uint64_t>>& nbPointsPerQuadrant, std::vector<uint64_t>& hi_map, std::vector< std::atomic<uint64_t>>& threads_hi_start_indexes)
{
	uint32_t x;
	uint32_t y;
	uint64_t hi;
	std::unordered_map<uint64_t, uint32_t > local_nbPointsPerQuadrant;
	for (uint64_t i = start; i < end; i++) {

		mapCellXY(points[i], x, y);
		hi = mortonToHilbert(coords_to_mortonindex(x, y));
		hi_map[i] = hi;
		//if entry hi already in map increment else insert
		if (local_nbPointsPerQuadrant.find(hi) != local_nbPointsPerQuadrant.end())
		{
			local_nbPointsPerQuadrant[hi] += 1;
			continue;
		}
		local_nbPointsPerQuadrant[hi] = 1;
	}

	uint64_t hi_interval = pow(4, order) / nb_threads;
	for (auto pair = local_nbPointsPerQuadrant.begin(); pair != local_nbPointsPerQuadrant.end(); pair++)
	{
		nbPointsPerQuadrant[pair->first] += pair->second;
		uint64_t associated_thread = (pair->first) / hi_interval + 1;
		for (uint64_t s = associated_thread; s < nb_threads; s++)
		{
			threads_hi_start_indexes[s] += pair->second;
		}
	}
}

void HilbertCurve2D_Fixed::generateQuadrants(uint64_t hi_start, uint64_t hi_end, std::vector<std::atomic<std::uint64_t>>& nbPointsPerQuadrant, uint64_t start_index, std::mutex& quadrants_mutex)
{
	uint64_t i;
	uint64_t pos = start_index;
	for (i = hi_start; i < hi_end; i++) {
		uint64_t nbpts = nbPointsPerQuadrant[i];
		if (nbpts)
		{
			quadrants_mutex.lock();
			quadrants.insert(std::pair<uint64_t, HilbertCurve2D::Quadrant>(
				i,
				HilbertCurve2D::Quadrant(pos, pos + nbpts - 1)
				)
			);
			quadrants_mutex.unlock();
			pos += nbpts;
		}
	}
}


void HilbertCurve2D_Fixed::generateCurve()
{
	uint32_t t;
	std::vector<uint64_t> hi_map(points.size());

	std::vector<std::thread> threads;
	uint64_t interval = points.size() / nb_threads;
	std::vector<std::atomic<std::uint64_t>> nbPointsPerQuadrant(pow(4, order));
	std::vector< std::atomic<uint64_t>> threads_hi_start_indexes(nb_threads);

	for (t = 0; t < nb_threads; t++)
	{
		uint64_t start = t * interval;
		uint64_t end = (t + 1) * interval;
		if (t == nb_threads - 1)
		{
			end = points.size();
		}
		threads.push_back(
			std::thread(&HilbertCurve2D_Fixed::mapPoints2HilbertIndex, this, start, end,
				std::ref(nbPointsPerQuadrant),
				std::ref(hi_map),
				std::ref(threads_hi_start_indexes)
			)
		);
	}
	for (t = 0; t < nb_threads; t++)
	{
		threads[t].join();
	}
	std::cout << "mapped\n";
	std::mutex quadrants_mutex;

	uint64_t hi_interval = pow(4, order) / nb_threads;
	for (t = 0; t < nb_threads; t++)
	{
		uint64_t hi_start = t * hi_interval;
		uint64_t hi_end = (t + 1) * hi_interval;
		if (t == nb_threads - 1)
		{
			hi_end = pow(4, order);
		}
		uint64_t start_index = threads_hi_start_indexes[t];
		threads[t] = std::thread(
			&HilbertCurve2D_Fixed::generateQuadrants, this,
			hi_start, hi_end,
			std::ref(nbPointsPerQuadrant),
			start_index,
			std::ref(quadrants_mutex)
		);
	}

	for (t = 0; t < nb_threads; t++)
	{
		threads[t].join();
	}
	std::cout << "quadrants generated \n";
	quick_sort(0, points.size() - 1, hi_map, nb_threads);
	std::cout << "sorted\n";

}

void HilbertCurve2D_Fixed::quick_sort(uint64_t start, uint64_t end, std::vector<uint64_t>& hi_map, uint32_t nbthreads)
{
	if (end > start)
	{
		//pick random uniform pivot
		uint64_t pivot_index = pickPivot(start, end);
		//place pivot at start
		swap(pivot_index, start, hi_map);
		pivot_index = start;

		// A pivot B with point in A if point.x < pivot.x ...
		for (uint64_t k = start + 1; k <= end; k++)
		{
			if (hi_map[k] <= hi_map[pivot_index])
			{
				swap(k, pivot_index + 1, hi_map);
				swap(pivot_index, pivot_index + 1, hi_map);
				pivot_index++;
			}
		}
		if (pivot_index == end) {
			quick_sort(start, pivot_index - 1, hi_map, nbthreads);
			return;
		}
		if (pivot_index == start) {
			quick_sort(start + 1, end, hi_map, nbthreads);
			return;
		}

		if (nbthreads > 1) {
			std::thread t = std::thread(&HilbertCurve2D_Fixed::quick_sort, this,
				pivot_index + 1, end,
				std::ref(hi_map),
				nbthreads - 1
			);
			quick_sort(start, pivot_index - 1, hi_map, nbthreads - 1);
			t.join();
		}
		else {
			quick_sort(start, pivot_index - 1, hi_map, nbthreads);
			quick_sort(pivot_index + 1, end, hi_map, nbthreads);
		}
	}
}

uint64_t HilbertCurve2D_Fixed::pickPivot(uint64_t start, uint64_t end)
{
	//pivot tire aleatirement uniforme
	uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<uint64_t> distribution(start, end);
	return distribution(generator);
}

void HilbertCurve2D_Fixed::swap(uint64_t i, uint64_t j, std::vector<uint64_t>& hi_map)
{
	Point2D tmp = points[i];
	uint64_t hitmp = hi_map[i];
	points[i] = points[j];
	hi_map[i] = hi_map[j];
	points[j] = tmp;
	hi_map[j] = hitmp;
}