#include "Point2D.h"
#include <random>
#include <chrono>

double Point2D::getX()
{
	return x;
}

double Point2D::getY()
{
	return y;
}

void Point2D::setX(double _x)
{
	x = _x;
}

void Point2D::setY(double _y)
{
	y = _y;
}

void Point2D::generatePoints(Point2D* dest_array, int n_points, double x_min, double y_min, double x_max, double y_max)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<double> distributionX(x_min, x_max);
	std::uniform_real_distribution<double> distributionY(y_min, y_max);

	for (int i = 0; i < n_points; i++)
	{
		dest_array[i] = Point2D(distributionX(generator), distributionY(generator));
	}
}
