#include "vector_generate.h"

int main()
{
	// get vector length
	int num_attributes = 0;

	std::cout << "\n Enter the number of attributes per vector: ";

	if (!(std::cin >> num_attributes))
	{
		std::cout << "\n Enter a number next time." << std::endl;

		return 0;
	}

	// get max attributes and create maximum vector
	std::vector<int> max_vector;
	int* kv_attributes = new int[num_attributes];
	int num_vectors = 1;

	for (int i = 0; i < num_attributes; i++)
	{
		std::cout << "\n Enter the k-value for attribute " << i << ": ";

		if (!(std::cin >> kv_attributes[i]))
		{
			std::cout << "\n Enter a number next time." << std::endl;

			return 0;
		}

		max_vector.push_back(kv_attributes[i] - 1);
		num_vectors *= kv_attributes[i];
	}

	std::cout << "\n Enter the k-value for the target attribute: ";
	int kv_target = 0;

	if (!(std::cin >> kv_target))
	{
		std::cout << "\n Enter a number next time." << std::endl;

		return 0;
	}

	char answer = 'n';
	std::cout << "\n Enter whether to print the vectors or not [y/n]: ";

	if (!(std::cin >> answer))
	{
		std::cout << "\n Enter a character next time." << std::endl;

		return 0;
	}

	int print = false;

	if (answer == 'y' || answer == 'Y')
	{
		print = true;
	}

	//vector_generate generate(kv_attributes, num_attributes, print);
	vector_generate generate(kv_attributes, kv_target, num_attributes, false, print);

	return 0;
}
