//
// Created by xiangyi on 8/1/19.
//

#ifndef SECUREIMAGESEARCHMIDDLEBOX_SUBMODULAR_H
#define SECUREIMAGESEARCHMIDDLEBOX_SUBMODULAR_H

#include <list>
#include <stack>

using namespace std;

// A C++ program to find strongly connected components in a given
// directed graph using Tarjan's algorithm (single DFS)
#include<iostream>
#include <list>
#include <stack>
#include "data_handler.h"

#define NIL -1
using namespace std;

// A class that represents an directed graph
class Graph
{
    int V; // No. of vertices
    list<int> *adj; // A dynamic array of adjacency lists

    // A Recursive DFS based function used by SCC()
    void SCCUtil(int u, int disc[], int low[],
                 stack<int> *st, bool stackMember[]);
public:
    Graph(int V); // Constructor
    void addEdge(int v, int w); // function to add an edge to graph
    void SCC(); // prints strongly connected components
};

const int dist_threshold = 5;
Graph generate_graph(data_pack *data);

#endif //SECUREIMAGESEARCHMIDDLEBOX_SUBMODULAR_H
