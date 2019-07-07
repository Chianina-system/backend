//
// Created by w on 19-6-30.
//

#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
#define BACKEND1_FREQUENTITEMSETSTORE_H


static const char *const filePath = "../lib/file/intSets.txt";

static const int support = 2;

#include "graphstore.h"
#include "art.h"



class FrequentItemsetStore : public GraphStore {

public:

    FrequentItemsetStore();

private:

    FrequentItemsetStore(vector<set<int>> graphs);

    ~FrequentItemsetStore() override;

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void loadGraphStore(const string &file) override;

private:
    std::unordered_map<PEGraph_Pointer, std::set<int>> intToEdgeSet;
    std::unordered_map<int, set<int>> intToFrequentItemset;
    std::unordered_map<int, Edge> intToEdge;
    std::unordered_map<Edge, int> edgeToInt;
    int frequentItemsetNum;
    int edgeId;
    int num_p;

    PEGraph *convertToPeGraph(set<int> edgeSet);

    set<int> convertToEdgeSet(PEGraph *peGraph);

    void retrieveSet(set<int> &graphSet, set<int> &realEdgeSet);

    set<int> frequentItemsetMining_closed(int min_support, vector<set<int>> &graphs);

    set<int> frequentItemsetMining_minimum(int min_support, vector<set<int>> &graphs);


    void writeToFile(vector<set<int>> &graphs);

    set<int> readFromFile();


};


#endif //BACKEND1_FREQUENTITEMSETSTORE_H
