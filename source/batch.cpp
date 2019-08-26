#include "batch.h"
#include "counting.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

batch::batch(reservoir_fleet R) {
	this->R = *new reservoir_fleet();
	this->R.set_max_size(R.get_maximum_size());
	decltype(dbl_ran.param()) new_range(0.0, 1.0);
	dbl_ran.param(new_range);
}

batch::~batch() {}

int& find_idx(int vertex, std::unordered_map <int, int> &hash_map, std::vector<int> &vertices, std::vector < std::vector<int> > &adj) {
	if (hash_map.find(vertex) == hash_map.end()) {
		hash_map[vertex] = (int)vertices.size();
		vertices.push_back(hash_map[vertex]);
		adj.push_back(std::vector <int>());
	}
	return hash_map[vertex];
}

void batch::process_edge(int &A, int &B) {
	this->time_step++;
	A = find_idx(A, this->vertex_idx_left, this->left_vertices, this->adj_left);
	B = find_idx(B, this->vertex_idx_right, this->right_vertices, this->adj_right);
	this->adj_left[A].push_back(B);
	this->adj_right[B].push_back(A);
}

void batch::process_stream(stream &S, int starting_edge) {
	this->time_secs = 0.;
	this->time_step = 0;
	this->global_counter = 0;

	for (auto& edge : S.edges) {
		int& A = edge.first;
		int& B = edge.second;
		this->process_edge(A, B);
		if (this->time_step >= starting_edge && this->time_step % S.batch_size == 0) { 
			clock_t beg = clock();
			this->global_counter = (long double)Kai_BFC_in_reservoir(this->left_vertices, this->right_vertices, this->adj_left, this->adj_right);
			clock_t end = clock();
			this->time_secs = ((double)end - beg) / CLOCKS_PER_SEC;
			printf("batch, %d, %.2f, %.2Lf\n", (int)this->time_step, this->time_secs, this->global_counter); fflush(stdout);
		}
	}
	return;
}