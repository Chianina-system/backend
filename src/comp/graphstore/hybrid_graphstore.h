/*
 * hybrid_graphstore.h
 *
 *  Created on: Oct 6, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_HYBRID_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_HYBRID_GRAPHSTORE_H_

#include "hybrid_graph.h"

using namespace std;

class HybridGraphStore {

public:

	HybridGraphStore(){

	}

	~HybridGraphStore(){
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		delete it->second;
    	}
	}


    void clear(){
    	for(auto it = graphs.begin(); it != graphs.end(); ){
    		delete it->second;
    		it = graphs.erase(it);
    	}
    }


    //shallow copy
    void addOneGraph_atomic(PEGraph_Pointer pointer, HybridGraph* graph){
    	std::lock_guard<std::mutex> lGuard (mtx);
    	this->graphs[pointer] = graph;
    }

	unordered_map<PEGraph_Pointer, HybridGraph*>& getGraphs() {
		return graphs;
	}

    void printOutInfo(){
    	int size_graphs = graphs.size();
    	long size_edges = 0;

    	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
//    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
    		size_edges += it->second->getNumEdges();
    	}

    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    }


private:

	unordered_map<PEGraph_Pointer, HybridGraph*> graphs;

	std::mutex mtx;

};


#endif /* COMP_GRAPHSTORE_HYBRID_GRAPHSTORE_H_ */
