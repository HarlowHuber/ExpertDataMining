#include "expertDataMining.h"


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


/// @brief generates a Hansel Chain from a given dimension and number
/// @param num 
/// @param vector_dimension 
/// @param chain a Hansel Chain
/// @return a Hansel Chain
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


/// @brief calculates a set of Hansel Chains from a given dimension and k-valued attributes
/// @param vector_dimension 
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


/// @brief asks questions based on a random sequence of "majority vectors."
/// These vectors do not have "prior" expansions because they occur before any other expansion that they produce.
/// Hence, they are not prior to any expansion except another majority vector, which is not possible because they 
/// would have the same Hamming Norm.
void majorityFlagQuestionsFunc()
{
	int trueMajority = 0;
	int foundTrueMajority = 0;

	std::cout << "Roughly how many triples will result in a value of true? (Enter 0 if that is unknown)." << std::endl;
	std::cin >> trueMajority;

	// find the "majority vectors"
	for (int i = 0; i < numChains; i++)
	{
		int chainSize = (int)hanselChainSet[i].size();

		for (int j = 0; j < chainSize; j++)
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

	srand((unsigned int)time(NULL));

	// ask expert what class each triple vector belongs to
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

				if (majorityVectors[random + 2]) continue; // skip if already visited

				i = majorityVectors[random];
				j = majorityVectors[random + 1];
				majorityVectors[random + 2] = 1;
				break;
			}
		}

		// if vector has not been visited, then ask user class
		// else, retrieve class
		if (!hanselChainSet[i][j].visited)
		{
			vector_class = askingOfQuestion(i, j);

			if (vector_class) foundTrueMajority++;
		}

		// expand the current vector
		for (int k = 0; k < dimension; k++)
		{
			// possible expansions from successive chains for a given class from the first chain
			possibleExpansions(vector_class, i, j, k, 0);
		}

		// determine if the number of triples found is sufficient
		if (trueMajority && (foundTrueMajority >= (trueMajority / 2) + (trueMajority % 2))) break;
	}
}


/// @brief the manual order of questions
void manualOrderQuestionsFunc()
{
	for (int i = 0; i < numChains; i++)
	{
		int chainSize = (int)hanselChainSet[i].size();

		for (int j = 0; j < chainSize; j++)
		{
			int vector_class = -1;

			// if vector has not been visited, then ask user class
			// else, retrieve class
			if (!hanselChainSet[i][j].visited)
			{
				vector_class = askingOfQuestion(i, j);
			}
			else if (hanselChainSet[i][j].majorityFlag)
			{
				continue;
			}
			else
			{
				vector_class = hanselChainSet[i][j]._class;
			}

			// expand the current vector
			for (int k = 0; k < dimension; k++)
			{
				// other one expansions (impossible expansions from previous chains, given a specific ordering of chains)
				// DO NOT VISIT OR ASSIGN CLASS
				priorExpansions(1, i, j, k);

				// other zero expansions (impossible expansions from previous chains, given a specific ordering of chains)
				// DO NOT VISIT OR ASSIGN CLASS
				priorExpansions(0, i, j, k);

				// possible expansions from successive chains for a given class from the current chain
				possibleExpansions(vector_class, i, j, k, i);
			}
		}
	}
}


/// @brief a prior expansion is an expansion that occurs in a chain that is prior to a given vector.
///	Hence, impossible if the order of questions is the same as the sequence of Hansel Chains (which is true 
/// in the of this program). This means that "majority vector" questions do not have prior expansions because
/// those questions are asked first.
/// @param _class class of prior expansion, either 1 or 0.
/// @param i the Hansel Chain
/// @param j a vector in the Hansel Chain
/// @param k an element in the vector
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
					hanselChainSet[i][j].prior_one.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
				}
				else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !_class)
				{
					hanselChainSet[i][j].prior_zero.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
				}
			}
		}

		if (!_class)
		{
			// in the current chain, search for expanded vectors up to the current vector (0 case only)
			for (int v = 0; v < j; v++)
			{
				// "Expand" the vector
				if (expanded.dataPoint == hanselChainSet[i][v].dataPoint)
				{
					hanselChainSet[i][j].prior_zero.push_back(std::to_string(i + 1) + "." + std::to_string(v + 1));
				}
			}
		}
	}
}


