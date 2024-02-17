#include "oeka.h"


std::vector<std::array<int, 3>> oeka::createGraph()
{
	// initilize edge list
	// first number and second numbers are location of the Hansel Chain
	// last number is the distance between them.
	std::vector<std::array<int, 3>> edgeList;

	int n = (numChains / 2) + (numChains % 2); // number of edges is n(n - 1) / 2, where n is numChains

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < numChains; j++)
		{
			if (i != j)
			{
				edgeList.push_back({ i, j, chainHammingDistance(i, j) });
			}
		}
	}

	return edgeList;
}


std::vector<std::array<int, 3>>  oeka::kruskal(std::vector<std::array<int, 3>> edgeList)
{
	std::sort(edgeList.begin(), edgeList.end(),
		[](auto const& l, auto const& r) { return l[2] < r[2]; });

	// finish
	return edgeList;
}



int oeka::chainHammingDistance(int l, int r)
{
	// compare the smaller chain to the larger chain by Hamming norm, or if there is no match, then the closest match
	if (hanselChainSet[l].size() != hanselChainSet[r].size())
	{
		std::vector<dvector>* smallChain;

		// find minimum size between the two hansel chains
		if (hanselChainSet[l].size() < hanselChainSet[r].size())
		{
			smallChain = &hanselChainSet[l];
		}
		else
		{
			smallChain = &hanselChainSet[r];
		}

		// get hamming norm from the specified element of a chain
		// will break if the parameter i exceeds the chain length
		auto getHammingNorm = [](std::vector<dvector>* chain, int i)
		{
			int hammingNorm = 0;

			for (int e : chain->at(i).dataPoint)
			{
				hammingNorm += e;
			}

			return hammingNorm;
		};

		int smallHamingNorm = getHammingNorm(smallChain, 0);
		

	}
	// simply append vectors to each other and compare with Hamming Distance
	else
	{
		
	}

	// finish
	return 0;

}


int oeka::chainHausdorffDistance(int l, int r)
{
	return 0;
}

