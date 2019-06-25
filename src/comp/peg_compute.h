#ifndef COMP_PEGCOMPUTE_H
#define COMP_PEGCOMPUTE_H
#include "grammar.h"
#include "computationset.h"
#include "containerstomerge.h"
#include "array/arraystomerge.h"
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"
#include "edgesToDelete.h"

class PEGCompute {
private:

public:
    PEGCompute();
    long startCompute_add(ComputationSet &compset,Grammar *grammar);
    long startCompute_delete(ComputationSet &compset, Grammar *grammar, std::unordered_map<int, EdgesToDelete*> &m);

    void computeOneIteration(ComputationSet &compset,Grammar *grammar);
    long computeOneVertex(vertexid_t index,ComputationSet &compset,Grammar *grammar);
//    void postProcessOneIteration_add(ComputationSet &compset);
    void postProcessOneIteration(ComputationSet &compset, bool isDelete, std::unordered_map<int, EdgesToDelete *> *m = nullptr);

    void getEdgesToMerge(vertexid_t index,ComputationSet &compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);
    void genS_RuleEdges_delta(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,Grammar *grammar);
    void genD_RuleEdges_delta(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,Grammar *grammar);
    void genD_RuleEdges_old(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,Grammar *grammar);
//    void checkEdges(vertexid_t dstInd,char dstVal,ComputationSet &compset,ContainersToMerge &containers,bool isOld,Grammar *grammar);

};
#endif
