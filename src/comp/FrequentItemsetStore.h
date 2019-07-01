//
// Created by w on 19-6-30.
//

#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
#define BACKEND1_FREQUENTITEMSETSTORE_H


#include "graphstore.h"
#include "art.h"



class FrequentItemsetStore : public GraphStore {
    FrequentItemsetStore();

    FrequentItemsetStore(std::vector<std::set<Edge>> graphs);

    ~FrequentItemsetStore() override;

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void loadGraphStore(const string &file_singleton) override;

private:
    std::unordered_map<PEGraph_Pointer, std::set<Edge>> intToEdgeSet;
    std::unordered_map<Edge, set<Edge>> mapToFrequentItemset;
    std::unordered_map<int, Edge> intToEdge;
    std::unordered_map<Edge, int> edgeToInt;
    int num_p;


    PEGraph *convertToPeGraph(set<Edge> edgeSet);

    set<Edge> convertToEdgeSet(PEGraph *peGraph);

    set<Edge> retrieveSet(set<Edge> &graphSet, set<Edge> &realEdgeSet);

    set<Edge> frequentItemsetMining(int min_support, vector<set<Edge>> &graphs);
};


#endif //BACKEND1_FREQUENTITEMSETSTORE_H
