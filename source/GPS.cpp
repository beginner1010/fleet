#include "GPS.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

GPS::GPS(reservoir R) {
	this->R = *new reservoir();
	this->R.set_max_size(R.get_maximum_size());
	decltype(dbl_ran.param()) new_range(1e-5, 1.0);
	dbl_ran.param(new_range);
	this->z_star = 0;
	this->rank_set.clear();
	this->lookup_weight.clear();
	this->idx_in_reservoir.clear();
}

GPS::~GPS() {}

double GPS::ran_double() { return dbl_ran(gen); }


long double GPS::bfc_per_edge_martingale(std::unordered_set <int> &adj_fst, std::unordered_set <int> &adj_sec, reservoir &R, int fst, int sec, int side) {
	std::unordered_map <int, long double> rev_probs;
	for (auto& neighbor_fst : adj_fst) {
		if (neighbor_fst == sec) continue;
		double w1 = side == 0 ? lookup_weight[to_hash(fst, neighbor_fst)] : lookup_weight[to_hash(neighbor_fst, fst)];
		double q1 = prob(w1);
		std::unordered_set <int>& two_hop_adj = side == 0 ? R.get_adj_right(neighbor_fst, true) : R.get_adj_left(neighbor_fst, true);
		for (auto& two_neighbor_fst : two_hop_adj) {
			double w2 = side == 0 ? lookup_weight[to_hash(two_neighbor_fst, neighbor_fst)] : lookup_weight[to_hash(neighbor_fst, two_neighbor_fst)];
			double q2 = prob(w2);
			rev_probs[two_neighbor_fst] += 1.0 / (q1 * q2);
		}
	}
	long double local_answer = 0;
	for (auto& neighbor_sec : adj_sec) {
		if (neighbor_sec != fst && rev_probs.find(neighbor_sec) != rev_probs.end()) {
			double w3 = side == 0 ? lookup_weight[to_hash(neighbor_sec, sec)] : lookup_weight[to_hash(sec, neighbor_sec)];
			double q3 = prob(w3);
			local_answer += rev_probs[neighbor_sec] * (1.0 / q3);
		}
	}
	return local_answer;
}

long double compute_cost_martingale(std::unordered_set<int> &adj, int side, reservoir &R) {
	long double res = 0;
	for (auto& neighbor : adj) {
		if (side == 0)
			res += (long double)R.get_adj_right(neighbor).size();
		else
			res += (long double)R.get_adj_left(neighbor).size();
	}
	return res;
}


double GPS::GPS_estimate(int& A, int& B, reservoir &R) {
	std::unordered_set <int>& adj_A = R.get_adj_left(A);
	std::unordered_set <int>& adj_B = R.get_adj_right(B);

	long double cost_A = compute_cost_martingale(adj_A, 0, R);
	long double cost_B = compute_cost_martingale(adj_B, 1, R);

	if (cost_A + (int)adj_B.size() < cost_B + (int)adj_A.size()) {
		return bfc_per_edge_martingale(adj_A, adj_B, R, A, B, 0);
	}
	else
		return bfc_per_edge_martingale(adj_B, adj_A, R, B, A, 1);
}


void GPS::GPS_update(int& A, int& B, reservoir &R) {
	this->time_step++;
	double w = this->compute_weight(A, B);
	double ran = ran_double();
	long long new_hash = to_hash(A, B);
	double rank = w / ran;

	auto cur_edge = std::make_pair(rank, std::make_pair(A, B));

	// The reservoir has still space for a new edge
	if (this->time_step <= this->R.get_maximum_size()) {
		rank_set.insert(cur_edge);
		R.add_edge_to_reservoir(A, B, (int)R.edge_pool.size());
		idx_in_reservoir[new_hash] = (int)R.edge_pool.size();
		this->lookup_weight[new_hash] = w;
		return;
	}

	// The rank of new edge is low. It is discarded, then.
	if (cur_edge < *rank_set.begin()) {
		z_star = mmax(z_star, rank);
		return;
	}

	/* 
		The new edge is added to the reservoir and 
		an edge with the lowest rank in the reservoir is removed.
	*/
	int rem_A = (*rank_set.begin()).second.first;
	int rem_B = (*rank_set.begin()).second.second;
	long long rem_hash = to_hash(rem_A, rem_B);
    
    z_star = mmax(z_star, (*rank_set.begin()).first);
	R.remove_edge_from_sampled_graph(rem_A, rem_B);
	idx_in_reservoir.erase(rem_hash);
	lookup_weight.erase(rem_hash);
	rank_set.erase(rank_set.begin());

	R.add_edge_to_reservoir(A, B);
	lookup_weight[new_hash] = w;
	rank_set.insert(cur_edge);
}

void GPS::process_stream(int itr, int res_sz, stream& S) {
	this->time_secs = 0.;
	this->time_step = 0;
	this->global_counter = 0;

	for (auto& edge : S.edges) {
		int& A = edge.first;
		int& B = edge.second;
		clock_t beg = clock();
		this->global_counter += this->GPS_estimate(A, B, R);
		this->GPS_update(A, B, R);
		clock_t end = clock();
		this->time_secs += ((double)end - beg) / CLOCKS_PER_SEC;
		if (this->time_step % S.batch_size == 0) {
			printf("%d, %d, GPS, %d, %.2f, %.2Lf\n", itr + 1, res_sz, (int)this->time_step, this->time_secs, this->global_counter);
			fflush(stdout);
		}
	}
	return;
}
