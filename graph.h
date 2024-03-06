#pragma once
#include "dvector.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <algorithm>

// below DSU and MST code taken and adapted from https://www.geeksforgeeks.org/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/

// DSU data structure
// path compression + rank by union 
class DSU 
{
    int* parent;
    int* rank;

public:
    DSU(int n);

    // Find function 
    int find(int i);

    // Union function 
    void unite(int x, int y);
};


class graph
{
public:
    /// @brief list of edges: last element is the weight between the two edges, and first two elements are the HC index locations
    std::vector<std::tuple<int, int, double>> completeGraphEdgeList;


    /// @brief create adjacency matrix for a complete graph of Hansel Chains
    graph(std::vector<std::vector<dvector>>* hanselChainSet);


    std::vector<std::tuple<int, int, double>> kruskal();

private:
    /// @brief same as hanselChainSet in oeka.h
    std::vector<std::vector<dvector>>* hanselChainSet;

    /// @brief size of graph
    int numChains; 

    /// @brief dimension of the vectors in the hansel chains
    int vectorDimension;

    double hammingDistance(int l, int r, int li, int rj);


    double chainHammingDistance(int l, int r);


    // it is the greatest of all the distances from a point in one set to the closest point in the other set
    double chainHausdorffDistance(int l, int r);


    //https://link.springer.com/article/10.1007/s13160-012-0089-6
    double groupAverageHammingDistance(int l, int r);


    void shortestPath(int l, int r);
};