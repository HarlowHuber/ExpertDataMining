/*
"Expert" Data Mining with Hansel Chains

Author: Harlow Huber
Supervisor: Dr. Boris Kovalerchuk
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iomanip>

struct dvector
{
	std::vector<int> dataPoint;
	int _class = -1;
	std::vector<std::string> used_zero_expansions;
	std::vector<std::string> used_one_expansions;
	std::vector<std::string> unused_zero_expansions;
	std::vector<std::string> unused_one_expansions;
	std::vector<std::string> other_zero_expansions;	// possible expansions if the Hansel Chain ordering were to be different
	std::vector<std::string> other_one_expansions;
	bool visited = false;
	bool asked = false;
};

template<typename T> 
void print(T t, const int& width, const char& separator)
{
	std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
}

std::vector<std::vector<dvector>> hanselChainSet;
std::vector<int> kv_attributes;

std::vector<std::vector<dvector>> genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain)
{
	std::unordered_map<int, std::vector<std::vector<dvector>>> chains = chain;

	//add key string "0","1",...,num to the beginning of each element in the corresponding key
	for (int j = 0; j < num; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			for (int l = 0; l < chains.at(j)[k].size(); l++)
			{
				std::vector<int> b(vector_dimension);
				b[0] = j;

				for (int i = 1; i < vector_dimension; i++)
				{
					b[i] = chains.at(j)[k][l].dataPoint[i - 1]; // used to be for std::vector: b[i] = chains.at(j)[k][l][i - 1];
				}

				//b.setLevel();
				dvector c;
				c.dataPoint = b;
				chains.at(j)[k][l] = c;
			}
		}
	}

	//move the largest of each sub std::vector to corresponding sub std::vector in the first key
	//then move the largest of each subsequent sub std::vector to the second key's sub std::vector
	//repeat until chain.at(num-2) is reached since the last key will have nothing moved 
	//to it
	//j = current key being added to
	//emptV = number of empty std::vectors
	for (int j = 0; j < chains.size() - 1; j++)
	{
		//count is to keep up with the elemental std::vector having elements removed/moved from
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			//I think I need to make the code below recursive
			int count = j;

			for (count; count < chains.size() - 1; count++)
			{
				if (count + 1 >= chain.size())
				{
					break;
				}

				/// USE FIND AND END because kth std::vector doesn't exist for 3d 3val, k=2
				if (k > chains.at(count + 1).size() - 1)
				{
					break;
				}

				if (chains.at(count + 1).at(k).empty())
				{
					//### DEAL WITH EMPTY std::vectorS~!###
					//chains.at(to_string(count + 1)).at(k).erase();
					break;
				}

				int end = (int)chains.at(count + 1).at(k).size();

				//add last element from position k of key count+1 to position k of j
				chains.at(j)[k].push_back(chains.at(count + 1)[k].at(end - 1));

				//pop off the moved element
				chains.at(count + 1).at(k).pop_back();
			}
		}
	}

	//we need to prepare for the next iteration of i in the first for loop
	//by adding all chains together, then adding combined chains to each key
	for (int j = 1; j < num; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			chains.at(0).push_back(chains.at(j)[k]);
		}
	}

	for (int j = 0; j < num; j++) {
		chains.at(j) = chains.at(0);
	}

	std::vector<std::vector<dvector>> chainsTogether;

	return chainsTogether = chains.at(0);
}

void calculateHanselChains(int vector_dimension)
{
	// For n dimensions, iterate through to generate chains and
	for (int dim_num = 0; dim_num < vector_dimension; dim_num++)
	{
		int num = kv_attributes[dim_num];

		// Need a base chain to use for genChains
		if (dim_num == 0)
		{
			//generate the base chain
			std::vector<dvector> baseChain;

			for (int k = 0; k < num; k++)
			{
				std::vector<int> b(vector_dimension);
				b[0] = k;
				dvector c;
				c.dataPoint = b;
				//b.setLevel();
				baseChain.push_back(c);
			}

			//create a std::vector of std::vectors to hold the baseChain
			hanselChainSet.push_back(baseChain);
		}
		else
		{
			std::unordered_map<int, std::vector<std::vector<dvector>>> chainNum;

			for (int k = 0; k < num; k++)
			{
				chainNum[k] = hanselChainSet;
			}

			hanselChainSet = genChains(num, vector_dimension, chainNum);
		}
	}

	for (size_t i = 0; i < hanselChainSet.size(); i++)
	{
		if (!hanselChainSet[i].size())
		{
			hanselChainSet.erase(hanselChainSet.begin() + i);
			i--;
		}
	}
}

int main()
{
	std::cout << "Expert Data Mining with Hansel Chains.\n" << std::endl;

	// determine the the number of attributes (dimension) and what those attributes are
	int dimension = -1;

	std::cout << "How many attributes are in this dataset (what is the dimension)?" << std::endl;
	std::cin >> dimension;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	std::vector<std::string> attributes(dimension);
	kv_attributes.resize(dimension);

	for (int i = 0; i < dimension; i++)
	{
		std::cout << "\nWhat is the name of attribute x" + std::to_string(i + 1) << "?" << std::endl;
		std::cin >> attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	for (int i = 0; i < dimension; i++)
	{
		std::cout << "\nWhat is the k_value of attribute " + attributes[i] + "?" << std::endl;
		std::cin >> kv_attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	calculateHanselChains(dimension);

	// ask expert what class each vector in each chain belongs to
	for (int i = (int)hanselChainSet.size() - 1; i >= 0; i--)
	{
		int chainSize = (int)hanselChainSet[i].size();

		for (int j = 0; j < chainSize; j++)
		{
			int vector_class = -1;

			// if vector has not been visited, then ask user class
			// else, retrieve class
			if (!hanselChainSet[i][j].visited)
			{
				std::cout << "\nEnter the class for this data point:\n";

				for (int k = 0; k < dimension; k++)
				{
					if (hanselChainSet[i][j].dataPoint[k])
					{
						std::cout << attributes[k] + "\t\t\t= true (1)" << std::endl;
					}
					else
					{
						std::cout << attributes[k] + "\t\t\t= false (0)" << std::endl;
					}
				}

				std::cout << "Class: " << std::flush;
				std::cin >> vector_class;
				std::cin.clear();
				std::cin.ignore(1000, '\n');
				
				hanselChainSet[i][j]._class = vector_class;
				hanselChainSet[i][j].visited = true;
				hanselChainSet[i][j].asked = true;
			}
			else vector_class = hanselChainSet[i][j]._class;
			
			// expand the current vector
			// FIX unused 0-0 includes previous elements in chain 
			for (int k = 0; k < dimension; k++)
			{
				// other one expansions (impossible expansions from previous chains, given a specific ordering of chains)
				// DO NOT VISIT OR ASSIGN CLASS
				if (1 != hanselChainSet[i][j].dataPoint[k])
				{
					dvector expanded;
					expanded.dataPoint = hanselChainSet[i][j].dataPoint;
					expanded.dataPoint[k] = 1;

					// starting in the first chain, search for expanded vectors up to the current chain 
					// not possible up to current vector because, by nature, those vectors would be 0 expansions
					for (int hc = (int)hanselChainSet.size() - 1; hc > i; hc--)
					{
						for (int v = 0; v < hanselChainSet[hc].size(); v++)
						{
							// "Expand" the vector,
							if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint)
							{
								hanselChainSet[i][j].other_one_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
							}
						}
					}
				}

				// other zero expansions (impossible expansions from previous chains, given a specific ordering of chains)
				// DO NOT VISIT OR ASSIGN CLASS
				if (0 != hanselChainSet[i][j].dataPoint[k])
				{
					dvector expanded;
					expanded.dataPoint = hanselChainSet[i][j].dataPoint;
					expanded.dataPoint[k] = 0;

					// starting in the first chain, search for expanded vectors up to the current chain
					for (int hc = (int)hanselChainSet.size() - 1; hc > i; hc--)
					{
						for (int v = 0; v < hanselChainSet[hc].size(); v++)
						{
							// "Expand" the vector
							if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint)
							{
								hanselChainSet[i][j].other_zero_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
							}
						}
					}

					// in the current chain, search for expanded vectors up to the current vector (0 case only)
					for (int v = 0; v < j; v++)
					{
						// "Expand" the vector
						if (expanded.dataPoint == hanselChainSet[i][v].dataPoint)
						{
							hanselChainSet[i][j].other_zero_expansions.push_back(std::to_string(i + 1) + "." + std::to_string(v + 1));
						}
					}
				}

				// possible expansions from successive chains for a given class
				if (vector_class != hanselChainSet[i][j].dataPoint[k])
				{
					dvector expanded;
					expanded.dataPoint = hanselChainSet[i][j].dataPoint;
					expanded.dataPoint[k] = vector_class;

					// starting in the current chain, search for expanded vector
					for (int hc = i; hc >= 0; hc--)
					{
						for (int v = 0; v < hanselChainSet[hc].size(); v++)
						{
							// expand the vector and mark it as visited
							// these are "used" expansions
							if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited)
							{
								if (vector_class)
								{
									hanselChainSet[i][j].used_one_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}
								else
								{
									hanselChainSet[i][j].used_zero_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}

								// mark as visited and expand vector (assign class)
								hanselChainSet[hc][v]._class = vector_class;
								hanselChainSet[hc][v].visited = true;
							}
							else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint) // if vector is already visited, then add to "unused" expansions
							{
								if (vector_class)
								{
									hanselChainSet[i][j].unused_one_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}
								else if ((hc == i && v > j) || hc < i)
								{
									hanselChainSet[i][j].unused_zero_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}
							}
						}
					}
				}

				// possible expansions from successive chains for the OPPOSITE of a given class
				// these are "unused" expansions
				// DO NOT VISIT OR ASSIGN CLASS
				int not_vector_class = 1;

				if (vector_class) not_vector_class = 0;

				if (not_vector_class != hanselChainSet[i][j].dataPoint[k])
				{
					dvector expanded;
					expanded.dataPoint = hanselChainSet[i][j].dataPoint;
					expanded.dataPoint[k] = not_vector_class;

					// starting in the current chain, search for expanded vector
					for (int hc = i; hc >= 0; hc--)
					{
						for (int v = 0; v < hanselChainSet[hc].size(); v++)
						{
							if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint)
							{
								if (not_vector_class)
								{
									hanselChainSet[i][j].unused_one_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}
								else if ((hc == i && v > j) || hc < i)
								{
									hanselChainSet[i][j].unused_zero_expansions.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
								}
							}
						}
					}
				}
			}
		}
	}

	// restore monotone Boolean function
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			if (hanselChainSet[i][j]._class)
			{
				bool first = false;

				for (int k = 0; k < dimension; k++)
				{
					if (hanselChainSet[i][j].dataPoint[k] == 1)
					{
						if (!first)
						{
							std::vector<int> temp(dimension);
							temp[k] = 1;
							boolFunc.push_back(temp);
							first = true;
						}
						else boolFunc[boolFunc.size() - 1][k] = 1;
					}
				}

				break;
			}
		}
	}

	// reduce monotone Boolean function
	for (size_t i = 0; i < boolFunc.size() - 1; i++)
	{
		for (size_t j = i + 1; j < boolFunc.size(); j++)
		{
			int difference = 0;
			int left_hamming_norm = 0;
			int right_hamming_norm = 0;

			for (int k = 0; k < dimension; k++)
			{
				if (boolFunc[i][k] != boolFunc[j][k]) difference++;

				left_hamming_norm += boolFunc[i][k];
				right_hamming_norm += boolFunc[j][k];
			}

			if (difference == 1 && left_hamming_norm != right_hamming_norm)
			{
				boolFunc.erase(boolFunc.begin() + j);
				j--;
			}
		}
	}

	// convert Boolean function to string
	std::string boolFuncStr = "";

	for (size_t i = 0; i < boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j]) temp += "x" + std::to_string(j + 1);
		}

		if (!temp.empty() && i > 0) boolFuncStr += " v " + temp;
		else if (!temp.empty()) boolFuncStr += temp;
	}

	// print vectors and monotone Boolean Function
	const char separator = ' ';
	const int width = 16;

	std::cout << "\nMonotone Boolean Function:\n" + boolFuncStr + "\n" << std::endl;
	print("Number", width, separator);
	print("Vector", width, separator);
	print("Class", width, separator);
	print("Asked", width, separator);
	print("Used 1-1", width, separator);
	print("Used 0-0", width, separator);
	print("Unused 1-1", width, separator);
	print("Unused 0-0", width, separator);
	print("Other 1-1", width, separator);
	print("Other 0-0", width, separator);
	std::cout << std::endl;

	for (int i = (int)hanselChainSet.size() - 1; i >= 0; i--)
	{
		for (size_t j = 0; j < hanselChainSet[i].size(); j++)
		{
			std::string vecStr = "";
			std::string usedOneExpStr = "";
			std::string usedZeroExpStr = "";
			std::string unusedOneExpStr = "";
			std::string unusedZeroExpStr = "";
			std::string otherOneExpStr = "";
			std::string otherZeroExpStr = "";

			// data point as string
			for (auto element: hanselChainSet[i][j].dataPoint)
			{
				vecStr += std::to_string(element);
			}

			// used one expansions
			for (auto element : hanselChainSet[i][j].used_one_expansions)
			{
				usedOneExpStr += element + ";";
			}

			// used zero expansions
			for (auto element : hanselChainSet[i][j].used_zero_expansions)
			{
				usedZeroExpStr += element + ";";
			}

			// unused one expansions
			for (auto element : hanselChainSet[i][j].unused_one_expansions)
			{
				unusedOneExpStr += element + ";";
			}

			// unused zero expansions
			for (auto element : hanselChainSet[i][j].unused_zero_expansions)
			{
				unusedZeroExpStr += element + ";";
			}

			// other one expansions
			for (auto element : hanselChainSet[i][j].other_one_expansions)
			{
				otherOneExpStr += element + ";";
			}

			// other zero expansions
			for (auto element : hanselChainSet[i][j].other_zero_expansions)
			{
				otherZeroExpStr += element + ";";
			}

			print(std::to_string(i + 1) + "." + std::to_string(j + 1) + ":", width, separator);
			print(vecStr, width, separator);
			print(hanselChainSet[i][j]._class, width, separator);
			print(hanselChainSet[i][j].asked, width, separator);
			print(usedOneExpStr, width, separator);
			print(usedZeroExpStr, width, separator);
			print(unusedOneExpStr, width, separator);
			print(unusedZeroExpStr, width, separator);
			print(otherOneExpStr, width, separator);
			print(otherZeroExpStr, width, separator);
			std::cout << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
