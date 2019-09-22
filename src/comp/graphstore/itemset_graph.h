/*
 * itemset_graph.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_ITEMSET_GRAPH_H_
#define COMP_GRAPHSTORE_ITEMSET_GRAPH_H_

class ItemsetGraph {

public:

	ItemsetGraph(){
		edge_ids = nullptr;
		len = 0;
	}

	ItemsetGraph(std::vector<int>& edge_vector){
		len = edge_vector.size();
		edge_ids = new int[len];
		std::copy(edge_vector.begin(), edge_vector.end(), edge_ids);
	}

	~ItemsetGraph(){
		if(edge_ids){
			delete[] edge_ids;
			len = 0;
		}
	}


private:
	int* edge_ids = nullptr;
	int len;




};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPH_H_ */
