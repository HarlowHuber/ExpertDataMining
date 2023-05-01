#include "expertDataMining.h"


std::vector<std::vector<dvector>> genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain)
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
					b[i] = chains.at(j)[k][l].dataPoint[i - 1]; // used to be for std::oldVector: b[i] = chains.at(j)[k][l][i - 1];
				}

				//b.setLevel();
				dvector c;
				c.dataPoint = b;
				chains.at(j)[k][l] = c;
			}
		}
	}

	// move the largest of each sub std::oldVector to corresponding sub std::oldVector in the first key
	// then move the largest of each subsequent sub std::oldVector to the second key's sub std::oldVector
	// repeat until chain.at(num-2) is reached since the last key will have nothing moved to it
	// j = current key being added to
	// emptV = number of empty std::vectors
	for (int j = 0; j < chains.size() - 1; j++)
	{
		// count is to keep up with the elemental std::oldVector having elements removed/moved from
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

				/// USE FIND AND END because kth std::oldVector doesn't exist for 3d 3val, k=2
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

			//create a std::oldVector of std::vectors to hold the baseChain
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
	// skip if oldVector is majority flag and usedMajorityFlag, was visited, and updatedQuery Order > 0 
	// (if updated query order is 0, then visited (expanded), but not asked, so retrieve class and do immediete expansions)
	// Otherwise, question may be asked.
	else
	{
		return true;
	}

	// if oldVector has not been visited, then ask user class
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


std::vector<std::string> parse_input(char delimiter, std::string temp)
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


void violationOfMonotonicity()
{
	std::cout << "\n\nChecking for violations of monotonicity:\n" << std::endl;

	const char separator = ' ';
	const int width = 15;

	//print("Number", width, separator);
	//print("Datapoint", width, separator);
	//print("Class", width, separator);
	std::cout << "Number\t\tDatapoint\t\tClass" << std::endl;

	for (int i = 0; i < hanselChainSet.size(); i++)
	{
		for (int j = 0; j < hanselChainSet[i].size(); j++)
		{
			//print(hanselChainSet[i][j].number, width, separator);
			//print(hanselChainSet[i][j].dataPoint, width, separator);
			//print(hanselChainSet[i][j]._class, width, separator);
			std::cout << hanselChainSet[i][j].number << "\t\t" << hanselChainSet[i][j].dataPoint 
				<< "\t\t" << hanselChainSet[i][j]._class << std::endl;
		}
	}

	std::string temp;
	std::cout << "Please enter the number of any vectors which are a violation of monotonicity in a comma-separalited list (e.g. 1.1, 3.2, ..., 7.4): " << std::flush;

	try
	{
		std::getline(std::cin, temp);
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}
	catch (std::exception e)
	{
		std::cerr << "Violation of monotonicity user input fail. " << e.what() << std::flush;

		return;
	}

	// parse user input
	auto tokens = parse_input(',', temp);

	// fix any violations of monotonicity
	for (auto token : tokens)
	{
		try
		{
			int pos = (int)token.find(".");
			std::string n = token.substr(0, pos);
			int i = stoi(n) - 1;
			token.erase(0, pos + 1);
			int j = stoi(token) - 1;

			checkViolationOfMonotonicityMethod(i, j);
		}
		catch (std::exception)
		{
			std::cout << "The vector specified does not exist, so it has been skipped." << std::endl;
		}
	}

	// add new attributes for those vectors which have violations 
	for (int i = 0; i < (int)addNewAttributesFor.size() - 2; i += 3)
	{
		// temporary: only latest attribute has the summary available
		orderOfAskingSummary.clear();
		questionsAsked = 0;

		std::cout << "Adding a new attribute." << std::endl;
		fixViolationOfMonotonicityAddAttr();

		std::cout << "Fixing the violation of monotonicity for that new attribute." << std::endl;
		fixViolationOfMonotonicityClass(addNewAttributesFor[i], addNewAttributesFor[i + 1], addNewAttributesFor[i + 2], true);
	}
}


void checkViolationOfMonotonicityMethod(int i, int j)
{
	int vector_class = hanselChainSet[i][j]._class;
	hanselChainSet[i][j].expanded_by = nullptr;

	if (vector_class)
	{
		vector_class = 0;
	}
	else
	{
		vector_class = 1;
	}

	// check if the change needs to be fixed in the first place
	// if the new vector class is supposed to be 1 and the vector above it is also one, then there is no violation
	// however, we still need to check those other vectors that are not monotonically related to the changed vector
	if (vector_class)
	{
		auto f1 = [&vector_class, &i, &j]()
		{
			// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
			// in this these, since the class went from 0 to 1, we re-ask those vectors which were expanded to a class of 0
			for (auto vector : hanselChainSet[i][j].expanded_zero)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				// if the class changed from 0 to 1 for those vectors which were re-asked due to being expanded by the source of the f-change,
				// then, we need to fix the expansions for those vectors.
				if (vector_class != 0)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}

			hanselChainSet[i][j].expanded_zero.clear();
			hanselChainSet[i][j]._class = vector_class;
		};

		try
		{
			// if there is a vector above and the classes are equal, or if the current vector is the top
			if (hanselChainSet[i].size() - 1 > j && vector_class == hanselChainSet[i][j + 1]._class || hanselChainSet[i].size() - 1 == j) 
			{
				f1();

				return;
			}
		}
		catch (std::exception&) // shouldn't be necessary
		{
			f1();

			return;
		};
	}

	// check if the change needs to be fixed in the first place
	// if the new vector class is supposed to be 0 and the vector below it is also 0, then there is no violation
	// however, we still need to check those other vectors that are not monotonically related to the changed vector
	else
	{
		auto f2 = [&vector_class, &i, &j]()
		{
			// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
			// in this case, since the class went from 1 to 0, were re-ask those vectors which were expanded to a class of 1
			for (auto vector : hanselChainSet[i][j].expanded_one)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				// if the class changed from 1 to 0 for those vectors which were re-asked due to being expanded by the source of the f-change,
				// then, we need to fix the expansions for those vectors.
				if (vector_class != 1) 
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}

			hanselChainSet[i][j].expanded_one.clear();
			hanselChainSet[i][j]._class = vector_class;
		};

		try
		{
			// if there is a vector below and the classes are equal, or if the current vector is the lowest in the chain
			if (j > 0 && vector_class == hanselChainSet[i][j - 1]._class || j == 0)
			{
				f2();

				return;
			}
		}
		catch (std::exception&) // this shouldn't be necessary
		{
			f2();

			return;
		};
	}

	int c;
	std::cout << "Do you want to fix the violation by adding a new attribute to the expert dataset or by changing the class? Enter (1/0): " << std::flush;
	std::cin >> c;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	// if the violation of monotoncity was an vector that was asked, directly rectify it 
	if (vector_class != hanselChainSet[i][j]._class)
	{
		if (c)
		{
			std::cout << "When adding a new attribute to fix the monotonicity, we assume that the current vector's class " 
				<< "will the opposite of what it was originally given." << std::endl;
			addNewAttributesFor.push_back(i);
			addNewAttributesFor.push_back(j);
			addNewAttributesFor.push_back(vector_class);
		}
		else // find correct answer
		{
			std::cout << "Does the user want to preserve monotonicity? Enter (1/0): " << std::flush;
			std::cin >> c;
			std::cin.clear();
			std::cin.ignore(1000, '\n');

			if (c)
			{
				fixViolationOfMonotonicityClass(i, j, vector_class, true);
			}
			else
			{
				fixViolationOfMonotonicityClass(i, j, vector_class, false);
			}
		}
	}


	// NOTE: below code is commented out because it is no longer needed to find the parent before fixing a violation,
	// but it is possible that we may want to do something with that idea in the future.

	// if the violation of monotoncity was a oldVector that was expanded, find the parent and rectify it.
	// then, rectify the children
	/*else if (vector_class != hanselChainSet[i][j]._class && hanselChainSet[i][j].expanded_by)
	{
		if (c)
		{
			std::cout << "When adding a new attribute to fix the monotonicity, we assume that the current vector's class "
				<< "will the opposite of what it was originally given." << std::endl;

			auto parent = hanselChainSet[i][j].expanded_by;

			while (true)
			{
				if (parent->expanded_by)
				{
					parent = parent->expanded_by;
				}
				else
				{
					break;
				}
			}

			addNewAttributesFor.push_back(parent->number.first - 1);
			addNewAttributesFor.push_back(parent->number.second - 1);
			addNewAttributesFor.push_back(vector_class);
		}
		else // find correct answer
		{
			auto parent = hanselChainSet[i][j].expanded_by;

			while (true)
			{
				if (parent->expanded_by)
				{
					parent = parent->expanded_by;
				}
				else
				{
					break;
				}
			}

			std::cout << "Does the user want to preserve monotonicity? Enter (1/0): " << std::flush;
			std::cin >> c;

			if (c)
			{
				fixViolationOfMonotonicityClass(parent->number.first - 1, parent->number.second - 1, vector_class, true);
			}
			else
			{
				fixViolationOfMonotonicityClass(parent->number.first - 1, parent->number.second - 1, vector_class, false);
			}
		}
	}*/
}


