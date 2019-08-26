#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class reservoir_fleet {
public:
	reservoir_fleet();
	~reservoir_fleet();
	std::vector<int> EMPTY;
	void add_edge_to_reservoir(int &A, int &B);
	void set_max_size(int);
	inline int get_maximum_size() const { return this->max_size_of_reservoir; }
	inline int get_current_size() const { return this->current_reservoir_size; }
	inline std::vector< int>& get_adj_left(const int &vertex, bool exist = false) { 
		if (exist || this->adj_left.find(vertex) != this->adj_left.end()) 
			return this->adj_left[vertex];
		return EMPTY;
	}
	inline std::vector< int>& get_adj_right(const int &vertex, bool exist = false) { 
		if (exist || this->adj_right.find(vertex) != this->adj_right.end()) 
			return this->adj_right[vertex]; 
		return EMPTY;
	}
	std::unordered_map < int, std::vector<int> >& left_partition();
	std::unordered_map < int, std::vector<int> >& right_partition();
	std::vector < long long > edge_pool;
	std::pair <int, int> to_edge(const long long& hash);
	long long to_hash(int& A, int& B);
	void clear();
private:
	inline void remove_if_empty(std::unordered_map <int, std::vector<int> >&adj, int &x) { if (adj[x].empty()) adj.erase(x); }
	std::unordered_map< int, std::vector<int> > adj_left;
	std::unordered_map< int, std::vector<int> > adj_right;
	int max_size_of_reservoir;
	int current_reservoir_size;
};