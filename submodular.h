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

    list<int> *adj; // A dynamic array of adjacency lists
    int numOfSCC; // The number of SCCs in the graph.

    // A Recursive DFS based function used by SCC()
    void SCCUtil(int u, int disc[], int low[],
                 stack<int> *st, bool stackMember[]);
public:
    Graph(data_pack *data); // Constructor
    void addEdge(int v, int w); // function to add an edge to graph
    int SCC(); // return the number of strongly connected components
    int V; // No. of vertices
    int **dist_mat;
    int *scc;
};

const int dist_threshold = 3;
const double kCov = 0.8, kDiv = 0.2;
Graph generate_graph(data_pack *data);
inline int coverage(Graph *g, vector<int> selectedImgs);
inline int diversity(Graph *g, vector<int> selectedImgs);
double submodular_func(Graph *g, vector<int> selectedImgs);
vector<int> submodular(Graph *g);
#endif //SECUREIMAGESEARCHMIDDLEBOX_SUBMODULAR_H
