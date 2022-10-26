#include "expertDataMining.h"

/// @brief 
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


void askMajorityFlag()
{
	// determine whether to do majority vectors first
	std::cout << "\nUse majority flag(1/0)?\n";
	std::cout << "Enter: " << std::flush;
	std::cin >> useMajorityFlag;

	findMajorityVectors();

	if (useMajorityFlag)
	{
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

		// pick a majority vector (sequential or random if trueMajority > 0)
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

			// if vector has not been visited, then ask user class
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
						trueVectorInd.push_back(j); // save location of true vector
					}
				}
			}

			// expand the current vector
			for (int k = 0; k < dimension; k++)
			{
				// possible expansions from successive chains for a given class from the first chain
				possibleExpansions(vector_class, i, j, k, 0, false);
			}

			// determine if the number of triples found is sufficient
			if (trueMajority && (foundTrueMajority >= (trueMajority / 2) + (trueMajority % 2)))
			{
				break;
			}
		}

		// must be set to false (0) to prevent error where in possibleExpansions, a vector could be expanded when the current chain is before the given chain
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

						// expand the current vector
						for (int k = 0; k < dimension; k++)
						{
							// possible expansions from successive chains for a given class from the first chain
							// only possible expansions because these expansions occur before any other
							possibleExpansions(vector_class, i, j, k, 0, true);
						}
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

						// expand the current vector
						for (int k = 0; k < dimension; k++)
						{
							// possible expansions from successive chains for a given class from the first chain
							// only possible expansions because these expansions occur before any other
							possibleExpansions(vector_class, i, j, k, 0, true);
						}
					}
				}
			}
		}
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


void findMajorityVectors()
{
	// find the "majority vectors"
	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			int hamming_norm = 0;

			for (int k = 0; k < dimension; k++)
			{
				if (hanselChainSet[i][j].dataPoint[k] == 1)
				{
					hamming_norm++;
				}
			}

			if (hamming_norm == ((dimension / 2) + (dimension % 2)))
			{
				majorityVectors.push_back(i);
				majorityVectors.push_back(j);
				majorityVectors.push_back(0);
				hanselChainSet[i][j].majorityFlag = true;
			}
		}
	}
}


bool questionFunc(int i, int j, int& vector_class)
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
	if (!hanselChainSet[i][j].visited)
	{
		vector_class = askingOfQuestion(i, j);
	}
	else
	{
		vector_class = hanselChainSet[i][j]._class;
	}

	return false;
}


void staticOrderQuestionsFunc()
{
	for (int i = 0; i < numChains; i++)
	{
		int chainSize = (int)hanselChainSet[i].size();

		for (int j = 0; j < chainSize; j++)
		{
			int vector_class = -1;

			if (questionFunc(i, j, vector_class))
			{
				continue;
			}

			// expand the current vector
			for (int k = 0; k < dimension; k++)
			{
				// other one expansions (impossible expansions from previous chains, given a specific ordering of chains)
				priorExpansions(1, i, j, k);

				// other zero expansions (impossible expansions from previous chains, given a specific ordering of chains)
				priorExpansions(0, i, j, k);

				// possible expansions from the current chain for the given vector and class
				possibleExpansions(vector_class, i, j, k, i, false);
			}
		}
	}
}


bool chainJumpMajorityFlagHelper(int i)
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


