#include "counting.h"

extern std::mt19937_64 gen;

template <class T, class T2>
long double compute_cost(T &adj, int side, T2 &R) {
	long double res = 0;
	for (auto& neighbor : adj) {
		if (side == 0)
			res += (long double)R.get_adj_right(neighbor).size();
		else
			res += (long double)R.get_adj_left(neighbor).size();
	}
	return res;
}

long long bfc_per_edge_ada(int A, int B, reservoir_fleet &R) {
	auto& adj_A = R.get_adj_left(A);
	auto& adj_B = R.get_adj_right(B);
	std::unordered_map <int, long long> counter;
	for (auto& neighbor_a : adj_A) {
		if (neighbor_a == B) continue;
		auto& two_hop_adj = R.get_adj_right(neighbor_a, true);
		for (auto& two_neighbor_a : two_hop_adj) {
			counter[two_neighbor_a] ++;
		}
	}
	long long local_answer = 0;
	for (auto& neighbor_b : adj_B) {
		if (neighbor_b != A && counter.find(neighbor_b) != counter.end()) {
			local_answer += counter[neighbor_b];
		}
	}
	return local_answer;
}

long double bfc_per_edge(std::unordered_set <int> &adj_fst, std::unordered_set <int> &adj_sec, reservoir &R, int fst, int sec, int side) {
	std::unordered_map <int, long double> counter;
	for (auto& neighbor_fst : adj_fst) {
		if (neighbor_fst == sec) continue;
		std::unordered_set <int>& two_hop_adj = side == 0 ? R.get_adj_right(neighbor_fst, true) : R.get_adj_left(neighbor_fst, true);
		for (auto& two_neighbor_fst : two_hop_adj) {
			counter[two_neighbor_fst] ++;
		}
	}
	long double local_answer = 0;
	for (auto& neighbor_sec : adj_sec) {
		if (neighbor_sec != fst && counter.find(neighbor_sec) != counter.end()) {
			local_answer += counter[neighbor_sec];
		}
	}
	return local_answer;
}

long double bfc_per_edge(int A, int B, reservoir& R) {
	std::unordered_set <int>& adj_A = R.get_adj_left(A);
	std::unordered_set <int>& adj_B = R.get_adj_right(B);

	long double cost_A = compute_cost(adj_A, 0, R);
	long double cost_B = compute_cost(adj_B, 1, R);

	if (cost_A + (int)adj_B.size() < cost_B + (int)adj_A.size()) {
		return bfc_per_edge(adj_A, adj_B, R, A, B, 0);
	}
	else
		return bfc_per_edge(adj_B, adj_A, R, B, A, 1);
}


const int N_ITERATIONS = 2200;

long double randomized_bfc_per_edge(int A, int B, reservoir &R) {
	std::unordered_set <int>& adjset_A = R.get_adj_left(A);
	std::unordered_set <int>& adjset_B = R.get_adj_right(B);

	if ((int)adjset_A.size() <= 1 || (int)adjset_B.size() <= 1)
		return 0;
	std::vector <int> adj_A(adjset_A.begin(), adjset_A.end());
	std::vector <int> adj_B(adjset_B.begin(), adjset_B.end());

	std::uniform_int_distribution<int> ran_A(0, (int)adj_A.size());
	std::uniform_int_distribution<int> ran_B(0, (int)adj_B.size());

	int n_iters = mmax(N_ITERATIONS, mmax((int)adjset_A.size(), (int)adjset_B.size()));

	long double local_answer = 0;
	for (int i = 0; i < n_iters; i++) {
		int neighbor_a = adj_A[ran_A(gen)];
		int neighbor_b = adj_B[ran_B(gen)];
		if (neighbor_a != B && neighbor_b != A &&
			R.get_adj_right(neighbor_a).find(neighbor_b) != R.get_adj_right(neighbor_a).end()) {
			local_answer++;
		}
	}
	local_answer /= n_iters;
	local_answer *= (long double)adjset_A.size() * (long double)adjset_B.size();
	return local_answer;
}

