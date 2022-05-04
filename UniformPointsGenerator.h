#pragma once
#include "PointsGenerator.h"


class UniformPointsGenerator :public PointsGenerator
{
public:

	UniformPointsGenerator(double _x_max, double _y_max);

	virtual std::vector<Point2D> generatePoints(uint64_t n, unsigned int nthreads);

	void genPoints(
		std::vector<Point2D>& output,
		uint64_t start, uint64_t end,
		std::uniform_real_distribution<double>& distx,
		std::uniform_real_distribution<double>& disty,
		std::default_random_engine& gen
	);



private:
	std::uniform_real_distribution<double> distributionX;
	std::uniform_real_distribution<double> distributionY;


};

