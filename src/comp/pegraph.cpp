/*
 * pegraph.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#include "pegraph.h"

PEGraph::PEGraph() {
    //TODO: create an empty graph

}

PEGraph::PEGraph(PEGraph *p) {
    this->firstVid = p->getFirstVid();
    this->numVertices = p->getNumVertices();
    this->isSingletonArray = new bool[this->numVertices];
    memcpy(this->isSingletonArray, p->getSingletonArray(), sizeof(bool) * this->numVertices);

    this->graph = new EdgeArrayMap;
    for (int i = 0; i < this->numVertices; ++i) {
        if (p->getGraph()->getEdgeArrayMap().at(i)->getSize())
            this->graph->setEdgeArray(i, p->getGraph()->getEdgeArray(i));
    }
}

void PEGraph::setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels) {
    this->graph->setEdgeArray(index, new EdgeArray(numEdges, edges, labels));
}

void PEGraph::clearEdgeArray(vertexid_t index) {
    this->graph->getEdgeArrayMap().at(index)->clear();
}



void PEGraph::setGraphMap(EdgeArrayMap *graphMap) {
    this->graph = graphMap;
}

void PEGraph::setFirstVid(vertexid_t _firstVid) {
    PEGraph::firstVid = _firstVid;
}

void PEGraph::setGraph(EdgeArrayMap *_graph) {
    PEGraph::graph = _graph;
}

bool *PEGraph::getIsSingletonArray() const {
    return isSingletonArray;
}

void PEGraph::setIsSingletonArray(bool *_isSingletonArray) {
    PEGraph::isSingletonArray = _isSingletonArray;
}

void PEGraph::setNumVertices(vertexid_t numVertices) {
    PEGraph::numVertices = numVertices;
}
