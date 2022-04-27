#pragma once
#include "HilbertCurve2D.h"
#include <atomic>
#include <mutex>
class HilbertCurve2D_Fixed :
	public HilbertCurve2D
{
public:
	HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, uint32_t _nb_threads = 1);
	Point2D get_mappedPoint(Point2D point);
	uint64_t get_MortonIndex(Point2D point);
	uint64_t get_HilbertIndex(Point2D point);
	std::vector<Point2D> get_points_from_hilbertindex(uint64_t hilbertindex);

private:
	double x_factor;
	double y_factor;

	double calulateFactor(double vmin, double vmax, int k);

	void mapCellXY(Point2D point, uint32_t& x, uint32_t& y);

	void mapPoints2HilbertIndex(
		uint64_t start, uint64_t end,
		std::vector<std::atomic<std::uint64_t>>& nbPointsPerQuadrant,
		std::vector<uint64_t>& hi_map,
		std::vector< std::atomic<uint64_t>>& threads_hi_start_indexes
	);
	void generateQuadrants(
		uint64_t hi_start, uint64_t hi_end,
		std::vector<std::atomic<std::uint64_t>>& nbPointsPerQuadrant,
		uint64_t start_index,
		std::mutex &quadrants_mutex
	);



	void generateCurve();

	void quick_sort(uint64_t start, uint64_t end, std::vector<uint64_t>& hi_map, uint32_t nbthreads = 1);
	uint64_t pickPivot(uint64_t start, uint64_t end);
	void swap(uint64_t i, uint64_t j, std::vector<uint64_t>& hi_map);

};

