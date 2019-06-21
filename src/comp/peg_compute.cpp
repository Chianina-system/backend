#include "peg_compute.h"

PEGCompute::PEGCompute() = default;

long PEGCompute::startCompute(ComputationSet &compset, Grammar *grammar, std::unordered_map<int, EdgesToDelete *> &m) {
    long totalAddedEdges = 0;

    while(true) {
        computeOneIteration(compset,grammar);
        postProcessOneIteration(compset, m);
        long realAddedEdgesPerIter = compset.getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if(!realAddedEdgesPerIter)
            break;
    }
    return totalAddedEdges;
}

long PEGCompute::startCompute(ComputationSet &compset,Grammar *grammar) {
    long totalAddedEdges = 0;

    while(true) {
        computeOneIteration(compset,grammar);
        postProcessOneIteration(compset);
        long realAddedEdgesPerIter = compset.getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if(!realAddedEdgesPerIter)
            break;
    }
    return totalAddedEdges;
}

void PEGCompute::computeOneIteration(ComputationSet &compset,Grammar *grammar) {
    vertexid_t numVertices = compset.getNumVertices();
    for(vertexid_t i = 0;i < numVertices;++i) {
        computeOneVertex(i,compset,grammar);
    }
}

long PEGCompute::computeOneVertex(vertexid_t index,ComputationSet &compset,Grammar *grammar) {
    long newEdgesNum = 0;
    bool oldEmpty = compset.oldEmpty(index);
    bool deltaEmpty = compset.deltaEmpty(index);
    if(oldEmpty && deltaEmpty) return 0; // if this vertex has no edges, no need to merge.
    ContainersToMerge *containers = new myarray::ArraysToMerge(); // use datastrucuture array
    getEdgesToMerge(index,compset,oldEmpty,deltaEmpty,*containers,grammar); // find new edges to containers
    containers->merge(); // merge and sort edges,remove duplicate edges.
    newEdgesNum = containers->getNumEdges();
    if(newEdgesNum)
        compset.setNews(index,newEdgesNum,containers->getEdgesFirstAddr(),containers->getLabelsFirstAddr());
    else
        compset.clearNews(index);

    containers->clear(); delete containers;
    return newEdgesNum;
}

void PEGCompute::getEdgesToMerge(vertexid_t index,ComputationSet &compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar) {
    // add s-rule edges
    if(!deltaEmpty)
        genS_RuleEdges(index,compset,containers,grammar);
    if(!oldEmpty)
        genD_RuleEdges(index,compset,containers,true,grammar);
    if(!deltaEmpty)
        genD_RuleEdges(index,compset,containers,false,grammar);
}

void PEGCompute::genS_RuleEdges(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,Grammar *grammar) {
    vertexid_t numEdges = compset.getDeltasNumEdges(index);                 //## can we make sure that the deltas is uniqueness
    vertexid_t *edges = compset.getDeltasEdges(index);
    char *labels = compset.getDeltasLabels(index);

    char newLabel;
    bool added = false;
    for(vertexid_t i = 0;i < numEdges;++i) {
        newLabel = grammar->checkRules(labels[i]);
        if(newLabel != (char)127) {
            if(!added) {                            // ##这是个啥意思
                containers.addOneContainer();
                added = true;
            }
            containers.addOneEdge(edges[i],newLabel);
        }
    }
}

void PEGCompute::genD_RuleEdges(vertexid_t index,ComputationSet &compset,ContainersToMerge &containers,bool isOld,Grammar *grammar) {
    vertexid_t numEdges;
    vertexid_t *edges;
    char *labels;
    if(isOld) {
        numEdges = compset.getOldsNumEdges(index);
        edges = compset.getOldsEdges(index);
        labels = compset.getOldsLabels(index);
    }
    else {
        numEdges = compset.getDeltasNumEdges(index);
        edges = compset.getDeltasEdges(index);
        labels = compset.getDeltasLabels(index);
    }

    vertexid_t indexInCompset;
    for(vertexid_t i = 0;i < numEdges;++i) {
        indexInCompset = edges[i] - compset.getFirstVid();
        // if target vertice is in memory
        if(indexInCompset >= 0 && indexInCompset < compset.getNumVertices())
            checkEdges(indexInCompset,labels[i],compset,containers,isOld,grammar);
    }
}

void PEGCompute::checkEdges(vertexid_t dstInd,char dstVal,ComputationSet &compset,ContainersToMerge &containers,bool isOld,Grammar *grammar) {
    vertexid_t numEdges;
    vertexid_t *edges;
    char *labels;
    numEdges = compset.getDeltasNumEdges(dstInd);
    edges = compset.getDeltasEdges(dstInd);
    labels = compset.getDeltasLabels(dstInd);

    char newVal;
    bool added = false;
    for(vertexid_t i = 0;i < numEdges;++i) {
        newVal = grammar->checkRules(dstVal,labels[i]);
        if(newVal != (char)127) {
            if(!added) {
                containers.addOneContainer();
                added = true;
            }
            containers.addOneEdge(edges[i],newVal);
        }
    }

    if(!isOld) {
        numEdges = compset.getOldsNumEdges(dstInd);
        edges = compset.getOldsEdges(dstInd);
        labels = compset.getOldsLabels(dstInd);
        added = false;
        for(vertexid_t i = 0;i < numEdges;++i) {
            newVal = grammar->checkRules(dstVal,labels[i]);
            if(newVal != (char)127) {
                if(!added) {
                    containers.addOneContainer();
                    added = true;
                }
                containers.addOneEdge(edges[i],newVal);
            }
        }
    }
}

