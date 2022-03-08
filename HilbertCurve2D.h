#pragma once
class HilbertCurve2D
{
	// inspired by https://en.wikipedia.org/wiki/Hilbert_curve
	// 
	// HilbertCurve2D represents a pseudo-Hilbert Curve of order n, mapping from 2d to 1d
	// order must be specified at object creation
	// we assume 0,0 pos at bottom left corner
	/* initial order = 0  ==> 4 squares
	*   ____ ____
	*  | 01 | 10 |
	*  |____|____|
	*  | 00 | 11 |
	*  |____|____|
	* 
	* order n => 2^(n+1) * 2^(n+1) squares
	*/
	/* maximum order = 15, 1d index is an unsigned int(we assume 32 bits) so
	* (order+1)*2 = 32 => order = 15
	*/
public:
	//gives hilbert index of point x,y
	unsigned int coords_to_index(unsigned int x, unsigned int y);

	//gives x,y of hilbert index i
	void index_to_coord(unsigned int index, unsigned int &x, unsigned int &y);

	//constructors
	HilbertCurve2D(int _order);
	//destructor
	~HilbertCurve2D() = default;

private:
	int order{ 0 };
};

