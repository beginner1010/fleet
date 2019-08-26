#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "counting.h"
#include "reservoir_Fleet.h"
#include "stream.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

class Fleet3 {
public:
	Fleet3(reservoir_fleet R, double gamma);
	~Fleet3();
	void process_stream(int, stream &);
private:
	double gamma;
	long long seen_bfly;
	double time_secs;
	void process_edge(int, int);
	double ran_double();
	long double global_counter;
	unsigned long long time_step;
	reservoir_fleet R;
	double estimated_butterfly;
	double prob;
	int n_full_reservoir;
	int n_sampled_edges;
};
