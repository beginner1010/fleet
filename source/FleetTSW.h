#ifndef STREAMBFLY_TIME_SLD_WINDOW_H
#define STREAMBFLY_TIME_SLD_WINDOW_H

#include <queue>
#include "stream.h"
#include "reservoir.h"

class FleetTSW {
public:
	FleetTSW(int M, int T, double gamma, std::unordered_map<int, int>& query_map);

	void process_stream(int itr, stream S);

	void process_exact_edge(int A, int B);

	void query_exact_edge(int time);

	void process_estimate_edge(int itr, int A, int B);

	void query_estimate_edge(int itr, int query_time_window);

	long long batch_bfly_count(std::vector<std::pair<int, int>> &edge_set);

	void new_reservoir();

	int M;    // expected number of sampled edges
	int T;    // number of reservoirs
	int num_edges;   // number of received edges
	double gamma;  // for Ada as init

				   // Data structures, <u, v, time>
	std::vector<std::deque<std::tuple<int, int, int>>> queues;
	std::vector<int> recent_delete_time;
	// Received edges with time.
	std::vector<std::tuple<int, int, int>> recv_edges;

	// Query window map.
	std::unordered_map<int, int> query_map;
};

#endif //STREAMBFLY_TIME_SLD_WINDOW_H