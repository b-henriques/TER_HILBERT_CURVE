#pragma once
#include "HilbertCurve2D.h"
class HilbertCurve2D_Fixed :
	public HilbertCurve2D
{
public:
	HilbertCurve2D_Fixed(std::vector<Point2D>& _points, uint32_t _order, Point2D _bl, Point2D _tr, int nthreads);
	Point2D get_mappedPoint(Point2D point);
	uint64_t get_MortonIndex(Point2D point);
	uint64_t get_HilbertIndex(Point2D point);
	std::vector<Point2D> get_points_from_hilbertindex(uint64_t hilbertindex);

private:

	double calulateFactor(double vmin, double vmax, int k);
	void mapCellX(
		std::vector<Point2D>& points, uint32_t start, uint32_t end, //source
		std::vector<uint32_t>& xmap, //dst where mapped values will be stored
		double vmin, double f
	);
	void mapCellY(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& xmap, double vmin, double f);
	void map_points_hi(std::vector<Point2D>& points, uint32_t start, uint32_t end, std::vector<uint32_t>& xmap, std::vector<uint32_t>& ymap, std::vector<uint64_t>& quadrant_points);
	void genQuadrants(std::vector<Point2D>& points, int nbthreads);
	
	void genQuadrantsWorker(
		std::vector<Point2D>& points, uint32_t start, uint32_t end, //workload
		std::vector<uint32_t>& xmap, double xmin, double xf,        //xmap
		std::vector<uint32_t>& ymap, double ymin, double yf,        //ymap
		std::vector<uint64_t>& quadrant_points          //output
	);
	void gatheroutput(std::vector<Point2D>& points, uint64_t start, uint64_t end, std::vector<uint64_t> outputs);
};

