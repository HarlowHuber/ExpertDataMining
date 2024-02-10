#include "oeka.h"


std::vector<std::vector<oeka::dvector>> oeka::genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain)
{
	std::unordered_map<int, std::vector<std::vector<dvector>>> chains = chain;

	// add key string "0","1",...,num to the beginning of each element in the corresponding key
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
					// FIX
					b[i] = chains.at(j)[k][l].dataPoint[i - 1]; // used to be for std::vector: b[i] = chains.at(j)[k][l][i - 1];
				}

				//std::reverse(b.begin(), b.end()); // its ugly, but i couldnt figure out how to do this in the immediete above code in the 10 min i tried. not as simple as it looks?

				//b.setLevel();
				dvector c;
				c.dataPoint = b;
				chains.at(j)[k][l] = c;
			}
		}
	}

	// move the largest of each sub std::vector to corresponding sub std::vector in the first key
	// then move the largest of each subsequent sub std::vector to the second key's sub std::vector
	// repeat until chain.at(num-2) is reached since the last key will have nothing moved to it
	// j = current key being added to
	// emptV = number of empty std::vectors
	for (int j = 0; j < chains.size() - 1; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			// I think I need to make the code below recursive
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
					// ### DEAL WITH EMPTY std::vectorS~!###
					// chains.at(to_string(count + 1)).at(k).erase();
					break;
				}

				int end = (int)chains.at(count + 1).at(k).size();

				// add last element from position k of key count+1 to position k of j
				chains.at(j)[k].push_back(chains.at(count + 1)[k].at(end - 1));

				// pop off the moved element
				chains.at(count + 1).at(k).pop_back();
			}
		}
	}

	// we need to prepare for the next iteration of i in the first for loop
	// by adding all chains together, then adding combined chains to each key
	for (int j = 1; j < num; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			chains.at(0).push_back(chains.at(j)[k]);
		}
	}

	for (int j = 0; j < num; j++) 
	{
		chains.at(j) = chains.at(0);
	}

	std::vector<std::vector<dvector>> chainsTogether;

	return chainsTogether = chains.at(0);
}

 
void oeka::calculateHanselChains(int vector_dimension)
{
	// For n dimensions, iterate through to generate chains and
	for (int dim_num = 0; dim_num < vector_dimension; dim_num++)
	{
		int num = attributes[dim_num].kv;

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

	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		if (!hanselChainSet[i].size())
		{
			hanselChainSet.erase(hanselChainSet.begin() + i);
			i--;
		}
	}
}


