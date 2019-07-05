//
// Created by w on 19-6-30.
//

#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
#define BACKEND1_FREQUENTITEMSETSTORE_H


#include "graphstore.h"
#include "art.h"



class FrequentItemsetStore : public GraphStore {

public:

    FrequentItemsetStore();

private:

    ~FrequentItemsetStore() override;

    PEGraph *retrieve_synchronous(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_asynchronous(PEGraph_Pointer graph_pointer) override;

    void update_synchronous(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_asynchronous(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

//    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    void loadGraphStore(const string &file_singleton) override;

private:
    std::unordered_map<PEGraph_Pointer, std::set<int>> intToEdgeSet;
    std::unordered_map<int, set<int>> intToFrequentItemset;
    std::unordered_map<int, Edge> intToEdge;
    std::unordered_map<Edge, int> edgeToInt;
    int frequentItemsetNum;
    int edgeId;


    PEGraph *convertToPeGraph(set<int> edgeSet);

    set<int> convertToEdgeSet(PEGraph *peGraph);

    void retrieveSet(set<int> &graphSet, set<int> &realEdgeSet);

    set<int> frequentItemsetMining(int min_support, vector<set<int>> &graphs);


};


#endif //BACKEND1_FREQUENTITEMSETSTORE_H
