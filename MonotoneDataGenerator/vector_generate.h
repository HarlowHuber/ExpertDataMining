#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

struct point
{
	std::vector<int> p;
	int function_value = 0;
	int class_value = 0;
};

class vector_generate
{
private:
	// array of k-values for each attribute
	int* kv_attributes;

	int function_kv;

	// number of attributes
	int num_attributes;

	std::vector<std::multimap<int, std::vector<int>, std::greater<int>>> mapped_vectors;

	// calculate all vectors grouped by Hamming norm and sorted
	void calculate_all_vectors(std::vector<int> max_vector, int max_hamming_norm, int max_vector_index, std::vector<std::multimap<int, struct point, std::greater<int>>>& sorted_vectors);

	// computes output based on input vector
	int monotone_function(std::vector<int> p);

	// calculate mixed base value of vector
	int calc_mb_value(std::vector<int> vector);

	// print a list of vectors grouped by Hamming norm
	// first line is the k-values and the function k-value
	void print_sorted_vectors(std::vector<std::multimap<int, struct point, std::greater<int>>> sorted_vectors);

	// print a list of vectors
	void print_vectors(int i, std::multimap<int, struct point, std::greater<int>>& all_vectors, std::ofstream& file);

public:
	// constructor
	vector_generate(int* _kv_attributes, int kv_target, int _num_attributes, bool print);

	// destructor
	~vector_generate();
};