void oeka::askMajorityFlag()
{
	// determine whether to do majority vectors first
	std::cout << "\nUse majority flag(1/0)?\n";
	std::cout << "Enter: " << std::flush;
	std::cin >> useMajorityFlag;

	if (useMajorityFlag)
	{
		findMajorityVectors();

		usedMajorityFlag = true;
		int foundTrueMajority = 0;

		std::cout << "Roughly how many majority vectors will result in a value of true? (Enter 0 if that is unknown)." << std::endl;
		std::cin >> trueMajority;

		// assign order to majority vectors
		for (int i = 0; i < numChains; i++)
		{
			for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
			{
				if (hanselChainSet[i][j].majorityFlag)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
		}

		// assign the planned order to the rest of the vectors
		for (int i = 0; i < numChains; i++)
		{
			if (top)
			{
				for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
				{
					if (!hanselChainSet[i][j].plannedQueryOrder)
					{
						hanselChainSet[i][j].plannedQueryOrder = questionOrder;
						questionOrder++;
					}
				}
			}
			else
			{
				for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
				{
					if (!hanselChainSet[i][j].plannedQueryOrder)
					{
						hanselChainSet[i][j].plannedQueryOrder = questionOrder;
						questionOrder++;
					}
				}
			}
		}

		// pick a majority oldVector (sequential or random if trueMajority > 0)
		questionOrder = 1;
		srand((unsigned int)time(NULL));

		for (int t = 0; t < majorityVectors.size() - 1; t += 3)
		{
			int vector_class = -1;
			int i = majorityVectors[t];
			int j = majorityVectors[t + 1];

			// if true triples is unknown (0), then do not ask randomly. 
			// otherwise, pick a randoom triple
			if (trueMajority > 0)
			{
				while (true)
				{
					int random = (rand() % ((majorityVectors.size() - 3) / 3 + 1)) * 3;

					// only if visited
					if (!majorityVectors[random + 2])
					{
						i = majorityVectors[random];
						j = majorityVectors[random + 1];
						majorityVectors[random + 2] = 1;
						break;
					}
				}
			}

			// updated order must go before actual question because its tracking the intention of the question, not whether it was asked.
			hanselChainSet[i][j].updatedQueryOrder = questionOrder;
			questionOrder++;

			// if oldVector has not been visited, then ask user class
			// class cannot be retrieve otherwise since majority vectors are asked first (cannot be expanded)
			if (!hanselChainSet[i][j].visited)
			{
				vector_class = askingOfQuestion(i, j);

				if (vector_class)
				{
					foundTrueMajority++;

					// save successful location if true
					if (chainJump && j + 1 < (int)hanselChainSet[i].size())
					{
						trueVectorInd.push_back(i);
						trueVectorInd.push_back(j); // save location of true oldVector
					}
				}
			}

			checkExpansions(vector_class, i, j);

			// determine if the number of triples found is sufficient
			if (trueMajority && (foundTrueMajority >= (trueMajority / 2) + (trueMajority % 2)))
			{
				break;
			}
		}

		// must be set to false (0) to prevent error where in possibleExpansions, a oldVector could be expanded when the current chain is before the given chain
		useMajorityFlag = 0;

		// go to successful chains
		if (chainJump)
		{
			for (int t = 0; t < (int)trueVectorInd.size() - 1; t += 2)
			{
				int vector_class = -1;
				int i = trueVectorInd[t];

				if (top)
				{
					for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
					{
						if (j == trueVectorInd[t + 1] || questionFunc(i, j, vector_class))
						{
							continue;
						}

						checkExpansions(vector_class, i, j);
					}
				}
				else
				{
					for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
					{
						if (j == trueVectorInd[t + 1] || questionFunc(i, j, vector_class))
						{
							continue;
						}

						checkExpansions(vector_class, i, j);
					}
				}
			}
		}

		std::cout << "\nEnd majority flagged questions." << std::endl;
	}
	else
	{
		// assign planned order of questions to vectors
		for (int i = 0; i < numChains; i++)
		{
			if (top)
			{
				for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
			else
			{
				for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
		}

		questionOrder = 1;
	}
}


void oeka::findMajorityVectors()
{
	// first used to determine if middlepoints can be used
	// then, if middlepoints should be used (user selected that option).
	bool middlePoint = false; 

	for (auto attr : attributes)
	{
		if (attr.kv > 2)
		{
			middlePoint = true;
			break;
		}
	}

	if (middlePoint)
	{
		std::cout << "Does the user want to use middle points to determine what constitutes a majority datapoint? (1/0) Please Enter: " << std::flush;
		int c;
		std::cin >> c;
		
		if (c)
		{
			middlePoint = true;
		}
		else
		{
			middlePoint = false;
		}
	}

	// calculate max hamming norm
	int max_hamming_norm = 0;

	for (auto attribute : attributes)
	{
		max_hamming_norm += attribute.kv - 1;
	}

	// the number of points that need a value greater than 0 for the datapoint to be qualified as a "majority datapoint"
	int majority = (max_hamming_norm / 2) + (max_hamming_norm % 2); 

	// find the "majority vectors"
	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			int hamming_norm = 0;
			int points = 0; // counts how many points in the datapoint have a value greater than 0

			for (int k = 0; k < dimension; k++)
			{
				if (hanselChainSet[i][j].dataPoint[k] > 0)
				{
					hamming_norm += hanselChainSet[i][j].dataPoint[k];
					points++;
				}
			}

			// if middle points are used, then the number of points need to be the same or more as the majority of the datapoint
			if (middlePoint)
			{
				// these two if statements need to be separated 
				if (hamming_norm == majority && points >= ((dimension / 2) + (dimension % 2)))
				{
					majorityVectors.push_back(i);
					majorityVectors.push_back(j);
					majorityVectors.push_back(0);
					hanselChainSet[i][j].majorityFlag = true;
				}
			}
			else if (hamming_norm == majority)
			{
				majorityVectors.push_back(i);
				majorityVectors.push_back(j);
				majorityVectors.push_back(0);
				hanselChainSet[i][j].majorityFlag = true;
			}
		}
	}
}


template<typename T>
void print(T t, const int& width, const char& separator)
{
	std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
}


std::ostream& operator<<(std::ostream& out, const std::vector<int>& v)
{
	for (int i = 0; i < (int)v.size() - 1; i++)
	{
		out << v[i] << ", ";
	}

	out << v[v.size() - 1];

	return out;
}


std::ostream& operator<<(std::ostream& out, const std::pair<int, int>& p)
{
	out << p.first << '.' << p.second;

	return out;
}


std::vector<std::string>oeka::parse_input(char delimiter, std::string temp)
{
	// parse a string by a delimiter
	std::vector<std::string> tokens;
	std::string token;
	int pos = 0;

	while ((pos = (int)temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		tokens.push_back(token);
		temp.erase(0, pos + 1);
	}
	
	tokens.push_back(temp);

	return tokens;
}


void oeka::binarySearch(int i, int l, int r)
{
	if (l <= r)
	{
		int j = l + (r - l) / 2; // this is middle
		int vector_class = -1;

		if (!questionFunc(i, j, vector_class))
		{
			// should expand everything
			checkExpansions(vector_class, i, j);

			// dual expansions
			dualExpansion(i, l, j);
			dualExpansion(i, j, r);
		}

		if (vector_class == function_kv - 1)
		{
			// update r with middle, go to left side
			binarySearch(i, l, j - 1);
		}
		else if (vector_class == 0)
		{
			// update left with middle, go to right side
			binarySearch(i, j + 1, r);
		}
		else
		{
			// go to left and right if function_kv > 2
			binarySearch(i, l, j - 1);
			binarySearch(i, j + 1, r);
		}
	}
}


void oeka::staticOrderQuestionsFunc()
{
	for (int i = 0; i < numChains; i++)
	{
		// start binary search
		int chainSize = (int)hanselChainSet[i].size();
		int l = 0;
		int r = chainSize - 1;
		int start = l + (r - l) / 2;
		
		binarySearch(i, l, r);

		/*
		// non-binary method
		for (int j = 0; j < chainSize; j++)
		{
			int vector_class = -1;

			if (questionFunc(i, j, vector_class))
			{
				continue;
			}

			checkExpansions(vector_class, i, j);
		}*/
	}
}


bool oeka::chainJumpMajorityFlagHelper(int i)
{
	for (int t = 0; t < (int)trueVectorInd.size() - 1; t += 2)
	{
		if (i == trueVectorInd[t])
		{
			return true;
		}
	}

	return false;
}


void oeka::chainJumpOrderQuestionsFunc()
{
	std::vector<int> skippedVectors;

	if (top)
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true oldVector" 
			if (usedMajorityFlag && chainJumpMajorityFlagHelper(i))
			{
				continue;
			}

			for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
			{
				int vector_class;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);

				// if vector_class = 1 and the oldVector is the first in the chain, 
				// then go to the next chain in hopes that it is 1 and can expand the previous chain
				if (vector_class && j == (int)hanselChainSet[i].size() - 1)
				{
					skippedVectors.push_back(i);
					break;
				}
			}
		}

		for (int i : skippedVectors)
		{
			for (int j = (int)hanselChainSet[i].size() - 2; j >= 0; j--)
			{
				int vector_class;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);
			}
		}
	}
	else
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true oldVector" 
			if (usedMajorityFlag && chainJumpMajorityFlagHelper(i))
			{
				continue;
			}

			for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
			{
				int vector_class;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);

				// if vector_class = 0 and the oldVector is the first in the chain, 
				// then go to the next chain in hopes that it is 1 and can expand the previous chain
				if (!vector_class && !j)
				{
					skippedVectors.push_back(i);
					break;
				}
			}
		}

		for (int i : skippedVectors)
		{
			for (int j = 1; j < (int)hanselChainSet[i].size(); j++)
			{
				int vector_class;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);
			}
		}
	}
}


void oeka::calculateAllPossibleExpansions()
{
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			for (int p = 0; p < dimension; p++)
			{
				// add k-value here
				for (int d = 0; d < attributes[p].kv; d++)
				{
					possibleExpansions(d, i, j, p, 0);
				}

				// used to be just this for boolean
				//possibleExpansions(1, i, j, p, 0);
				//possibleExpansions(0, i, j, p, 0);
			}
		}
	}
}


