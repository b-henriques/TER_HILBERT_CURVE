#include "HilbertCurve2D_Adaptive.h"
#include <string> 
#include <algorithm> /*nth_element*/
#include <functional> /*ref*/
#include <iterator>
#include <thread>
#include <math.h> /*ceil*/
#include <unordered_map>
#include <queue>
#include <set>
#include<iostream>

HilbertCurve2D_Adaptive::HilbertCurve2D_Adaptive(std::vector<Point2D>& _points, uint32_t _order, double _x_max, double _y_max, uint32_t _nb_threads) : HilbertCurve2D(_order, _x_max, _y_max, _points, _nb_threads)
{
	tree = std::unique_ptr<Node>(generateTree());
	//std::cout << "\t hi \t start \t end\n";
	//for (auto itr = quadrants.begin(); itr != quadrants.end(); ++itr) {
	//	std::cout << '\t' << itr->first << '\t' << (itr->second.start) << '\t' << (itr->second.end) << '\n';
	//	for (int i = itr->second.start; i <= itr->second.end; i++) {
	//		auto p = points[i];
	//		std::cout << "(" << p.getX() << "," << p.getY() << ")" << std::endl;
	//	}
	//}
	//auto l = tree.get();
	//while (l) {
	//	std::cout << "leaf? " << l->leaf << "value " << l->value << std::endl;
	//	l = l->right.get();
	//}
}

std::pair<uint32_t, uint32_t> HilbertCurve2D_Adaptive::get_mappedPoint(Point2D point)
{
	checkPoint(point);
	uint64_t zindex = get_MortonIndex(point);
	uint32_t x, y;
	mortonindex_to_coord(zindex, x, y);
	return std::pair<uint32_t, uint32_t>(x, y);
}

uint64_t HilbertCurve2D_Adaptive::get_MortonIndex(Point2D point)
{
	checkPoint(point);
	uint64_t zindex = 0;
	uint64_t level = 0;
	Node* node = tree.get();
	while (!node->leaf) {
		if (!level % 2) {
			if (point.getX() > node->value)
			{
				node = (node->right).get();
				zindex = zindex << 1 | 1;
			}
			else {
				node = (node->left).get();
				zindex = zindex << 1;
			}
			level++;
			continue;
		}
		if (point.getY() > node->value)
		{
			node = (node->right).get();
			zindex = zindex << 1 | 1;
		}
		else {
			node = (node->left).get();
			zindex = zindex << 1;
		}
		level++;
	}
	if (level != 2 * order) {
		zindex = zindex << 2 * order - level;
	}
	return zindex;
}

uint64_t HilbertCurve2D_Adaptive::get_HilbertIndex(Point2D point)
{
	checkPoint(point);
	uint64_t zindex = get_MortonIndex(point);
	return mortonToHilbert(zindex);
}


std::vector<Point2D> HilbertCurve2D_Adaptive::get_n_closest(Point2D point, uint32_t n)
{
	checkPoint(point);

	//maxheap
	auto cmp = [this, point](uint64_t left, uint64_t right) { return (points[left].dist(point)) < (points[right].dist(point)); };
	std::priority_queue<uint64_t, std::vector<uint64_t>, decltype(cmp)> closest_points(cmp);
	std::set<uint64_t> closest_points_set;

	uint64_t hi = get_HilbertIndex(point);
	auto indexes = get_points_from_hilbertindex(hi);
	uint64_t i;

	//initialize maxheap with points in quadrant or neighboring quadrants
	for (i = indexes.first; i < (indexes.second + 1) && closest_points.size() < n; i++)
	{
		closest_points.push(i);
		closest_points_set.insert(i);
	}
	bool after = true;
	while (closest_points.size() < n)
	{
		if (indexes.second == (points.size() - 1))
		{
			if (indexes.first == 0) return std::vector<Point2D>(points);
			indexes.first--;
			closest_points.push(indexes.first);
			closest_points_set.insert(indexes.first);
		}
		else {
			if (indexes.first == 0 || after) {
				indexes.second++;
				closest_points.push(indexes.second);
				closest_points_set.insert(indexes.second);
				after = false;
				continue;
			}
			indexes.first--;
			closest_points.push(indexes.first);
			closest_points_set.insert(indexes.first);
			after = true;
		}
	}
	if (indexes.second == (points.size() - 1) && indexes.first == 0) return std::vector<Point2D>(points);

	struct to_search {
		bool level;
		Node* node;
	};

	std::vector<to_search> heap;
	heap.push_back(to_search{ true, tree.get() });

	//uint64_t ii = 0;
	while (!heap.empty()) {
		//ii++;

		to_search current_element = heap.back();
		heap.pop_back();
		if (!((current_element.node)->leaf))
		{
			double dist;
			if (current_element.level) dist = (current_element.node->value) - point.getX();
			else dist = (current_element.node->value) - point.getY();


			if (abs(dist) > points[closest_points.top()].dist(point))
			{
				if (dist < 0 && current_element.node->right != nullptr)
					heap.push_back(to_search{ !current_element.level,((current_element.node)->right).get() });
				else
					heap.push_back(to_search{ !current_element.level,((current_element.node)->left).get() });

			}
			else {
				if (current_element.node->right != nullptr)
					heap.push_back(to_search{ !current_element.level,((current_element.node)->right).get() });
				heap.push_back(to_search{ !current_element.level,((current_element.node)->left).get() });
			}
		}
		else {
			hi = current_element.node->quadrant_index;
			indexes = get_points_from_hilbertindex(hi);
			for (i = indexes.first; i <= indexes.second; i++)
			{
				if (points[i].dist(point) < points[closest_points.top()].dist(point))
				{
					if (closest_points_set.find(i) == closest_points_set.end())
					{
						closest_points.pop();
						closest_points.push(i);
					}
				}
			}
		}
	}

	std::vector<Point2D> ret(n);
	for (i = n; i > 0; i--) {
		ret[i - 1] = points[closest_points.top()];
		closest_points.pop();
	}

	//std::cout << "iiADPT:" << ii << "\n";
	return ret;
}

