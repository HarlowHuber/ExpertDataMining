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

/// @brief dvector, or datapoint vector, is simply a vector with information about a datapoint
struct dvector
{
	/// @brief the n-D data point to be considered as a vector
	std::vector<int> dataPoint;

	/// @brief the class or value of the dataPoint
	int _class = -1;

	/// @brief the index of this vector's Hansel Chain and the index of the vector in the Hansel Chain, respectively
	std::pair<int, int> number;

	/// @brief the zero-to-zero expansions that are used
	std::vector<dvector*> expandable_zero;

	/// @brief the one-to-one expansions that are used
	std::vector<dvector*> expandable_one;

	/// @brief zero-to-zero expansions that are impossible because the given class was not zero
	std::vector<dvector*> unexpandable_zero;

	/// @brief one-to-one expansions that are impossible because the given class was not one
	std::vector<dvector*> unexpandable_one;

	/// @brief possible expansions if the Hansel Chains were to be ordered differently (expansions from previous Chains)
	std::vector<dvector*> prior_zero;

	/// @brief possible expansions if the Hansel Chains were to be ordered differently (expansions from previous Chains)
	std::vector<dvector*> prior_one;

	/// @brief if the vector is visited
	bool visited = false;

	/// @brief if this vector was asked to the expert/user
	bool asked = false;

	/// @brief signals whether the vector contains a majority of true attributes, meaning either 50% are true or the ceiling of that
	bool majorityFlag = false;

	/// @brief this number represents the order that this question was presented to the expert/user
	int finalQueryOrder = 0;

	/// @brief this number represents the order after being updated due to a dynamic ordering of questions
	int updatedQueryOrder = 0;

	/// @brief this number represents the order of questions that was planned to be presented to the expert/user
	int plannedQueryOrder = 0;
};

/// @brief set of Hansel Chains
std::vector<std::vector<dvector>> hanselChainSet;

/// @brief k-value for each attribute of the dataset
std::vector<int> kv_attributes;

/// @brief order of the Hansel Chains
std::vector<int> hanselChainOrder;

/// @brief check whether each Chain has been ordered when user is manually ordering them
std::vector<bool> chainsVisited;

/// @brief the order of questions that are asked to the expert/user. Even elements are the Chain of the asked question, whereas the odd element is the vector.
/// used for the summary in the results output.
std::vector<int> orderOfAskingSummary;

/// @brief list of attributes which must be true for a datapoint/vector to be true
std::vector<int> trueAttributes;

/// @brief the total number of questions that were asked
int questionsAsked = 0;

/// @brief this number, at the time of visitation, is the number that represents the order in the sequence of questions. Used to assign planned and updated query order
int questionOrder = 1;

/// @brief the number of Hansel Chains
int numChains;

/// @brief signals whether to use the majority flag or not
int useMajorityFlag;

/// @brief signals whether to use a dynamic ordering of questions
int dynamic;

/// @brief for dynamic ordering, start at the top of the chain
int top;

/// @brief first element is chain, next element is vector, next element is if that vector is visited
std::vector<int> majorityVectors;

/// @brief determine the the number of attributes (dimension) and what those attributes are
int dimension = -1;

/// @brief the name of the attributes of the datapoint
std::vector<std::string> attributes;


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


/// @brief the order of questions in a static pattern (the assigned order
void staticOrderQuestionsFunc();


/// @brief the order of questions in a dynamic pattern (wthe assigned order as default, but with dynamic capabilities)
void dynamicOrderQuestionsFunc();



/// @brief order the Hansel Chains manually by a given sequence of numbers
void manualHanselChainOrder();


/// @brief a prior expansion is an expansion that occurs in a chain that is prior to a given vector.
///	Hence, impossible if the order of questions is the same as the sequence of Hansel Chains (which is true 
/// in the of this program). This means that "majority vector" questions do not have prior expansions because
/// those questions are asked first.
/// @param _class class of prior expansion, either 1 or 0.
/// @param i the Hansel Chain
/// @param j a vector in the Hansel Chain
/// @param k an element in the vector
void priorExpansions(int _class, int i, int j, int k);


/// @brief expandable expansions and unexpandable expansions
/// Expandable expansions are expansions which are used. Unexpandable expansions are expansions which are unused 
/// because either the vector that would have been expanded was already expanded or if the vector of origin's 
/// class is different than that of the expansion (e.g. a vector of class 1 with an expansion of class 0).
/// @param vector_class either 1 or 0
/// @param i the Hansel Chain
/// @param j a vector in the Hansel Chain
/// @param k an element in the vector
/// @param startChain equal to either i for a standard ordering of questions, or 0 for majority vector questions.
void possibleExpansions(int vector_class, int i, int j, int k, int startChain);


/// @brief helper function that asks the user a question. Assigns the final query order
/// @return the class of the vector;
int askingOfQuestion(int i, int j);


/// @brief assign numbers to vectors
void numberAssignment();
