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
    void sort();

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

    void write_unreadable(FILE* f){
    	fwrite((const void*)& size, sizeof(int), 1, f);
    	fwrite((const void*)& capacity, sizeof(int), 1, f);
		fwrite((const void*) edges, sizeof(vertexid_t), size, f);
		fwrite((const void*) labels, sizeof(label_t), size, f);
    }

    void load_unreadable(FILE* fp){
    	size_t freadRes = 0; // clear warnings
    	freadRes = fread(&size, sizeof(int), 1, fp);
//    	freadRes = fread(&capacity, sizeof(int), 1, fp);

		int bufsize = sizeof(int) + (sizeof(vertexid_t) + sizeof(label_t)) * size;
		char *buf = (char*)malloc(bufsize);
		freadRes = fread(buf, bufsize, 1, fp);
		capacity = *((int*)(buf));
    	this->edges = new vertexid_t[size];
    	memcpy(edges, buf + sizeof(int), sizeof(vertexid_t) * size);
    	this->labels = new label_t[size];
    	memcpy(labels, buf + sizeof(int) + sizeof(vertexid_t) * size, sizeof(label_t) * size);
    	free(buf);
    }

    size_t get_size_bytes(){
    	return sizeof(int) * 2 + (sizeof(vertexid_t) + sizeof(label_t)) * size;
    }

    size_t write_to_buf(char* buf, size_t offset){
    	memcpy(buf + offset, (char*)&size, sizeof(int));
    	offset += sizeof(int);
    	memcpy(buf + offset, (char*)&capacity, sizeof(int));
    	offset += sizeof(int);
    	memcpy(buf + offset, edges, sizeof(vertexid_t) * size);
    	offset += sizeof(vertexid_t) * size;
    	memcpy(buf + offset, labels, sizeof(label_t) * size);
    	offset += sizeof(label_t) * size;

    	return offset;
    }

    size_t read_from_buf(char* buf, size_t offset){
    	size = *((int*)(buf + offset));
    	offset += sizeof(int);
    	capacity = *((int*)(buf + offset));
    	offset += sizeof(int);
    	this->edges = new vertexid_t[size];
    	memcpy(edges, buf + offset, sizeof(vertexid_t) * size);
    	offset += sizeof(vertexid_t) * size;
    	this->labels = new label_t[size];
    	memcpy(labels, buf + offset, sizeof(label_t) * size);
    	offset += sizeof(label_t) * size;

    	return offset;
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