void chainJumpOrderQuestionsFunc()
{
	std::vector<int> skippedVectors;

	if (top)
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true vector" 
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

				// expand the current vector
				for (int k = 0; k < dimension; k++)
				{
					// possible expansions from any chain for the given vector and class 
					// must include current chain if top and vector_class = 0
					possibleExpansions(vector_class, i, j, k, 0, false);
				}

				// if vector_class = 1 and the vector is the first in the chain, 
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

				// expand the current vector
				for (int k = 0; k < dimension; k++)
				{
					// other one expansions (impossible expansions from previous chains, given a specific ordering of chains)
					priorExpansions(1, i, j, k);

					// other zero expansions (impossible expansions from previous chains, given a specific ordering of chains)
					priorExpansions(0, i, j, k);

					// possible expansions from successive chains for a given class from the current chain
					possibleExpansions(vector_class, i, j, k, i, true);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true vector" 
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

				// expand the current vector
				for (int k = 0; k < dimension; k++)
				{
					// possible expansions from any chain for the given vector and class
					possibleExpansions(vector_class, i, j, k, 0, false);
				}

				// if vector_class = 0 and the vector is the first in the chain, 
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

				// expand the current vector
				for (int k = 0; k < dimension; k++)
				{
					// other one expansions (impossible expansions from previous chains, given a specific ordering of chains)
					priorExpansions(1, i, j, k);

					// other zero expansions (impossible expansions from previous chains, given a specific ordering of chains)
					priorExpansions(0, i, j, k);

					// possible expansions from successive chains for a given class from the current chain
					possibleExpansions(vector_class, i, j, k, i, true);
				}
			}
		}
	}
}


void manualHanselChainOrder()
{
	std::cout << "\nThere are " << numChains << " Hansel Chains, labelled 1 through " << numChains << "." << std::endl;

	for (int i = 0; i < numChains; i++)
	{
		std::string suffix;

		if (i == 0) suffix = "st";
		else if (i == 1) suffix = "nd";
		else if (i == 2) suffix = "rd";
		else suffix = "th";

		std::cout << "\nWhat is the " << i + 1 << suffix << " Hansel Chain?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> hanselChainOrder[i];
		hanselChainOrder[i]--;

		if (hanselChainOrder[i] == -1)
		{
			std::cout << "There is no chain zero. Select a different number chain." << std::endl;
			i--;
		}
		else if (!chainsVisited[hanselChainOrder[i]])
		{
			chainsVisited[hanselChainOrder[i]] = true;
		}
		else
		{
			std::cout << "You already selected this chain. Pick a different chain to go next." << std::endl;
			i--;
		}

		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	auto tempSet = hanselChainSet;

	for (int i = 0; i < numChains; i++)
	{
		hanselChainSet[i] = tempSet[hanselChainOrder[i]];
	}
}


void anyVectorOrder()
{
	std::fstream table;
	table.open("table.csv", std::ios::out | std::ios::app);

	// write vectors to file

	table.close();
	LPCWSTR str = L"excel.exe table.csv";
	ShellExecute(NULL, L"open", str, NULL, NULL, SW_SHOWNORMAL);

	// read vectors from file
}


void priorExpansions(int _class, int i, int j, int k)
{
	if (_class != hanselChainSet[i][j].dataPoint[k])
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[k] = _class;

		// starting in the first chain, search for expanded vectors up to the current chain 
		// not possible up to current vector because, by nature, those vectors would be 0 expansions
		for (int hc = 0; hc < i; hc++)
		{
			for (int v = 0; v < hanselChainSet[hc].size(); v++)
			{
				// "Expand" the vector,
				if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && _class)
				{
					hanselChainSet[i][j].prior_one.push_back(&hanselChainSet[hc][v]);

					return;
				}
				else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !_class)
				{
					hanselChainSet[i][j].prior_zero.push_back(&hanselChainSet[hc][v]);

					return;
				}
			}
		}

		if (!_class && !top)
		{
			// in the current chain, search for expanded vectors up to the current vector (0 case only)
			for (int v = 0; v < j; v++)
			{
				// "Expand" the vector
				if (expanded.dataPoint == hanselChainSet[i][v].dataPoint)
				{
					hanselChainSet[i][j].prior_zero.push_back(&hanselChainSet[i][v]);

					return;
				}
			}
		}
	}
}