void oeka::possibleExpansions(int newValue, int i, int j, int p, int startChain)
{
	int oldValue = hanselChainSet[i][j].dataPoint[p];

	// possible expansions from successive chains for a given class
	if (newValue != oldValue)
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[p] = newValue;

		// starting in the current chain, search for expanded oldVector
		for (int hc = startChain; hc < numChains; hc++)
		{
			for (int v = (int)hanselChainSet[hc].size() - 1; v >= 0; v--)
			{
				// expand the oldVector and mark it as visited
				// these are "used" expansions
				if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // comparing memory locations in last clause
				{
					// if newValue is greater than the oldValue, it means that it is an expansion in the positive direction, so the class should be 1.
					if (newValue == oldValue + 1) // used to be just ">"
					{
						hanselChainSet[i][j].up_expandable.push_back(&hanselChainSet[hc][v]);
					}

					// if newValue is less than the oldValue, it measn that the expansion is in the negative direction, so the class should be 0.
					else if (newValue == oldValue - 1) // used to be just "<", or nothing actually (simply an else statement)
					{
						hanselChainSet[i][j].down_expandable.push_back(&hanselChainSet[hc][v]);
					}

					return;
				}

				// this may be obsolete now
				/*else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // if oldVector is visited, then add to "unused" expansions
				{
					// if newValue is greater than the oldValue, it means that it is an expansion in the positive direction, so the class should be 1.
					if (newValue > oldValue)
					{
						hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);
					}
					// if newVlua is less than the oldValue, it measn that the expansion is in the negative direction, so the class should be 0.
					else
					{
						hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);
					}

					return;
				}*/
			}
		}
	}
}


void oeka::checkExpansions(int vector_class, int i, int j)
{
	// use dynamic programming solution to not keep checking stuff
	std::map<int, std::vector<int>>* visited_map = new std::map<int, std::vector<int>>;

	if (function_kv == 2)
	{
		if (vector_class)
		{
			checkUp(i, j, vector_class, visited_map);
		}
		else
		{
			checkDown(i, j, vector_class, visited_map);
		}
	}
	else if (function_kv > 2)
	{
		checkUp(i, j, vector_class, visited_map);
		checkDown(i, j, vector_class, visited_map);
	}

	// now go through and find any dual expansions
	for (int k = 0; k < numChains; k++)
	{
		int s = (int)hanselChainSet[k].size();

		if (numConfirmedInChains[k] <= s - 2 && s > 2)
		{
			findDualExpansion(k);
		}
	}

	delete visited_map;
}


void oeka::expandUp(int i, int j, dvector* vector, int vector_class)
{
	if (!vector->visited)
	{
		hanselChainSet[i][j].up_expansions.push_back(vector);
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
		vector->visited = true;
	}

	// if vector class is greater than the current class even though the previous was already visited
	// the vector class must be "greater than or equal" to in this case
	else if (vector_class > vector->_class && !vector->lessThan && !vector->asked)
	{
		// delete the pointer to this vector in the vector that expanded this one
		std::erase(vector->expanded_by->up_expansions, vector);

		// reassign expanded by
		// don't need to mark as visited
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
	}

	// the vector is a strong value if it is maximum function kv and was up expanded
	if (vector->_class == function_kv - 1)
	{
		vector->weak = false;
		vector->confirmed = true; // always confirmed when Boolean
		numConfirmedInChains[vector->number.first - 1]++;
	}
}


void oeka::checkUp(int i, int j, int vector_class, std::map<int, std::vector<int>>* visited_map)
{
	for (auto vector : hanselChainSet[i][j].up_expandable)
	{
		int e_i = vector->number.first - 1;
		int e_j = vector->number.second - 1;
		// base case
		// if vector is not in visited map, then recurse
		if (visited_map->find(e_i) == visited_map->end() ||
			!std::binary_search(visited_map->at(e_i).begin(), visited_map->at(e_i).end(), e_j))
		{
			expandUp(i, j, vector, vector_class);

			// insert expanded vector into map
			if (visited_map->find(e_i) == visited_map->end())
			{
				visited_map->insert(std::pair<int, std::vector<int>>(e_i, std::vector<int>{e_j}));
			}
			else
			{
				visited_map->at(e_i).push_back(e_j);
			}

			// recurse on expanded vector
			checkUp(e_i, e_j, vector->_class, visited_map);
		}
	}
}


void oeka::expandDown(int i, int j, dvector* vector, int vector_class)
{
	if (!vector->visited)
	{
		hanselChainSet[i][j].down_expansions.push_back(vector);
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
		vector->visited = true;
		vector->lessThan = true; // mark as "less than or equal to" for the vector class
	}

	// if vector class is less than the current class even though the previous was already visited
	// the vector class must be "less than or equal to", in this case.
	else if (vector_class < vector->_class && vector->lessThan && !vector->asked)
	{
		// delete the pointer to this vector in the vector that expanded this one
		std::erase(vector->expanded_by->down_expansions, vector);

		// reassign expanded by
		// don't need to mark as visited
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
	}

	// the vector is a strong value if is 0 and was down expanded
	if (vector->_class == 0)
	{
		vector->weak = false;
		vector->confirmed = true; // always confirmed when Boolean
		numConfirmedInChains[vector->number.first - 1]++;
	}
}


void oeka::checkDown(int i, int j, int vector_class, std::map<int, std::vector<int>>* visited_map)
{
	for (auto vector : hanselChainSet[i][j].down_expandable)
	{
		// update i and j with location for expanded vector
		int e_i = vector->number.first - 1;
		int e_j = vector->number.second - 1;

		// base case
		// if vector is not in visited map, then recurse
		if (visited_map->find(e_i) == visited_map->end() ||
			!std::binary_search(visited_map->at(e_i).begin(), visited_map->at(e_i).end(), e_j))
		{
			expandDown(i, j, vector, vector_class);

			// insert expanded vector into map
			if (visited_map->find(e_i) == visited_map->end())
			{
				// make new vector in the map for hansel chain at e_i 
				visited_map->insert(std::pair<int, std::vector<int>>(e_i, std::vector<int>{e_j}));
			}
			else
			{
				// insert into existing hansel chain
				visited_map->at(e_i).push_back(e_j);
			}

			checkDown(e_i, e_j, vector->_class, visited_map);
		}
	}
}


