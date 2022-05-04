#pragma once
#include "UniformPointsGenerator.h"
#include "GammaPointsGenerator.h"
#include "PiecewiseConstantPointsGenerator.h"
class PointsGeneratorBuilder
{
public:
	UniformPointsGenerator mk_uniform(Point2D topRight);
	GammaPointsGenerator mk_gamma(Point2D topRight, std::pair<double, double>x_params, std::pair<double, double>y_params);
	PiecewiseConstantPointsGenerator mk_piecewise_constant(
		std::pair<std::vector<double>, std::vector<double>>x_params,
		std::pair<std::vector<double>, std::vector<double>>y_params
	);
};

