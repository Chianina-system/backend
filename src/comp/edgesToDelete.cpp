#include "edgesToDelete.h"

EdgesToDelete::EdgesToDelete() {
    edges = NULL; labels = NULL;
    size = capacity = realNumEdges = 0;
}

void EdgesToDelete::clear() {
    if(size) {
        if(edges) {delete[] edges; edges = NULL;}
        if(labels) {delete[] labels; labels = NULL;}
        size = capacity = realNumEdges = 0;
    }
}

void EdgesToDelete::addOneEdge(vertexid_t edge,label_t label) {
    if(size == 0) {
        capacity = CAPACITY_VALUE;
        edges = new vertexid_t[capacity];
        labels = new label_t[capacity];
        //initial
        for(int i = 0;i < capacity;++i) {
            edges[i] = -1;
            labels[i] = (char)127;
        }
    }
    else {
        if(size >= capacity) {
            capacity *= 2;
            myalgo::myrealloc(edges,size,capacity);
            myalgo::myrealloc(labels,size,capacity);
            for(int i = size;i < capacity;++i) {
                edges[i] = -1;
                labels[i] = (char)127;
            }
        }
    }
    // add edge
    edges[size] = edge;
    labels[size] = label;
    ++size;
}

void EdgesToDelete::merge() {
    // sort edges
    myalgo::quickSort(edges,labels,0,size-1);
    // remove duplicate edges
    vertexid_t *_edges = new vertexid_t[size];
    label_t *_labels = new label_t[size];
    int _numEdges = 0;
    myalgo::removeDuple(_numEdges,_edges,_labels,size,edges,labels);
    realNumEdges = _numEdges;
    memcpy(edges,_edges,sizeof(vertexid_t) * _numEdges);
    memcpy(labels,_labels,sizeof(label_t) * _numEdges);
    delete[] _edges; delete[] _labels;
}

