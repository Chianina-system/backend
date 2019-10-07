/*
 * hybridgraph.h
 *
 *  Created on: Oct 6, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_HYBRID_GRAPH_H_
#define COMP_GRAPHSTORE_HYBRID_GRAPH_H_

#include "../pegraph.h"


class HybridGraph {

public:
	HybridGraph(){

	}

	HybridGraph(MyArray* edge_vector, PEGraph* pegraph) {
		itemsetGraph = new ItemsetGraph(edge_vector);
		peGraph = pegraph;
	}


	~HybridGraph(){
		if(peGraph){
			delete peGraph;
		}

		if(itemsetGraph){
			delete itemsetGraph;
		}
	}

	bool equals(ItemsetGraph *another) {
		if(!another){
			return false;
		}

		if(!this->peGraph->isEmpty()){
			return false;
		}

		return this->itemsetGraph->equals(another);

	}

	bool isEmpty(){
		return this->peGraph->isEmpty() && itemsetGraph->isEmpty();
	}

	ItemsetGraph* getItemsetGraph() {
		return itemsetGraph;
	}

	PEGraph* getPeGraph() {
		return peGraph;
	}

	int getNumEdges(){
		return peGraph->getNumEdges() + itemsetGraph->getNumEdges();
	}


private:
	PEGraph* peGraph = nullptr;
	ItemsetGraph* itemsetGraph = nullptr;


};



#endif /* COMP_GRAPHSTORE_HYBRID_GRAPH_H_ */
