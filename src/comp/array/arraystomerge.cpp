#include "arraystomerge.h"
#include "../myalgorithm.h"
#include "../minheap.h"
#include <limits.h>

namespace myarray {

    ArraysToMerge::ArraysToMerge() {
        size = capacity = arraySize = arrayCapacity = numEdges = 0;
        resEdges = NULL; resLabels = NULL;
    }

    void ArraysToMerge::mergeKArrays() {
        if(size) {
            vertexid_t *newEdges;
            char *newLabels;
            // minHeap algorithm to merge k arrays
            if(arraySize > 1) {
                newEdges = new vertexid_t[size]; newLabels = new char[size];
                // initial k-MinHeap
                MinHeapNode *harr = new MinHeapNode[arraySize];
                for(int i = 0;i < arraySize;++i) {
                    harr[i].key_v = edges[addr[i]];
                    harr[i].key_c = labels[addr[i]];
                    harr[i].i = i;
                    harr[i].j = 1;
                }
                MinHeap hp(harr,arraySize);
                for(int i = 0;i < arraySize;++i) {
                    for(int j = 0;j < index[i];++j) {
                        MinHeapNode root = hp.getMin();
                        newEdges[addr[i] + j] = root.key_v;
                        newLabels[addr[i] + j] = root.key_c;
                        if(root.j < index[root.i]) {
                            root.key_v = edges[addr[root.i] + root.j];
                            root.key_c = labels[addr[root.i] + root.j];
                            ++root.j;
                        }
                        else
                            root.key_v = INT_MAX;
                        hp.replaceMin(root);
                    }
                }
                delete[] harr;
            }
            // remove duplicate edges
            vertexid_t *edge_v = new vertexid_t[size];
            char *edge_l = new char[size];
            int len = 0;
            if(arraySize > 1) {
                myalgo::removeDuple(len,edge_v,edge_l,size,newEdges,newLabels);
                delete[] newEdges; delete[] newLabels;
            }
            else
                myalgo::removeDuple(len,edge_v,edge_l,size,edges,labels);

            numEdges = len;
            memcpy(edges,edge_v,sizeof(vertexid_t) * len);
            memcpy(labels,edge_l,sizeof(char) * len);
            resEdges = edges; resLabels = labels;
            delete[] edge_v; delete[] edge_l;
        }
    }

    void ArraysToMerge::clear() {
        if(capacity) {
            if(edges) {	delete[] edges; edges = NULL;}
            if(resEdges) { resEdges = NULL;}
            if(labels) { delete[] labels; labels = NULL;}
            if(resLabels) {  resLabels = NULL;}
            capacity = size = 0;
        }
        if(arrayCapacity) {
            if(index) { delete[] index; index = NULL; }
            if(addr) { delete[] addr; addr = NULL; }
            arrayCapacity = arraySize;
        }
        numEdges = 0;
    }

    void ArraysToMerge::addOneContainer() {
        if(arraySize == 0) {
            arrayCapacity = _CAPACITY_VALUE;
            capacity = CAPACITY_VALUE;
            edges = new vertexid_t[capacity];
            labels = new char[capacity];
            index = new vertexid_t[arrayCapacity];
            addr = new vertexid_t[arrayCapacity];
            for(int i = 0;i < capacity;++i) {
                edges[i] = -1;
                labels[i] = (char)127;
            }
            for(int i = 0;i < arrayCapacity;++i)
                index[i] = addr[i] = 0;
        }
        else {
            if(arraySize >= arrayCapacity) {
                arrayCapacity *= 2;
                myalgo::myrealloc(index,arraySize,arrayCapacity);
                myalgo::myrealloc(addr,arraySize,arrayCapacity);
                for(int i = arraySize;i < arrayCapacity;++i)
                    index[i] = addr[i] = 0;
            }
        }
        // add one empty array
        ++arraySize;
    }

    void ArraysToMerge::addOneEdge(vertexid_t edge,char label) {
        if(arraySize) {
            if(size >= capacity) {
                capacity *= 2;
                myalgo::myrealloc(edges,size,capacity);
                myalgo::myrealloc(labels,size,capacity);
                for(int i = size;i < capacity;++i) {
                    edges[i] = -1;
                    labels[i] = (char)127;
                }
            }
            // add edge
            if(index[arraySize-1] == 0) {
                addr[arraySize-1] = size;
            }
            edges[size] = edge;
            labels[size] = label;
            ++index[arraySize-1];
            ++size;
        }
        else {
            cout << "add edge failed! " << endl;
        }
    }

    void ArraysToMerge::merge() {
        mergeKArrays();
    }

}
