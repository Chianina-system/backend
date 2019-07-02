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
	//for debugging
	Logger::print_thread_info_locked("pegraph copy starting...\n", 1);

    for (auto it = p->getGraph().begin(); it != p->getGraph().end(); it++){
        graph[it->first] = it->second;
    }

	//for debugging
	Logger::print_thread_info_locked("pegraph copy finished.\n", 1);
}

void PEGraph::merge(PEGraph * graph_toMerge){
	for (auto it = graph_toMerge->getGraph().begin(); it != graph_toMerge->getGraph().end(); it++) {
		if (this->getGraph().find(it->first) != this->getGraph().end()) {
			// merge the edgearray with the same src in graph_1 and graph_2
			int n1 = it->second.getSize();
			int n2 = this->getNumEdges(it->first);

			vertexid_t *edges = new vertexid_t[n1 + n2];
			char *labels = new char[n1 + n2];
			int len = myalgo::unionTwoArray(edges, labels, n1,
					it->second.getEdges(), it->second.getLabels(), n2,
					this->getEdges(it->first),
					this->getLabels(it->first));

			this->graph[it->first].set(len, edges, labels);

			delete[] edges;
			delete[] labels;

		}
		else {
			this->graph[it->first] = it->second;
		}
	}

}

std::unordered_map<vertexid_t, EdgeArray> & PEGraph::getGraph() {
    return graph;
}

//void PEGraph::setGraph(std::unordered_map<vertexid_t, EdgeArray> &_graph) {
//    this->graph = _graph;
//}

bool PEGraph::equals(PEGraph *another) {
	//TODO:
	if(!this || !another){
		return false;
	}

	if(this == another){
		return true;
	}

	if(this->graph.size() != another->graph.size()){
		return false;
	}

	for(auto it = this->graph.begin(); it != this->graph.end(); ++it){
		vertexid_t id = it->first;
		if(another->graph.find(id) == another->graph.end()){
			return false;
		}
		else if (!it->second.equals(&(another->graph.at(id)))){
			return false;
		}
	}

    return true;
}

void PEGraph::clearEdgeArray(vertexid_t index) {
	this->graph.erase(index);
}

void PEGraph::setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels) {
    if(this->graph.find(index)==this->graph.end()){
        this->graph[index] = EdgeArray();
    }
    this->graph[index].set(numEdges,edges,labels);
}

//const std::set<vertexid_t> &PEGraph::getSingletonSet() const {
//    return singletonSet;
//}

//void PEGraph::setSingletonSet(const std::set<vertexid_t> &singletonSet) {
//    PEGraph::singletonSet = singletonSet;
//}

