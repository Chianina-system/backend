#ifndef LISTTOMERGE_H
#define LISTTOMERGE_H
#include "edgelist.h"
#include "../containerstomerge.h"

namespace mylist {

class ListsToMerge : public ContainersToMerge {
	private:
		EdgeList *lists;	
		int numOfLists;
		int capacityOfLists;
		int numEdges;	// exclude duplicate edges
		
		// store result
		vertexid_t *edges;
		char *labels;

	public:
		ListsToMerge();
		// getters and setters
		inline bool isEmpty() {return !numOfLists;}
		void mergeKLists();

		// virtual functions
		virtual void addOneContainer();
		virtual void addOneEdge(vertexid_t edge,char label);
		virtual int getNumEdges() {return numEdges;}
		virtual void merge();
		virtual vertexid_t* getEdgesFirstAddr() {return edges;}
		virtual char* getLabelsFirstAddr() {return labels;}
		virtual void clear();

};	
}
#endif