std::vector<Point2D> HilbertCurve2D_Adaptive::get_points_in_range(Point2D point, double dist_max)
{
	checkPoint(point);
	std::vector<Point2D> ret;
	struct to_search {
		bool level;
		Node* node;
	};
	std::vector<to_search> heap;
	heap.push_back(to_search{ true, tree.get() });
	//uint64_t ii = 0;
	while (!heap.empty()) {
		//ii++;
		to_search current_element = heap.back();
		heap.pop_back();
		if (!((current_element.node)->leaf))
		{
			double dist;
			if (current_element.level) dist = (current_element.node->value) - point.getX();
			else dist = (current_element.node->value) - point.getY();


			if (abs(dist) > dist_max)
			{
				if (dist < 0 && current_element.node->right != nullptr)
					heap.push_back(to_search{ !current_element.level,((current_element.node)->right).get() });
				else
					heap.push_back(to_search{ !current_element.level,((current_element.node)->left).get() });

			}
			else {
				if (current_element.node->right != nullptr)
					heap.push_back(to_search{ !current_element.level,((current_element.node)->right).get() });
				heap.push_back(to_search{ !current_element.level,((current_element.node)->left).get() });
			}
		}
		else {
			uint64_t hi = current_element.node->quadrant_index;
			auto indexes = get_points_from_hilbertindex(hi);
			for (uint64_t i = indexes.first; i <= indexes.second; i++)
			{
				if (points[i].dist(point) < dist_max) ret.push_back(points[i]);
			}
		}
	}
	//std::cout << "iiADPT:" << ii << "\n";
	return ret;
}


HilbertCurve2D_Adaptive::Node* HilbertCurve2D_Adaptive::generateTree()
{
	Node* node = new Node();
	genTreePar(0, points.size() - 1, 0, 0, node, nb_threads);
	return node;
}
void HilbertCurve2D_Adaptive::genTreePar(uint32_t start, uint32_t end, uint32_t level, uint64_t zindex, Node* output, uint32_t nbthreads)
{
	//if number of points = 1 or level of detail achieved
	if (end - start == 0 || level == 2 * order)
	{
		//if level of detail not achieved, correct zindex to prevent collision
		if (level != 2 * order) {
			zindex = zindex << 2 * order - level;
		}

		uint64_t hilbertindex = mortonToHilbert(zindex);
		//create leaf
		output->leaf = true;
		output->quadrant_index = hilbertindex;

		////create quadrant
		quadrants.insert(std::make_pair(hilbertindex, HilbertCurve2D::Quadrant(start, end)
		));

		return;
	}
	else { //number of points >= 2
		uint32_t pivot_index;
		//if level even split by x, else by y
		if (!(level % 2))
		{
			pivot_index = select_median_x(start, end);

			//check for points with same coord => as they should belong to same quadrant
			while (pivot_index < end && points[pivot_index].getX() == points[pivot_index + 1].getX())
				pivot_index++;

			//add split_value to tree
			output->value = points[pivot_index].getX();
		}
		else {
			pivot_index = select_median_y(start, end);

			//check for points with same coord => as they should belong to same quadrant
			while (pivot_index < end && points[pivot_index].getY() == points[pivot_index + 1].getY())
				pivot_index++;

			//add split_value to tree
			output->value = points[pivot_index].getY();
		}

		uint32_t z_left, z_right;
		z_left = zindex << 1;
		z_right = zindex << 1 | 1;

		Node* node_l = new Node();
		Node* node_r = new Node();


		if (pivot_index < end)
		{
			if (nbthreads > 1) {

				uint32_t left = ceil(nbthreads / 2.0);


				std::thread t = std::thread(&HilbertCurve2D_Adaptive::genTreePar, this,
					start,
					pivot_index,
					level + 1,
					z_left,
					node_l,
					left
				);

				genTreePar(pivot_index + 1, end, level + 1, z_right, node_r, nbthreads - left);
				t.join();
				output->left = std::unique_ptr<Node>(node_l);
				output->right = std::unique_ptr<Node>(node_r);
			}
			else {
				genTreePar(start, pivot_index, level + 1, z_left, node_l, nbthreads);
				genTreePar(pivot_index + 1, end, level + 1, z_right, node_r, nbthreads);

				output->left = std::unique_ptr<Node>(node_l);
				output->right = std::unique_ptr<Node>(node_r);
			}
		}
		else {
			genTreePar(start, pivot_index, level + 1, z_left, node_l, nbthreads);
			output->left = std::unique_ptr<Node>(node_l);
		}
	}
}

uint32_t HilbertCurve2D_Adaptive::select_median_x(uint32_t start, uint32_t end)
{
	uint32_t m = start + ((end - start) / 2);
	std::nth_element(points.begin() + start, points.begin() + m, points.begin() + end + 1, [](Point2D a, Point2D b) {return a.getX() < b.getX(); });
	return m;
}

uint32_t HilbertCurve2D_Adaptive::select_median_y(uint32_t start, uint32_t end)
{
	uint32_t m = start + ((end - start) / 2);
	std::nth_element(points.begin() + start, points.begin() + m, points.begin() + end + 1, [](Point2D a, Point2D b) {return a.getY() < b.getY(); });
	return m;
}

