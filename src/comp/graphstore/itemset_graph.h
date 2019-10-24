/*
 * itemset_graph.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_ITEMSET_GRAPH_H_
#define COMP_GRAPHSTORE_ITEMSET_GRAPH_H_

#include "myarray.h"
//#include "itemset_graphstore.h"

using namespace std;

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

	ItemsetGraph(unsigned int l){
		edge_ids = new int[l];
		this->len = l;
	}

	ItemsetGraph(MyArray* edge_vector) {
		len = edge_vector->getLength();
		if (len) {
			edge_ids = edge_vector->getData();
		} else {
			edge_ids = NULL;
		}
	}

	ItemsetGraph(ItemsetGraph* base_graph, MyArray* edge_vector) {
		len = base_graph->getLength() + edge_vector->getLength();
		if (len) {
			edge_ids = edge_vector->getData();
		} else {
			edge_ids = NULL;
		}
	}

	ItemsetGraph(std::vector<int>& edge_vector){
		len = edge_vector.size();
		if(len){
			edge_ids = new int[len];
			memcpy(edge_ids, edge_vector.data(), sizeof(int)*len);
		}
		else{
			edge_ids = NULL;
		}
	}

	ItemsetGraph(std::set<int>& edge_set){
		len = edge_set.size();
		if(len){
			edge_ids = new int[len];
			std::copy(edge_set.begin(), edge_set.end(), edge_ids);
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
			edge_ids = NULL;
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

    unsigned getItemsetIndex(int id_itemset){
    	return 0 - (id_itemset + 1);
    }

    bool isItemset(int id_edge){
    	return id_edge < 0;
    }

	//TODO: update this for graph compression
	unsigned int getNumEdges(vector<ItemsetGraph*>& intToItemset){
		if(intToItemset.empty()){
			return len;
		}

    	vector<int> edges;
    	vector<ItemsetGraph*> itemset_graphs;
    	itemset_graphs.push_back(this);
    	while(!itemset_graphs.empty()){
    		ItemsetGraph* g = itemset_graphs.back();
    		itemset_graphs.pop_back();
			for(unsigned int i = 0; i < g->getLength(); ++i){
				int id_edge = g->getEdgeId(i);
				if(isItemset(id_edge)){
					itemset_graphs.push_back(intToItemset[getItemsetIndex(id_edge)]);
				}
				else{
					edges.push_back(id_edge);
				}
			}
    	}

    	return edges.size();
	}

	inline int* getEdgeIds() const {
		return edge_ids;
	}

	inline int getEdgeId(unsigned int index) const {
		assert(index < len);
		return edge_ids[index];
	}

	inline unsigned int getLength() const {
		return len;
	}

	inline bool isEmpty() const {
		return len == 0;
	}

//	// write an itemset_graph into a line separated by " " for itemset mining later
//	void write_for_mining(std::ofstream& os){
//		for(int i = 0; i < len; i++){
//			os << edge_ids[i] << " ";
//		}
//	}

	bool equals(ItemsetGraph* another) const {
		if(!another){
			return false;
		}

		if(this == another){
			return true;
		}

		if(this->getLength() != another->getLength()){
			return false;
		}

		if(this->isEmpty()){
			return true;
		}

		if(memcmp(this->edge_ids, another->edge_ids, sizeof(label_t)*len)){
			return false;
		}

		return true;
	}


    std::string toString(){
      	std::ostringstream strm;
		strm << "{size=" << len << "; ";
		for(unsigned int i = 0; i < len; i++){
			strm << edge_ids[i] << ", ";
		}
		strm << "}";
		return strm.str();
    }


	long write_to_buf(char* buf, size_t offset){
    	memcpy(buf + offset, (char*)& len, sizeof(unsigned int));
    	offset += sizeof(unsigned int);
    	memcpy(buf + offset, (char*) edge_ids, len * sizeof(int));
    	offset += len * sizeof(int);

    	return offset;
	}

//    size_t read_from_buf(char* buf, size_t offset){
//    	size = *((int*)(buf + offset));
//    	offset += sizeof(int);
//    	capacity = *((int*)(buf + offset));
//    	offset += sizeof(int);
//    	this->edges = new vertexid_t[size];
//    	memcpy(edges, buf + offset, sizeof(vertexid_t) * size);
//    	offset += sizeof(vertexid_t) * size;
//    	this->labels = new label_t[size];
//    	memcpy(labels, buf + offset, sizeof(label_t) * size);
//    	offset += sizeof(label_t) * size;
//
//    	return offset;
//    }

	void write_unreadable(FILE* f){
    	fwrite((const void*)& len, sizeof(unsigned int), 1, f);
		fwrite((const void*) edge_ids, sizeof(int) * len, 1, f);
	}

	void load_unreadable(FILE* fp){
    	size_t freadRes = 0; // clear warnings
    	freadRes = fread(&len, sizeof(unsigned int), 1, fp);
    	this->edge_ids = new int[len];
		freadRes = fread(edge_ids, sizeof(int) * len, 1, fp);
	}

	void setEdges(int* edges, unsigned int l){
		edge_ids = edges;
		len = l;
	}


private:
	int* edge_ids = NULL;
	unsigned int len;




};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPH_H_ */
