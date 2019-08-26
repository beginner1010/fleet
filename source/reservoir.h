#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class reservoir {
public:
	reservoir();
	~reservoir();
	std::unordered_set<int> EMPTY;
	void add_edge_to_reservoir(int& A, int& B, int);
	void add_edge_to_reservoir(int &A, int &B);
	void remove_edge_from_sampled_graph(int);
	void remove_edge_from_sampled_graph(int& A, int& B);
	void set_max_size(int);
	inline int get_maximum_size() const { return this->max_size_of_reservoir; }
	inline int get_current_size() const { return this->current_reservoir_size; }
	inline std::unordered_set<int>& get_adj_left(const int& vertex, bool exist = false) { 
		if (exist || adj_left.find(vertex) != adj_left.end()) 
			return this->adj_left[vertex];
		return EMPTY;
	}
	inline std::unordered_set<int>& get_adj_right(const int& vertex, bool exist = false) { 
		if (exist || adj_right.find(vertex) != adj_right.end()) 
			return this->adj_right[vertex];
		return EMPTY;
	}
	std::vector < std::pair<int, int> > edge_pool;
	void clear();
private:
	inline void remove_if_empty(std::unordered_map <int, std::unordered_set<int> >&adj, int &x) { if (adj[x].empty()) adj.erase(x); }
	std::unordered_map< int, std::unordered_set<int> > adj_left;
	std::unordered_map< int, std::unordered_set<int> > adj_right;
	int max_size_of_reservoir;
	int current_reservoir_size;
};