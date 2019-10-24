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


	bool equals(DeltaGraph* another, PEGraph* out) const {
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
			if(this->base == another->base){
				assert(this->base == another->base);
				return this->added->equals(another->added) && this->deleted->equals(another->deleted);
			}
			else{
//				return this->base->equals(another->base) && this->added->equals(another->added) && this->deleted->equals(another->deleted);
				if(this->getNumEdges() != another->getNumEdges()){
					return false;
				}
				else{
					PEGraph* another_peg = new PEGraph();
					another_peg->merge(another->getBase());
					another_peg->merge(another->getAdded());
					another_peg->subtract(another->getDeleted());
					bool equal = out->equals(another_peg);
					delete another_peg;
					return equal;
				}

			}
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

	int getNumEdges() const {
		return base->getNumEdges() + added->getNumEdges() - deleted->getNumEdges();
	}



private:
	PEGraph* base = NULL;
	PEGraph* added = NULL;
	PEGraph* deleted = NULL;
	bool is_naive = true;


};



#endif /* COMP_GRAPHSTORE_DELTA_GRAPH_H_ */
