#ifndef COMP_PEGCOMPUTE_PARALLEL_H
#define COMP_PEGCOMPUTE_PARALLEL_H
#include "grammar.h"
#include "computationset.h"
#include "containerstomerge.h"
#include "array/arraystomerge.h"
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"
#include "edgesToDelete.h"
#include "computationset.h"

class PEGCompute_parallel {
private:
    static void computeOneIteration(ComputationSet *compset,Grammar *grammar);
    static long computeOneVertex(vertexid_t index, ComputationSet *compset, Grammar *grammar);
    static void postProcessOneIteration(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m = nullptr);
    static void postProcessOneVertex(vertexid_t id, ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m);

    static void getEdgesToMerge(vertexid_t index, ComputationSet *compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);
    static void genS_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_old(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet* compset);

public:
    static long startCompute_add(ComputationSet *compset,Grammar *grammar);
    static long startCompute_delete(ComputationSet *compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> &m);


};
#endif
