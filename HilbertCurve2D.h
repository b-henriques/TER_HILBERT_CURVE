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



unsigned pattern_de_base [4] [4] = {
	{0,1,3,2},
	{0,3,1,2},
	{2,3,1,0},
	{2,1,3,0}};

unsigned configuration [4] [4] = {
	{1,0,3,0},
	{0,2,1,1},
	{2,1,2,3},
	{3,3,0,2}};

unsigned int morton_Hilbert(unsigned int zorder, unsigned k){
	unsigned config = 0;
	unsigned int hilbert = 0;
	for (int i=k-1; i>=0; i--){
		unsigned quadrant = zorder>>(2*i) & 3;
		hilbert  = hilbert<<2 | base_pattern[config][quadrant];
		config = configuration[config][quadrant];
	}
	return hilbert;
}
