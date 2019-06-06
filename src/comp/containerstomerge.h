#ifndef CONTAINERSTOMERGE_H
#define CONTAINERSTOMERGE_H
#include "../common/CommonLibs.hpp"

class ContainersToMerge {
public:
	virtual void addOneContainer() = 0;
	virtual void addOneEdge(vertexid_t vid,char label) = 0;
	virtual int getNumEdges() = 0;
	virtual void merge() = 0;	
	virtual vertexid_t* getEdgesFirstAddr() = 0;
	virtual char* getLabelsFirstAddr() = 0;
	virtual void clear() = 0;	
};
#endif