void possibleExpansions(int vector_class, int i, int j, int k, int startChain, bool callFromSkipped)
{
	// possible expansions from successive chains for a given class
	if (vector_class != hanselChainSet[i][j].dataPoint[k])
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[k] = vector_class;

		// starting in the current chain, search for expanded vector
		for (int hc = startChain; hc < numChains; hc++)
		{
			if (top)
			{
				for (int v = (int)hanselChainSet[hc].size() - 1; v >= 0; v--)
				{
					// expand the vector and mark it as visited
					// these are "used" expansions
					if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited && &hanselChainSet[hc][v] != &hanselChainSet[i][j])
					{
						if (vector_class)
						{
							hanselChainSet[i][j].expandable_one.push_back(&hanselChainSet[hc][v]);
						}
						else
						{
							hanselChainSet[i][j].expandable_zero.push_back(&hanselChainSet[hc][v]);
						}

						// mark as visited and expand vector (assign class)
						hanselChainSet[hc][v].expanded_by = &hanselChainSet[i][j];
						hanselChainSet[hc][v]._class = vector_class;
						hanselChainSet[hc][v].visited = true;

						return;
					}
					else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // if vector is visited, then add to "unused" expansions
					{
						if (vector_class)
						{
							hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);
						}
						else
						{
							hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);
						}

						return;
					}
				}
			}
			
			// start at bottom of chain (if vector class is 0, must check if current chain is greater than given chain,
			// or if useMajorityFlag (whiched to false when done) or if chainJump and j === 0
			else
			{
				for (int v = 0; v < (int)hanselChainSet[hc].size(); v++)
				{
					// expand the vector and mark it as visited
					// these are "used" expansions
					if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited && &hanselChainSet[hc][v] != &hanselChainSet[i][j])
					{
						if (vector_class)
						{
							hanselChainSet[i][j].expandable_one.push_back(&hanselChainSet[hc][v]);
						}
						else if (hc > i || useMajorityFlag || (usedMajorityFlag && callFromSkipped) || (chainJump && j == 0))
						{
							hanselChainSet[i][j].expandable_zero.push_back(&hanselChainSet[hc][v]);
						}

						// mark as visited and expand vector (assign class)
						hanselChainSet[hc][v].expanded_by = &hanselChainSet[i][j];
						hanselChainSet[hc][v]._class = vector_class;
						hanselChainSet[hc][v].visited = true;

						return;
					}
					else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // if vector is visited, then add to "unused" expansions
					{
						if (vector_class)
						{
							hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);

							return;
						}
						else if (hc > i || useMajorityFlag || (usedMajorityFlag && callFromSkipped) || (chainJump && j == 0))
						{
							hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);

							return;
						}
					}
				}
			}
		}
	}

	// possible expansions from successive chains for the OPPOSITE of a given class
	// these are "unused" expansions
	// DO NOT VISIT OR ASSIGN CLASS
	int not_vector_class = 1;

	if (vector_class)
	{
		not_vector_class = 0;
	}

	if (not_vector_class != hanselChainSet[i][j].dataPoint[k])
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[k] = not_vector_class;

		// starting in the current chain, search for expanded vector
		for (int hc = startChain; hc < numChains; hc++)
		{
			if (top)
			{
				for (int v = (int)hanselChainSet[hc].size() - 1; v >= 0; v--)
				{
					if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j])
					{
						if (not_vector_class)
						{
							hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);
						}
						else
						{
							hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);
						}

						return;
					}
				}
			}
			else
			{
				for (int v = 0; v < hanselChainSet[hc].size(); v++)
				{
					if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j])
					{
						if (not_vector_class)
						{
							hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);

							return;
						}
						else if (hc > i || useMajorityFlag || (usedMajorityFlag && callFromSkipped) || (chainJump && j == 0))
						{
							hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);

							return;
						}
					}
				}
			}
		}
	}
}


