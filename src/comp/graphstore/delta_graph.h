/*
 * delta_graph.h
 *
 *  Created on: Oct 20, 2019
 *      Author: zzq
 */

#ifndef COMP_GRAPHSTORE_DELTA_GRAPH_H_
#define COMP_GRAPHSTORE_DELTA_GRAPH_H_

#include "../pegraph.h"


class DeltaGraph {

public:
	DeltaGraph(){
		base = new PEGraph();
		added = new PEGraph();
		deleted = new PEGraph();
		is_naive = true;
	}

	DeltaGraph(PEGraph* b){
		base = b;
		added = new PEGraph();
		deleted = new PEGraph();
		is_naive = true;
	}

	DeltaGraph(PEGraph* b, PEGraph* a, PEGraph* d, bool isNaive){
		base = b;
		added = a;
		deleted = d;
		is_naive = isNaive;
	}

	~DeltaGraph(){
		if(is_naive && base){
			delete base;
		}
		if(added){
			delete added;
		}
		if(deleted){
			delete deleted;
		}
	}


	bool equals(DeltaGraph* another) const {
		if(another == NULL){
			return false;
		}
		else if(another->isNaive()){
			if(this->isNaive()){
				return this->base->equals(another->getBase());
			}
			else{
				return false;
			}
		}
		else{
			assert(this->base == another->base);
			return this->added->equals(another->added) && this->deleted->equals(another->deleted);
		}

	}

	bool isNaive() const {
//		return added->isEmpty() && deleted->isEmpty();
		return is_naive;
	}

	void setNaive(bool n){
		this->is_naive = n;
	}

	PEGraph* getBase() const {
		return base;
	}

	PEGraph* getAdded() const {
		return added;
	}

	PEGraph* getDeleted() const {
		return deleted;
	}




private:
	PEGraph* base = NULL;
	PEGraph* added = NULL;
	PEGraph* deleted = NULL;
	bool is_naive = true;


};



#endif /* COMP_GRAPHSTORE_DELTA_GRAPH_H_ */
