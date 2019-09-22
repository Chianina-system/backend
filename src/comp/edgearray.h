#ifndef COMP_EDGEARRAY_H
#define COMP_EDGEARRAY_H

//#include "../utility/Logger.hpp"
#include "../preproc/grammar.h"
#include "myalgorithm.h"

#define CAPACITY_VALUE 8

class EdgeArray {

	friend std::ostream & operator<<(std::ostream & strm, const EdgeArray& edgeArray) {
		strm << "{size=" << edgeArray.size << "; ";
		for(int i = 0; i < edgeArray.size; i++){
			strm << "(" << edgeArray.edges[i] << ", " << (int)edgeArray.labels[i] << ") ";
		}
		strm << "}";
		return strm;
	}

public:
    EdgeArray();

    ~EdgeArray(){
    	clear();
    }


    //copy constructor
    EdgeArray(const EdgeArray& array);

    //copy assign operator
    EdgeArray& operator=(const EdgeArray& array);


    void clear();


    inline vertexid_t *getEdges() const{return edges;}
    inline label_t* getLabels() const{return labels;}
    inline int getSize() const{return size;}
    inline bool isEmpty(){ return size == 0;}

    void set(int size,vertexid_t *edges,label_t *labels);

//    inline int getRealNumEdges() {return realNumEdges;}
//    void setRealNumEdges(int realNumEdges);

    void addOneEdge(vertexid_t edge,label_t label);
    void addEdges(int len, vertexid_t* _edges, label_t* _labels);
    void merge();

    bool equals(EdgeArray* another) const ;


    void write_readable(std::ofstream& os){
    	os << size << "\t";
    	os << capacity << "\t";
    	for(int i = 0; i < size; i++){
    		os << edges[i] << "\t";
    		os << (int)labels[i] << "\t";
    	}
    }

    void load_readable(std::stringstream& stream){
    	std::string item;
    	stream >> item;
    	this->size = atoi(item.c_str());
    	stream >> item;
    	this->capacity = atoi(item.c_str());
    	this->edges = new vertexid_t[size];
    	this->labels = new label_t[size];
    	for(int i = 0; i < size; i++){
        	stream >> item;
        	this->edges[i] = atoi(item.c_str());
        	stream >> item;
        	this->labels[i] = (char) atoi(item.c_str());
    	}
    }

    std::string toString(Grammar* grammar){
      	std::ostringstream strm;
		strm << "{size=" << size << "; ";
		for(int i = 0; i < size; i++){
			strm << "(" << edges[i] << ", " << grammar->getRawLabel(labels[i]) << ") ";
		}
		strm << "}";
		return strm.str();
    }


private:
    vertexid_t *edges = nullptr;
    label_t *labels = nullptr;
    int size = 0;
    int capacity = 0;



};

#endif