void monotonicityReaffirmation(int i, int a, int j, int vector_class)
{
	std::cout << "Monotonicity reaffirmation process:\n"
		<< "What is the class of the vector : (" << hanselChainSet[i][a].dataPoint << ") ? Enter (1/0) : " << std::flush;
	int c;
	std::cin >> c;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	if (c != vector_class)
	{
		std::cout << "The vector (" << hanselChainSet[i][a].dataPoint << ") is adjacent to the vector which was specified to have an f-change,\nbut the class"
			<< " is different, causing a violation. Since monotonicity was not preserved,\nwould the user like to add a new attribute to fix the monotonicity (1),"
			<< " or would the user prefer to use a non-monotonic function (0)? Enter (1/0): " << std::flush;
		std::cin >> c;
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		if (c)
		{
			std::cout << "\nWhen adding a new attribute to fix the monotonicity, we assume that the current vector's class "
				<< "will the opposite of what it was originally given so that it matches with its parent vector." << std::endl;
			addNewAttributesFor.push_back(i);
			addNewAttributesFor.push_back(a);
			addNewAttributesFor.push_back(vector_class);
		}
		// non-monotonic function
		else
		{
			nonMonotonicVectors.push_back(&hanselChainSet[i][j]);
		}
	}
}


void fixViolationOfMonotonicityClass(int i, int j, int vector_class, bool preserve)
{
	hanselChainSet[i][j]._class = vector_class; // reassign class here 
	hanselChainSet[i][j].f_change = true;

	// if vector class is true
	if (vector_class)
	{
		// need to delete any "expanded ones" of other vectors that possibly expanded this one because the class has been changed.
		// needs to be done before the actual process of fixing because these expanded_zeroes 
		// 
		// needs to check the expandable ones to check for those vectors which are greater than the source of the f-change (changed to one, so check vectors above which expanded to below)
		for (auto& vector : hanselChainSet[i][j].expandable_one)
		{
			bool flag = false;

			for (int k = 0; k < (int)vector->expanded_zero.size(); k++)
			{
				if (vector->expanded_zero[k] == &hanselChainSet[i][j])
				{
					vector->expanded_zero.erase(vector->expanded_zero.begin() + k);
					flag = true;
					break;
				}
			}

			// break again because there can only be one parent that expanded a particular vector
			if (flag)
			{
				break;
			}
		}

		hanselChainSet[i][j].expanded_one.clear();

		// monotonicity preservation
		if (preserve)
		{
			for (auto vector : hanselChainSet[i][j].expandable_one)
			{
				if (!vector->f_change && vector->_class != hanselChainSet[i][j]._class)
				{
					vector->expanded_by = &hanselChainSet[i][j];
					hanselChainSet[i][j].expanded_one.push_back(vector);
					fixViolationOfMonotonicityClass(vector->number.first - 1, vector->number.second - 1, vector_class, true);
				}
			}
		}
		// monotonicity reaffirmation
		else
		{
			if (hanselChainSet[i].size() > j + 1)
			{
				monotonicityReaffirmation(i, j + 1, j, vector_class);
			}
		}

		// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
		// in this case, since the class went from 0 to 1, were re-ask those vectors which were expanded to a class of 0
		for (auto vector : hanselChainSet[i][j].expanded_zero)
		{
			if (!vector->f_change)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				if (vector_class != vector->_class)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}
		}

		hanselChainSet[i][j].expanded_zero.clear();
	}
	// vector class is false
	else
	{
		// need to delete any "expanded ones" of other vectors that possibly expanded this one because the class has been changed.
		// needs to be done before the actual process of fixing because these expanded_zeroes 
		// 
		// needs to check the expandable zeroes to check for those vectors which are smaller than the source of the f-change (changed to zero, so check below that expanded above)
		for (auto& vector : hanselChainSet[i][j].expandable_zero)
		{
			bool flag = false;

			for (int k = 0; k < (int)vector->expanded_one.size(); k++)
			{
				if (vector->expanded_one[k] == &hanselChainSet[i][j])
				{
					vector->expanded_one.erase(vector->expanded_one.begin() + k);
					flag = true;
					break;
				}
			}

			// break again because there can only be one parent that expanded a particular vector
			if (flag)
			{
				break;
			}
		}

		hanselChainSet[i][j].expanded_zero.clear();

		// monotonicity preservation
		if (preserve)
		{
			for (auto vector : hanselChainSet[i][j].expandable_zero)
			{
				if (!vector->f_change && vector->_class != hanselChainSet[i][j]._class)
				{
					vector->expanded_by = &hanselChainSet[i][j];
					hanselChainSet[i][j].expanded_zero.push_back(vector);
					fixViolationOfMonotonicityClass(vector->number.first - 1, vector->number.second - 1, vector_class, true);
				}
			}
		}
		// monotonicitiy reaffirmation
		else
		{
			if (j > 0)
			{
				monotonicityReaffirmation(i, j - 1, j, vector_class);
			}
		}

		// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
		// in this these, since the class went from 1 to 0, were re-ask those vectors which were expanded to a class of 1
		for (auto vector : hanselChainSet[i][j].expanded_one)
		{
			if (!vector->f_change)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				if (vector_class != vector->_class)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}
		}

		hanselChainSet[i][j].expanded_one.clear();
	}
}


