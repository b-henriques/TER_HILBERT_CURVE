#include "PointsGeneratorBuilder.h"

//TODO: check if input params are correct

UniformPointsGenerator PointsGeneratorBuilder::mk_uniform(Point2D topRight)
{
	return UniformPointsGenerator(topRight.getX(), topRight.getY());
}

GammaPointsGenerator PointsGeneratorBuilder::mk_gamma(Point2D topRight, std::pair<double, double> x_params, std::pair<double, double> y_params)
{
	return GammaPointsGenerator(
		topRight.getX(), topRight.getY(),
		x_params.first, x_params.second,
		y_params.first, y_params.second
	);
}

PiecewiseConstantPointsGenerator PointsGeneratorBuilder::mk_piecewise_constant(std::pair<std::vector<double>, std::vector<double>> x_params, std::pair<std::vector<double>, std::vector<double>> y_params)
{
	return PiecewiseConstantPointsGenerator(
		x_params.first, x_params.second,
		y_params.first, y_params.second
	);
}
