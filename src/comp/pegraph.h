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
class PEGraph{

public:
	PEGraph();
	~PEGraph();

	PEGraph(PEGraph *p); 
	bool equals(PEGraph* another);
	
	// getters and setters
	inline vertexid_t getFirstVid() {return firstVid;}
	inline EdgeArray* getGraph() {return graph;}
	inline bool* getSingletonArray() {return isSingletonArray;}
	inline vertexid_t getNumVertices() {return numVertices;}
	inline bool isSingleton(vertexid_t vid) {return isSingletonArray[vid - firstVid];}	


	inline vertexid_t* getEdges(vertexid_t index) {return graph[index].getEdges();}
	inline label_t* getLabels(vertexid_t index) {return graph[index].getLabels();}
	inline vertexid_t getNumEdges(vertexid_t index) {return graph[index].getSize();}
	
	void setEdgeArray(vertexid_t index,int numEdges,vertexid_t *edges,label_t *labels);
	void clearEdgeArray(vertexid_t index);

private:
	vertexid_t firstVid;
	EdgeArray *graph;
	bool *isSingletonArray;
	vertexid_t numVertices;
};




#endif /* COMP_PEGRAPH_H_ */