void fixViolationOfMonotonicityAddAttr()
{
	dimension++;
	attributes.push_back("");
	attributes[attributes.size() - 1] = 'a' + dimension - 1; // to string does not work for some reason... have to do this method
	kv_attributes.push_back(2);
	auto oldChains = hanselChainSet;
	hanselChainSet.clear();
	//std::cout << "What is the name of the new attribute?: " << std::flush;
	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	calculateAllPossibleExpansions();
	numberAssignment();

	// for each old oldVector in the old chains
	for (int i = 0; i < (int)oldChains.size(); i++)
	{
		for (int j = 0; j < (int)oldChains[i].size(); j++)
		{
			auto oldVector = oldChains[i][j];

			// for each new oldVector in the new chains
			for (int k = 0; k < (int)hanselChainSet.size(); k++)
			{
				bool flag = false;

				for (int l = 0; l < (int)hanselChainSet[k].size(); l++)
				{
					bool difference = false;

					// for each value of a oldVector in the new hansel chains
					for (int o = 0; o < dimension - 1; o++)
					{
						if (hanselChainSet[k][l].dataPoint[o] != oldVector.dataPoint[o])
						{
							difference = true;
						}
					}

					// if no difference (besides the new attribute)
					// it is assumed that the new attribute with a value of 1, in addition to an already true data point, will be true.
					// else, it can be assumed that a new attribute with a value of 0, in addition to an already false data point will be true
					if (!difference &&
						(hanselChainSet[k][l].dataPoint[dimension - 1] == 1 && oldVector._class == 1 ||
							hanselChainSet[k][l].dataPoint[dimension - 1] == 0 && oldVector._class == 0))
					{
						hanselChainSet[k][l].visited = true;
						hanselChainSet[k][l]._class = oldVector._class;

						// update index locations of add new attributes.
						// this works because if the old vectors meet the constraints of above, then we can update with the new location of the new vector
						for (int a = 0; a < (int)addNewAttributesFor.size() - 2; a += 3)
						{
							if (oldVector.number.first - 1 == addNewAttributesFor[a] && oldVector.number.second - 1 == addNewAttributesFor[a + 1])
							{
								addNewAttributesFor[a] = k;
								addNewAttributesFor[a + 1] = l;
							}
						}

						flag = true;
					}
				}

				if (flag)
				{
					break;
				}
			}
		}
	}

	// ask new questions on the new vectors
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			if (!hanselChainSet[i][j].visited)
			{
				int vector_class = -1;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);
			}
		}
	}

	std::cout << "Done adding new attributes and asking questions about those attributes." << std::endl;
}


