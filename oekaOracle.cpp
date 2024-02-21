#include "oeka.h"


// these functions are called in the initialization if the test dataset file is present

std::map<int, std::vector<std::vector<int>>> oeka::readFile(std::string fileName)
{
	std::string line;
	int h = 0; // hamming norm of vector
	std::map<int, std::vector<std::vector<int>>> oracle; // last element in each vector is the class

	std::ifstream file;
	file.open(fileName);

	if (file.is_open())
	{
		std::getline(file, line);
		
		while (file.good())
		{
			std::getline(file, line);
			auto in = parse_input(',', line);
			h = in[in.size() - 1]; 
			std::vector<int> v;
			v.insert(v.begin(), in.begin(), in.end() - 1); // copy vector, except for hamming norm
			oracle[h].push_back(v);
		}
	}

	file.close();

	return oracle;
}


void oeka::assignOracle(std::map<int, std::vector<std::vector<int>>> oracle)
{
	/*
	// map is in ascending order, so last element key + 1 is the number of keys (number of hamming norms)
	// counter is for keeping track of 
	std::vector<int> counter(oracle.end()->first + 1); 

	for (auto& [key, value] : oracle)
	{
		counter[key] = (int)value.size();
	}*/

	// index location of class in the oracle, which is simply the last element
	int c = oracle[0][0][oracle[0][0].size() - 1];

	// do this dynamically
	// store oracle within the hanselChainSet as int
	// use vector or counter or somethingg to keep track of questions which have already been answered
	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			// get hamming norm of current data point to quickly find the 
			int h = 0;

			for (auto e : hanselChainSet[i][j].dataPoint)
			{
				h += e;
			}

			for (int k = 0; k < (int)oracle[h].size(); k++)
			{
				std::vector<int> v;
				v.insert(v.end(), oracle[h][k].begin(), oracle[h][k].end() - 1); // last element is class, so remove it for comparison

				if (hanselChainSet[i][j].dataPoint == v)
				{
					hanselChainSet[i][j].oracle = oracle[h][k][c];
				}
			}
		}
	}
}