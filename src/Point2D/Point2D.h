#pragma once
#include <vector>

class Point2D
{
public:
	Point2D() : x(0.0), y(0.0) {}
	Point2D(double _x, double _y) :x(_x), y(_y) {}

	double getX();
	double getY();
	void setX(double _x);
	void setY(double _y);

	double dist(Point2D point);

private:
	double x;
	double y;
};

