#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "counting.h"
#include "reservoir.h"
#include "stream.h"

extern std::mt19937_64 gen;
extern std::uniform_real_distribution<double> dbl_ran;
const long long SHIFT = 1000 * 1000 * 100;

#define mmin(x,y) ((x)>(y)?(y):(x))

class GPS {
public:
	GPS(reservoir);
	~GPS();
	void process_stream(int, int, stream &);
private:
	double time_secs;
	void GPS_update(int& A, int& B, reservoir &R);
	double GPS_estimate(int& A, int& B, reservoir &R);
	double ran_double();
	long double bfc_per_edge_martingale(std::unordered_set <int> &adj_fst, std::unordered_set <int> &adj_sec, reservoir &R, int fst, int sec, int side);
	inline long long to_hash(int A, int B) { return A * SHIFT + B; }
	inline double prob(double weight) { return mmin(1.0, std::fabs(this->z_star) <= 1e-6 ? 1.0 : weight / this->z_star); }
	inline double compute_weight(int& A, int& B) {
		return 16.0 * bfc_per_edge(A, B, this->R) + 1.0;	
	}
	double z_star;
	long double global_counter;
	unsigned long long time_step;
	reservoir R;
	std::unordered_map <long long, double> lookup_weight;
	std::unordered_map <long long, int> idx_in_reservoir;
	std::set < std::pair< double, std::pair <int, int> > > rank_set;
};
