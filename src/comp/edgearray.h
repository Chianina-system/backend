#ifndef COMP_EDGEARRAY_H
#define COMP_EDGEARRAY_H
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"

#define CAPACITY_VALUE 8

class EdgeArray {

	friend std::ostream & operator<<(std::ostream & strm, const EdgeArray& edgeArray) {
		strm << "{size: " << edgeArray.size << "; ";
		for(int i = 0; i < edgeArray.size; i++){
			strm << "(" << edgeArray.edges[i] << "," << edgeArray.labels[i] << ") ";
		}
		strm << "}";
		return strm;
	}


public:
    EdgeArray();
//    EdgeArray(int size,vertexid_t *edges,label_t *labels);
    ~EdgeArray(){
    	clear();
    }
    void clear();


    inline vertexid_t *getEdges() const{return edges;}
    inline label_t* getLabels() const{return labels;}
    inline int getSize() const{return size;}

    void set(int size,vertexid_t *edges,label_t *labels);

//    inline int getRealNumEdges() {return realNumEdges;}
//    void setRealNumEdges(int realNumEdges);

    void addOneEdge(vertexid_t edge,label_t label);
    void addEdges(int len, vertexid_t* _edges, label_t* _labels);
    void merge();


private:
    vertexid_t *edges;
    label_t *labels;
    int size;
    int capacity;
//    int realNumEdges; // exclude duplicate edges



};

#endif
