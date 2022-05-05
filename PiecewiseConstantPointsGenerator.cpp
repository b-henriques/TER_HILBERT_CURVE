#include "PiecewiseConstantPointsGenerator.h"
#include<thread>
#include<functional>

PiecewiseConstantPointsGenerator::PiecewiseConstantPointsGenerator(std::vector<double> x_intervals, std::vector<double> x_weights, std::vector<double> y_intervals, std::vector<double> y_weights) : PointsGenerator()
{
	distributionX = std::piecewise_constant_distribution<double>(x_intervals.begin(), x_intervals.end(), x_weights.begin());
	distributionY = std::piecewise_constant_distribution<double>(y_intervals.begin(), y_intervals.end(), y_weights.begin());
}

std::vector<Point2D> PiecewiseConstantPointsGenerator::generatePoints(uint64_t n, unsigned int nthreads)
{
	std::vector<Point2D> vec(n);
	if (nthreads > 1) {
		std::vector<std::piecewise_constant_distribution<double>> distx;
		std::vector<std::piecewise_constant_distribution<double>> disty;
		std::vector<std::default_random_engine> gen;

		unsigned seed;
		for (unsigned int i = 1; i < nthreads; i++)
		{
			seed = std::chrono::system_clock::now().time_since_epoch().count() + i;
			distx.push_back(std::piecewise_constant_distribution<double>(distributionX));
			disty.push_back(std::piecewise_constant_distribution<double>(distributionY));
			gen.push_back(std::default_random_engine(seed));
		}

		std::vector<std::thread> threads;
		uint64_t points_interval = n / nthreads;
		for (unsigned int t = 0; t < nthreads - 1; t++)
		{
			uint64_t start = t * points_interval;
			uint64_t end = (t + 1) * points_interval;
			threads.push_back(
				std::thread(&PiecewiseConstantPointsGenerator::genPoints, this,
					std::ref(vec),
					start, end,
					std::ref(distx[t]),
					std::ref(disty[t]),
					std::ref(gen[t])
				)
			);
		}

		for (int i = (nthreads - 1) * points_interval; i < n; i++)
		{
			vec[i] = Point2D(distributionX(generator), distributionY(generator));
		}

		for (unsigned int t = 0; t < nthreads - 1; t++)
		{
			threads[t].join();
		}

		return vec;
	}
	for (int i = 0; i < n; i++)
	{
		vec[i] = Point2D(distributionX(generator), distributionY(generator));
	}
	return vec;
}

void PiecewiseConstantPointsGenerator::genPoints(std::vector<Point2D>& output, uint64_t start, uint64_t end, std::piecewise_constant_distribution<double>& distx, std::piecewise_constant_distribution<double>& disty, std::default_random_engine& gen)
{
}
