/*
 * PEGraph.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_PEGRAPH_H_
#define COMP_PEGRAPH_H_
#include "../common/CommonLibs.hpp"
#include "edgearray.h"
#include "EdgeArrayMap.h"
class PEGraph{

public:
    PEGraph();
    virtual ~PEGraph(){}

    PEGraph(PEGraph *p);
    bool equals(PEGraph* another);

    // getters and setters
    inline vertexid_t getFirstVid() {return firstVid;}
//	inline EdgeArray* getGraph() {return graph;}
    EdgeArrayMap *getGraph() const {return graph;}

    inline bool* getSingletonArray() {return isSingletonArray;}
    inline vertexid_t getNumVertices() {return numVertices;}
    inline bool isSingleton(vertexid_t vid) {return isSingletonArray[vid - firstVid];}

    void setGraphMap(EdgeArrayMap *graphMap);

    inline vertexid_t* getEdges(vertexid_t index) {return graph->getEdgeArray(index)->getEdges();}
    inline label_t* getLabels(vertexid_t index) {return graph->getEdgeArray(index)->getLabels();}
    inline vertexid_t getNumEdges(vertexid_t index) {return graph->getEdgeArray(index)->getSize();}

    void setEdgeArray(vertexid_t index,int numEdges,vertexid_t *edges,label_t *labels);
    void clearEdgeArray(vertexid_t index);

    static PEGraph* merge(const PEGraph* graph_1, const PEGraph* graph_2){
        //merge graph_1 and graph_2 together to generate a new graph

        delete graph_1;
        delete graph_2;

//		return NULL;
    }

    void setFirstVid(vertexid_t _firstVid);

    void setGraph(EdgeArrayMap *_graph);

    bool *getIsSingletonArray() const;

    void setIsSingletonArray(bool *_isSingletonArray);

    void setNumVertices(vertexid_t numVertices);

private:
    vertexid_t firstVid;
    EdgeArrayMap *graph;
    bool *isSingletonArray;
    vertexid_t numVertices;
};


#endif /* COMP_PEGRAPH_H_ */
