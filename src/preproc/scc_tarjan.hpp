#ifndef scc_tarjan_hpp
#define scc_tarjan_hpp

// A C++ program to find strongly connected components in a given
// directed graph using Tarjan's algorithm (single DFS)
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <unordered_set>
#define NIL -1

// A class that represents an directed graph
class Graph
{
    int V; // No. of vertices
    std::list<int> *adj; // A dynamic array of adjacency lists 每个节点对应一个list

    // A Recursive DFS based function used by SCC()
    void SCCUtil(int u, int disc[], int low[],
                std::stack<int> *st, bool stackMember[]);
public:
    Graph(int V); // Constructor
    ~Graph();
    void addEdge(int v, int w); // function to add an edge to graph
    void SCC(); // prints strongly connected components
    
    std::vector<std::vector<int>> sccs;
};

#endif
