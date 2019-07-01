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
    std::set<Edge> peGraphSet = intToEdgeSet[graph_pointer];
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

    intToEdgeSet.erase(graph_pointer);
    // todo delete the pointer
    std::set<int> edgeSet = convertToEdgeSet(pegraph);
    intToEdgeSet[graph_pointer] = edgeSet;
}


FrequentItemsetStore::FrequentItemsetStore(vector<set<int>> graphs) {
    set<int> frequentItemset = frequentItemsetMining(2, graphs);       //compute the frequent itemset
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
//                Edge newEdge = Edge(-num_p, 0, 0);
                int newEdge = -num_p;
                num_p++;
                intToFrequentItemset[newEdge] = frequentItemset;
                graph.insert(newEdge);
            }
        }
        frequentItemset = frequentItemsetMining(2, graphs);
    }
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

    return set<int>();

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