/// @brief expandable expansions and unexpandable expansions
/// Expandable expansions are expansions which are used. Unexpandable expansions are expansions which are unused 
/// because either the vector that would have been expanded was already expanded or if the vector of origin's 
/// class is different than that of the expansion (e.g. a vector of class 1 with an expansion of class 0).
/// @param vector_class either 1 or 0
/// @param i the Hansel Chain
/// @param j a vector in the Hansel Chain
/// @param k an element in the Hansel Chain
/// @param startChain equal to either i for a standard ordering of questions, or 0 for majority vector questions.
void possibleExpansions(int vector_class, int i, int j, int k, int startChain)
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
			for (int v = 0; v < hanselChainSet[hc].size(); v++)
			{
				// expand the vector and mark it as visited
				// these are "used" expansions
				if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited)
				{
					if (vector_class)
					{
						hanselChainSet[i][j].expandable_one.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
					}
					else
					{
						hanselChainSet[i][j].expandable_zero.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
					}

					// mark as visited and expand vector (assign class)
					hanselChainSet[hc][v]._class = vector_class;
					hanselChainSet[hc][v].visited = true;
				}
				else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint) // if vector is visited, then add to "unused" expansions
				{
					if (vector_class)
					{
						hanselChainSet[i][j].unexpandable_one.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
					}
					else
					{
						hanselChainSet[i][j].unexpandable_zero.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
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
		for (int hc = startChain; hc < numChains; hc++)
		{
			for (int v = 0; v < hanselChainSet[hc].size(); v++)
			{
				if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint)
				{
					if (not_vector_class)
					{
						hanselChainSet[i][j].unexpandable_one.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
					}
					else
					{
						hanselChainSet[i][j].unexpandable_zero.push_back(std::to_string(hc + 1) + "." + std::to_string(v + 1));
					}
				}
			}
		}
	}
}


/// @brief helper function that asks the user a question
/// @return the class of the vector;
int askingOfQuestion(int i, int j)
{
	int vector_class = -1;
	orderOfAsking.push_back(i + 1);
	orderOfAsking.push_back(j + 1);

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
	questionsAsked++;
	hanselChainSet[i][j].orderOfQuestion = questionsAsked;

	return vector_class;
}


