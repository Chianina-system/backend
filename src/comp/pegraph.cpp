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
    std::unordered_map<vertexid_t, EdgeArray> _graph;
    std::set<vertexid_t> _singletonSet;
    for (auto it = p->getGraph().begin(); it != p->getGraph().end(); it++){
        _graph[it->first] = it->second;
    }
    for (auto it = p->getSingletonSet().begin(); it != p->getSingletonSet().end(); it++){
        _singletonSet.insert(*it);
    }
    this->setGraph(_graph);
    this->setSingletonSet(singletonSet);
}

const std::unordered_map<vertexid_t, EdgeArray> & PEGraph::getGraph() const {
    return graph;
}

void PEGraph::setGraph(const std::unordered_map<vertexid_t, EdgeArray> &_graph) {
    this->graph = _graph;
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

const std::set<vertexid_t> &PEGraph::getSingletonSet() const {
    return singletonSet;
}

void PEGraph::setSingletonSet(const std::set<vertexid_t> &singletonSet) {
    PEGraph::singletonSet = singletonSet;
}

