#include "GammaPointsGenerator.h"
#include<thread>
#include<functional>

GammaPointsGenerator::GammaPointsGenerator(double _max_x, double _max_y, double a_x, double b_x, double a_y, double b_y) : PointsGenerator()
{
	
	distributionX = std::gamma_distribution<double>(a_x, b_x);
	distributionY = std::gamma_distribution<double>(a_y, b_y);
	max_x = _max_x;
	max_y = _max_y;
}

std::vector<Point2D> GammaPointsGenerator::generatePoints(uint64_t n, unsigned int nthreads)
{
	std::vector<Point2D> vec(n);
	if (nthreads > 1) {
		std::vector<std::gamma_distribution<double>> distx;
		std::vector<std::gamma_distribution<double>> disty;
		std::vector<std::default_random_engine> gen;

		unsigned seed;
		for (unsigned int i = 1; i < nthreads; i++)
		{
			seed = std::chrono::system_clock::now().time_since_epoch().count() + i;
			distx.push_back(std::gamma_distribution<double>(distributionX));
			disty.push_back(std::gamma_distribution<double>(distributionY));
			gen.push_back(std::default_random_engine(seed));
		}

		std::vector<std::thread> threads;
		uint64_t points_interval = n / nthreads;
		for (unsigned int t = 0; t < nthreads - 1; t++)
		{
			uint64_t start = t * points_interval;
			uint64_t end = (t + 1) * points_interval;
			threads.push_back(
				std::thread(&GammaPointsGenerator::genPoints, this,
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
		auto p = Point2D(distributionX(generator), distributionY(generator));
		while (p.getX() > max_x || p.getY() > max_y) {
			p = Point2D(distributionX(generator), distributionY(generator));
		}
		vec[i] = p;
	}
	return vec;
}

void GammaPointsGenerator::genPoints(std::vector<Point2D>& output, uint64_t start, uint64_t end, std::gamma_distribution<double>& distx, std::gamma_distribution<double>& disty, std::default_random_engine& gen)
{
	for (uint64_t i = start; i < end; i++) {
		auto p = Point2D(distx(gen), disty(gen));
		while (p.getX() > max_x || p.getY() > max_y) {
			p = Point2D(distx(gen), disty(gen));
		}
		output[i] = p;
	}
}