/// @brief main function
/// @return 
int main()
{
	std::cout << "Expert Data Mining with Hansel Chains.\n" << std::endl;

	std::cout << "How many attributes are in this dataset (what is the dimension)?" << std::endl;
	std::cin >> dimension;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	attributes.resize(dimension);
	kv_attributes.resize(dimension);

	// name every attribute to reduce confusion for user
	for (int i = 0; i < dimension; i++)
	{
		std::cout << "\nWhat is the name of attribute x" + std::to_string(i + 1) << "?" << std::endl;
		std::cin >> attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	// ask for k-values for each attribute
	// THIS SIMULATION ONLY WORKS FOR K-VALUES OF 2, CURRENTLY!!!
	for (int i = 0; i < dimension; i++)
	{
		std::cout << "\nWhat is the k_value of attribute " + attributes[i] + "?" << std::endl;
		std::cin >> kv_attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	order.resize(numChains);				
	chainsVisited.resize(numChains);

	// let the user determine the order of the Hansel Chains
	// giving the number of a chain which is the same as the current number will preserve the natural order
	std::cout << "\nThere are " << numChains << " Hansel Chains, labelled 1 through " << numChains << "." << std::endl;

	for (int i = 0; i < numChains; i++)
	{
		std::string suffix;

		if (i == 0) suffix = "st";
		else if (i == 1) suffix = "nd";
		else if (i == 2) suffix = "rd";
		else suffix = "th";

		std::cout << "\nWhat is the " << i + 1 << suffix << " Hansel Chain? " << std::endl;
		std::cin >> order[i];
		order[i]--;

		if (order[i] == -1)
		{
			std::cout << "There is no chain zero. Select a different number chain." << std::endl;
			i--;
		}
		else if (!chainsVisited[order[i]]) chainsVisited[order[i]] = true;
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
		hanselChainSet[i] = tempSet[order[i]];
	}

	std::cout << "\nUse majority flag or manual order (1/0)?" << std::flush;
	std::cin >> useMajorityFlag;

	if (useMajorityFlag)
	{
		majorityFlagQuestionsFunc();
	}

	int option = 0;

	switch (option)
	{
	case 1:
		break;

	case 2:
		break;

	case 3:
		break;

	default:
		manualOrderQuestionsFunc();
	}


	// restore monotone Boolean function
	// iterate over every hansel chain, and check each chain for its "lower one" vector, if it has one
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < hanselChainSet.size(); i++)
	{
		bool all_zero = true;

		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
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
						else boolFunc[boolFunc.size() - 1][k] = 1;
					}
				}

				break;
			}
		}

		if (all_zero) break;
	}

	std::string boolFuncStr2 = "";

	for (size_t i = 0; i < boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j]) temp += "x" + std::to_string(j + 1);
		}

		if (!temp.empty() && i > 0) boolFuncStr2 += " v " + temp;
		else if (!temp.empty()) boolFuncStr2 += temp;
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

	// print vectors and monotone Boolean Function to the console and to a file
	const char separator = ' ';
	const int vWidth = 10;
	const int width = 18;

	std::cout << "\nMonotone Boolean Function:\n" + boolFuncStr + "\n" << std::endl;
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

	std::string askStr = "";
	std::string answerStr = "";

	for (size_t i = 0; i < orderOfAsking.size() - 2; i += 2)
	{
		answerStr += std::to_string(hanselChainSet[orderOfAsking[i] - 1][orderOfAsking[i + 1] - 1]._class) + ",";
		askStr += std::to_string(orderOfAsking[i]) + "." + std::to_string(orderOfAsking[i + 1]) + ", ";
	}

	results << "Monotone Boolean Function Simplified: " + boolFuncStr + "\n";
	results << "Monotone Boolean Function Non-simplified:" + boolFuncStr2 + "\n";
	results << "Order of Questions:," + askStr + "\n";
	results << "Answers:," + answerStr + "\n";
	results << "Total Questions: " + std::to_string(questionsAsked) + "\n\n";
	results << "Number" << ",";
	results << "Vector" << ",";
	results << "Class" << ",";
	results << "Asked" << ",";
	results << "Majority Flag" << ",";
	results << "Expanded 1-1" << ",";
	results << "Expanded 0-0" << ",";
	results << "Unexpandable 1-1" << ",";
	results << "Unexpandable 0-0" << ",";
	results << "Prior 1-1" << ",";
	results << "Prior 0-0" << ",";
	results << "Order of Questions\n";

	for (int i = 0; i < numChains; i++)
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
			for (int k = 0; k < dimension - 1; k++)
			{
				vecStr += std::to_string(hanselChainSet[i][j].dataPoint[k]) + ";";
			}

			vecStr += std::to_string(hanselChainSet[i][j].dataPoint[dimension - 1]);

			// used one expansions
			for (auto element : hanselChainSet[i][j].expandable_one)
			{
				usedOneExpStr += element + ";";
			}

			// used zero expansions
			for (auto element : hanselChainSet[i][j].expandable_zero)
			{
				usedZeroExpStr += element + ";";
			}

			// unused one expansions
			for (auto element : hanselChainSet[i][j].unexpandable_one)
			{
				unusedOneExpStr += element + ";";
			}

			// unused zero expansions
			for (auto element : hanselChainSet[i][j].unexpandable_zero)
			{
				unusedZeroExpStr += element + ";";
			}

			// other one expansions
			for (auto element : hanselChainSet[i][j].prior_one)
			{
				otherOneExpStr += element + ";";
			}

			// other zero expansions
			for (auto element : hanselChainSet[i][j].prior_zero)
			{
				otherZeroExpStr += element + ";";
			}

			print(std::to_string(i + 1) + "." + std::to_string(j + 1) + ":", vWidth, separator);
			print(vecStr, vWidth, separator);
			print(hanselChainSet[i][j]._class, vWidth, separator);
			print(hanselChainSet[i][j].asked, vWidth, separator);
			print(hanselChainSet[i][j].majorityFlag, vWidth, separator);
			print(usedOneExpStr, width, separator);
			print(usedZeroExpStr, width, separator);
			print(unusedOneExpStr, width, separator);
			print(unusedZeroExpStr, width, separator);
			print(otherOneExpStr, width, separator);
			print(otherZeroExpStr, width, separator);
			std::cout << std::endl;

			results << std::to_string(i + 1) + "." + std::to_string(j + 1) + ",";
			results << std::setfill('0') << std::setw(dimension) << vecStr << ",";
			results << hanselChainSet[i][j]._class << ",";
			results << hanselChainSet[i][j].asked << ",";
			results << hanselChainSet[i][j].majorityFlag << ",";
			results << usedOneExpStr << ",";
			results << usedZeroExpStr << ",";
			results << unusedOneExpStr << ",";
			results << unusedZeroExpStr << ",";
			results << otherOneExpStr << ",";
			results << otherZeroExpStr << ",";

			if (hanselChainSet[i][j].orderOfQuestion)
			{
				results << std::to_string(hanselChainSet[i][j].orderOfQuestion) + "\n";
			}
			else
			{
				results << "\n";
			}
		}
	}

	print("Questions Asked:", width, separator);
	print("", width, separator);
	print("", width, separator);
	print(questionsAsked, width, separator);

	results << "\nQuestions Asked" << "," << questionsAsked << "\n";
	results << "\nAttributes:\n";

	for (auto x : attributes)
	{
		results << x + "\n";
	}

	results << "\n";
	results.close();

	return EXIT_SUCCESS;
}
