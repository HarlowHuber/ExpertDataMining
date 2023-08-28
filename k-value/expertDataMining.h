/*
"Expert" Data Mining with Hansel Chains

Author: Harlow Huber
Supervisor: Dr. Boris Kovalerchuk
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <windows.h>
#include <shellapi.h>
#include <unordered_map>
#include <iomanip>
#include <filesystem>

class expertDataMining
{
public:
	/// @brief dvector, or datapoint vector, is simply a vector with information about a datapoint
	struct dvector
	{
		/// @brief the n-D data point to be considered as a vector
		std::vector<int> dataPoint;

		/// @brief the class or value of the dataPoint
		int _class = -1;

		/// @brief by default the class is great than or equal to. However, if this flag is true, then the class is less than or equal to.
		bool lessThan = false;

		/// @brief the index of this vector's Hansel Chain and the index of the vector in the Hansel Chain, respectively
		std::pair<int, int> number;

		/// @brief the zero-to-zero expansions that occured
		std::vector<dvector*> down_expansions;

		/// @brief the one-to-one expansions that occured
		std::vector<dvector*> up_expansions;

		/// @brief the zero-to-zero expansions that are possible
		std::vector<dvector*> down_expandable;

		/// @brief the one-to-one expansions that are possible
		std::vector<dvector*> up_expandable;

		// unexpandable is probably obsolete now

		/// @brief zero-to-zero expansions that are impossible because the given class was not zero
		//std::vector<dvector*> unexpandable_zero;

		/// @brief one-to-one expansions that are impossible because the given class was not one
		//std::vector<dvector*> unexpandable_one;

		/// @brief true if the vector was "fixed" as a result of a violation of monotonicity, or simply an f-change
		bool f_change = false;

		/// @brief a pointer to a dvector if it expanded this dvector
		dvector* expanded_by = NULL;

		/// @brief if the vector is visited
		bool visited = false;

		/// @brief if this vector was asked to the expert/user
		bool asked = false;

		/// @brief signals whether the vector contains a majority of true attributes, meaning either 50% are true or the ceiling of that
		bool majorityFlag = false;

		/// @brief this number represents the order that this question was presented to the expert/user
		int finalQueryOrder = 0;

		/// @brief this number represents the order after being updated due to a chainJump ordering of questions
		int updatedQueryOrder = 0;

		/// @brief this number represents the order of questions that was planned to be presented to the expert/user
		int plannedQueryOrder = 0;
	};


	struct attribute
	{
		std::string name = "";
		int kv = 2;
		int trueValue = -1;
		int trueIndex = -1;
	};


	/// @brief flag for developer. Ask KV questions, yes or no?
	bool askKV = true;

	/// @brief by default, this is 2 (a Boolean function), but sometimes it could be a k-value function in the case of nested functions
	int function_kv = -1;

	/// @brief the parent function, or attribute, of the current Boolean function, which represents the attribute that the current sub-function is for. E.g. 1 is attribute x1
	std::string parent_attribute = "";

	/// @brief these are the sub-attributes of the current Boolean function. Index location 0 is for the subattributes of parent attribute x1 (of this Boolean function)
	std::vector<std::vector<std::string>> childAttributes;

	/// @brief the symbols that is used for this Boolean function
	char attributeSymbol = 'a';

	/// @brief name of this Boolean function
	std::string functionName = "";
	
	/// @brief file name of dataset of real data (not expert/ideal data). first line of file are attribute names
	std::string filename = "dataset.csv";

	/// @brief true if there is real data supplied
	bool realData = false;

	/// @brief of the real dataset, if applicable
	std::vector<std::string> real_attributes;

	/// @brief set of Hansel Chains
	std::vector<std::vector<dvector>> hanselChainSet;

	/// @brief k-value for each attribute of the dataset
	//std::vector<int> kv_attributes;

	/// @brief order of the Hansel Chains
	std::vector<int> hanselChainOrder;

	/// @brief check whether each Chain has been ordered when user is manually ordering them
	std::vector<bool> chainsVisited;

	/// @brief the order of questions that are asked to the expert/user. Even elements are the Chain of the asked question, whereas the odd element is the vector.
	/// used for the summary in the results output.
	std::vector<int> orderOfAskingSummary;

	/// @brief list of attributes which must be true for a datapoint/vector to be true. List of index locations
	//std::vector<int> trueAttributes;

	/// @brief the order of Hansel Chains to be used
	int orderOption = -1;

	/// @brief the total number of questions that were asked
	int questionsAsked = 0;

	/// @brief this number, at the time of visitation, is the number that represents the order in the sequence of questions. Used to assign planned and updated query order
	int questionOrder = 1;

	/// @brief the number of Hansel Chains
	int numChains = -1;

	/// @brief signals whether to use the majority flag or not. True if majority vectors are IN USE. Switches to false when majority vectors have been asked.
	int useMajorityFlag = -1;

	/// @brief signals whether the majority flag WAS USED or not. Value does not change, unlike useMajorityFlag
	bool usedMajorityFlag = false;

	/// @brief if majority flag is used, then this states how many true majority vectors there are (user specified). A value of 0 means the number is unknown.
	int trueMajority = 0;

	/// @brief indices of successful vectors in order of Hansel Chain and vector, alternating even and odd indices. Used for chainJump majority vectors.
	std::vector<int> trueVectorInd;

	/// @brief signals whether to use a chainJump ordering of questions
	int chainJump = -1;

	/// @brief for chainJump ordering, start at the top of the chain
	int top = -1;

	/// @brief first element is chain, next element is vector, next element is if that vector is visited
	std::vector<int> majorityVectors;

	/// @brief determine the the number of attributes (dimension) and what those attributes are
	int dimension = -1;

	/// @brief the name of the attributes of the datapoint
	std::vector<attribute> attributes;

	/// @brief represents what vectors to rectify when adding new attributes
	std::vector<int> addNewAttributesFor;

	/// @brief a list of vectors where each vector cannot be represented by the monotone Boolean Function, so if there is a lower one in the chain that it belongs to, 
	/// they will be represented by a "NOT" clause
	std::vector<dvector*> nonMonotonicVectors;

	/// @brief truye if nonMonotonicVectors is used
	bool non_monotone = false;

	/// @brief generates a Hansel Chain from a given dimension and number
	/// @param num 
	/// @param vector_dimension 
	/// @param chain a Hansel Chain
	/// @return a Hansel Chain
	std::vector<std::vector<dvector>> genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain);


	/// @brief calculates a set of Hansel Chains from a given dimension and k-valued attributes
	/// @param vector_dimension 
	void calculateHanselChains(int vector_dimension);


	/// @brief ask if majority flag should be used, and if yes, then asks questions based on a random sequence of "majority vectors."
	/// These vectors do not have "prior" expansions because they occur before any other expansion that they produce.
	/// Hence, they are not prior to any expansion except another majority vector, which is not possible because they 
	/// would have the same Hamming Norm. Also assigns planned and updated order of questions
	void askMajorityFlag();


	/// @brief Find the majority vectors in the set of Hansel Chains
	void findMajorityVectors();


	/// @brief Updates the query order of the vector, then calls askingOfQuestion(), and then determines if it should be expanded or was already expanded.
	/// @param i the Hansel Chain
	/// @param j the vector in the Hansel Chain
	/// @param vector_class vector class variable to be assigned to
	/// @return true if the vector is to be expanded. False otherwise
	bool questionFunc(int i, int j, int& vector_class);


	/// @brief the order of questions in a static pattern (the assigned order
	void staticOrderQuestionsFunc();


	/// @brief Check if the index i corresponds to the Hansel Chain index of a true majority vector
	/// @param i index of a Hansel Chain
	/// @return True if the index i corresponds to the Hansel Chain index of a true majority vector
	bool chainJumpMajorityFlagHelper(int i);


	/// @brief the order of questions in a chainJump pattern (the assigned order as default, but with chainJump capabilities)
	void chainJumpOrderQuestionsFunc();


	/// @brief order the Hansel Chains manually by a given sequence of numbers
	void manualHanselChainOrder();


	/// @brief expandable expansions and unexpandable expansions
	/// Expandable expansions are expansions which are used. Unexpandable expansions are expansions which are unused 
	/// because either the vector that would have been expanded was already expanded or if the vector of origin's 
	/// class is different than that of the expansion (e.g. a vector of class 1 with an expansion of class 0).
	/// @param newValue the new value at the point k. Determined by k-value for that attribute, so newValue is between 0 and k
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	/// @param p an element in the vector that was determined to be changed
	/// @param startChain equal to either i for a standard ordering of questions, or 0 for majority vector questions.
	void possibleExpansions(int newValue, int i, int j, int p, int startChain);


	/// @brief checks the possible expansions for a given vector (i, j), and expands those vectors if possible.
	/// @param vector_class 
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	void checkExpansions(int vector_class, int i, int j);


	/// @brief similar to checkExpansions, except it checks if a vector was expanded and hasn't been monotonically fixed. Used for violation of monotonicity rectification.
	/// @param vector_class 
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	void fixExpansions(int vector_class, int i, int j);


	/// @brief helper function that asks the user a question. Assigns the final query order
	/// @return the class of the vector;
	int askingOfQuestion(int i, int j);


	/// @brief assign numbers to vectors
	void numberAssignment();


	/// @brief call checkViolationOfMonotonicity ofor vectors whichc have violations of monotonicity
	void f_change_check();


	/// @brief monotonicity reaffirmation: described in detail in README
	/// @param i the hansel chain
	/// @param a the adjacent vector
	/// @param j the source of the f-change or potential violation
	void monotonicityReaffirmation(int i, int a, int j, int vector_class);


	/// @brief determine what method to use to fix violations of monotonicity
	void checkViolationOfMonotonicityMethod(int i, int j);


	/// @brief fix violation of monotonicity by changing the class
	/// @param root is true if the vector in question was asked. False if it was expanded.
	void fixViolationOfMonotonicityClass(int i, int j, int vector_class, bool preserve);


	/// @brief add a new attribute to the dataset to fix violation of monotonicity
	void fixViolationOfMonotonicityAddAttr();


	/// @brief add new attributes to real data
	/// @return the dataset, minus the classes
	std::vector<std::vector<int>> addNewAttributesToRealData();


	void changeAttributesOfRealData(std::vector<std::vector<int>> dataset);


	void applyBoolFuncToRealData(std::vector<std::vector<int>> boolFunc, std::vector<std::vector<int>> dataset);


	/// @brief get the necessary user inputs to run the program
	/// @return vector starts with -1 if it is a list of attributes with child functions. Otherwise, first number is group size, next numbers are attributes in that group, and repeat
	std::vector<int> init();


	void findTrueAttributes();


	/// @brief call possibleExpansions on every single vector
	void calculateAllPossibleExpansions();


	/// @brief calculate the function based on the user's answers
	/// @param target restore with respect to the target value
	/// @return a pair of the function: a non-simplified function, and a simplified function
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> restoreFunction(int target);


	/// @brief convert the function to a pair of strings
	/// @return a pair of the function: a simplified function and a non-simplied version of the function
	std::pair<std::string, std::string> functionToString(std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFuncPair, std::string sign);


	/// @brief write a table to the results file that contains the user's answers
	/// @param results output file
	/// @param boolFuncStrSimplified 
	/// @param boolFuncStrNonSimplified 
	/// @param include_violation variable which speciied whether or not to include the "fixed" field of the dvector struct
	void printTable(std::fstream& results, std::vector<int> targets, std::vector<std::string> boolFuncStrSimplified, std::vector<std::string> boolFuncStrNonSimplified, bool include_f_change);


	/// @brief generic printing function
	/// @tparam T 
	/// @param t 
	/// @param width the width of the output in terms of character spaces
	/// @param separator the characters to use as fillers in the output if needed to reach the specified width
	template<typename T>
	void print(T t, const int& width, const char& separator);


	/// @brief parse a string by a given delimiter
	std::vector<std::string> parse_input(char delimiter, std::string temp);


	/// @brief constructor. Used for the main boolean function.
	expertDataMining(char attributeSymbol);


	/// @brief constructor. Used for sub-functions in a  top-down approach
	/// @param parent_attribute is the number of the attribute that the expert data mining object is for
	//expertDataMining(char attributeSymbol, std::string parent_attribute);

	
	/// @brief constructor. used to create parent function in a bottom-up approach
	/// @param child_attributes is a list of the number of child attributes that each parent attribute has
	//expertDataMining(char attributeSymbol, std::vector<expertDataMining> child_attributes);


	/// @brief constructor. Used to create sub-functions for a parent functions in a bottom-up approach
	/// @param parent_attribute is the number of the attribute that the expert data mining object is for
	/// @paraam dimension is the dimension
	expertDataMining(char attributeSymbol, std::string parent_attribute, int dimension);


	/// @brief similar constructor as above, but with a function k-value
	/// @param attributeSymbol 
	/// @param parent_attribute 
	/// @param dimension 
	/// @param function_kv
	expertDataMining(char attributeSymbol, std::string parent_attribute, int dimension, int function_kv);


	/// @brief initialize parent with associated attributes already given
	/// @param attributeSymbol 
	/// @param asociated_attributes 
	expertDataMining(char attributeSymbol,std::vector<std::vector<std::string>> childAttributes);


	/// @brief start the expert data mining sequence for any nested attributes
	/// @param i the counter for the number of levels of attributes
	//void startChildren(int i);
	

	/// @brief print results to file
	/// @param results the results file
	void printToFile(std::fstream& results);


	/// @brief start expert data mining sequence
	void start();
};

/// @brief << overload for printing std::vector<int>
std::ostream& operator<<(std::ostream& out, const std::vector<int>& v);


/// @brief << overload for printing std::pair<int, int>
std::ostream& operator<<(std::ostream& out, const std::pair<int, int>& p);
