#pragma once
#include "PointsGenerator.h"
#include <random>
class PiecewiseConstantPointsGenerator : public PointsGenerator
{
public:
	PiecewiseConstantPointsGenerator(
		std::vector<double>x_intervals,
		std::vector<double>x_weights,
		std::vector<double>y_intervals,
		std::vector<double>y_weights
	);

	virtual std::vector<Point2D> generatePoints(uint64_t n, unsigned int nthreads);

	void genPoints(
		std::vector<Point2D>& output,
		uint64_t start, uint64_t end,
		std::piecewise_constant_distribution<double>& distx,
		std::piecewise_constant_distribution<double>& disty,
		std::default_random_engine& gen
	);


private:
	std::piecewise_constant_distribution<double> distributionX;
	std::piecewise_constant_distribution<double> distributionY;
};

