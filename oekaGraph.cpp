#include "graph.h"

// add graph instance to oeka class


DSU::DSU(int n)
{
	parent = new int[n];
	rank = new int[n];

	for (int i = 0; i < n; i++) 
	{
		parent[i] = -1;
		rank[i] = 1;
	}
}


// Find function 
int DSU::find(int i)
{
	if (parent[i] == -1)
	{
		return i;
	}

	return parent[i] = find(parent[i]);
}


// Union function 
void DSU::unite(int x, int y)
{
	int s1 = find(x);
	int s2 = find(y);

	if (s1 != s2) 
	{
		if (rank[s1] < rank[s2]) 
		{
			parent[s1] = s2;
		}
		else if (rank[s1] > rank[s2]) 
		{
			parent[s2] = s1;
		}
		else 
		{
			parent[s2] = s1;
			rank[s1]++;
		}
	}
}


graph::graph(std::vector<std::vector<dvector>>* _hanselChainSet)
{
	// initialize member variables
	hanselChainSet = _hanselChainSet;
	numChains = (int)hanselChainSet->size();
	vectorDimension = (int)hanselChainSet->at(0)[0].dataPoint.size();

	for (int i = 0; i < numChains - 1; i++)
	{
		for (int j = i + 1; j < numChains; j++)
		{
			//completeGraphEdgeList.push_back({ i, j, chainHammingDistance(i, j) });
			//completeGraphEdgeList.push_back({ i, j, chainHausdorffDistance(i, j) });
			completeGraphEdgeList.push_back({ i, j, groupAverageHammingDistance(i, j) });
		}
	}
}


std::vector<std::tuple<int, int, double>> graph::kruskal()
{
	/// @brief resultant edgeList of minimal spanning tree
	std::vector<std::tuple<int, int, double>> minSpanTreeEdgeList;
	minSpanTreeEdgeList.reserve(numChains);

	std::sort(completeGraphEdgeList.begin(), completeGraphEdgeList.end(),
		[](auto const& l, auto const& r) 
		{ 
			return std::get<2>(l) < std::get<2>(r); 
		});

	DSU set(numChains);
	double cost = 0;

	for (auto edge : completeGraphEdgeList) 
	{
		int x = std::get<0>(edge); 
		int y = std::get<1>(edge);
		double w = std::get<2>(edge);

		// Take this edge in MST if it does 
		// not forms a cycle 
		if (set.find(x) != set.find(y)) 
		{
			set.unite(x, y);
			cost += w;

			minSpanTreeEdgeList.push_back(edge);
		}
	}

	std::cout << "Minimum Cost Spanning Tree: " << cost;

	return minSpanTreeEdgeList;
}


double graph::hammingDistance(int l, int r, int li, int rj)
{
	double hammingDistance = 0;

	for (int k = 0; k < vectorDimension; k++)
	{
		hammingDistance += abs(hanselChainSet->at(l)[li].dataPoint[k] - hanselChainSet->at(r)[rj].dataPoint[k]);
	}

	return hammingDistance;
}


double graph::chainHammingDistance(int l, int r)
{
	std::vector<dvector>* smallChain;
	std::vector<dvector>* largeChain;
	int largeChainSize;

	// find minimum size between the two hansel chains
	if (hanselChainSet->at(l).size() < hanselChainSet->at(r).size())
	{
		smallChain = &hanselChainSet->at(l);
		largeChain = &hanselChainSet->at(r);
	}
	else
	{
		smallChain = &hanselChainSet->at(r);
		largeChain = &hanselChainSet->at(l);
	}

	largeChainSize = (int)largeChain->size();

	// if you take the size of the larger hanselChain and subtract the size of the smaller hansel chain and divide the result by 2:
	// the result is the index location of the vector with the same Hamming Norm as the first vector of the smaller chain
	// due to this property, we can simply do this to find where to compare the two different hansel chains
	int start = (largeChainSize - (int)smallChain->size()) / 2;
	int end = largeChainSize - start;
	double hammingDistance = 0;

	for (int i = 0; i < largeChainSize; i++)
	{
		// directly compare i-th vector in large chain to [i-th - start] vector in small chain
		if (start <= i && i < end)
		{
			for (int j = 0; j < vectorDimension; j++)
			{
				if (largeChain->at(i).dataPoint[j] != smallChain->at(i - start).dataPoint[j])
				{
					// this is to accomodate for k-value, but maybe leave it so that we only add 1 to hammingDistance if not equal
					hammingDistance += abs(largeChain->at(i).dataPoint[j] - smallChain->at(i - start).dataPoint[j]);
				}
			}
		}
		// else, the hamming distance is the size of the vector
		// maybe change this to also include k-value?
		/*else
		{
			hammingDistance += vectorDimension;
		}*/
	}

	return hammingDistance / (int)smallChain->size();
	//return hammingDistance / (largeChainSize + (int)smallChain->size());
}


double graph::chainHausdorffDistance(int l, int r)
{
	std::vector<double> distances;

	for (int i = 0; i < (int)hanselChainSet->at(l).size(); i++ )
	{
		double minDistance = DBL_MAX;

		// find minimum distance of element in one chain to an element in another chain
		for (int j = 0; j < (int)hanselChainSet->at(r).size(); j++)
		{
			double d = hammingDistance(l, r, i, j);

			if (d < minDistance)
			{
				minDistance = d;
			}
		}

		// don't add it when the inner for loop isnt executed...
		if (minDistance != DBL_MAX)
		{
			distances.push_back(minDistance);
		}
	}

	// find maximum distance of min distances
	double hausDistance = *std::max_element(distances.begin(), distances.end());

	return hausDistance;
}


double graph::groupAverageHammingDistance(int l, int r)
{
	double distance = 0;

	for (int i = 0; i < (int)hanselChainSet->at(l).size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet->at(r).size(); j++)
		{
			distance += hammingDistance(l, r, i, j);
		}
	}

	distance /= (int)(hanselChainSet->at(l).size() * hanselChainSet->at(r).size());

	return distance;
}
