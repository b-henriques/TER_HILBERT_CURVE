#include "UniformPointsGenerator.h"
#include<thread>
#include <functional>
#include<random>

UniformPointsGenerator::UniformPointsGenerator(double _x_max, double _y_max) : PointsGenerator()
{
	if (_x_max <= 0 || _y_max <= 0)
	{
		throw std::runtime_error("x, y values must be > 0");
	}

	distributionX = std::uniform_real_distribution<double> (0, _x_max);
	distributionY = std::uniform_real_distribution<double> (0, _y_max);
}

std::vector<Point2D> UniformPointsGenerator::generatePoints(uint64_t n, unsigned int nthreads)
{
	std::vector<Point2D> vec(n);
	if (nthreads > 1) {
		std::vector<std::uniform_real_distribution<double>> distx;
		std::vector<std::uniform_real_distribution<double>> disty;
		std::vector<std::default_random_engine> gen;

		for (unsigned int i = 1; i < nthreads; i++)
		{
			distx.push_back(std::uniform_real_distribution<double>(distributionX));
			disty.push_back(std::uniform_real_distribution<double>(distributionY));
			gen.push_back(std::default_random_engine(generator));
		}

		std::vector<std::thread> threads;
		uint64_t points_interval = n / nthreads;
		for (unsigned int t = 0; t < nthreads-1; t++)
		{
			uint64_t start = t * points_interval;
			uint64_t end = (t + 1) * points_interval;
			threads.push_back(
				std::thread(&UniformPointsGenerator::genPoints, this, 
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

		for (unsigned int t = 0; t < nthreads-1; t++)
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

void UniformPointsGenerator::genPoints(std::vector<Point2D>& output, uint64_t start, uint64_t end, std::uniform_real_distribution<double>& distx, std::uniform_real_distribution<double>& disty, std::default_random_engine& gen)
{
	for (uint64_t i = start; i < end; i++) {
		output[i] = Point2D(distx(gen), disty(gen));
	}

}
