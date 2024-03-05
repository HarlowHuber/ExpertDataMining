#include "oeka.h"

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


std::vector<int> oeka::parse_input_int(char delimiter, std::string temp)
{
	// parse a string by a delimiter
	std::vector<int> tokens;
	std::string token;
	int pos = 0;

	while ((pos = (int)temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		tokens.push_back(stoi(token));
		temp.erase(0, pos + 1);
	}

	if (temp != "")
	{
		tokens.push_back(stoi(temp)); 
	}

	return tokens;
}


std::vector<std::string> oeka::parse_input_string(char delimiter, std::string temp)
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

	// automatic testing by using a dataset as an oracle
	if (useOracle && std::filesystem::exists(oraclePath))
	{
		auto oracle = readFile(oraclePath);

		calculateHanselChains(dimension);
		numChains = (int)hanselChainSet.size();
		numConfirmedInChains.resize(numChains);
		hanselChainOrder.resize(numChains);
		chainsVisited.resize(numChains);

		assignOracle(oracle);
	}
	else
	{
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

			auto tokens = parse_input_int(',', temp);

			if (!tokens.empty() && tokens[0] != -1)
			{
				genericParentOrChildList.reserve((int)tokens.size() + 1);
				genericParentOrChildList.push_back(-1);

				for (auto token : tokens)
				{
					try
					{
						//int i = stoi(token);
						genericParentOrChildList.push_back(token);
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

					auto tokens = parse_input_int(',', temp);

					genericParentOrChildList.push_back((int)tokens.size()); // group size

					for (auto token : tokens)
					{
						try
						{
							genericParentOrChildList.push_back(token); // each attribute in this group
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

		// ask for k-values for each attribute
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
		std::cout << "This dataset represents the nested attributes for the attribute " << parent_attribute << " of the function that is one level higher." << std::endl;
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
	std::vector<int> tokens;
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
		tokens = parse_input_int(',', tmp);

		for (auto token : tokens)
		{
			int target = token;
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

	// commented out because its not being used right now
	/*
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
	*/
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

	results << "Class,"
		<< "Oracle,"
		<< "Confirmed,";

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
			results << hanselChainSet[i][j].oracle << ",";
			results << hanselChainSet[i][j].confirmed << ",";

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
