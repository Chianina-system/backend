#include "computationset.h"
#include <unistd.h>
#include <cstdlib>

ComputationSet::ComputationSet() {
	firstVid = numVertices = 0;	
}

void ComputationSet::init(PEGraph *out,Stmt *stmt) {
	// TODO: after adding assign edge based on stmt,numVertices may increase.
	this->numVertices = out->getNumVertices();	
	this->firstVid = out->getFirstVid();
	Olds = new EdgeArray[this->numVertices];
	Deltas = new EdgeArray[this->numVertices];
	News = new EdgeArray[this->numVertices];

	// OldsV <- (),NewsV <- (),DeltasV <- (out,stmt)
	for(int i = 0;i < numVertices;++i) {
		Olds[i] = EdgeArray();
		News[i] = EdgeArray();
		if(i == (stmt->getSrc() - firstVid)) {
			int len = 0; int n1 = out->getNumEdges(i); int n2 = 1;
			vertexid_t *stmt_edge = new vertexid_t[n2];
			label_t *stmt_label = new label_t[n2];
			*stmt_edge = stmt->getDst();
			*stmt_label = 'A'; //TODO: this label means "ASSIGN"

			vertexid_t *edges = new vertexid_t[n1+n2];
			label_t *labels = new label_t[n1+n2];
			myalgo::unionTwoArray(len,edges,labels,n1,out->getEdges(i),out->getLabels(i),n2,stmt_edge,stmt_label);	
			Deltas[i] = EdgeArray(len,edges,labels);
			delete[] edges; delete[] labels;
			delete[] stmt_edge; delete[] stmt_label;
		}
		else {
			if(out->getNumEdges(i))
				Deltas[i] = EdgeArray(out->getNumEdges(i),out->getEdges(i),out->getLabels(i));	
			else
				Deltas[i] = EdgeArray();	
		}
	}
}

void ComputationSet::clear() {
	if(numVertices) {
		if(Olds) {
			for(int i = 0;i < numVertices;++i)	
				Olds[i].clear();	
			delete[] Olds;
			Olds = NULL;
		}			
		if(Deltas) {
			for(int i = 0;i < numVertices;++i)
				Deltas[i].clear();
			delete[] Deltas;
			Deltas = NULL;
		}
		if(News) {
			for(int i = 0;i < numVertices;++i)
				News[i].clear();
			delete[] News;
			News = NULL;
		}
		numVertices = 0;
	}	
}

long ComputationSet::getOldsTotalNumEdges() {
	long num = 0;
	for(int i = 0;i < numVertices;++i)
		num += getOldsNumEdges(i);
	return num;
}

long ComputationSet::getDeltasTotalNumEdges() {
	long num = 0;
	for(int i = 0;i < numVertices;++i)
		num += getDeltasNumEdges(i);		
	return num;
}

vertexid_t ComputationSet::getDeltasNumRealVertices() {
	vertexid_t num = 0;
	for(int i = 0;i < numVertices;++i)
		if(Deltas[i].getSize())
			++num;
	return num;
}

long ComputationSet::getNewsTotalNumEdges() {
	long num = 0;
	for(int i = 0;i < numVertices;++i)
		num += getNewsNumEdges(i);
	return num;
}

void ComputationSet::setOlds(vertexid_t index,int numEdges,vertexid_t *edges,char *labels) {
	Olds[index].set(numEdges,edges,labels);
}

void ComputationSet::setDeltas(vertexid_t index,int numEdges,vertexid_t *edges,char *labels) {
	Deltas[index].set(numEdges,edges,labels);	
}

void ComputationSet::setNews(vertexid_t index,int numEdges,vertexid_t *edges,char *labels) {
	News[index].set(numEdges,edges,labels);
}

void ComputationSet::clearOlds(vertexid_t index) {
	Olds[index].clear();	
}

void ComputationSet::clearDeltas(vertexid_t index) {
	Deltas[index].clear();	
}

void ComputationSet::clearNews(vertexid_t index) {
	News[index].clear();	
}
