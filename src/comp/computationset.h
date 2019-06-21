#ifndef COMP_COMPUTATIONSET_H
#define COMP_COMPUTATIONSET_H
#include "edgearray.h"
#include "../common/CommonLibs.hpp"
#include "pegraph.h"
#include "stmt.h"
#include "myalgorithm.h"
#include "edgesToDelete.h"

class ComputationSet {
private:
    EdgeArrayMap  *Olds;	// Ov
    EdgeArrayMap *Deltas;	// Dv
    EdgeArrayMap *News;
    vertexid_t firstVid;
    vertexid_t numVertices;

public:
    ComputationSet();
    void clear();

    void init(PEGraph *out,Stmt *stmt);

    void init(PEGraph *out, std::unordered_map<int, EdgesToDelete*>& m);

    // getters and setters
    inline vertexid_t getFirstVid() {return firstVid;}
    inline vertexid_t getNumVertices() {return numVertices;}
    inline bool oldEmpty(vertexid_t index) {return Olds->getEdgeArray(index)->isEmpty();}
    inline bool deltaEmpty(vertexid_t index) {return Deltas->getEdgeArray(index)->isEmpty();}
    inline bool newEmpty(vertexid_t index) {return News->getEdgeArray(index)->isEmpty();}

    inline vertexid_t getOldsNumEdges(vertexid_t index) {return Olds->getEdgeArray(index)->getSize();}
    inline vertexid_t getDeltasNumEdges(vertexid_t index) {return Deltas->getEdgeArray(index)->getSize();}
    inline vertexid_t getNewsNumEdges(vertexid_t index) {return News->getEdgeArray(index)->getSize();}

    inline char* getOldsLabels(vertexid_t index) {return Olds->getEdgeArray(index)->getLabels();}
    inline char* getDeltasLabels(vertexid_t index) {return Deltas->getEdgeArray(index)->getLabels();}
    inline char* getNewsLabels(vertexid_t index) {return News->getEdgeArray(index)->getLabels();}

    inline vertexid_t* getOldsEdges(vertexid_t index) {return Olds->getEdgeArray(index)->getEdges();}
    inline vertexid_t* getDeltasEdges(vertexid_t index) {return Deltas->getEdgeArray(index)->getEdges();}
    inline vertexid_t* getNewsEdges(vertexid_t index) {return News->getEdgeArray(index)->getEdges();}

    long getOldsTotalNumEdges();
    long getDeltasTotalNumEdges();
    long getNewsTotalNumEdges();

    void setOlds(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);
    void setDeltas(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);
    void setNews(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);

    void clearOlds(vertexid_t index);
    void clearDeltas(vertexid_t index);
    void clearNews(vertexid_t index);

    vertexid_t getDeltasNumRealVertices();
};
#endif
