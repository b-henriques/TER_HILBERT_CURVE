#pragma once
#include "Point2D.h"
#include <cstdint> /*uint*/
#include <unordered_map>
#include <vector>

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
	/* maximum order = 16, 1d index is an unsigned int(we assume 32 bits) so
	* (order)*2 = 32 => order = 16
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

	virtual Point2D get_mappedPoint(Point2D point) = 0;
	virtual uint64_t get_MortonIndex(Point2D point) = 0;
	virtual uint64_t get_HilbertIndex(Point2D point) = 0;
	virtual std::vector<Point2D> get_points_from_hilbertindex(uint64_t hilbertindex) = 0;

	//TODO:
	/*
	* get_n_closest_points(point)
	* get_points_in_range(point, dist_max)
	*/

	//constructors
	HilbertCurve2D(uint32_t _order, Point2D _bl, Point2D _tr, std::vector<Point2D>& _points, uint32_t _nb_threads = 1);
	//destructor
	~HilbertCurve2D() = default;


protected:
	uint32_t order{ 1 };
	Point2D bottomLeft{ 0,0 };
	Point2D topRight{ 10, 10 };
	std::vector<Point2D>& points;
	uint32_t nb_threads;

	struct Quadrant {
		int start;
		int end;

		Quadrant(int _s, int _e) : start(_s), end(_e) {};
	};
	std::unordered_map<uint64_t, Quadrant> quadrants;

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