std::vector<std::vector<int>> addNewAttributesToRealData()
{
	std::vector<std::vector<int>> dataset;
	std::fstream file;
	file.open(filename);

	if (file)
	{
		std::string str;
		std::getline(file, str);
		auto tokens = parse_input(',', str);
		real_attributes = tokens;
		int attributesToAdd = dimension - ((int)tokens.size() - 1); // subtract 1 because last element is class

		// put dataset into data structure
		while (std::getline(file, str))
		{
			// parse input
			auto temp = parse_input(',', str);
			std::vector<int> tokens;
			tokens.reserve(temp.size());

			for (auto token : temp)
			{
				tokens.push_back(stoi(token));
			}

			dataset.push_back(tokens);
		}

		// add attributes to dataset
		for (int i = 0; i < attributesToAdd; i++)
		{
			const int size = (int)dataset.size(); 

			for (int j = 0; j < size; j++)
			{
				// copy datapoint
				std::vector<int> new_datapoint(dataset[j].size());
				std::copy(dataset[j].begin(), dataset[j].end(), new_datapoint.begin());

				// append 1 to old datapoint
				dataset[j].insert(dataset[j].end() - 1, 1);

				// append 0 to new datapoint and add to dataset
				new_datapoint.insert(dataset[j].end() - 1, 0);
				dataset.push_back(new_datapoint);
			}
		}

		file.close();
	}

	return dataset;
}


