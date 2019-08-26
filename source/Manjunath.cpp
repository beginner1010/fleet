/*
An implementation of "Towards Tighter Space Bounds for Counting Triangles and Other Substructures in Graph Streams"
Authors: Suman K. Bera and Amit Chakrabarti
*/

#include "Manjunath.h"

extern std::mt19937_64 gen;

Manjunath::Manjunath() {}
Manjunath::~Manjunath() {}

std::complex<double> Manjunath::get_z_j (int j) {
    double pow_2 = std::pow(2, 1./j) ;
    double pi = std::atan(1.0) * 4 ;
    double real_part = pow_2 * std::cos(pi / j) ; 
    double imag_part = pow_2 * std::sin(pi / j) ;
    std::complex <double> ret ;
    ret.real(real_part) ;
    ret.imag(imag_part) ;
    return ret ;
}

std::complex<double> Manjunath::hash_function(int iter_r_sz, int hash_id, int vertex_id) {
    long long seed = vertex_id * 10 + hash_id ;
	seed = seed * 10000000 + iter_r_sz;
    std::default_random_engine generator(seed) ;
    std::uniform_real_distribution <double> dist(0.0, 1.0) ;
    int k = 4 ;
    int d = 2 ;
    std::complex<double> H (1.0, 0.0) ;
    for (int j = 1 ; j <= 2 * k ; j ++) {
        if (j == d)
            continue ;
        double prob = dist(generator) ;
        std::complex<double> H_j;
        if (prob <= 2./3.) {
            H_j.real(1.0);
            H_j.imag(0.0);
        } else {
            H_j = this->get_z_j(j);
        }
        H *= H_j ;
    }
    return H ;
}

void Manjunath::process_stream(int r_sz, stream &S) {
	r_sz /= 4; // as we use 4 complex random variable, a.k.a Zs.
	this->time_step = 0;
	std::vector < std::vector < std::complex<double> > > Z(r_sz, std::vector < std::complex<double> >(4)); // Z values for four edges of butterfly H

	for (int itr = 0; itr < r_sz; itr++)
		for (int i = 0; i < 4; i++)
			Z[itr][i] = std::complex<double>(0, 0);

	this->time_secs = 0;
	for (auto& edge : S.edges) {
		clock_t start = clock();
		this->time_step++;
		this->estimated_butterfly = 0;
		for (int itr = 0; itr < r_sz; itr++) {
			int u = edge.first * 2;
			int v = edge.second * 2 + 1;
			for (int i = 0; i < 4; i++) {
				int w = i + 1;
				int t = (i + 1) % 4 + 1;
				Z[itr][i] += (this->hash_function(itr, w, u) * this->hash_function(itr, t, v) + this->hash_function(itr, w, v) * this->hash_function(itr, t, u))
					* this->hash_function(itr, 0, u) * this->hash_function(itr, 0, v);
			}
			std::complex <double> ret = Z[itr][0];
			for (int i = 1; i < 4; i++) {
				ret *= Z[itr][i];
			}
			this->estimated_butterfly += ret.real(); // return the real part
		}
		this->time_secs += ((double)clock() - start) / CLOCKS_PER_SEC;
		if (this->time_step % S.batch_size == 0) {
			this->estimated_butterfly = this->estimated_butterfly / r_sz; // we take the mean of different estimates
			printf("%d, Manjunath, %d, %.2f, %.2Lf\n", r_sz * 4, (int)this->time_step, this->time_secs, this->estimated_butterfly);
			fflush(stdout);
		}
		if (this->time_secs > 3600) {
			break;
		}
	}
}