void oeka::findDualExpansion(int i)
{
	int chainSize = (int)hanselChainSet[i].size();

	for (int l = 0, r = chainSize - 1; l < chainSize - 1 && r > 0; l += 2, r += 2)
	{
		// dual expansion if classes are not zero, are equal, but not the same vector
		if (hanselChainSet[i][l]._class != -1 && hanselChainSet[i][l]._class == hanselChainSet[i][r]._class &&
			&hanselChainSet[i][l] == &hanselChainSet[i][r])
		{
			dualExpansion(i, l, r);

			return;
		}
		else if (hanselChainSet[i][l + 1]._class != -1 && hanselChainSet[i][l + 1]._class == hanselChainSet[i][r]._class &&
			&hanselChainSet[i][l + 1] == &hanselChainSet[i][r])
		{
			dualExpansion(i, l + 1, r);

			return;
		}
		else if (hanselChainSet[i][l]._class != -1 && hanselChainSet[i][l]._class == hanselChainSet[i][r - 1]._class &&
			&hanselChainSet[i][l] == &hanselChainSet[i][r - 1])
		{
			dualExpansion(i, l, r - 1);

			return;
		}
		else if (hanselChainSet[i][l + 1]._class != -1 && hanselChainSet[i][l + 1]._class == hanselChainSet[i][r - 1]._class &&
			&hanselChainSet[i][l + 1] == &hanselChainSet[i][r - 1])
		{
			dualExpansion(i, l + 1, r - 1);

			return;
		}
	}
}


void oeka::dualExpansion(int i, int l, int r)
{
	if (hanselChainSet[i][l]._class == hanselChainSet[i][r]._class)
	{
		// j is for current to be expanded vector
		for (int j = l + 1; l < r; j++)
		{
			hanselChainSet[i][j]._class = hanselChainSet[i][l]._class;
			hanselChainSet[i][j - 1].up_expansions.push_back(&hanselChainSet[i][j]);
			hanselChainSet[i][j].expanded_by = &hanselChainSet[i][j - 1];
			hanselChainSet[i][j].visited = true;


			// the vector is a strong value if it is maximum function kv and was up expanded
			if (hanselChainSet[i][j]._class == function_kv - 1)
			{
				hanselChainSet[i][j].weak = false;
				hanselChainSet[i][j].confirmed = true; // always confirmed when Boolean
				numConfirmedInChains[i]++;
			}
			// the vector is a strong value if is 0 and was down expanded
			else if (hanselChainSet[i][j]._class == 0)
			{
				hanselChainSet[i][j].weak = false;
				hanselChainSet[i][j].confirmed = true; // always confirmed when Boolean
				numConfirmedInChains[i]++;
			}
		}
	}
}


void oeka::fixExpansions(int vector_class, int i, int j)
{
	if (vector_class)
	{
		for (auto vector : hanselChainSet[i][j].up_expandable)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].up_expansions.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->down_expansions.clear(); // clear expanded zero because we are now going to expand vectors with a class of 1
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
	else
	{
		for (auto vector : hanselChainSet[i][j].down_expandable)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].down_expansions.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->up_expansions.clear(); // clear expanded one because we are now going to expand vectors with a class of 0
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
}


bool oeka::questionFunc(int i, int j, int& vector_class)
{
	// updated order must go before actual question because its tracking the intention of the question, not whether it was asked.
	if (!hanselChainSet[i][j].majorityFlag || !usedMajorityFlag || !hanselChainSet[i][j].visited || !hanselChainSet[i][j].updatedQueryOrder) // used, not useMajority flag because otherwise may expand twice.
	{
		hanselChainSet[i][j].updatedQueryOrder = questionOrder;
		questionOrder++;
	}

	// skip if vector is majority flag and usedMajorityFlag, was visited, and updatedQuery Order > 0 
	// (if updated query order is 0, then visited (expanded), but not asked, so retrieve class and do immediete expansions)
	// Otherwise, question may be asked.
	else
	{
		return true;
	}

	// if vector has not been visited, then ask user class
	// else, retrieve class
	//if (!hanselChainSet[i][j].visited || hanselChainSet[i][j].weak) // or class is a weak value
	if (!hanselChainSet[i][j].confirmed)
	{
		// used to be in the if statement above
		// || (hanselChainSet[i][j]._class > -1 && hanselChainSet[i][j]._class < function_kv - 1)
		vector_class = askingOfQuestion(i, j);
	}
	else
	{
		vector_class = hanselChainSet[i][j]._class;
	}

	return false;
}


int oeka::askingOfQuestion(int i, int j)
{
	int vector_class = -1;
	bool ask = true;

	// check if the vector contains any attribute that must be true for the vector to be true
	// if not, then that oldVector must have a class of 0
	for (auto attribute : attributes)
	{
		int t = attribute.trueIndex;

		if (-1 < t && t < dimension && hanselChainSet[i][j].dataPoint[t] < attribute.trueValue)
		{
			ask = false;
			vector_class = 0;
			break;
		}
	}

	if (ask)
	{
		orderOfAskingSummary.push_back(i);
		orderOfAskingSummary.push_back(j);


		std::cout << "\nEnter the class for this data point:\n";

		for (int k = 0; k < dimension; k++)
		{
			/*if (hanselChainSet[i][j].dataPoint[k])
			{
				std::cout << attributes[k] + "\t\t\t= true (1)" << std::endl;
			}
			else
			{
				std::cout << attributes[k] + "\t\t\t= false (0)" << std::endl;
			}*/

			// k-value here

			std::cout << attributes[k].name + "\t\t\t= " << hanselChainSet[i][j].dataPoint[k] << std::endl;
		}

		// if function kv is not binary, the current vector's class is not -1 (unassigned), and there is a possibility that the class could be greater (class is less than highest value)
		if (function_kv > 2) // this used to be here, but its also in questionFunc(): && hanselChainSet[i][j]._class > -1 && hanselChainSet[i][j]._class < function_kv - 1
		{
			// do something
			if (!hanselChainSet[i][j].lessThan)
			{
				std::cout << "\nThe current class of this datapoint can be between " << hanselChainSet[i][j]._class << " and " << function_kv - 1
					<< ".\nIt was already expanded, but the class can be a higher value according to the given function k-value." << std::flush;
				std::cout << "Enter Class (" << hanselChainSet[i][j]._class << " - " << function_kv - 1 << "): " << std::flush;
			}
			else
			{
				std::cout << "\nThe current class of this datapoint can be between 0 and " << hanselChainSet[i][j]._class
					<< ".\nIt was already expanded, but the class can be a lower value according to the given function k-value." << std::flush;
				std::cout << "Enter Class (" << 0 << " - " << hanselChainSet[i][j]._class << "): " << std::flush;
			}
		}
		else
		{
			std::cout << "Enter Class (0 - " << function_kv - 1 << "): " << std::flush;
		}

		std::cin >> vector_class;
		std::cin.clear();
		std::cin.ignore(1000, '\n');
		questionsAsked++;
		hanselChainSet[i][j].asked = true;
		hanselChainSet[i][j].confirmed = true;
		numConfirmedInChains[i]++;
		hanselChainSet[i][j].finalQueryOrder = questionsAsked;
	}

	hanselChainSet[i][j]._class = vector_class;
	hanselChainSet[i][j].visited = true;

	if (hanselChainSet[i][j]._class == 0 || hanselChainSet[i][j]._class == function_kv - 1)
	{
		hanselChainSet[i][j].weak = false;
	}

	return vector_class;
}


