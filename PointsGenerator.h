#pragma once
#include"Point2D.h"
#include <vector>
#include <random>
#include <chrono> 

class PointsGenerator
{
public:
	PointsGenerator()
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		generator = std::default_random_engine(seed);
	}
	virtual std::vector<Point2D> generatePoints(uint64_t n, unsigned int nthreads)=0;


protected:
	std::default_random_engine generator;
};