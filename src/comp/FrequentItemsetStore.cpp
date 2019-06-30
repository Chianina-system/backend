//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

FrequentItemsetStore::FrequentItemsetStore() : GraphStore() {

}

void FrequentItemsetStore::loadGraphStore(const string &file_singleton) {
    //handle the singleton.txt
    std::ifstream fin;
    fin.open(file_singleton);
    if (!fin) {
        cout << "can't load file_singleton: " << file_singleton << endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(fin, line) && line != "") {
        std::stringstream stream(line);
        std::string id;
        stream >> id;
        addOneSingleton(atoi(id.c_str()));
//        this->addOneSingleton(atoi(id.c_str()));
    }
    fin.close();
}


FrequentItemsetStore::~FrequentItemsetStore() {

}

PEGraph *FrequentItemsetStore::retrieve(PEGraph_Pointer graph_pointer) {
//    retrieve the pegraph set
    std::set<Edge> *peGraphSet = mapToEdgesSet[graph_pointer];
//    convert it to PEGraph*
    PEGraph *peGraph = convertToPeGraph(peGraphSet);
    return nullptr;

//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        return new PEGraph(mapToPEG[graph_pointer]);
//    }
//    mapToPEG[graph_pointer] = new PEGraph();
//    return new PEGraph();
}

void FrequentItemsetStore::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        delete mapToPEG[graph_pointer];
//    }
//    mapToPEG[graph_pointer] = new PEGraph(pegraph);


//    std::set<Edge>* graphSet = mapToEdgesSet[graph_pointer];

    mapToEdgesSet.erase(graph_pointer);
    // todo delete the pointer
    std::set<Edge> edgeSet = convertToEdgeSet(pegraph);
    mapToEdgesSet[graph_pointer] = &edgeSet;
}


FrequentItemsetStore::FrequentItemsetStore(std::vector<std::set<Edge>> graphs) {
    set<Edge> frequentItemset = frequentItemsetMining(2, graphs);       //compute the frequent itemset
    while (!frequentItemset.empty()) {
        for (auto &graph: graphs) {
            bool flag = true;           //check if the graphSet need to update
            for (auto &elem : frequentItemset) {
                if (!graph.count(elem)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
//              delete the element in FISet
                for (auto &elem : frequentItemset) {
                    graph.erase(graph.find(elem));
                }
//              insert a special Edge for the num_p
                Edge newEdge = Edge(-num_p, 0, 0);
                num_p++;
                mapToFrequentItemset[newEdge] = frequentItemset;
                graph.insert(newEdge);
            }
        }
        frequentItemset = frequentItemsetMining(2, graphs);
    }
}

// convert from edgeSet to peGraph*
PEGraph *FrequentItemsetStore::convertToPeGraph(set<Edge> *edgeSet) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;
    set<Edge> realGraphSet;
    retrieveSet(*edgeSet, realGraphSet);

    for (auto &edge : *edgeSet) {
        if (graph.find(edge.src) == graph.end()) {
            graph[edge.src].addOneEdge(edge.des, edge.label);
        }
    }
    peGraph->setGraph(graph);
    return peGraph;
}

set<Edge> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
    return set<Edge>();
}

set<Edge> FrequentItemsetStore::retrieveSet(set<Edge> &graphSet, set<Edge> &realEdgeSet) {

    for (auto it = graphSet.begin(); it != graphSet.end(); it++) {
        if (it->src >= 0) realEdgeSet.insert(*it);
        else retrieveSet(mapToFrequentItemset[*it], realEdgeSet);
    }
}

// still under coding
set<Edge> FrequentItemsetStore::frequentItemsetMining(int min_support, vector<set<Edge>> &graphs) {

    return set<Edge>();
}


