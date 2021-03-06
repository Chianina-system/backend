#ifndef COMP_PEGCOMPUTE_H
#define COMP_PEGCOMPUTE_H
#include "computationset.h"
#include "containerstomerge.h"
#include "array/arraystomerge.h"
#include "../common/CommonLibs.hpp"
#include "../preproc/grammar.h"
#include "myalgorithm.h"
//#include "../utility/Logger.hpp"
#include "computationset.h"
#include "../utility/timer_diff.hpp"
#include "../utility/Timer_wrapper_inmemory.h"
#include "../utility/Timer_wrapper_ooc.h"

class PEGCompute {
private:

    static void genS_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_old(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet* compset);
    static void getEdgesToMerge(vertexid_t index, ComputationSet *compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);

public:
    static long startCompute_add(ComputationSet *compset,Grammar *grammar, Timer_wrapper_inmemory* timer);
    static long startCompute_delete(ComputationSet *compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> *m);

    static long computeOneVertex(vertexid_t index, ComputationSet *compset, Grammar *grammar, Timer_wrapper_inmemory* timer = nullptr);

    static void computeOneIteration(ComputationSet *compset,Grammar *grammar, Timer_wrapper_inmemory* timer = nullptr);
    static void postProcessOneIteration(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m = nullptr);

};
#endif
