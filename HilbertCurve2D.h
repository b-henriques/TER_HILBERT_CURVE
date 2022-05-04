#pragma once
#include "Point2D.h"
#include <cstdint> /*uint*/
#include <vector>

//https://docs.microsoft.com/fr-fr/cpp/parallel/concrt/parallel-containers-and-objects?view=msvc-170#unordered_map
#include <ppl.h>
#include <concurrent_unordered_map.h>

class HilbertCurve2D
{
	// inspired by https://en.wikipedia.org/wiki/Hilbert_curve
	// 
	// HilbertCurve2D represents a pseudo-Hilbert Curve of order n, mapping from 2d to 1d
	// order must be specified at object creation
	// we assume 0,0 pos at bottom left corner
	/* initial order = 1  ==> 4 squares
	*   ____ ____
	*  | 01 | 10 |
	*  |____|____|
	*  | 00 | 11 |
	*  |____|____|
	*
	* order n => 2^(n) * 2^(n) squares
	*/
	/* maximum order = 32, 1d index is an unsigned long long int(we assume 64 bits) so
	* (order)*2 = 64 => order = 32
	*/
public:
	//gives hilbert index of cell x,y
	uint64_t coords_to_hilbertindex(uint32_t x, uint32_t y);

	//gives x,y of hilbert index i
	void hilbertindex_to_coord(uint64_t index, uint32_t& x, uint32_t& y);

	//gives morton index of cell x,y
	uint64_t coords_to_mortonindex(uint32_t x, uint32_t y);

	//gives cell coordinates x,y of morton index i
	void mortonindex_to_coord(uint64_t index, uint32_t& x, uint32_t& y);

	//gives hilbert index from morton index
	uint64_t mortonToHilbert(uint64_t zorder);

	//returns pair(start, end) representing range of indexes in array containing points
	//returns (1,0) if no points associated with hilbertindex
	std::pair<uint64_t, uint64_t> get_points_from_hilbertindex(uint64_t hilbertindex);


	///////////////////
	//VIRTUAL METHODS//
	//////////////////

	//returns pair of mapped coords
	virtual std::pair<uint32_t, uint32_t> get_mappedPoint(Point2D point) = 0;

	//returns zindex of point
	virtual uint64_t get_MortonIndex(Point2D point) = 0;

	//returns hilbert index of point
	virtual uint64_t get_HilbertIndex(Point2D point) = 0;


	//SEARCH METHODS//
	
	//returns vector of n closest points sorted by dist, user must verify vector size on return as it may not exist enough points
	virtual std::vector<Point2D> get_n_closest(Point2D point, uint32_t n) = 0;
	//returns vector of points in range
	virtual std::vector<Point2D> get_points_in_range(Point2D point, double dist_max) = 0;

	//constructors
	HilbertCurve2D(uint32_t _order, double _x_max, double _y_max, std::vector<Point2D>& _points, uint32_t _nb_threads = 1);
	//destructor
	~HilbertCurve2D() = default;


protected:
	uint32_t order{ 1 };
	double x_max{10.0};
	double y_max{ 10.0 };
	std::vector<Point2D>& points;
	uint32_t nb_threads;

	struct Quadrant {
		uint64_t start;
		uint64_t end;

		Quadrant(int _s, int _e) : start(_s), end(_e) {};
	};


	concurrency::concurrent_unordered_map<uint64_t, Quadrant> quadrants;

	void checkPoint(Point2D point);
	void checkXY(uint32_t x, uint32_t y);
	void checkHilberIndex(uint64_t hi);


private:

	static constexpr uint32_t base_pattern[4][4] = {
	{0,1,3,2},
	{0,3,1,2},
	{2,3,1,0},
	{2,1,3,0}
	};

	static constexpr uint32_t configuration[4][4] = {
		{1,0,3,0},
		{0,2,1,1},
		{2,1,2,3},
		{3,3,0,2}
	};
};



/*#include <pybind11/pybind.h>
PYBIND11_MODULE(example, m) {
	py::class_<Nomdelaclasse>(m, "Nomdelaclasse")
		.def("nomfonction", &NomClasse::NomFonction);
	}
*/
