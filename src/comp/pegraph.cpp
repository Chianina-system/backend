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
//    this->graph = p->getGraph();
//    this->isSingletonArray = p->getIsSingletonArray();

    std::unordered_map<vertexid_t, EdgeArray> _graph;
    std::unordered_map<vertexid_t, bool> _isSingletonArray;
    for (auto it = p->getGraph().begin(); it != p->getGraph().end(); it++){
        _graph[it->first] = it->second;
    }
    for (auto it = p->getIsSingletonArray().begin(); it != p->getIsSingletonArray().end(); it++){
        _isSingletonArray[it->first] = it->second;
    }
    this->setGraph(_graph);
    this->setIsSingletonArray(_isSingletonArray);

}

const std::unordered_map<vertexid_t, EdgeArray> & PEGraph::getGraph() const {
    return graph;
}

void PEGraph::setGraph(const std::unordered_map<vertexid_t, EdgeArray> &_graph) {
    this->graph = _graph;
}

const std::unordered_map<vertexid_t, bool> &PEGraph::getIsSingletonArray() const {
    return isSingletonArray;
}

bool PEGraph::equals(PEGraph *another) {
    return false;
}

void PEGraph::clearEdgeArray(vertexid_t index) {
    this->graph[index].clear();
}

void PEGraph::setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels) {
    if(this->graph.find(index)==this->graph.end()){
        this->graph[index] = EdgeArray();
    }
    this->graph[index].set(numEdges,edges,labels);
}

void PEGraph::setIsSingletonArray(const std::unordered_map<vertexid_t, bool> &_isSingletonArray) {
    this->isSingletonArray = _isSingletonArray;
}