void applyBoolFuncToRealData(std::vector<std::vector<int>> boolFunc, std::vector<std::vector<int>> dataset)
{
	for (auto& datapoint : dataset)
	{
		// check if datapoint matches a clause in the Boolean function
		// clauses represent if a clause in the Boolean function is true or false for a given datapoint
		std::vector<bool> clauses(boolFunc.size(), true); // if clause i is true, then clauses[i] = true

		for (int i = 0; i < boolFunc.size(); i++)
		{
			for (int j = 0; j < dimension; j++)
			{
				if (boolFunc[i][j] && !datapoint[j])
				{
					clauses[i] = false;
					break;
				}
			}

			// even if datapoint succeeded in being true, we need to check if it is a nonmonotonic exception 
			if (clauses[i] && (int)boolFunc[i].size() > dimension)
			{
				bool isException = true; // this variable needs to be true if there is a nonmonotonic exception

				for (int j = dimension; j < (int)boolFunc[i].size(); j++)
				{
					if (boolFunc[i][j] != datapoint[j])
					{
						isException = false; // flipped to false if at any point the exception does not match the current datapoint, meaning there is no exception
						break;
					}
				}

				// if the datapoint is a nonmonotonic exception, then the datapoint is false whereas it would have been true otherwise
				if (isException) 
				{
					clauses[i] = false;
				}
			}
		}

		// determine class
		bool wrote_1 = false;

		for (bool clause : clauses)
		{
			if (clause) // as long as one clause is true, then class is 1
			{
				datapoint[datapoint.size() - 1] = 1;
				wrote_1 = true;
				break;
			}
		}

		if (!wrote_1) // if no clause is true, then class is 0
		{
			datapoint[datapoint.size() - 1] = 0;
		}
	}

	// write new dataset to file
	std::ofstream file("new_" + filename);

	if (file)
	{
		for (auto at : real_attributes)
		{
			file << at << ",";
		}

		file << "\n";

		for (auto datapoint : dataset)
		{
			file << datapoint << "\n";
		}

		file.close();
	}
}


