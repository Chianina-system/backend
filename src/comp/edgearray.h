#ifndef COMP_EDGEARRAY_H
#define COMP_EDGEARRAY_H
#include "../common/CommonLibs.hpp"

class EdgeArray {
private:
    vertexid_t *edges;
    label_t *labels;
    int size;

public:
    EdgeArray();
    EdgeArray(int size,vertexid_t *edges,label_t *labels);
    ~EdgeArray(){}

    // getters and setters
    inline bool isEmpty() {return !size;}
    inline vertexid_t *getEdges() {return edges;}
    inline label_t* getLabels() {return labels;}
    inline int getSize() {return size;}

    void set(int size,vertexid_t *edges,label_t *labels);
    void clear();

    //void print();
};

#endif
