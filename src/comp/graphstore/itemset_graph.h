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
	int* edge_ids = nullptr;
	int len;




};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPH_H_ */