void oeka::numberAssignment()
{
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			hanselChainSet[i][j].number.first = i + 1;
			hanselChainSet[i][j].number.second = j + 1;
		}
	}
}


std::vector<int> oeka::init()
{
	std::vector<int> genericParentOrChildList;

	if (dimension <= 0) // only executed when the constructor was not initialized with a child_attributes 
	{
		std::cout << "How many attributes are in this dataset (what is the dimension)?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> dimension;
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		attributes.resize(dimension);

		// start sub-functions
		std::cout << "Are there any nested attributes (sub-functions) for this dataset?"
			<< "\nIf not, then enter -1. If yes, please enter a comma-delimited list of which attributes have nested attributes, where:" << std::endl;

		for (int i = 1; i < dimension + 1; i++)
		{
			std::cout << i << " - x" << i << std::endl;
		}

		std::cout << "Please enter: " << std::flush;
		std::string temp;
		std::getline(std::cin, temp);
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		auto tokens = parse_input(',', temp);

		if (!tokens.empty() && tokens[0] != "-1")
		{
			genericParentOrChildList.reserve((int)tokens.size() + 1);
			genericParentOrChildList.push_back(-1);

			for (auto token : tokens)
			{
				try
				{
					int i = stoi(token);
					genericParentOrChildList.push_back(i);
				}
				catch (std::exception& e)
				{
					std::cerr << "User input fail. " << e.what() << std::endl;
				}
			}
		}
		// end sub-functions
	}

	// start parent functions (grouping of attributes)
	if (dimension > 5)
	{
		std::cout << "Are there any groupings of attributes?"
			<< "\nIf not, then enter -1. If yes, please enter how many groups there are (please keep this to 2-5 groups): " << std::endl;
		int groups;
		std::cin >> groups;
		std::cin.clear();
		std::cin.ignore(1000, '\n');
		
		if (groups > 0)
		{
			std::cout << "Please note that all attributes must be in a group since this option was chosen, even if that group is of attributes that are unrelated." << std::endl;

			genericParentOrChildList.reserve(groups + dimension);

			for (int i = 0; i < groups; i++)
			{
				std::cout << "Please enter a comma-delimited list of what attributes are in group " << i + 1 << ": " << std::flush;
			
				std::string temp;
				std::getline(std::cin, temp);
				std::cin.clear();
				std::cin.ignore(1000, '\n');

				auto tokens = parse_input(',', temp);

				genericParentOrChildList.push_back((int)tokens.size()); // group size

				for (auto token : tokens)
				{
					try
					{
						genericParentOrChildList.push_back(stoi(token)); // each attribute in this group
					}
					catch (std::exception& e)
					{
						std::cerr << "User input fail. " << e.what() << std::endl;
					}
				}
			}

			std::cout << "Since parent attributes have been specified (the groupings), the expert data mining process will start with those attributes first." << std::endl;
			return genericParentOrChildList; // return early since this object will be popped out of the tree anyway
		}
	}

	// name every attribute to reduce confusion for user
	for (int i = 0; i < dimension; i++)
	{
		/*std::cout << "\nWhat is the name of attribute x" + std::to_string(i + 1) << "?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');*/
		attributes[i].name = attributeSymbol + std::to_string(i + 1);
	}

	// ask for k-values for each attribute
	// THIS SIMULATION ONLY WORKS FOR K-VALUES OF 2, CURRENTLY!!!
	for (int i = 0; i < dimension; i++)
	{
		if (askKV)
		{
			std::cout << "\nWhat is the k_value of attribute " + attributes[i].name + "?";
			std::cout << "\nEnter: " << std::flush;

			try
			{
				std::cin >> attributes[i].kv;

				if (attributes[i].kv < 2)
				{
					attributes[i].kv = 2;
				}

				std::cin.clear();
				std::cin.ignore(1000, '\n');
			}
			catch (std::exception& e) 
			{
				std::cerr << e.what() << std::endl;
				attributes[i].kv = 2;
			}
		}
		else
		{
			attributes[i].kv = 2;
		}
	}

	// get function_kv if it wasnt already assigned by the constructor
	if (function_kv == -1)
	{
		std::cout << "What is kn+1? It must be 2 or greater. Please enter: " << std::flush;
		int c;
		std::cin >> c;

		if (c < 2)
		{
			std::cout << "By default, kn+1 will be 2." << std::endl;
			function_kv = 2;
		}
		else
		{
			function_kv = c;
		}
	}

	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	numConfirmedInChains.resize(numChains);
	hanselChainOrder.resize(numChains);
	chainsVisited.resize(numChains);

	// let the user determine the order of the Hansel Chains
	std::cout << "\nWhat order to use for the Hansel Chains?";
	std::cout << "\n0 - Shortest Hansel Chain First";
	std::cout << "\n1 - Longest Hansel Chain First";
	std::cout << "\n2 - Manual Hansel Chain Order";
	std::cout << "\n3 - Default Hansel Chain Algorithm Order";
	//std::cout << "\n4 - Any Vector Order"; // NOTE: not implemented and likely won't be, but leave it here anyway
	std::cout << "\nEnter: " << std::flush;

	try
	{
		std::cin >> orderOption;
	}
	catch (std::exception& e)
	{
		std::cerr << "user input fail. " << e.what() << std::endl;
	}

	findTrueAttributes();

	// determine if program should be chainJump or static
	std::cout << "\nDo you want to use chain jumping (1/0)?";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> chainJump;

	calculateAllPossibleExpansions();

	return genericParentOrChildList;
}