void changeAttributesOfRealData(std::vector<std::vector<int>> dataset)
{
	// ask user which attribute to change, or if just the class can be changed
	auto askAttributeToChange = [](std::vector<int>& datapoint)
	{
		int c;
		std::cout << "The real datapoint (" << datapoint << ") has a value which does not match the value of the expert datapoint.\n"
			<< "Do we want to change the real datapoint? Enter (1/0): " << std::flush;
		std::cin >> c;

		if (c)
		{
			std::cout << "\nDo we want to change the class of the real datapoint to match the expert datapoint (1),"
				<< " or do we want to change an attribute of the real datapoint (0)? Enter (1/0): " << std::flush;
			std::cin >> c;

			if (c)
			{
				return; 
			}
			else
			{
				std::cout << "\nWhich attribute needs to be changed in this datapoint to match the expert data: ("
					<< datapoint << ")? (Enter a valid index location, or anything else to stop): " << std::flush;
				std::cin >> c;

				try
				{
					if (datapoint[c])
					{
						datapoint[c] = 0;
					}
					else
					{
						datapoint[c] = 1;
					}
				}
				catch (std::exception e)
				{
					std::cerr << "Improper index location: " << e.what() << std::flush;
				}
			}
		}
	};

	// change real data attributes or class
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < hanselChainSet[i].size(); j++)
		{
			for (auto& datapoint : dataset)
			{
				auto temp = datapoint;
				int _class = datapoint.back();
				temp.pop_back(); // pop off class for comparison

				if (hanselChainSet[i][j].dataPoint == temp && hanselChainSet[i][j]._class != _class)
				{
					// ask user which attribute to swap
					datapoint.pop_back();
					askAttributeToChange(datapoint);
					datapoint.push_back(hanselChainSet[i][j]._class); // should be equal to dimension, but just in case
				}
			}
		}
	}

	std::ofstream file("new_" + filename);

	if (file)
	{
		for (auto at : real_attributes)
		{
			file << at << ",";
		}

		file << "\n";

		for (auto datapoint : dataset)
		{
			file << datapoint << "\n";
		}

		file.close();
	}
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

			checkExpansions(vector_class, i, j);
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


void calculateAllPossibleExpansions()
{
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			for (int k = 0; k < dimension; k++)
			{
				possibleExpansions(1, i, j, k, 0);
				possibleExpansions(0, i, j, k, 0);
			}
		}
	}
}


void possibleExpansions(int vector_class, int i, int j, int k, int startChain)
{
	// possible expansions from successive chains for a given class
	if (vector_class != hanselChainSet[i][j].dataPoint[k])
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[k] = vector_class;

		// starting in the current chain, search for expanded oldVector
		for (int hc = startChain; hc < numChains; hc++)
		{
			for (int v = (int)hanselChainSet[hc].size() - 1; v >= 0; v--)
			{
				// expand the oldVector and mark it as visited
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

					return;
				}
				else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // if oldVector is visited, then add to "unused" expansions
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
	}

}


void checkExpansions(int vector_class, int i, int j)
{
	if (vector_class)
	{
		for (auto vector : hanselChainSet[i][j].expandable_one)
		{
			if (!vector->visited)
			{
				hanselChainSet[i][j].expanded_one.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->visited = true;
			}
		}
	}
	else
	{
		for (auto vector : hanselChainSet[i][j].expandable_zero)
		{
			if (!vector->visited)
			{
				hanselChainSet[i][j].expanded_zero.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->visited = true;
			}
		}
	}
}


void fixExpansions(int vector_class, int i, int j)
{
	if (vector_class)
	{
		for (auto vector : hanselChainSet[i][j].expandable_one)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].expanded_one.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->expanded_zero.clear(); // clear expanded zero because we are now going to expand vectors with a class of 1
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
	else
	{
		for (auto vector : hanselChainSet[i][j].expandable_zero)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].expanded_zero.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->expanded_one.clear(); // clear expanded one because we are now going to expand vectors with a class of 0
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
}


int askingOfQuestion(int i, int j)
{
	int vector_class = -1;
	bool ask = true;

	// check if the oldVector contains any attribute that must be true.
	// if not, then that oldVector must have a class of 0
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


int init()
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

	calculateAllPossibleExpansions();

	return option;
}


void expertDataMining(int option)
{
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

			// any oldVector order
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

			// any oldVector order
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

}


std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> restoreFunction()
{
	// restore monotone Boolean function
	// iterate over every hansel chain, and check each chain for its "lower one" vector, if it has one
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < (int)hanselChainSet.size(); i++)
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

				break;
			}
		}

		if (all_zero && hanselChainSet[i][--j]._class)
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
					difference++;
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


