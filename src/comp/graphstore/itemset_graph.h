/*
 * itemset_graph.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_ITEMSET_GRAPH_H_
#define COMP_GRAPHSTORE_ITEMSET_GRAPH_H_

class ItemsetGraph {

	friend std::ostream & operator<<(std::ostream & strm, const ItemsetGraph & itemsetGraph) {
		strm << "ItemsetGraph<<<<\n---------------------" << endl;
		strm << "size=" << itemsetGraph.len << endl;
		strm << "---------------------" << endl;
		return strm;
	}

public:

	ItemsetGraph(){
		edge_ids = NULL;
		len = 0;
	}

	ItemsetGraph(std::vector<int>& edge_vector){
		len = edge_vector.size();
		if(len){
			edge_ids = new int[len];
	//		std::copy(edge_vector.begin(), edge_vector.end(), edge_ids);
			memcpy(edge_ids, edge_vector.data(), sizeof(int)*len);
		}
		else{
			edge_ids = NULL;
		}
	}

	ItemsetGraph(const ItemsetGraph& graph){
		len = graph.len;
		if(len){
			edge_ids = new int[len];
	        memcpy(edge_ids, graph.edge_ids, sizeof(int)*len);
		}
		else{
			edge_ids = NULL;
		}
	}

	~ItemsetGraph(){
		if(edge_ids){
			delete[] edge_ids;
			len = 0;
		}
	}

    //copy assign operator
	ItemsetGraph& operator=(const ItemsetGraph& graph) {
		len = graph.len;
		if(len){
			edge_ids = new int[len];
	        memcpy(edge_ids, graph.edge_ids, sizeof(int)*len);
		}
		else{
			edge_ids = NULL;
		}

		return *this;
    }


	int getNumEdges(){
		return len;
	}

	inline int* getEdgeIds() const {
		return edge_ids;
	}

	inline int getEdgeId(int index) const {
		assert(index < len);
		return edge_ids[index];
	}

	inline int getLength() const {
		return len;
	}

private:
	int* edge_ids = NULL;
	int len;




};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPH_H_ */
