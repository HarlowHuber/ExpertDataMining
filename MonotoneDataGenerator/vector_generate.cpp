#include "vector_generate.h"
#include <chrono>

// constructor
vector_generate::vector_generate(int* _kv_attributes, int kv_target, int _num_attributes, bool continuous, bool print)
{
	auto start = std::chrono::high_resolution_clock::now();
	kv_attributes = _kv_attributes;
	num_attributes = _num_attributes;
	function_kv = kv_target;
	std::vector<int> max_vector(num_attributes);

	// construct maximum vector
	for (int i = 0; i < num_attributes; i++)
	{
		max_vector[i] = kv_attributes[i] - 1;
	}

	int hamming_norm = 0;

	for (int a : max_vector)
	{
		hamming_norm += a;
	}

	// multimap key is mixed base value
	// vector of multimaps
	std::vector<std::multimap<int, struct point, std::greater<int>>> mapped_vectors(static_cast<__int64>(hamming_norm) + 1);

	struct point maxPoint {max_vector, monotone_function(max_vector), 0 };
	mapped_vectors[hamming_norm].insert(std::pair(calc_mb_value(max_vector), maxPoint));

	// calculate all vectors and assign monotone function values
	calculate_all_vectors(max_vector, hamming_norm, num_attributes - 1, mapped_vectors, continuous);


	// assign ordinal class values if continuous function is used
	if (continuous)
	{
		int numberPoints = 1;

		for (int i = 0; i < num_attributes; i++)
		{
			numberPoints *= kv_attributes[i];
		}

		int split = numberPoints / kv_target;
		int class_value = 0;
		int counter = 0;

		for (auto&& group : mapped_vectors)
		{
			for (std::multimap<int, point>::reverse_iterator it = group.rbegin(); it != group.rend(); it++)
			{
				if (counter++ >= split)
				{
					class_value++;
					counter = 0;
				}

				it->second.class_value = class_value;
			}
		}
	}
	// use discrete function directly
	// change program to be able to use flags to create more continuous and/or discrete functions???
	// 
	else
	{
		for (auto&& group : mapped_vectors)
		{
			for (std::multimap<int, point>::reverse_iterator it = group.rbegin(); it != group.rend(); it++)
			{
				// function is x1-x3-x5
				if (it->second.p[0] == 1 && it->second.p[2] == 1 && it->second.p[4] == 1 || 
					it->second.p[1] == 1 && it->second.p[2] == 1 && it->second.p[4] == 1 ||
						it->second.p[3] == 1 && it->second.p[4] == 1)
				{

					it->second.class_value = 1;
				}
			}
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	std::cout << "\n Program took " << duration.count() << " microseconds to generate vectors." << std::endl;

	if (print)
	{
		print_sorted_vectors(mapped_vectors);
	}
}


// destructor
vector_generate::~vector_generate()
{
	delete[] kv_attributes;
}


/*
 *	calculate all vectors below the given max_vector
 
	ALGORITHM:
	This algorithm calculates all possible vectors for a given MAXIMUM input vector. Therefore, if the input vectors
	have k-values of k = (3, 5, 2), the maximum input vector will be (2, 4, 1).
	The number of vectors that are generated will be the product of all k-values multiplied against each other.
	Therefore, using the last example, the number of vectors will be: 3 * 5 * 2 = 30.
*/
void vector_generate::calculate_all_vectors(std::vector<int> max_vector, int max_hamming_norm, int max_vector_index, 
	std::vector<std::multimap<int, struct point, std::greater<int>>>& sorted_vectors, bool continuous)
{
	// iterate over every index starting from the given index
	for (int i = max_vector_index; i >= 0; i--)
	{
		int value = max_vector[i];
		int decrement = 1;

		// decrement the value at the specific index until it is 0
		// add the new point to the list of all vectors
		// then, recurse on the new point on the next higher-order value
		while (value > 0)
		{
			struct point newPoint{max_vector, 0, 0};
			newPoint.p[i] -= decrement;

			if (continuous)
			{
				newPoint.function_value = monotone_function(newPoint.p);
			}
		
			int hamming_norm = max_hamming_norm - decrement;
			value = newPoint.p[i];
			decrement++;

			// insert new point into sorted_vectors by looping over possible vectors with the same mixed-base value
			// then, insert based on what vector has higher order elements or greater higher order elements
			int key = calc_mb_value(newPoint.p);
			auto it1 = sorted_vectors[hamming_norm].equal_range(key); // pair of two iterators; first points at before the range of the key; second points to after
			bool higher_order = false;

			// iterate over every vector with the same key
			for (auto it2 = it1.first; it2 != it1.second && !higher_order; it2++)
			{
				// iterate over elements of the copy_vector and a vector with the same key
				// if copy_vector has greater elements, it is placed before the other vector
				for (int i = 0; i < num_attributes; i++)
				{
					if (it2->second.p[i] < newPoint.p[i])
					{
						higher_order = true;
						sorted_vectors[hamming_norm].emplace_hint(it2, std::pair<int, struct point>(key, newPoint)); // inserts before the vector that it2 points to
						break;
					}
					else if (it2->second.p[i] > newPoint.p[i]) break;
				}
			}
			
			// insert at the end of the vectors with the same key (mixed-base value) if it is smaller
			if (!higher_order)
			{
				sorted_vectors[hamming_norm].emplace_hint(it1.second, key, newPoint);
			}

			if (i > 0)
			{
				calculate_all_vectors(newPoint.p, hamming_norm, i - 1, sorted_vectors, continuous); 
			}
		}
	}
}


int vector_generate::monotone_function(std::vector<int> p)
{
	// this function can change
	int function_value = 0;

	// sum of attributes * (dimension - attribute_index)
	for (int i = 0; i < (int)p.size(); i++)
	{
		function_value += p[i];
	}

	return function_value;
}


// calculate mixed base value
int vector_generate::calc_mb_value(std::vector<int> vector)
{
	int vector_value = 0;

	for (int i = 0; i < num_attributes; i++)
	{
		vector_value += vector[i] * (int)std::pow(kv_attributes[i], num_attributes - i - 1);
	}

	return vector_value;
}


// print all sorted vectors
void vector_generate::print_sorted_vectors(std::vector<std::multimap<int, struct point, std::greater<int>>> sorted_vectors)
{
	std::ofstream file;
	file.open("test.csv");
	 
	for (int i = 0; i < num_attributes; i++)
	{
		file << kv_attributes[i] << ",";
	}

	file << function_kv << "," << std::endl;

	for (int i = (int)sorted_vectors.size() - 1; i >= 0; i--)
	{
		std::cout << "\n Hamming norm: " << i << std::endl;
		print_vectors(i, sorted_vectors[i], file);
	}

	file.close();
}


// print a list of vectors
void vector_generate::print_vectors(int i, std::multimap<int, struct point, std::greater<int>>& all_vectors, std::ofstream& file)
{
	for (auto [key, point] : all_vectors)
	{
		std::cout << "\tVector: ";

		for (int i : point.p)
		{
			std::cout << i << ",";
			file << i << ",";
		}

		std::cout << "\tMixed Base Value: " << key;
		std::cout << "\tFunction Value: " << point.function_value;
		std::cout << "\tClass Value: " << point.class_value << std::endl;

		file << point.class_value << "," << i << std::endl;
	}
}
