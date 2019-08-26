#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "reservoir_Fleet.h"
#include "stream.h"

class batch {
public:
	batch(reservoir_fleet);
	~batch();
	void process_stream(stream &S, int starting_edge);
private:
	std::vector <int> left_vertices;
	std::vector <int> right_vertices;
	std::vector < std::vector<int> > adj_left;
	std::vector < std::vector<int> > adj_right;
	std::unordered_map <int, int> vertex_idx_left;
	std::unordered_map <int, int> vertex_idx_right;
	double time_secs;
	void process_edge(int &A, int &B);
	long double global_counter;
	unsigned long long time_step;
	reservoir_fleet R;
};