#include <unordered_map>
#include <unordered_set>
#include "FleetSSW.h"
#include "reservoir.h"
#include "counting.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

using namespace std;

FleetSSW::FleetSSW(int W, int power, int num_queries, int stream_size, double gamma) {
	num_edges = 0;
	sampling_prob = 1.0;
	this->gamma = gamma;

	prob_threshold = pow(gamma, power);
	M = (int)(prob_threshold * W);
	beta = 0;
	window_size = W;

	assert(num_queries > 1);
	query_interval = stream_size / num_queries;

	// Init reservoir.
	R = reservoir();
	R.set_max_size(M);
}

void FleetSSW::process_exact_edge(int A, int B) {
	num_edges++;


	left_adj[A].insert(B);
	right_adj[B].insert(A);
	this->left_par.insert(A);
	this->right_par.insert(B);

	// Update history window (queue), pop out the expired edges.
	sampled_window_edges.push_back(make_tuple(A, B, num_edges));
	while ((int)sampled_window_edges.size() > window_size) {
		int A_rem = get<0>(sampled_window_edges.front());
		int B_rem = get<0>(sampled_window_edges.front());
		this->left_adj[A_rem].erase(B_rem);
		this->left_par.erase(A_rem);

		this->right_adj[B_rem].erase(A_rem);
		this->right_par.erase(B_rem);

		sampled_window_edges.pop_front();
	}

	// Compute query result.
	if (num_edges % query_interval == 0) {
		// Compute exact number of butterflies.

		long long bfly_count = (long long)batch_bfly_count();

		printf("exact-sld,%d,%d,%lld\n", window_size, num_edges, bfly_count);
		fflush(stdout);
	}
}

void FleetSSW::process_estimate_edge(int itr, int A, int B) {
	num_edges++;

	if (abs(sampling_prob - prob_threshold) > 0.0000001) {
		// Run new ada.
		if ((this->R.get_current_size() >= this->R.get_maximum_size())) {
			while (this->R.get_current_size() >= this->R.get_maximum_size()) {
				// Init new R and sampled window edges.
				reservoir new_R = reservoir();
				new_R.set_max_size(this->R.get_maximum_size());
				std::deque<std::tuple<int, int, int>> new_sampled_window_edges;

				// Iterate old sampled edges and sampling.
				for (auto &e : sampled_window_edges) {
					int x = get<0>(e), y = get<1>(e);
					if (dbl_ran(gen) <= gamma) {
						new_R.add_edge_to_reservoir(x, y);
						new_sampled_window_edges.push_back(e);
					}
				}
				R = new_R;
				sampled_window_edges = new_sampled_window_edges;
			}
			sampling_prob *= gamma;
		}

		// Update butterfly count.
		if (dbl_ran(gen) <= sampling_prob) {
			double inv_p = 1.0 / sampling_prob;
			long double bfly = bfc_per_edge(A, B, R);
			beta += bfly * inv_p * inv_p * inv_p * inv_p;

			this->R.add_edge_to_reservoir(A, B);
			sampled_window_edges.push_back(make_tuple(A, B, num_edges));
		}

	}
	else {
		sampling_prob = prob_threshold;
		if (dbl_ran(gen) < sampling_prob) {
			sampled_window_edges.push_back(make_tuple(A, B, num_edges));
			R.add_edge_to_reservoir(A, B);

			beta += pow(sampling_prob, -4) * bfc_per_edge(A, B, R);
		}

	}

	// Delete expired edges.
	auto top = sampled_window_edges.front();
	if (get<2>(top) <= (num_edges - window_size)) {
		// Update butterfly count.
		int x = get<0>(top);
		int y = get<1>(top);
		beta -= bfc_per_edge(x, y, R) * pow(sampling_prob, -4);

		R.remove_edge_from_sampled_graph(x, y);
		sampled_window_edges.pop_front();
	}

	// Post query.
	if (num_edges % query_interval == 0) {
		string current_in_bern = "false";
		if (sampling_prob == prob_threshold) {
			current_in_bern = "true";
		}
		printf("est-sld,%d,%s,%d,%d,%.0Lf\n", (itr + 1), current_in_bern.c_str(),
			(int)sampled_window_edges.size(), num_edges, beta);
		fflush(stdout);
	}
}

void FleetSSW::process_stream(int itr, stream S) {
	if (itr == -1) { // the algorithm should be exact
		for (auto edge : S.edges) {
			int A = edge.first;
			int B = edge.second;
			this->process_exact_edge(A, B);
		}
	}
	else {
		for (auto edge : S.edges) {
			int A = edge.first;
			int B = edge.second;
			this->process_estimate_edge(itr, A, B);
		}
	}
}

long double FleetSSW::batch_bfly_count() {
	return Kai_BFC_in_reservoir(this->left_par, this->right_par, this->left_adj, this->right_adj);
}