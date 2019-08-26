#ifndef STREAMBFLY_SLIDING_WINDOW_H
#define STREAMBFLY_SLIDING_WINDOW_H

#include <queue>
#include "stream.h"
#include "reservoir.h"

class FleetSSW {
public:
	FleetSSW(int W, int power, int num_queries, int stream_size, double gamma);

	void process_stream(int itr, stream S);

	void process_exact_edge(int A, int B);

	void process_estimate_edge(int itr, int A, int B);

	long double batch_bfly_count();

	int M;    // expected number of sampled edges
	int num_edges;   // number of received edges
	int query_interval;
	int window_size;
	long double beta;
	double sampling_prob;
	double prob_threshold;
	double gamma; 

	// Convert the edges to adjacency list.
	std::unordered_map<int, std::unordered_set<int> > left_adj;
	std::unordered_map<int, std::unordered_set<int> > right_adj;
	std::unordered_set<int> left_par;
	std::unordered_set<int> right_par;

	// <u, v, time>
	std::deque<std::tuple<int, int, int>> sampled_window_edges;

	// adj list for current window
	reservoir R;

	// Store the query results, each value is the number of
	// butterflies (exact or estimated) at one time.
	std::vector<long long> exact_result;
	std::vector<long long> estimate_result;
};


#endif //STREAMBFLY_SLIDING_WINDOW_H