unsigned long long degree_sq(std::unordered_map < int, std::vector<int> > &partition) {
	unsigned long long res = 0;
	for (auto& p : partition) {
		res += ((unsigned long long)p.second.size()) * ((unsigned long long)p.second.size());
	}
	return res;
}

unsigned long long count_BFC(std::unordered_map < int, std::vector<int> > &left_par, std::unordered_map < int, std::vector<int> > &right_par) {
	unsigned long long total_count = 0;
	for (auto& p : left_par) {
		int l = p.first;
		std::unordered_map <int, long long> counter;
		for (auto& r : left_par[l]) {
			for (auto& l2 : right_par[r]) if (l2 > l) {
				total_count += counter[l2];
				counter[l2] ++;
			}
		}
	}
	return total_count;
}

unsigned long long BFC_in_reservoir(reservoir_fleet &R) {
	auto& left_par = R.left_partition();
	auto& right_par = R.right_partition();
	if (degree_sq(left_par) < degree_sq(right_par))
		return count_BFC(right_par, left_par);
	else
		return count_BFC(left_par, right_par);
}

// Exact butterfly counting, a.k.a BFC-VP, introduced by Kai Wang et. al. (paper: Vertex Priority Based Butterfly Counting for Large-scale Bipartite Networks, on arXiv 2018)
inline int idx(int &v, int &side) {
	return v * 10 + side;
}

bool higher_rank(int &x, int &y, int degree_x, int degree_y, int side_x, int side_y) {
	if (degree_x != degree_y)
		return degree_x > degree_y;
	return idx(x, side_x) > idx(y, side_y);
}

long long process(int side, std::vector<int> & par, std::vector<int> & other, std::vector < std::vector<int> > &adj_par, std::vector < std::vector<int> > &adj_other) {
	long long ans = 0;
	for (auto& start : par) {
		std::unordered_map <int, long long> counter;
		for (auto& middle : adj_par[start]) if (higher_rank(start, middle, (int)adj_par[start].size(), (int)adj_other[middle].size(), side, 1 - side)) {
			for (auto& end : adj_other[middle]) if (higher_rank(start, end, (int)adj_par[start].size(), (int)adj_par[end].size(), side, side)) {
				ans += counter[end];
				counter[end] ++;
			}
		}
	}
	return ans;
}

long double Kai_BFC_in_reservoir(std::vector<int> &left_par, std::vector<int> &right_par, std::vector < std::vector<int> > &adj_left, std::vector < std::vector<int> > &adj_right) {
	long double ans = (long double)process(0, left_par, right_par, adj_left, adj_right);
	ans += process(1, right_par, left_par, adj_right, adj_left);
	return ans;
}


// for sliding window
long long process(int side, std::unordered_set<int> & par, std::unordered_set<int> & other, std::unordered_map < int, std::unordered_set<int> > &adj_par, std::unordered_map < int, std::unordered_set<int> > &adj_other) {
	long long ans = 0;
	for (auto start : par) {
		std::unordered_map <int, long long> counter;
		for (auto middle : adj_par[start]) if (higher_rank(start, middle, (int)adj_par[start].size(), (int)adj_other[middle].size(), side, 1 - side)) {
			for (auto end : adj_other[middle]) if (higher_rank(start, end, (int)adj_par[start].size(), (int)adj_par[end].size(), side, side)) {
				ans += counter[end];
				counter[end] ++;
			}
		}
	}
	return ans;
}

long double Kai_BFC_in_reservoir(std::unordered_set<int> &left_par, std::unordered_set<int> &right_par, std::unordered_map < int, std::unordered_set<int> > &adj_left, std::unordered_map < int, std::unordered_set<int> > &adj_right) {
	long double ans = (long double)process(0, left_par, right_par, adj_left, adj_right);
	ans += process(1, right_par, left_par, adj_right, adj_left);
	return ans;
}
