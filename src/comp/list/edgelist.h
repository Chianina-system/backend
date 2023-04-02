#ifndef LIST_EDGELIST_H
#define LIST_EDGELIST_H

#include "../../common/CommonLibs.hpp"

namespace mylist {
    typedef struct edgeNode {
        vertexid_t vid;
        char label;
        struct edgeNode *next;
    } EdgeNode;

    class EdgeList {
    private:
        vertexid_t numOutEdges;
        EdgeNode *head;
        EdgeNode *tail;

    public:
        EdgeList();

        inline vertexid_t getNumOutEdges() { return numOutEdges; }

        inline EdgeNode *getHead() { return head; }

        void addEdge(vertexid_t vid, char label);

        bool checkDuple();

        void clear();

        void print();
    };
}
#endif
