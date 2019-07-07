//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

void FrequentItemsetStore::loadGraphStore(const string &file_singleton) {

}


FrequentItemsetStore::~FrequentItemsetStore() {

}

PEGraph *FrequentItemsetStore::retrieve_locked(PEGraph_Pointer graph_pointer) {

}

PEGraph *FrequentItemsetStore::retrieve(PEGraph_Pointer graph_pointer) {
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

void FrequentItemsetStore::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {

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
    writeToFile(graphs);

    vector<set<int>> v = readFromFile();
    return set<int>();
}

FrequentItemsetStore::FrequentItemsetStore() : frequentItemsetNum(0), edgeId(0) {}

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

void FrequentItemsetStore::writeToFile(vector<set<int>> &graphs) {
    ofstream output;
    output.open(filePath);

    for (auto graph : graphs) {
        for (auto edgeId: graph) {
            cout << edgeId << " ";            // todo check the end of the line
        }
        cout << endl;
    }
    output.close();
}

vector<set<int>> FrequentItemsetStore::readFromFile() {
    vector<set<int>> graphs;

    ifstream fin;
    fin.open(filePath);
    if (!fin) {
        cout << "can't load file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(fin, line) && line != "") {
        set<int> graph;
        std::stringstream stream(line);
        int id;
        while (stream >> id) graph.insert(id);
        graphs.push_back(graph);
    }
    fin.close();
}