void PEGCompute::postProcessOneIteration(ComputationSet &compset) {
    // oldsV <- {oldsV,deltasV}
    for(int i = 0;i < compset.getNumVertices();++i) {
        bool oldEmpty = compset.oldEmpty(i);
        bool deltaEmpty = compset.deltaEmpty(i);
        if(oldEmpty) {
            if(deltaEmpty)
                compset.clearOlds(i);
            else {
                compset.setOlds(i,compset.getDeltasNumEdges(i),compset.getDeltasEdges(i),compset.getDeltasLabels(i));
            }
        }
        else {
            if(!deltaEmpty) {
                int len = 0; int n1 = compset.getOldsNumEdges(i); int n2 = compset.getDeltasNumEdges(i);
                vertexid_t *edges = new vertexid_t[n1+n2];
                char *labels = new char[n1+n2];
                myalgo::unionTwoArray(len,edges,labels,n1,compset.getOldsEdges(i),compset.getOldsLabels(i),n2,compset.getDeltasEdges(i),compset.getDeltasLabels(i));
                compset.setOlds(i,len,edges,labels);
                delete[] edges; delete[] labels;
            }
        }
        compset.clearDeltas(i);
    }
    // deltasV <- newsV - oldsV, newsV <= empty set
    for(int i = 0;i < compset.getNumVertices();++i) {
        bool newEmpty = compset.newEmpty(i);
        if(!newEmpty) {
            int len = 0; int n1 = compset.getNewsNumEdges(i); int n2 = compset.getOldsNumEdges(i);
            vertexid_t *edges = new vertexid_t[n1];
            char *labels = new char[n1];
            myalgo::minusTwoArray(len,edges,labels,n1,compset.getNewsEdges(i),compset.getNewsLabels(i),n2,compset.getOldsEdges(i),compset.getOldsLabels(i));
            if(len)
                compset.setDeltas(i,len,edges,labels);
            else
                compset.clearDeltas(i);
            delete[] edges; delete[] labels;
        }
        else
            compset.clearDeltas(i);
        compset.clearNews(i);
    }
}

void PEGCompute::postProcessOneIteration(ComputationSet &compset, std::unordered_map<int, EdgesToDelete *> &m) {
    // oldsV <- {oldsV,deltasV}
    for(int i = 0;i < compset.getNumVertices();++i) {
        bool oldEmpty = compset.oldEmpty(i);
        bool deltaEmpty = compset.deltaEmpty(i);
        if(oldEmpty) {
            if(deltaEmpty)
                compset.clearOlds(i);
            else {
                compset.setOlds(i,compset.getDeltasNumEdges(i),compset.getDeltasEdges(i),compset.getDeltasLabels(i));
            }
        }
        else {
            if(!deltaEmpty) {
                int len = 0; int n1 = compset.getOldsNumEdges(i); int n2 = compset.getDeltasNumEdges(i);
                vertexid_t *edges = new vertexid_t[n1+n2];
                char *labels = new char[n1+n2];
                myalgo::unionTwoArray(len,edges,labels,n1,compset.getOldsEdges(i),compset.getOldsLabels(i),n2,compset.getDeltasEdges(i),compset.getDeltasLabels(i));
                compset.setOlds(i,len,edges,labels);
                delete[] edges; delete[] labels;
            }
        }
        compset.clearDeltas(i);
    }
    // deltasV <- newsV - oldsV, newsV <= empty set
    for(int i = 0;i < compset.getNumVertices();++i) {
        bool newEmpty = compset.newEmpty(i);
        if(!newEmpty) {
            int len = 0; int n1 = compset.getNewsNumEdges(i); int n2 = compset.getOldsNumEdges(i);
            vertexid_t *edges = new vertexid_t[n1];
            char *labels = new char[n1];
            myalgo::minusTwoArray(len,edges,labels,n1,compset.getNewsEdges(i),compset.getNewsLabels(i),n2,compset.getOldsEdges(i),compset.getOldsLabels(i));

            for (int j = 0; j < len; ++j) {
                m[i]->addOneEdge(edges[j], labels[j]);
            }

            if(len)
                compset.setDeltas(i,len,edges,labels);
            else
                compset.clearDeltas(i);
            delete[] edges; delete[] labels;
        }
        else
            compset.clearDeltas(i);
        compset.clearNews(i);
    }
}
