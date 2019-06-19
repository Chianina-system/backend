#ifndef COMP_EDGESTODELETE_H
#define COMP_EDGESTODELETE_H
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"

#define CAPACITY_VALUE 8

class EdgesToDelete {
private:
    vertexid_t *edges;
    label_t *labels;
    int size;
    int capacity;
    int realNumEdges; // exclude duplicate edges

public:
    EdgesToDelete();
    ~EdgesToDelete(){}
    void clear();

    // getters and setters
    inline int getRealNumEdges() {return realNumEdges;}
    inline vertexid_t *getEdges() {return edges;}
    inline label_t *getLabels() {return labels;}

    void addOneEdge(vertexid_t edge,label_t label);
    void merge();

};

#endif
