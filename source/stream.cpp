#include "stream.h"

stream::stream() {}
stream::~stream() {}
extern std::string algorithm;

void stream::read_filenames() {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<unsigned long long> distr;
	std::cerr << " Insert the input (bipartite graph stream) file location" << std::endl;
	std::cerr << " >>> "; std::cin >> input_address;
	std::cerr << " Insert the output file" << std::endl;
	std::cerr << " >>> "; std::cin >> output_address;
	if (algorithm != "BC") {
		std::cerr << " Insert the number of snapshots" << std::endl;
		std::cerr << " >>> "; std::cin >> this->snap_shots;
	}
	assert(this->snap_shots != 0);
	std::cerr << " ---------------------------------------------------------------------------------------------------------------------- \n";
	std::cerr << " Preprocessing the graph (removing text lines and multiple edges from the input) ..." << std::endl;
	preprocessing();
	fprintf(stderr, " There are %d edges, %d vertices in the left partition, and %d in the right partition\n", this->E, this->n_L, this->n_R);
	fflush(stdout), fflush(stderr);
}

int stream::to_s(std::string &x) { std::stringstream aux; aux << x; int res; aux >> res; return res; }

bool stream::all_num(std::string &s) {
	for (int i = 0; i < (int)s.size(); i++) if ((s[i] >= '0' && s[i] <= '9') == false) return false;
	return true;
}


void stream::preprocessing() {
	FILE *f_in;
	f_in = freopen(input_address, "r", stdin);

	std::string s;
	this->E = 0;
	std::set < std::pair <int, int> > seen_edges;
	std::set < int > ver_l, ver_r;
	std::map < int, int > degree_l, degree_r;
	this->max_degree = 0;
	this->edges.clear();

	while (std::getline(std::cin, s)) {
		std::stringstream ss; ss << s;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < mmin(2, (int)vec_str.size()); i++) is_all_num &= this->all_num(vec_str[i]);
			if (is_all_num) {
				int A = to_s(vec_str[0]), B = to_s(vec_str[1]);
				if (seen_edges.find(std::make_pair(A, B)) != seen_edges.end())
					continue;

				seen_edges.insert(std::make_pair(A, B));
				this->edges.push_back(std::make_pair(A, B));
				ver_l.insert(A);
				ver_r.insert(B);
				degree_l[A] ++;
				degree_r[B] ++;
				this->max_degree = mmax(this->max_degree, mmax(degree_l[A], degree_r[B]));
				E++;
			}
		}
	}

	fclose(f_in);
	this->n_L = (int)ver_l.size();
	this->n_R = (int)ver_r.size();
	this->batch_size = this->E / this->snap_shots;
}