void oeka::findTrueAttributes()
{
	// is there any attribute which must be true for answer to be true?
	std::cout << "\nIs there any attribute which must be true for the datapoint to be true?"
		<< "\nEnter the number assigned to each attribute or -1 if there is no such attribute."
		<< "\nIf there are multiple attributes, separate them with a comma.\n";

	for (int i = 0; i < dimension; i++)
	{
		std::cout << attributes[i].name + " - " + std::to_string(i) + "\n";
	}

	std::cout << "Enter: " << std::flush;
	std::string temp = "";

	try
	{
		std::cin >> temp;
	}
	catch (std::exception& e)
	{
		std::cerr << "user input fail. " << e.what() << std::endl;
	}

	size_t pos = 0;
	std::string token;

	auto f = [&temp, &pos, this](int i)
	{
		if (-1 < i && i < dimension)
		{
			attributes[i].trueIndex = i;

			if (attributes[i].kv > 2)
			{
				std::cout << "The k-value of this attribute is: " << attributes[i].kv << std::endl;
				std::cout << "For what value of k is this attribute true? Please enter: " << std::flush;
				int c;
				std::cin >> c;

				if (-1 < i && i < dimension)
				{
					attributes[i].trueValue = c;
				}
				else
				{
					attributes[i].trueValue = 1;
				}
			}
		}

		temp.erase(0, pos + 1);
	};

	while ((pos = temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		f(std::stoi(token));
	}

	f(std::stoi(temp));
}


oeka::oeka(char attributeSymbol) 
{
	this->attributeSymbol = attributeSymbol;
}


oeka::oeka(char attributeSymbol, std::string parent_attribute, int dimension) // used in top down approach after parent is constructed, create the children for that parent
{
	this->attributeSymbol = attributeSymbol;
	this->parent_attribute = parent_attribute;
	this->dimension = dimension;
	attributes.resize(dimension);
	childAttributes.resize(dimension);
}


oeka::oeka(char attributeSymbol, std::string parent_attribute, int dimension, int function_kv) // used in top down approach after parent is constructed, create the children for that parent
{
	this->attributeSymbol = attributeSymbol;
	this->parent_attribute = parent_attribute;
	this->dimension = dimension;
	this->function_kv = function_kv;
	attributes.resize(dimension);
	childAttributes.resize(dimension);
}


oeka::oeka(char attributeSymbol, std::vector<std::vector<std::string>> childAttributes) // used in top down approach, creating parent
{
	this->attributeSymbol = attributeSymbol;
	this->childAttributes = childAttributes;
	dimension = (int)childAttributes.size();
	attributes.resize(dimension);
}


void oeka::start()
{
	if (parent_attribute != "")
	{
		std::cout << "This dataset represents the nested attributes for the attribute " << parent_attribute << " of the function that is one level higher."  << std::endl;
	}

	// find real data
	if (parent_attribute != "" && std::filesystem::exists(filename))
	{
		// FIX: dimension is already assigned if file is given
		realData = true;
	}

	// order vectors and ask questions
	if (chainJump)
	{
		std::cout << "\nShould the program start at the top of the Hansel Chains? (1/0)";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> top;

		switch (orderOption)
		{
			// longest chain first order
		case 1:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() > b.size();
				});;
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;

			// manual Hansel Chain order
			// let user order chains however they want
		case 2:
			manualHanselChainOrder();
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;

			// default order
		case 3:
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;

			// any oldVector order
		/*case 4:
			anyVectorOrder();
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;*/

			// shortest chain first order
		default:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() < b.size();
				});
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;
		}
	}
	else
	{
		switch (orderOption)
		{
			// longest chain first order
		case 1:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() > b.size();
				});;
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;

			// manual Hansel Chain order
			// let user order chains however they want
		case 2:
			manualHanselChainOrder();
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;

			// default order
		case 3:
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;

			// any oldVector order
		/*case 4:
			anyVectorOrder();
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;*/

			// shortest chain first order
		default:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() < b.size();
				});
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;
		}
	}
}


void oeka::printToFile(std::fstream& results)
{
	// print vectors and monotone Boolean Function to a file
	
	if (parent_attribute != "")
	{
		results << "This results file represents the nested function that is associated with attribute " << parent_attribute
			<< " of the function that is one level higher.\n\n";
	}

	results << "Attributes, k-value, sub-attributes\n";

	for (int i = 0; i < dimension; i++)
	{
		results << attributeSymbol << i + 1 << "," << attributes[i].kv << ",";

		if (!childAttributes.empty())
		{
			if (!childAttributes[i].empty())
			{
				for (auto associateAttr : childAttributes[i])
				{
					results << associateAttr << ",";
				}
			}
			else
			{
				results << "None";
			}
		}

		results << "\n";
	}

	// Pilot Questions:
	results << "\nPilot Questions:\n";

	// order
	switch (orderOption)
	{
	case 1:
		results << "Longest Hansel Chain First\n";
		break;

	case 2:
		results << "Manual Hansel Chain Order\n";
		break;

	case 3:
		results << "Default Order\n";
		break;

	case 4:
		results << "Any Vector Order\n";
		break;

	default:
		results << "Shortest Hansel Chain First\n";
		break;
	}

	// true attributes
	results << "True Attributes:,";

	for (auto attribute : attributes)
	{
		if (attribute.trueIndex != -1)
		{
			results << attributeSymbol << attribute.trueIndex + 1 << "=" << attribute.trueValue << ", ";
		}
	}

	results << "\n";

	// majority flag
	if (usedMajorityFlag)
	{
		results << "Majority Flag used," << trueMajority << "\n";
	}
	else
	{
		results << "Majority Flag not used\n";
	}

	// chainJump
	if (chainJump)
	{
		results << "Chain jump ";

		if (top)
		{
			results << "top: start at the top and skip that chain if the oldVector class is 1 (revisit later)\n\n";
		}
		else
		{
			results << "bottom: start at the bottom and skip that chain if the oldVector class is 0 (revisit later)\n\n";
		}
	}
	else
	{
		results << "Static\n\n";
	}

	// print original results
	results << "Original Results\n";
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFunc;
	std::vector<std::string> boolFuncs;
	std::vector<std::string> boolFuncsSimplified;
	std::vector<int> restoreValues;
	std::string tmp;
	std::vector<std::string> tokens;
	std::vector<int> targets;

	//std::cout << "Restore function for all values of kn+1? Please Enter (1/0): " << std::flush;
	int r = 1;
	//std::cin >> r;

	if (r)
	{
		for (int i = 1; i < function_kv; i++)
		{
			targets.push_back(i);
			boolFunc = restoreFunction(i);
			auto str = functionToString(boolFunc, ">=");
			boolFuncs.push_back(str.second);
			boolFuncsSimplified.push_back(str.first);

			// push back again, for target = 0, which is just target = 1 but with "<" signs instead of ">="
			if (i == 1)
			{
				targets.insert(targets.begin(), i - 1);
				auto str = functionToString(boolFunc, "<");
				boolFuncs.insert(boolFuncs.begin(), str.second);
				boolFuncsSimplified.insert(boolFuncsSimplified.begin(), str.first);
			}
		}
	}
	else
	{
		std::cout << "For what values of k " << 0 << function_kv - 1 << " does the user want to restore? Please enter a comma-delimited list: " << std::flush;
		std::getline(std::cin, tmp);
		tokens = parse_input(',', tmp);

		for (auto token : tokens)
		{
			int target = stoi(token);
			targets.push_back(target);
			boolFunc = restoreFunction(target);
			auto str = functionToString(boolFunc, ">=");
			boolFuncs.push_back(str.second);
			boolFuncsSimplified.push_back(str.first);

			// push back again, for target = 0, which is just target = 1 but with "<" signs instead of ">="
			if (target == 1)
			{
				targets.insert(targets.begin(), target - 1);
				auto str = functionToString(boolFunc, "<");
				boolFuncs.insert(boolFuncs.begin(), str.second);
				boolFuncsSimplified.insert(boolFuncsSimplified.begin(), str.first);
			}
		}
	}

	printTable(results, targets, boolFuncsSimplified, boolFuncs, false);

	// reset variables
	questionsAsked = 0;
	orderOfAskingSummary.clear();
	boolFunc.first.clear();
	boolFunc.second.clear();
	boolFuncs.clear();
	boolFuncsSimplified.clear();

	// print rectified results
	results << "\nResults After Applying Any f-Changes\n";
	f_change_check();

	for (auto target : targets)
	{
		std::pair<std::string, std::string> str;

		if (target == 0)
		{
			boolFunc = restoreFunction(target + 1);
			str = functionToString(boolFunc, "<");
		}
		else
		{
			boolFunc = restoreFunction(target);
			str = functionToString(boolFunc, ">=");
		}

		boolFuncs.push_back(str.second);
		boolFuncsSimplified.push_back(str.first);
	}


	if (realData)
	{
		std::cout << "Does the user want to change the attributes of the real data or to apply the Boolean function to the real data? (1/0): " << std::flush;
		int c;
		std::cin >> c;
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		// if new attributes have been added, real data must be expanded
		auto dataset = addNewAttributesToRealData();

		if (c)
		{
			changeAttributesOfRealData(dataset);
		}
		// apply monotoone Boolean Function on real data
		// use boolFunc.first for simplified function
		else
		{
			applyBoolFuncToRealData(boolFunc.first, dataset);
		}
	}

	printTable(results, targets, boolFuncsSimplified, boolFuncs, true);

	// close file
	results << "\n\n";
	results.close();
}


