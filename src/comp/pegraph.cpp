/*
 * pegraph.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#include "pegraph.h"

PEGraph::PEGraph(PEGraph* p) {
	this->firstVid = p->getFirstVid();
	this->numVertices = p->getNumVertices();
	this->isSingletonArray = new bool[this->numVertices];
	memcpy(this->isSingletonArray,p->getSingletonArray(),sizeof(bool)*this->numVertices);
	
	this->graph = new EdgeArray[this->numVertices];
	for(int i = 0;i < this->numVertices;++i) {
		if(p->getNumEdges(i))	
			this->graph[i] = EdgeArray(p->getNumEdges(i),p->getEdges(i),p->getLabels(i));
		else
			this->graph[i] = EdgeArray();	
	}
}

bool PEGraph::equals(PEGraph* another){

}

void PEGraph::setEdgeArray(vertexid_t index,int numEdges,vertexid_t *edges,label_t *labels) {
	this->graph[index].set(numEdges,edges,labels);	
}

void PEGraph::clearEdgeArray(vertexid_t index) {
	this->graph[index].clear();
}
