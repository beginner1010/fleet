#pragma once
#include <unordered_map>
#include <random>
#include "reservoir.h"
#include "reservoir_Fleet.h"

#define mmin(x, y) ((x)<(y)?(x):(y))
#define mmax(x, y) ((x)>(y)?(x):(y))

extern std::mt19937_64 gen;

long long bfc_per_edge_ada(int A, int B, reservoir_fleet &R);

template <class T, class T2>
long double compute_cost(T &adj, int side, T2 &R);
long double bfc_per_edge(std::unordered_set <int> &adj_fst, std::unordered_set <int> &adj_sec, reservoir &R, int fst, int sec, int side);
long double bfc_per_edge(int A, int B, reservoir& R);

long double randomized_bfc_per_edge(int A, int B, reservoir &R);


unsigned long long degree_sq(std::unordered_map < int, std::vector<int> > &partition);
unsigned long long count_BFC(std::unordered_map < int, std::vector<int> > &left_par, std::unordered_map < int, std::vector<int> > &right_par);

unsigned long long BFC_in_reservoir(reservoir_fleet &R);

// Exact butterfly counting, a.k.a BFC-VP, introduced by Kai Wang et. al. (paper: Vertex Priority Based Butterfly Counting for Large-scale Bipartite Networks, on arXiv 2018)
long double Kai_BFC_in_reservoir(std::vector<int> &left_par, std::vector<int> &right_par, std::vector < std::vector<int> > &adj_left, std::vector < std::vector<int> > &adj_right);


//for sliding window
long long process(int side, std::unordered_set<int> & par, std::unordered_set<int> & other, std::unordered_map < int, std::unordered_set<int> > &adj_par, std::unordered_map < int, std::unordered_set<int> > &adj_other);
long double Kai_BFC_in_reservoir(std::unordered_set<int> &left_par, std::unordered_set<int> &right_par, std::unordered_map < int, std::unordered_set<int> > &adj_left, std::unordered_map < int, std::unordered_set<int> > &adj_right);