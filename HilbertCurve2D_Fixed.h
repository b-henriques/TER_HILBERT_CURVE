#pragma once
#include "HilbertCurve2D.h"
#include<atomic>
#include<unordered_map>

class HilbertCurve2D_Fixed : public HilbertCurve2D
{
public:

	HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, double _x_max, double _y_max, uint32_t _nb_threads = 1);


	virtual std::pair<uint32_t, uint32_t> get_mappedPoint(Point2D point);
	virtual uint64_t get_MortonIndex(Point2D point);
	virtual uint64_t get_HilbertIndex(Point2D point);

	virtual std::vector<Point2D> get_n_closest(Point2D point, uint32_t n);
	virtual std::vector<Point2D> get_points_in_range(Point2D point, double dist_max);


private:
	double x_factor;
	double y_factor;

	double calulateFactor(double vmin, double vmax, int k);

	void mapCellXY(Point2D point, uint32_t& x, uint32_t& y);

	struct Quadrants_info {
		std::atomic<std::uint64_t> nbpoints;
		std::atomic<std::uint64_t> start_pos;
	};

	void mapPoints2HilbertIndex(
		uint64_t start, uint64_t end,
		std::vector<Quadrants_info>& quadrants_info,
		std::vector<uint64_t>& hi_map
	);
	void generateQuadrants(
		uint64_t hi_start, uint64_t hi_end,
		std::vector<Quadrants_info>& quadrants_info
	);
	void orderPoints(
		uint64_t start, uint64_t end,
		std::vector<Quadrants_info>& quadrants_info,
		const std::vector<uint64_t>& hi_map,
		const std::vector<Point2D>& tmpPoints
	);

	void generateCurve();

};

