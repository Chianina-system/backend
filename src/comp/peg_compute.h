#ifndef COMP_PEGCOMPUTE_H
#define COMP_PEGCOMPUTE_H
#include "grammar.h"
#include "computationset.h"
#include "containerstomerge.h"
#include "array/arraystomerge.h"
#include "../common/CommonLibs.hpp"
#include "myalgorithm.h"

class PEGCompute {
private:

public:
    PEGCompute();
    long startCompute(ComputationSet &compset,Grammar *grammar);

    void computeOneIteration(ComputationSet &compset,Grammar *grammar);
    long computeOneVertex(vertexid_t index,ComputationSet &compset,Grammar *grammar);
    void postProcessOneIteration(ComputationSet &compset);

    void getEdgesToMerge(vertexid_t index,ComputationSet &compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);
    void genS_RuleEdges(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,Grammar *grammar);
    void genD_RuleEdges(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,bool isOld,Grammar *grammar);
    void checkEdges(vertexid_t dstInd,char dstVal,ComputationSet &compset,ContainersToMerge &containers,bool isOld,Grammar *grammar);

};
#endif
