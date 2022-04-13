#include "HilbertCurve2D_Fixed.h"
#include <thread>
#include <functional>
#include<iostream>

HilbertCurve2D_Fixed::HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, int nthreads) : HilbertCurve2D(_order, _bl, _tr)
{
	genQuadrants(_points, nthreads);
	
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

void HilbertCurve2D_Fixed::mapCellX(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& xmap, double vmin, double f)
{
	for (int i = start; i < end; i++) {
		xmap[i] = (points[i].getX() - vmin) * f;
	}
}

void HilbertCurve2D_Fixed::mapCellY(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& ymap, double vmin, double f)
{
	for (int i = start; i < end; i++) {
		ymap[i] = (points[i].getY() - vmin) * f;
	}
}

void HilbertCurve2D_Fixed::points_to_quadrants(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& xmap, std::vector<uint32_t>& ymap, std::vector<std::vector<Point2D>>& quadrant_points)
{
	quadrant_points = std::vector<std::vector<Point2D>>(pow(4, order));
	for (int i = start; i < end; i++) {
		//quadrant_points[hilbertindex].pushback(point)
		quadrant_points[
			mortonToHilbert(
				coords_to_mortonindex(xmap[i], ymap[i])
			)
		].push_back(points[i]);
	}
}

void HilbertCurve2D_Fixed::genQuadrants(std::vector<Point2D>& points, int nbthreads)
{
	double xf = calulateFactor(bottomLeft.getX(), topRight.getX(), order);
	double yf = calulateFactor(bottomLeft.getY(), topRight.getY(), order);
	std::vector<uint32_t> xmap(points.size());
	std::vector<uint32_t> ymap(points.size());

	std::vector<std::vector<std::vector<Point2D>>> thread_output(nbthreads);

	std::vector<std::thread> threads;
	uint32_t interval = points.size() / nbthreads;
	for (int t = 0; t < nbthreads; t++)
	{
		uint32_t start = t * interval;
		uint32_t end = (t + 1) * interval;
		if (t == nbthreads - 1)
		{
			end = points.size();
		}
		threads.push_back(
			std::thread(
				&HilbertCurve2D_Fixed::genQuadrantsWorker, this,
				std::ref(points), start, end,
				std::ref(xmap), bottomLeft.getX(), xf,
				std::ref(ymap), bottomLeft.getY(), yf,
				std::ref(thread_output[t])
			)
		);
	}
	for (int t = 0; t < nbthreads; t++)
	{
		threads[t].join();
	}

	uint64_t hi_interval = quadrants.size() / nbthreads;
	for (int t = 0; t < nbthreads; t++)
	{
		uint32_t start = t * hi_interval;
		uint32_t end = (t + 1) * hi_interval;
		if (t == nbthreads - 1)
		{
			end = quadrants.size();
		}
		threads[t] =
			std::thread(
				&HilbertCurve2D_Fixed::gatheroutput, this,
				start, end,
				std::ref(thread_output)
			)
			;
	}

	for (int t = 0; t < nbthreads; t++)
	{
		threads[t].join();
	}
}

void HilbertCurve2D_Fixed::genQuadrantsWorker(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& xmap, double xmin, double xf, std::vector<uint32_t>& ymap, double ymin, double yf, std::vector<std::vector<Point2D>>& quadrant_points)
{
	mapCellX(points, start, end, xmap, xmin, xf);
	mapCellY(points, start, end, ymap, ymin, yf);
	points_to_quadrants(
		points, start, end,
		xmap, ymap,
		quadrant_points
	);
}

void HilbertCurve2D_Fixed::gatheroutput(uint64_t start, uint64_t end, std::vector<std::vector<std::vector<Point2D>>> outputs)
{
	for (int t = 0; t < outputs.size(); t++)
	{
		for (int pts = start; pts < end; pts++)
		{
			quadrants[pts].addPoints(outputs[t][pts]);
		}
	}
}


