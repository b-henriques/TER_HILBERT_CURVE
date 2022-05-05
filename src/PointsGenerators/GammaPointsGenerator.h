#pragma once
#include "PointsGenerator.h"
#include <random>


class GammaPointsGenerator : public PointsGenerator
{
public:
	GammaPointsGenerator(double _max_x, double _max_y, double a_x, double b_x, double a_y, double b_y);

	virtual std::vector<Point2D> generatePoints(uint64_t n, unsigned int nthreads);

	void genPoints(
		std::vector<Point2D>& output,
		uint64_t start, uint64_t end,
		std::gamma_distribution<double>& distx,
		std::gamma_distribution<double>& disty,
		std::default_random_engine& gen
	);

private:
	std::gamma_distribution<double> distributionX;
	std::gamma_distribution<double> distributionY;
	double max_x;
	double max_y;
};

