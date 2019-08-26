#include "FleetTSW.h"
#include "FleetSSW.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

using namespace std;

FleetTSW::FleetTSW(int M, int T, double gamma, unordered_map<int, int>& query_map) {
	num_edges = 0;
	this->M = M;
	this->gamma = gamma;
	this->T = T;

	// Query time windows, <query_stream_size, query_window>
	this->query_map = query_map;
}

void FleetTSW::process_exact_edge(int A, int B) {
	num_edges++;
	recv_edges.push_back(make_tuple(A, B, num_edges));
}

void FleetTSW::query_exact_edge(int query_time_window) {
	// Compute exact number of butterflies.
	std::vector<std::pair<int, int>> exact_edge_set;
	for (int idx = (int)recv_edges.size() - query_time_window; idx < (int)recv_edges.size(); idx++) {
		exact_edge_set.push_back(make_pair(get<0>(recv_edges[idx]), get<1>(recv_edges[idx])));
	}

	long long bfly_count = batch_bfly_count(exact_edge_set);
	// algorithm, query window, num_edges_received, bfly_count
	printf("time-exact-sld,%d,%d,%lld\n", query_time_window, num_edges, bfly_count);
	fflush(stdout);
}

void FleetTSW::new_reservoir() {
	queues.push_back(std::deque<std::tuple<int, int, int>>());
	recent_delete_time.push_back(-1);
}

void FleetTSW::process_estimate_edge(int itr, int A, int B) {
	num_edges++;

	int level = 0;
	do {
		if (level == (int)queues.size()) {
			new_reservoir();
		}
		queues[level].push_back(std::tuple<int, int, int>(A, B, num_edges));
		if ((int)(queues[level].size()) * T > this->M) {
			std::tuple<int, int, int> top = queues[level].front();
			recent_delete_time[level] = get<2>(top);
			queues[level].pop_front();
		}
		level++;
	} while (dbl_ran(gen) < gamma);
}


void FleetTSW::query_estimate_edge(int itr, int query_time_window) {
	int target_level = 0;
	for (int idx = 0; idx < (int)recent_delete_time.size(); idx++) {
		if (recent_delete_time[idx] <= (num_edges - query_time_window)) {
			target_level = idx;
			break;
		}
	}

	std::vector<std::pair<int, int>> qualified_edges;
	std::deque<std::tuple<int, int, int>> &target_queue = queues[target_level];
	for (int i = 0; i<(int)target_queue.size(); i++) {
		std::tuple<int, int, int> &tuple = target_queue[i];
		if (get<2>(tuple) >(num_edges - query_time_window)) {
			qualified_edges.push_back(std::pair<int, int>(get<0>(tuple), get<1>(tuple)));
		}
	}

	long double bfly_count = batch_bfly_count(qualified_edges) * pow(pow(gamma, target_level), -4);
	// algorithm, iteration, current_in_bern, reservoir_size, num_edges_received, bfly_count
	printf("time-est-sld,%d,%d,%d,%.0Lf,%d\n", (itr + 1), query_time_window, num_edges, bfly_count, target_level);
	fflush(stdout);
}

void FleetTSW::process_stream(int itr, stream S) {
	if (itr == -1) { // the algorithm should be exact
		for (auto edge : S.edges) {
			int A = edge.first;
			int B = edge.second;
			this->process_exact_edge(A, B);

			// post queries
			if (query_map.find(num_edges) != query_map.end()) {
				query_exact_edge(query_map[num_edges]);
			}
		}
	}
	else {
		for (auto edge : S.edges) {
			int A = edge.first;
			int B = edge.second;
			this->process_estimate_edge(itr, A, B);

			// post queries
			if (query_map.find(num_edges) != query_map.end()) {
				query_estimate_edge(itr, query_map[num_edges]);
			}
		}
	}
}

inline unsigned long long degree_sq(unordered_map<int, unordered_set<int>> &partition) {
	unsigned long long res = 0;
	for (auto p : partition) {
		res += p.second.size() * p.second.size();
	}
	return res;
}

long long FleetTSW::batch_bfly_count(std::vector<std::pair<int, int>> &edge_vec) {
	// Convert the edges to adjacency list.
	unordered_map<int, unordered_set<int> > left_par;
	unordered_map<int, unordered_set<int> > right_par;
	for (auto it = edge_vec.begin(); it != edge_vec.end(); it++) {
		int A = it->first;
		int B = it->second;
		left_par[A].insert(B);
		right_par[B].insert(A);
	}
	unsigned long long d2_l = degree_sq(left_par);
	unsigned long long d2_r = degree_sq(right_par);
	if (d2_l < d2_r) {
		std::swap(left_par, right_par);
	}

	long long total_count = 0;
	for (auto p : left_par) {
		int l = p.first;
		std::unordered_map<int, long long> counter;
		for (auto r : left_par[l]) {
			for (auto l2 : right_par[r])
				if (l2 > l) {
					if (counter.find(l2) != counter.end()) {
						total_count += counter[l2];
						counter[l2]++;
					}
					else {
						counter[l2] = 1;
					}
				}
		}
	}
	return total_count;
}