// assume a "<=" relationship when using k-value. for example k = 3. attribute is 1, but user said that the datapoint was true, so then if attrbute is 2, it is also true
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> oeka::restoreFunction(int target)
{
	if (target >= function_kv)
	{
		std::cout << "impossible to restore function." << std::endl;
	}

	// restore monotone Boolean function
	// iterate over every hansel chain, and check each chain for its "lower one" vector, if it has one
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		bool all_zero = true; // to make sure that a datapoint with the correect target class is not all zeroes
		int j;

		for (j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			// first vector of class 1 is "lower one" vector
			if (hanselChainSet[i][j]._class == target) // must be equal to highest function_kv to be "low unit"
			{
				bool first = false;

				// change for k-value

				// for every element 1 in the "lower" one vector, 
				// that element is in a clause in the monotone boolean funciton
				for (int k = 0; k < dimension; k++)
				{
					// if target is 0, then treat datapoints with 0 as a value as a low unit
					if (hanselChainSet[i][j].dataPoint[k] > 0 || !target) // used to be " == 1"
					{
						all_zero = false;

						if (!first)
						{
							std::vector<int> temp(dimension);
							temp[k] = hanselChainSet[i][j].dataPoint[k]; // used to be " = 1"
							boolFunc.push_back(temp);
							first = true;
						}
						else
						{
							boolFunc[boolFunc.size() - 1][k] = hanselChainSet[i][j].dataPoint[k];
						}
					}
				}

				// need to check if the latest clause has a non-monotonic vector
				// if yes, then add that vector to the end of the clause
				for (int k = 0; k < (int)nonMonotonicVectors.size(); k++)
				{
					auto vector = nonMonotonicVectors[k];

					if (i == vector->number.first - 1) // i is the index location of current hansel chain...
					{
						boolFunc[boolFunc.size() - 1].insert(boolFunc[boolFunc.size() - 1].end(), vector->dataPoint.begin(), vector->dataPoint.end());
						nonMonotonicVectors.erase(nonMonotonicVectors.begin() + k);
						k--;
					}
				}

				break; // break if we found low unit
			}
		}

		if (all_zero && hanselChainSet[i][--j]._class == target) // break if datapoint is all zeroes
		{
			break;
		}
	}

	const auto nonSimplifiedBoolFunc = boolFunc;

	// simplify monotone Boolean function
	// check if the absolute value of two difference clauses Hamming norms are equal to the difference of the vectors
	// difference, as in, how many attributes differ from each other
	// then, determine which statement is minimal (less attributes)
	for (int i = 0; i < (int)boolFunc.size() - 1; i++)
	{
		bool decrement_i = false;

		for (int j = i + 1; j < (int)boolFunc.size(); j++)
		{
			int difference = 0;
			int left_hamming_norm = 0;
			int right_hamming_norm = 0;

			for (int k = 0; k < dimension; k++)
			{
				if (boolFunc[i][k] != boolFunc[j][k])
				{
					difference += abs(boolFunc[i][k] - boolFunc[j][k]);
					// used to be for simply Boolean: difference++
				}

				left_hamming_norm += boolFunc[i][k];
				right_hamming_norm += boolFunc[j][k];
			}

			if (abs(left_hamming_norm - right_hamming_norm) == difference)
			{
				if (left_hamming_norm < right_hamming_norm)
				{
					boolFunc.erase(boolFunc.begin() + j);
					j--;
				}
				else if (left_hamming_norm > right_hamming_norm)
				{
					boolFunc.erase(boolFunc.begin() + i);
					decrement_i = true;
				}
			}
		}

		if (decrement_i)
		{
			i--;
		}
	}

	return std::make_pair(boolFunc, nonSimplifiedBoolFunc);
}


