#include "Fleet2.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

Fleet2::Fleet2(reservoir_fleet R, double gamma) {
	this->gamma = gamma;
	this->R = *new reservoir_fleet();
	this->R.set_max_size(R.get_maximum_size());
	decltype(dbl_ran.param()) new_range(0.0, 1.0);
	dbl_ran.param(new_range);
}

Fleet2::~Fleet2() {}

double Fleet2::ran_double() { return dbl_ran(gen); }

void Fleet2::process_edge(int A, int B) {
	this->time_step++;
	if ((this->R.get_current_size() >= this->R.get_maximum_size())) {
		while (this->R.get_current_size() >= this->R.get_maximum_size()) {
			reservoir_fleet *new_R = new reservoir_fleet();
			new_R->set_max_size(this->R.get_maximum_size());
			for (auto& e : this->R.edge_pool) {
				std::pair<int, int> p = R.to_edge(e);
				int x = p.first, y = p.second;
				if (ran_double() <= gamma) {
					new_R->add_edge_to_reservoir(x, y);
				}
			}
			this->R = *new_R;
			delete new_R;
		}
		this->n_full_reservoir++;
		this->prob *= this->gamma ;
	}
	if (ran_double() <= this->prob) {
		this->n_sampled_edges++;
		double inv_p = 1.0 / this->prob;
		long long bfly = bfc_per_edge_ada(A, B, R);
		this->seen_bfly += bfly;
		this->global_counter += bfly * inv_p * inv_p * inv_p * inv_p;
		this->R.add_edge_to_reservoir(A, B);
	}
}

void Fleet2::process_stream(int itr, stream& S) {
	this->prob = 1.;
	this->time_secs = 0.;
	this->time_step = 0;
	this->global_counter = 0;
	this->n_full_reservoir = 0;
	this->n_sampled_edges = 0;
	this->seen_bfly = 0;

	for (auto& edge : S.edges) {
		int& A = edge.first;
		int& B = edge.second;
		clock_t beg = clock();
		this->process_edge(A, B);
		clock_t end = clock();
		this->time_secs += ((double)end - beg) / CLOCKS_PER_SEC;
		if (this->time_step % S.batch_size == 0) {
			printf("%d, %d, %.2f, Fleet2, %d, %.2f, %.2Lf, %d, %d, %lld\n", itr + 1, this->R.get_maximum_size(), this->gamma, (int)this->time_step,
				this->time_secs, this->global_counter, this->n_sampled_edges, this->n_full_reservoir, this->seen_bfly);
			fflush(stdout);
		}
	}
	return;
}
