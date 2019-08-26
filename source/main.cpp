#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "stream.h"
#include "batch.h"
#include "Fleet1.h"
#include "Fleet2.h"
#include "Fleet3.h"
#include "GPS.h"
#include "BC.h"
#include "reservoir.h"
#include "reservoir_Fleet.h"
#include "FleetSSW.h"
#include "FleetTSW.h"
#include "Manjunath.h"

std::random_device rd;
std::mt19937_64 gen(0);
std::uniform_real_distribution<double> dbl_ran;
std::uniform_int_distribution<int> int_ran;
std::string algorithm;

#define BATCH_ALGO "batch"
#define EXACT_ALGO "exact"
#define Fleet1_ALGO "Fleet1"
#define Fleet2_ALGO "Fleet2"
#define Fleet3_ALGO "Fleet3"
#define GPS_ALGO "GPS"
#define BC_ALGO "BC"
#define FLEETSSW_ALGO "FleetTSSW"
#define FLEETTSW_ALGO "FleetTTSW"
#define MANJUNATH "Manjunath"

int main(int argc, const char * argv[]) {


	std::ios::sync_with_stdio(false);
	int MAX_ITERATIONS = 0;
	int start_res_sz = 0;
	int end_res_sz = 0;
	double start_gamma = 0;
	double end_gamma = 0;
	int starting_edge = 0;
	int M = 0;
	int window_size = 0;
	int power = 0;

	BC* bc = NULL;

	if (argc <= 1) {
		fprintf(stderr, "Error: file <name of algorithm>\n"); fflush(stderr);
		fprintf(stderr, "Options: batch | exact | Fleet1 | Fleet2 | Fleet3 | GPS | BC | FleetTSSW | FleetTTSW | Manjunath \n"); fflush(stderr);
		exit(0);
	}

	algorithm = argv[1];
	std::unordered_set<std::string> algorithms_name = { EXACT_ALGO, BATCH_ALGO, Fleet1_ALGO, Fleet2_ALGO, Fleet3_ALGO, GPS_ALGO, BC_ALGO, FLEETSSW_ALGO, FLEETTSW_ALGO, MANJUNATH };

	if (algorithms_name.find(algorithm) == algorithms_name.end()) {
		fprintf(stderr, " Algorithm not found\n"); fflush(stderr);
		exit(0);
	}

	if (algorithm == BATCH_ALGO) {
		std::cerr << " Insert the starting edge" << std::endl;
		std::cerr << " >>> "; std::cin >> starting_edge;
	}

	if (algorithm != EXACT_ALGO && algorithm != BATCH_ALGO) {
		if (algorithm != MANJUNATH) {
			std::cerr << " Insert the number of repeation of the algorithm" << std::endl;
			std::cerr << " >>> "; std::cin >> MAX_ITERATIONS;
		}
		if (algorithm != FLEETSSW_ALGO && algorithm != FLEETSSW_ALGO) {
			std::cerr << " Insert the minimum reservoir size" << std::endl;
			std::cerr << " >>> "; std::cin >> start_res_sz;
			std::cerr << " Insert the maximum reservoir size" << std::endl;
			std::cerr << " >>> "; std::cin >> end_res_sz;
		}
	}

	if (algorithm.substr((int)algorithm.size() - 3) == Fleet2_ALGO) {
		std::cerr << " Insert the minimum gamma" << std::endl;
		std::cerr << " >>> "; std::cin >> start_gamma;
		assert(0 < start_gamma && start_gamma <= 1.0);
		std::cerr << " Insert the maximum gamma" << std::endl;
		std::cerr << " >>> "; std::cin >> end_gamma;
		assert(start_gamma <= end_gamma && end_gamma <= 1.0);
	}

	if (algorithm == FLEETSSW_ALGO || algorithm == FLEETSSW_ALGO) {
		if (algorithm == FLEETSSW_ALGO) {
			std::cerr << " Insert the window size" << std::endl;
			std::cerr << " >>> ";
			std::cin >> window_size;

			std::cerr << " Insert the power (what is the power of gamma "
				"(0.9) to be the sampling rate (0.1, 0.2...)?)" << std::endl;
			std::cerr << " >>> ";
			std::cin >> power;
		}

		std::cerr << " Insert the total memory size (for time sliding window) " << std::endl;
		std::cerr << " >>> ";
		std::cin >> M;
	}

	stream S;
	S.read_filenames();
	freopen(S.output_address, "w", stdout);

	fprintf(stderr, " #E, #V(Left), #V(Right), max degree, average degree\n"); fflush(stderr);
	fprintf(stderr, " %d,%d,%d,%d,%.2f\n", S.E, S.n_L, S.n_R, S.max_degree, ((double)S.E * 2.) / (S.n_L + S.n_R));

	if (algorithm == BATCH_ALGO) {
		// batch exact algorithm 
		reservoir_fleet* R_batch = new reservoir_fleet();
		R_batch->set_max_size((int)1e9);
		batch* batch_instance = new batch(*R_batch);
		printf("algo, batch, time, bfly\n");
		batch_instance->process_stream(S, starting_edge);
	}
	else {
		if (algorithm == Fleet1_ALGO) {
			printf("iteration, res-sz, gamma, algo, batch, time, bfly, samplededges, full-reservoir, seen-bfly\n");
		}
		else if (algorithm == Fleet2_ALGO) {
			printf("iteration, res-sz, gamma, algo, batch, time, bfly, samplededges, full-reservoir, seen-bfly\n");
		}
		else if (algorithm == Fleet3_ALGO) {
			printf("iteration, res-sz, gamma, algo, batch, time, bfly, samplededges, full-reservoir, seen-bfly\n");
		}
		else if (algorithm == GPS_ALGO) {
			printf("iteration, res-sz, algo, batch, time, bfly\n");
		}
		else if (algorithm == BC_ALGO) {
			printf("iteration, res-sz, algo, bfly\n");
			bc = new BC(S);
		}
		else if (algorithm == MANJUNATH) {
			printf("res-sz, algo, batch, time, bfly\n");
		}

		if (algorithm == FLEETSSW_ALGO || algorithm == FLEETSSW_ALGO) {
			double gamma = 0.9;
			int num_queries = 100;
			if (algorithm == FLEETTSW_ALGO) {
				// Generate time query window.
				int time_num_queries = 10;
				int T = 30;
				std::unordered_map<int, int> query_map;
				for (int i = 0; i<time_num_queries; i++) {
					int query_stream_size = (i + 1) * (S.edges.size() / time_num_queries);
					int query_window = (int)(query_stream_size * dbl_ran(gen));
					query_map.insert(std::pair<int, int>(query_stream_size, query_window));
				}
				// Run exact time based window.
				printf("time-exact-sld, query_time_window, num_edges, bfly\n");
				FleetTSW time_window_exact(M, T, gamma, query_map);
				time_window_exact.process_stream(-1, S);

				// Run approx time based window.
				printf("time-est-sld,itr, query_time_window, num_edges,bfly, target_level\n");
				for (int i = 0; i < MAX_ITERATIONS; i++) {
					FleetTSW time_window_approx = FleetTSW(M, T, gamma, query_map);
					time_window_approx.process_stream(i, S);
				}

				printf("time-est-sld,itr, query_time_window, num_edges,bfly, target_level\n");
				for (int i = 0; i < MAX_ITERATIONS; i++) {
					FleetTSW time_window_approx = FleetTSW(M, T, gamma, query_map);
					time_window_approx.process_stream(i, S);
				}
			}
			else if (algorithm == FLEETSSW_ALGO) {
				if (MAX_ITERATIONS == -1) {
					// Exact sliding window.
					printf("exact-sld,res-sz, num-edges, bfly\n");
					fflush(stdout);
					FleetSSW sld_exact(window_size, power, num_queries, S.edges.size(), gamma);
					sld_exact.process_stream(-1, S);
				}
				else {
					// Approximate sliding window.
					printf("approx-sld, itr, in_bern, res-sz, num-edges, bfly\n");
					fflush(stdout);
					for (int i = 0; i < MAX_ITERATIONS; i++) {
						FleetSSW sld_approx(window_size, power, num_queries, S.edges.size(), gamma);
						sld_approx.process_stream(i, S);
					}
				}
			}
		}
		else {
			for (int r_sz = start_res_sz; r_sz <= end_res_sz; r_sz *= 2) {
				if (algorithm == MANJUNATH) {
					Manjunath* manj = new Manjunath();
					manj->process_stream(r_sz, S);
				}
				else {
					for (int itr = 0; itr < MAX_ITERATIONS; itr++) {
						if (algorithm == Fleet1_ALGO) {
							for (double gamma = start_gamma; gamma <= end_gamma; gamma += 0.1) {
								reservoir_fleet* R_Fleet1 = new reservoir_fleet();
								R_Fleet1->set_max_size(r_sz);
								Fleet1* fleet1 = new Fleet1(*R_Fleet1, gamma);
								fleet1->process_stream(itr, S);
								delete R_Fleet1;
								delete fleet1;
							}
						}
						else if (algorithm == Fleet2_ALGO) {
							for (double gamma = start_gamma; gamma <= end_gamma; gamma += 0.1) {
								reservoir_fleet* R_Fleet2 = new reservoir_fleet();
								R_Fleet2->set_max_size(r_sz);
								Fleet2* fleet2 = new Fleet2(*R_Fleet2, gamma);
								fleet2->process_stream(itr, S);
								delete R_Fleet2;
								delete fleet2;
							}
						}
						else if (algorithm == Fleet3_ALGO) {
							for (double gamma = start_gamma; gamma <= end_gamma; gamma += 0.1) {
								reservoir_fleet* R_Fleet3 = new reservoir_fleet();
								R_Fleet3->set_max_size(r_sz);
								Fleet3* fleet3 = new Fleet3(*R_Fleet3, gamma);
								fleet3->process_stream(itr, S);
								delete R_Fleet3;
								delete fleet3;
							}
						}
						else if (algorithm == GPS_ALGO) {
							reservoir* R_GPS = new reservoir();
							R_GPS->set_max_size(r_sz / 2);
							GPS* gps = new GPS(*R_GPS);
							gps->process_stream(itr, r_sz, S);
							delete R_GPS;
							delete gps;
						}
						else if (algorithm == BC_ALGO) {
							bc->process_stream(itr, r_sz / 2, S);
						}
					}
				}
				fprintf(stderr, " reservoir size %d is finished\n", r_sz); fflush(stderr);
			}

			if (algorithm == BC_ALGO)
				delete bc;
		}
	}

	return 0;
}
