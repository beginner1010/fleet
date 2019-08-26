#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "counting.h"
#include "reservoir_Fleet.h"
#include "stream.h"

extern std::mt19937_64 gen;
extern std::uniform_int_distribution<int> int_ran;

class BC {
public:
	BC(stream &S);
	~BC();
	void process_stream(int itr, int r_sz, stream& S);
private:
	int ran_int(int A, int B);
	long double estimated_butterfly;
	long double estimate_butterfly(int r_sz, std::vector < std::pair<int, int> > &edges);
	std::unordered_map < int, std::unordered_set<int> > adj;
	bool form_butterfly(std::pair<int, int> &edge1, std::pair<int, int> &edge2);
};