int askingOfQuestion(int i, int j)
{
	int vector_class = -1;
	bool ask = true;

	// check if the vector contains any attribute that must be true.
	// if not, then that vector must have a class of 0
	for (auto k : trueAttributes)
	{
		if (-1 < k && k < dimension && !hanselChainSet[i][j].dataPoint[k])
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
			if (hanselChainSet[i][j].dataPoint[k])
			{
				std::cout << attributes[k] + "\t\t\t= true (1)" << std::endl;
			}
			else
			{
				std::cout << attributes[k] + "\t\t\t= false (0)" << std::endl;
			}
		}

		std::cout << "Enter Class: " << std::flush;
		std::cin >> vector_class;
		std::cin.clear();
		std::cin.ignore(1000, '\n');
		questionsAsked++;
		hanselChainSet[i][j].asked = true;
		hanselChainSet[i][j].finalQueryOrder = questionsAsked;
	}

	hanselChainSet[i][j]._class = vector_class;
	hanselChainSet[i][j].visited = true;

	return vector_class;
}


void numberAssignment()
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


/// @brief generic printing function
/// @tparam T 
/// @param t 
/// @param width the width of the output in terms of character spaces
/// @param separator the characters to use as fillers in the output if needed to reach the specified width
template<typename T>
void print(T t, const int& width, const char& separator)
{
	std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
}