std::pair<std::string, std::string> functionToString(std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFuncPair)
{
	// non-simplified
	auto boolFunc = boolFuncPair.second;

	// convert Boolean function to string
	std::string boolFuncStrNonSimplified = "";

	// convert not clause to string
	auto checkNotClause = [&boolFunc](std::string& temp, int& i)
	{
		if (boolFunc[i].size() > dimension)
		{
			temp += " & !";

			for (int j = dimension; j < boolFunc[i].size(); j++)
			{
				if (boolFunc[i][j])
				{
					temp += "x" + std::to_string(j + 1 - dimension);
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
				temp += "x" + std::to_string(j + 1);
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
				temp += "x" + std::to_string(j + 1);
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


void printTable(std::fstream& results, std::string boolFuncStrSimplified, std::string boolFuncStrNonSimplified, bool include_violation)
{
	std::string askStr = "";
	std::string answerStr = "";

	for (int i = 0; i < (int)orderOfAskingSummary.size() - 1; i += 2)
	{
		answerStr += std::to_string(hanselChainSet[orderOfAskingSummary[i]][orderOfAskingSummary[i + 1]]._class) + ",";
		askStr += std::to_string(orderOfAskingSummary[i] + 1) + "." + std::to_string(orderOfAskingSummary[i + 1] + 1) + ",";
	}

	results << "Monotone Boolean Function Simplified: " + boolFuncStrSimplified + "\n";
	results << "Monotone Boolean Function Non-simplified:" + boolFuncStrNonSimplified + "\n";
	results << "Order of Questions:," + askStr + "\n";
	results << "Answers:," + answerStr + "\n";
	results << "Total Questions: " + std::to_string(questionsAsked) + "\n\n";
	results << "Reference Number,Vector,";

	// these orders are irrelevent for violation of monotonicity
	if (!include_violation)
	{
		results << "Planned Query Order,"
			<< "Updated Query Order,"
			<< "Final Query Order,"
			<< "Majority Flag,";
	}

	results << "Class,";

	if (include_violation)
	{
		results << "f-Change,";
	}

	results << "Expanded By,"
		<< "Expanded 1-1,"
		<< "Expanded 0-0,"
		<< "Expandable 1-1,"
		<< "Expandable 0-0\n";

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

			// expanded one expansions
			for (auto element : hanselChainSet[i][j].expanded_one)
			{
				expandedOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expanded zero expansions
			for (auto element : hanselChainSet[i][j].expanded_zero)
			{
				expandedZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable one expansions
			for (auto element : hanselChainSet[i][j].expandable_one)
			{
				expandableOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable zero expansions
			for (auto element : hanselChainSet[i][j].expandable_zero)
			{
				expandableZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			results << std::to_string(i + 1) + "." + std::to_string(j + 1) + ",";
			results << std::setfill('0') << std::setw(dimension) << vecStr << ",";
			
			if (!include_violation)
			{
				results << plannedQueryOrder << ",";
				results << updatedQueryOrder << ",";
				results << finalQueryOrder << ",";
				results << hanselChainSet[i][j].majorityFlag << ",";
			}

			results << hanselChainSet[i][j]._class << ",";

			if (include_violation)
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


/// @brief main function
/// @return 
int main()
{
	// find real data
	if (std::filesystem::exists(filename))
	{
		// FIX: dimension is already assigned if file is given
		realData = true;
	}

	int option = init();
	expertDataMining(option);

	// print vectors and monotone Boolean Function to a file
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

		results << "x" << trueAttributes[trueAttributes.size() - 1] + 1 << "\n";
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
	auto boolFunc = restoreFunction();
	auto str = functionToString(boolFunc);
	std::string boolFuncStrSimplified = str.first;
	std::string boolFuncStrNonSimplified = str.second;
	printTable(results, boolFuncStrSimplified, boolFuncStrNonSimplified, false);

	// reset variables
	questionsAsked = 0;
	orderOfAskingSummary.clear();

	// print rectified results
	results << "\nResults After Resolving Any Violations of Monotonicity\n";
	violationOfMonotonicity();
	boolFunc = restoreFunction();

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

	str = functionToString(boolFunc);
	boolFuncStrSimplified = str.first;
	boolFuncStrNonSimplified = str.second;
	printTable(results, boolFuncStrSimplified, boolFuncStrNonSimplified, true);

	// close file
	results << "\n\n";
	results.close();

	return EXIT_SUCCESS;
}
