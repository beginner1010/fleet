#include "reservoir_Fleet.h"

const long long shift = 1000 * 1000 * 1000;

reservoir_fleet::reservoir_fleet() {
	this->clear();
}

reservoir_fleet::~reservoir_fleet() {}

long long reservoir_fleet::to_hash(int& A, int& B) {
	return A * shift + B;
}

std::pair <int, int> reservoir_fleet::to_edge(const long long& hash) {
	return std::make_pair(hash / shift, hash % shift);
}

void reservoir_fleet::add_edge_to_reservoir(int& A, int& B) {
	this->adj_left[A].push_back(B);
	this->adj_right[B].push_back(A);
	this->edge_pool.push_back(this->to_hash(A, B));
	this->current_reservoir_size++;
}

void reservoir_fleet::set_max_size(int sz) {
	this->max_size_of_reservoir = sz;
	return;
}

void reservoir_fleet::clear() {
	this->adj_left.clear();
	this->adj_right.clear();
	this->edge_pool.clear();
	this->EMPTY.clear();
	this->current_reservoir_size = 0;
}

std::unordered_map < int, std::vector<int> >& reservoir_fleet::left_partition() {
	return this->adj_left;
}
std::unordered_map < int, std::vector<int> >& reservoir_fleet::right_partition() {
	return this->adj_right;
}