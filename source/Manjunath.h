#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <complex>
#include <cmath>


#include "counting.h"
#include "reservoir_Fleet.h"
#include "stream.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;

class Manjunath {
public:
	Manjunath();
	~Manjunath();
	void process_stream(int r_sz, stream &S);
private:
	double time_secs;
	unsigned long long time_step;
	long double estimated_butterfly;
	long double estimate_butterfly(int r_sz, std::vector < std::pair<int, int> > &edges);
    std::complex<double> hash_function(int iter_r_sz, int hash_id, int vertex_id);
    std::complex<double> get_z_j (int j);
};
