#include "reservoir.h"

reservoir::reservoir() {
	this->clear();
}

reservoir::~reservoir() {}

void reservoir::add_edge_to_reservoir(int &A, int &B, int idx) {
	if ((int)this->edge_pool.size() == idx) {
		edge_pool.push_back(std::make_pair(A, B));
	}
	else {
		edge_pool[idx] = std::make_pair(A, B);
	}
	this->add_edge_to_reservoir(A, B);
}

void reservoir::add_edge_to_reservoir(int &A, int &B) {
	this->adj_left[A].insert(B);
	this->adj_right[B].insert(A);
	this->current_reservoir_size++;
}

void reservoir::remove_edge_from_sampled_graph(int idx) {
	int& A = this->edge_pool[idx].first;
	int& B = this->edge_pool[idx].second;
	this->remove_edge_from_sampled_graph(A, B);
}

void reservoir::remove_edge_from_sampled_graph(int& A, int &B) {
	this->adj_left[A].erase(B);
	remove_if_empty(adj_left, A);
	this->adj_right[B].erase(A);
	remove_if_empty(adj_right, B);
	this->current_reservoir_size--;
}


void reservoir::set_max_size(int sz) {
	this->max_size_of_reservoir = sz;
	return;
}

void reservoir::clear() {
	this->adj_left.clear();
	this->adj_right.clear();
	this->edge_pool.clear();
	this->EMPTY.clear();
	this->current_reservoir_size = 0;
}