std::pair<std::string, std::string> oeka::functionToString(std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFuncPair, std::string sign)
{
	// non-simplified
	auto boolFunc = boolFuncPair.second;

	// convert Boolean function to string
	std::string boolFuncStrNonSimplified = "";

	// convert not clause to string
	auto checkNotClause = [&boolFunc, &sign, this](std::string& temp, int& i)
	{
		if (boolFunc[i].size() > dimension)
		{
			temp += " & !";

			for (int j = dimension; j < boolFunc[i].size(); j++)
			{
				if (boolFunc[i][j])
				{
					if (attributes[j].kv > 2)
					{
						temp += "(";
						temp += attributeSymbol;
						temp += std::to_string(j + 1 - dimension); // just change to boolFunc[i][j] for k-value?
						temp += sign;
						temp += std::to_string(boolFunc[i][j]);
						temp += ")";
					}
					else
					{
						temp += attributeSymbol;
						temp += std::to_string(j + 1);
					}
				}
			}
		}
	};

	for (int i = 0; i < (int)boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j])
			{
				if (attributes[j].kv > 2)
				{
					temp += "(";
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
					temp += sign;
					temp += std::to_string(boolFunc[i][j]);
					temp += ")";
				}
				else
				{
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
				}
			}
		}

		checkNotClause(temp, i);

		if (!temp.empty() && i > 0)
		{
			boolFuncStrNonSimplified += " v " + temp;
		}
		else if (!temp.empty())
		{
			boolFuncStrNonSimplified += temp;
		}
	}

	// simplified
	boolFunc = boolFuncPair.first;

	// convert Boolean function to string
	std::string boolFuncStrSimplified = "";

	for (int i = 0; i < (int)boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j])
			{
				if (attributes[j].kv > 2)
				{
					temp += "(";
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
					temp += sign;
					temp += std::to_string(boolFunc[i][j]);
					temp += ")";
				}
				else
				{
					// if sign is "less than" and attribute is boolean, then use a "NOT" symbol in the function
					if (sign == "<")
					{
						temp += "!";
					}

					temp += attributeSymbol;
					temp += std::to_string(j + 1);
				}
			}
		}

		checkNotClause(temp, i);

		if (!temp.empty() && i > 0)
		{
			boolFuncStrSimplified += " v " + temp;
		}
		else if (!temp.empty())
		{
			boolFuncStrSimplified += temp;
		}
	}

	return std::make_pair(boolFuncStrSimplified, boolFuncStrNonSimplified);
}


void oeka::printTable(std::fstream& results, std::vector<int> targets, std::vector<std::string> boolFuncsSimplified, std::vector<std::string> boolFuncs, bool include_f_change)
{
	std::string askStr = "";
	std::string answerStr = "";

	for (int i = 0; i < (int)orderOfAskingSummary.size() - 1; i += 2)
	{
		answerStr += std::to_string(hanselChainSet[orderOfAskingSummary[i]][orderOfAskingSummary[i + 1]]._class) + ",";
		askStr += std::to_string(orderOfAskingSummary[i] + 1) + "." + std::to_string(orderOfAskingSummary[i + 1] + 1) + ",";
	}

	if (non_monotone)
	{
		for (int i = 0; i < (int)boolFuncsSimplified.size(); i++)
		{
			results << "Non-monotone Boolean Function Simplified: k = " << targets[i] << " = " << boolFuncsSimplified[i] + "\n";
			results << "Non-monotone Boolean Function Non-simplified: k = " << targets[i] << " = " << boolFuncs[i] + "\n";
		}
	}
	else
	{
		for (int i = 0; i < (int)boolFuncsSimplified.size(); i++)
		{
			results << "Monotone Boolean Function Simplified: k = " << targets[i] << " = " << boolFuncsSimplified[i] + "\n";
			results << "Monotone Boolean Function Non-simplified: k = " << targets[i] << " = " << boolFuncs[i] + "\n";
		}
	}

	results << "Order of Questions:," + askStr + "\n";
	results << "Answers:," + answerStr + "\n";
	results << "Total Questions: " + std::to_string(questionsAsked) + "\n\n";
	results << "Reference Number,Vector,";

	// these orders are irrelevent for violation of monotonicity
	if (!include_f_change)
	{
		results << "Planned Query Order,"
			<< "Updated Query Order,"
			<< "Final Query Order,"
			<< "Majority Flag,";
	}

	results << "Class,";

	if (include_f_change)
	{
		results << "f-Change,";
	}

	results << "Expanded By,"
		<< "Up Expansions,"
		<< "Down Expansions,"
		<< "Up Expandable,"
		<< "Down Expandable\n";

	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			std::string vecStr = "";
			std::string expandedOneStr = "";
			std::string expandedZeroStr = "";
			std::string expandableOneStr = "";
			std::string expandableZeroStr = "";

			// data point as string
			for (int k = 0; k < dimension - 1; k++)
			{
				vecStr += std::to_string(hanselChainSet[i][j].dataPoint[k]) + ";";
			}

			vecStr += std::to_string(hanselChainSet[i][j].dataPoint[dimension - 1]);

			// planned order of questions
			std::string plannedQueryOrder = std::to_string(hanselChainSet[i][j].plannedQueryOrder);

			// updated order of questions
			std::string updatedQueryOrder = std::to_string(hanselChainSet[i][j].updatedQueryOrder);

			// final query order
			std::string finalQueryOrder = "";

			if (hanselChainSet[i][j].finalQueryOrder)
			{
				finalQueryOrder += std::to_string(hanselChainSet[i][j].finalQueryOrder);
			}

			// expanded up expansions
			for (auto element : hanselChainSet[i][j].up_expansions)
			{
				expandedOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expanded down expansions
			for (auto element : hanselChainSet[i][j].down_expansions)
			{
				expandedZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable up expansions
			for (auto element : hanselChainSet[i][j].up_expandable)
			{
				expandableOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable down expansions
			for (auto element : hanselChainSet[i][j].down_expandable)
			{
				expandableZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			results << std::to_string(i + 1) + "." + std::to_string(j + 1) + ",";
			results << std::setfill('0') << std::setw(dimension) << vecStr << ",";
			
			if (!include_f_change)
			{
				results << plannedQueryOrder << ",";
				results << updatedQueryOrder << ",";
				results << finalQueryOrder << ",";
				results << hanselChainSet[i][j].majorityFlag << ",";
			}

			results << hanselChainSet[i][j]._class << ",";

			if (include_f_change)
			{
				if (hanselChainSet[i][j].f_change)
				{
					results << "1,";
				}
				else
				{
					results << ",";
				}
			}

			if (hanselChainSet[i][j].expanded_by)
			{
				results << hanselChainSet[i][j].expanded_by->number.first << "." << hanselChainSet[i][j].expanded_by->number.second << ",";
			}
			else
			{
				results << ",";
			}

			results << expandedOneStr << ",";
			results << expandedZeroStr << ",";
			results << expandableOneStr << ",";
			results << expandableZeroStr << "\n";
		}
	}

	results << "\n";
}
