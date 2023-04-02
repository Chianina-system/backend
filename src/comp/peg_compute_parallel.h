#ifndef COMP_PEGCOMPUTE_PARALLEL_H
#define COMP_PEGCOMPUTE_PARALLEL_H
//#include "grammar.h"
//#include "computationset.h"
//#include "containerstomerge.h"
//#include "array/arraystomerge.h"
//#include "../common/CommonLibs.hpp"
//#include "myalgorithm.h"
//#include "edgesToDelete.h"
//#include "computationset.h"
#include "concurrent_worklist/concurrent_workqueue.h"
#include "concurrent_worklist/concurrent_workset.h"
#include "peg_compute.h"
//#include "concurrent_worklist.h"

class PEGCompute_parallel {
private:
    static void computeOneIteration(ComputationSet *compset,Grammar *grammar);
    static void computeOneVertex(ComputationSet *compset, Grammar *grammar, Concurrent_Worklist<vertexid_t>* worklist);
//    static long computeOneVertex(int index, ComputationSet *compset, Grammar *grammar);
    static void postProcessOneIteration(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m = nullptr);
    static void postProcessOneVertex(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m, Concurrent_Worklist<vertexid_t>* worklist);
    static void postProcessOneVertex(vertexid_t id, ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m);

    static void initiateTasks(ComputationSet* compset, Concurrent_Worklist<vertexid_t>* worklist);

//    static void getEdgesToMerge(vertexid_t index, ComputationSet *compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);
//    static void genS_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
//    static void genD_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
//    static void genD_RuleEdges_old(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet* compset);

    static void initiateComputationSet(std::unordered_set<vertexid_t>& vertexSet, ComputationSet *compset, std::unordered_map<vertexid_t, EdgeArray> *m = nullptr);

public:
    static long startCompute_add(ComputationSet *compset,Grammar *grammar);
    static long startCompute_delete(ComputationSet *compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> *m);


};
#endif
