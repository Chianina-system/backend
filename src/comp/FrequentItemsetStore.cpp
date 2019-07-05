//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

void FrequentItemsetStore::loadGraphStore(const string &file_singleton) {

}


FrequentItemsetStore::~FrequentItemsetStore() {

}

PEGraph *FrequentItemsetStore::retrieve_synchronous(PEGraph_Pointer graph_pointer) {

}

PEGraph *FrequentItemsetStore::retrieve_asynchronous(PEGraph_Pointer graph_pointer) {
//    retrieve the pegraph set
    std::set<int> edgeSet = intToEdgeSet[graph_pointer];
//    convert it to PEGraph*
    PEGraph *peGraph = convertToPeGraph(edgeSet);
    return nullptr;

//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        return new PEGraph(mapToPEG[graph_pointer]);
//    }
//    mapToPEG[graph_pointer] = new PEGraph();
//    return new PEGraph();
}

void FrequentItemsetStore::update_synchronous(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        delete mapToPEG[graph_pointer];
//    }
//    mapToPEG[graph_pointer] = new PEGraph(pegraph);


//    std::set<Edge>* graphSet = mapToEdgesSet[graph_pointer];

    intToEdgeSet.erase(graph_pointer);
    // todo delete the pointer
    std::set<int> edgeSet = convertToEdgeSet(pegraph);
    intToEdgeSet[graph_pointer] = edgeSet;
}

void FrequentItemsetStore::update_asynchronous(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {

}

// convert from edgeSet to peGraph*
PEGraph *FrequentItemsetStore::convertToPeGraph(set<int> edgeSet) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;
    set<int> realGraphSet;
    retrieveSet(edgeSet, realGraphSet);

    for (auto edgeId : edgeSet) {
        vertexid_t src = intToEdge[edgeId].src;
        vertexid_t dst = intToEdge[edgeId].des;
        label_t label = intToEdge[edgeId].label;
        if (graph.find(src) == graph.end()) {
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }
    peGraph->setGraph(graph);
    return peGraph;
}

set<int> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
    set<int> edgeSet;
    for (auto &it : peGraph->getGraph()) {

        int size = it.second.getSize();
        vertexid_t *edges = it.second.getEdges();
        label_t *labels = it.second.getLabels();
        Edge edge;
        for (int i = 0; i < size; ++i) {
            edge = Edge(it.first, edges[i], labels[i]);
            if (edgeToInt.find(edge) == edgeToInt.end()) {
                edgeToInt[edge] = edgeId;
                intToEdge[edgeId++] = edge;
            }
            edgeSet.insert(edgeToInt[edge]);
        }
    }

    return edgeSet;
}

void FrequentItemsetStore::retrieveSet(set<int> &graphSet, set<int> &realEdgeSet) {

    for (int it : graphSet) {
        if (it >= 0) realEdgeSet.insert(it);
        else retrieveSet(intToFrequentItemset[it], realEdgeSet);
    }
}

// still under coding
set<int> FrequentItemsetStore::frequentItemsetMining(int min_support, vector<set<int>> &graphs) {

    return set<int>();
}

FrequentItemsetStore::FrequentItemsetStore() :frequentItemsetNum(0), edgeId(0) {}