/// @brief main function
/// @return 
int main()
{
	std::cout << "Expert Data Mining with Hansel Chains.\n" << std::endl;

	std::cout << "How many attributes are in this dataset (what is the dimension)?";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> dimension;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	attributes.resize(dimension);
	kv_attributes.resize(dimension);

	// name every attribute to reduce confusion for user
	for (int i = 0; i < dimension; i++)
	{
		/*std::cout << "\nWhat is the name of attribute x" + std::to_string(i + 1) << "?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');*/
		attributes[i] = 'a' + i;
	}

	// ask for k-values for each attribute
	// THIS SIMULATION ONLY WORKS FOR K-VALUES OF 2, CURRENTLY!!!
	for (int i = 0; i < dimension; i++)
	{
		/*std::cout << "\nWhat is the k_value of attribute " + attributes[i] + "?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> kv_attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');*/
		kv_attributes[i] = 2;
	}

	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	hanselChainOrder.resize(numChains);				
	chainsVisited.resize(numChains);

	// let the user determine the order of the Hansel Chains
	int option;
	std::cout << "\nWhat order to use for the Hansel Chains?";
	std::cout << "\n0 - Shortest Hansel Chain First";
	std::cout << "\n1 - Longest Hansel Chain First";
	std::cout << "\n2 - Manual Hansel Chain Order";
	std::cout << "\n3 - Default Order";
	std::cout << "\n4 - Any Vector Order";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> option;

	// is there any attribute which must be true for answer to be true?
	std::cout << "\nIs there any attribute which must be true for the datapoint to be true?"
		<< "\nEnter the number assigned to each attribute or -1 if there is no such attribute."
		<< "\nIf there are multiple attributes, separate them with a comma.\n";

	for (int i = 0; i < dimension; i++)
	{
		std::cout << attributes[i] + " - " + std::to_string(i) + "\n";
	}

	std::cout << "Enter: " << std::flush;
	std::string temp;
	std::cin >> temp;
	size_t pos = 0;
	std::string token;

	auto f = [&temp, &pos](int i)
	{
		if (-1 < i && i < dimension)
		{
			trueAttributes.push_back(i);
		}

		temp.erase(0, pos + 1);
	};

	while ((pos = temp.find(",")) != std::string::npos) 
	{
		token = temp.substr(0, pos);
		f(std::stoi(token));
	}

	f(std::stoi(temp));


	// determine if program should be chainJump or static
	std::cout << "\nDo you want to use chain jumping (1/0)?";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> chainJump;

	// order vectors and ask questions
	if (chainJump)
	{
		std::cout << "\nShould the program start at the top of the Hansel Chains? (1/0)";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> top;

		switch (option)
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

			// any vector order
		case 4:
			anyVectorOrder();
			numberAssignment();
			askMajorityFlag();
			chainJumpOrderQuestionsFunc();
			break;

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
		switch (option)
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

		// any vector order
		case 4:
			anyVectorOrder();
			numberAssignment();
			askMajorityFlag();
			staticOrderQuestionsFunc();
			break;

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

	// restore monotone Boolean function
	// iterate over every hansel chain, and check each chain for its "lower one" vector, if it has one
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < hanselChainSet.size(); i++)
	{
		bool all_zero = true;
		int j;

		for (j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			// first vector of class 1 is "lower one" vector
			if (hanselChainSet[i][j]._class)
			{
				bool first = false;

				// for every element 1 in the "lower" one vector, 
				// that element is in a clause in the monotone boolean funciton
				for (int k = 0; k < dimension; k++)
				{
					if (hanselChainSet[i][j].dataPoint[k] == 1) 
					{
						all_zero = false;

						if (!first)
						{
							std::vector<int> temp(dimension);
							temp[k] = 1;
							boolFunc.push_back(temp);
							first = true;
						}
						else
						{
							boolFunc[boolFunc.size() - 1][k] = 1;
						}
					}
				}

				break;
			}
		}

		if (all_zero && hanselChainSet[i][--j]._class)
		{
			break;
		}
	}

	std::string boolFuncStrNonSimplified = "";

	for (size_t i = 0; i < boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j]) temp += "x" + std::to_string(j + 1);
		}

		if (!temp.empty() && i > 0) boolFuncStrNonSimplified += " v " + temp;
		else if (!temp.empty()) boolFuncStrNonSimplified += temp;
	}

	// simplify monotone Boolean function
	// check if the absolute value of two difference clauses Hamming norms are equal to the difference of the vectors
	// difference, as in, how many attributes differ from each other
	// then, determine which statement is minimal (less attributes)
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
					i--;
				}
			}
		}
	}

	// convert Boolean function to string
	std::string boolFuncStrSimplified = "";

	for (size_t i = 0; i < boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j]) temp += "x" + std::to_string(j + 1);
		}

		if (!temp.empty() && i > 0)
		{
			boolFuncStrSimplified += " v " + temp;
		}
		else if (!temp.empty())
		{
			boolFuncStrSimplified += temp;
		}
	}

	// print vectors and monotone Boolean Function to the console and to a file
	const char separator = ' ';
	const int vWidth = 10;
	const int width = 18;

	std::cout << "\nMonotone Boolean Function:\n" + boolFuncStrSimplified + "\n" << std::endl;
	print("Number", vWidth, separator);
	print("Vector", vWidth, separator);
	print("Class", vWidth, separator);
	print("Asked", vWidth, separator);
	print("Triple", vWidth, separator);
	print("Expanded 1-1", width, separator);
	print("Expanded 0-0", width, separator);
	print("Unexpandable 1-1", width, separator);
	print("Unexpandable 0-0", width, separator);
	print("Prior 1-1", width, separator);
	print("Prior 0-0", width, separator);
	std::cout << std::endl;

	std::fstream results;
	results.open("results.csv", std::ios::out | std::ios::app);
	results << "Attributes, k-value\n";

	for (int i = 0; i < dimension; i++)
	{
		results << "x" << i + 1 << "," << kv_attributes[i] << "\n";
	}

	// Pilot Questions:
	results << "\nPilot Questions:\n";
	
	// order
	switch (option)
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
	if (trueAttributes.size() > 0)
	{
		results << "True Attributes:,";

		for (int i = 0; i < (int)trueAttributes.size() - 1; i++)
		{
			results << "x" << trueAttributes[i] + 1 << ",";
		}

		results << "x" << trueAttributes[trueAttributes.size() - 1] << "\n";
	}

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
			results << "top: start at the top and skip that chain if the vector class is 1 (revisit later)\n\n";
		}
		else
		{
			results << "bottom: start at the bottom and skip that chain if the vector class is 0 (revisit later)\n\n";
		}
	}
	else
	{
		results << "Static\n\n";
	}


	std::string askStr = "";
	std::string answerStr = "";

	for (size_t i = 0; i < orderOfAskingSummary.size() - 1; i += 2)
	{
		answerStr += std::to_string(hanselChainSet[orderOfAskingSummary[i]][orderOfAskingSummary[i + 1] ]._class) + ",";
		askStr += std::to_string(orderOfAskingSummary[i] + 1) + "." + std::to_string(orderOfAskingSummary[i + 1] + 1) + ",";
	}

	results << "Monotone Boolean Function Simplified: " + boolFuncStrSimplified + "\n";
	results << "Monotone Boolean Function Non-simplified:" + boolFuncStrNonSimplified + "\n";
	results << "Order of Questions:," + askStr + "\n";
	results << "Answers:," + answerStr + "\n";
	results << "Total Questions: " + std::to_string(questionsAsked) + "\n\n";
	results << "Reference Number" << ",";
	results << "Vector" << ",";
	results << "Planned Query Order" << ",";
	results << "Updated Query Order,";
	results << "Final Query Order" << ",";
	results << "Class" << ",";
	results << "Majority Flag" << ",";
	results << "Expanded 1-1" << ",";
	results << "Expanded 0-0" << ",";
	results << "Unexpandable 1-1" << ",";
	results << "Unexpandable 0-0" << ",";
	results << "Prior 1-1" << ",";
	results << "Prior 0-0" << ",\n";

	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			std::string vecStr = "";
			std::string expandedOneStr = "";
			std::string expandedZeroStr = "";
			std::string unexpandableOneStr = "";
			std::string unexpandableZeroStr = "";
			std::string priorOneStr = "";
			std::string priorZeroStr = "";

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

			// expanded one expansions
			for (auto element : hanselChainSet[i][j].expandable_one)
			{
				expandedOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expanded zero expansions
			for (auto element : hanselChainSet[i][j].expandable_zero)
			{
				expandedZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// unexpandable one expansions
			for (auto element : hanselChainSet[i][j].unexpandable_one)
			{
				unexpandableOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// unexpandable zero expansions
			for (auto element : hanselChainSet[i][j].unexpandable_zero)
			{
				unexpandableZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// prior one expansions
			for (auto element : hanselChainSet[i][j].prior_one)
			{
				priorOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// prior zero expansions
			for (auto element : hanselChainSet[i][j].prior_zero)
			{
				priorZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			print(std::to_string(i + 1) + "." + std::to_string(j + 1) + ":", vWidth, separator);
			print(vecStr, vWidth, separator);
			print(hanselChainSet[i][j]._class, vWidth, separator);
			print(hanselChainSet[i][j].asked, vWidth, separator);
			print(hanselChainSet[i][j].majorityFlag, vWidth, separator);
			print(expandedOneStr, width, separator);
			print(expandedZeroStr, width, separator);
			print(unexpandableOneStr, width, separator);
			print(unexpandableZeroStr, width, separator);
			print(priorOneStr, width, separator);
			print(priorZeroStr, width, separator);
			std::cout << std::endl;

			results << std::to_string(i + 1) + "." + std::to_string(j + 1) + ",";
			results << std::setfill('0') << std::setw(dimension) << vecStr << ",";
			results << plannedQueryOrder << ",";
			results << updatedQueryOrder << ",";
			results << finalQueryOrder << ",";
			results << hanselChainSet[i][j]._class << ",";
			results << hanselChainSet[i][j].majorityFlag << ",";
			results << expandedOneStr << ",";
			results << expandedZeroStr << ",";
			results << unexpandableOneStr << ",";
			results << unexpandableZeroStr << ",";
			results << priorOneStr << ",";
			results << priorZeroStr << ",\n";
		}
	}

	print("Questions Asked:", width, separator);
	print("", width, separator);
	print("", width, separator);
	print(questionsAsked, width, separator);

	results << "\n\n";
	results.close();

	return EXIT_SUCCESS;
}
