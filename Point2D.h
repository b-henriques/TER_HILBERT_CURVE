#pragma once
class Point2D
{
public:
	Point2D(): x(0.0), y(0.0){}
	Point2D(double _x, double _y):x(_x),y(_y){}

	double getX();
	double getY();
	void setX(double _x);
	void setY(double _y);

	/// <summary>
	///  generates n_points with 
	///		x_min&lt;=point.x&lt;x_max  and  
	///		y_min&lt;=point.y&lt;y_max
	/// and stores them in dest_array
	/// </summary>
	/// <param name="n_points"></param>
	/// <param name="x_min"></param>
	/// <param name="y_min"></param>
	/// <param name="x_max"></param>
	/// <param name="y_max"></param>
	static void generatePoints(Point2D* dest_array, int n_points, double x_min, double y_min, double x_max, double y_max);


private:
	double x;
	double y;
};

