/*
 * graphstore.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_H_

#include "pegraph.h"
//#include "peGraphPointer.h"
//#include "cfg_compute.h"

using namespace std;

class GraphStore{

	friend std::ostream & operator<<(std::ostream & strm, GraphStore& graphstore) {
		strm << "Graphstore<<<<\n============================================" << endl;
		graphstore.print(strm);
		strm << "============================================" << endl;
		return strm;
	}

public:
    GraphStore(){}

    virtual ~GraphStore(){}

    virtual PEGraph* retrieve_asynchronous(PEGraph_Pointer graph_pointer) = 0;

    virtual PEGraph* retrieve_synchronous(PEGraph_Pointer graph_pointer) = 0;

    virtual void update_asynchronous(PEGraph_Pointer graph_pointer, PEGraph* pegraph) = 0;

    virtual void update_synchronous(PEGraph_Pointer graph_pointer, PEGraph* pegraph) = 0;

    virtual void loadGraphStore(const string& file_singleton) = 0;

    virtual void addOneGraph(PEGraph_Pointer pointer, PEGraph* graph) = 0;

    virtual void update_graphs(GraphStore* another) = 0;

    virtual void clearEntryOnly() = 0;

    virtual void clear() = 0;

    virtual void addOneSingleton(vertexid_t t) {
        this->singletonSet.insert(t);
    }

    virtual inline bool isSingleton(vertexid_t vid) { return singletonSet.find(vid)!= singletonSet.end();}

    virtual std::string toString(){
    	std::ostringstream strm;
		strm << "Graphstore<<<<\n============================================" << endl;
		toString_sub(strm);
		strm << "============================================" << endl;
		return strm.str();
    }


protected:
    std::set<vertexid_t> singletonSet;

    virtual void print(std::ostream& str) = 0;

    virtual void toString_sub(std::ostringstream& strm) = 0;


    std::mutex mutex;

private:
    //	void add();



};



#endif /* COMP_GRAPHSTORE_H_ */
