#ifndef MINHEAP_H
#define MINHEAP_H
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"

typedef struct minHeapNode {
    vertexid_t key_v;
    char key_c;
    int i;	// index of array
    int j;	// next element's index
}MinHeapNode;

class MinHeap {
private:
    MinHeapNode *harr;	// only store address of array
    int size;
public:
    MinHeap(MinHeapNode a[],int size);
    inline int left(int i) {return 2*i+1;}
    inline int right(int i) {return 2*i+2;}
    inline MinHeapNode getMin() {return harr[0];}

    void MinHeapify(int i);
    void replaceMin(MinHeapNode p);
    void swap(MinHeapNode *x,MinHeapNode *y);
};
#endif
