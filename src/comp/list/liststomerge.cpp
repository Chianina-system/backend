//#include <limits.h>
//
//#include "liststomerge.h"
//#include "../myalgorithm.h"
//#include "../minheap.h"
//
//namespace mylist {
//
//    ListsToMerge::ListsToMerge() {
//        lists = NULL;
//        edges = NULL;
//        labels = NULL;
//        numOfLists = capacityOfLists = numEdges = 0;
//    }
//
//    void ListsToMerge::mergeKLists() {
//        if (numEdges) {
//            // initial edges,labels
//            vertexid_t *newEdges;
//            char *newLabels;
//            edges = new vertexid_t[numEdges];
//            labels = new char[numEdges];
//            vertexid_t *addr = new vertexid_t[numOfLists];
//            vertexid_t *index = new vertexid_t[numOfLists];
//            int count = 0;
//            for (int i = 0; i < numOfLists; ++i) {
//                EdgeNode *p = lists[i].getHead();
//                addr[i] = count;
//                index[i] = 0;
//                while (p) {
//                    edges[count] = p->vid;
//                    labels[count++] = p->label;
//                    ++index[i];
//                    p = p->next;
//                }
//            }
//
//            if (numOfLists > 1) {
//                newEdges = new vertexid_t[numEdges];
//                newLabels = new char[numEdges];
//                // initial k-MinHeap
//                MinHeapNode *harr = new MinHeapNode[numOfLists];
//                for (int i = 0; i < numOfLists; ++i) {
//                    harr[i].key_v = edges[addr[i]];
//                    harr[i].key_c = labels[addr[i]];
//                    harr[i].i = i;
//                    harr[i].j = 1;
//                }
//                MinHeap hp(harr, numOfLists);
//                for (int i = 0; i < numOfLists; ++i) {
//                    for (int j = 0; j < index[i]; ++j) {
//                        MinHeapNode root = hp.getMin();
//                        newEdges[addr[i] + j] = root.key_v;
//                        newLabels[addr[i] + j] = root.key_c;
//                        if (root.j < index[root.i]) {
//                            root.key_v = edges[addr[root.i] + root.j];
//                            root.key_c = labels[addr[root.i] + root.j];
//                            ++root.j;
//                        } else
//                            root.key_v = INT_MAX;
//                        hp.replaceMin(root);
//                    }
//                }
//                delete[] harr;
//            }
//            // remove duplicate edges
//            vertexid_t *edge_v = new vertexid_t[numEdges];
//            char *edge_l = new char[numEdges];
//            int len = 0;
//            if (numOfLists > 1) {
//                myalgo::removeDuple(len, edge_v, edge_l, numEdges, newEdges, newLabels);
//                delete[] newEdges;
//                delete[] newLabels;
//            } else
//                myalgo::removeDuple(len, edge_v, edge_l, numEdges, edges, labels);
//            memcpy(edges, edge_v, sizeof(vertexid_t) * len);
//            memcpy(labels, edge_l, sizeof(char) * len);
//            numEdges = len;
//            delete[] edge_v;
//            delete[] edge_l;
//            delete[] addr;
//            delete[] index;
//        }
//    }
//
//    void ListsToMerge::clear() {
//        if (capacityOfLists) {
//            if (lists) {
//                for (int i = 0; i < capacityOfLists; ++i)
//                    lists[i].clear();
//                delete[] lists;
//                lists = NULL;
//            }
//            capacityOfLists = numOfLists = 0;
//        }
//        if (numEdges) {
//            if (edges) {
//                delete[] edges;
//                edges = NULL;
//            }
//            if (labels) {
//                delete[] labels;
//                labels = NULL;
//            }
//            numEdges = 0;
//        }
//    }
//
//    void ListsToMerge::addOneContainer() {
//        if (numOfLists == 0) {
//            capacityOfLists = 8;
//            lists = new EdgeList[capacityOfLists];
//        } else {
//            if (numOfLists >= capacityOfLists) {
//                capacityOfLists *= 2;
//                myalgo::myrealloc(lists, numOfLists, capacityOfLists);
//            }
//        }
//        // add one empty list
//        ++numOfLists;
//    }
//
//    void ListsToMerge::addOneEdge(vertexid_t edge, char label) {
//        if (numOfLists) {
//            lists[numOfLists - 1].addEdge(edge, label);
//            ++numEdges;
//        } else
//            cout << "add edge failed! " << endl;
//    }
//
//    void ListsToMerge::merge() {
//        mergeKLists();
//    }
//
//}
