/*
An implementation of "Towards Tighter Space Bounds for Counting Triangles and Other Substructures in Graph Streams"
Authors: Suman K. Bera and Amit Chakrabarti
*/

#include "BC.h"

extern std::mt19937_64 gen;
extern std::uniform_int_distribution<int> int_ran;

BC::BC(stream& S) {
	this->adj.clear();
	for (auto& e : S.edges) {
		adj[e.first].insert(e.second);
	}
}
BC::~BC() {}

int BC::ran_int(int A, int B) {
	decltype(int_ran.param()) new_range(A, B);
	int_ran.param(new_range);
	return int_ran(gen);
}

bool BC::form_butterfly(std::pair<int, int> &edge1, std::pair<int, int> &edge2) {
	if (edge1.first == edge2.first)
		return false;
	if (edge1.second == edge2.second)
		return false;
	if (adj[edge1.first].find(edge2.second) == adj[edge1.first].end())
		return false;
	if (adj[edge2.first].find(edge1.second) == adj[edge2.first].end())
		return false;
	return true;
}

long double BC::estimate_butterfly(int r_sz, std::vector < std::pair<int,int> > &edges) {
	this->estimated_butterfly = 0;
	for (int i = 0; i < r_sz; i++) {
		int idx1 = ran_int(0, (int)edges.size());
		int idx2 = ran_int(0, (int)edges.size() - 1);
		if (idx2 >= idx1) 
			idx2++;
		if (this->form_butterfly(edges[idx1], edges[idx2])) // we keep all edges of the stream just to simulate two-pass algorithm of Bera and Chakrabarti's algorithm.
			this->estimated_butterfly ++;
	}
	long double m = (long double)edges.size();
	long double beta = 2.0;
	long double xi = 2.0;
	this->estimated_butterfly = this->estimated_butterfly / r_sz;
	this->estimated_butterfly = this->estimated_butterfly / xi * m * m;
	return this->estimated_butterfly;
}

void BC::process_stream(int itr, int r_sz, stream& S) {
	auto res = estimate_butterfly(r_sz, S.edges);
	printf("%d, %d, BC, %.2Lf\n", itr, r_sz, res);
	return;
}
