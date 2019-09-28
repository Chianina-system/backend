/*
 * graphstore.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_GRAPHSTORE_H_

#include "../pegraph.h"
#include "../cfg.h"
#include "../concurrent_worklist/concurrent_workset.h"
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

    //deep copy; locked version for asynchronous mode
    PEGraph* retrieve_locked(PEGraph_Pointer graph_pointer){
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	return retrieve(graph_pointer);
    }

    virtual PEGraph* retrieve(PEGraph_Pointer graph_pointer) = 0;

    //deep copy; locked version for asynchronous mode
    void update_locked(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	update(graph_pointer, pegraph);
    }

    virtual void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) = 0;

    virtual void loadGraphStore(const string& file, const string& file_in) = 0;

//    virtual void init(CFG* cfg) = 0;

//    //shallow copy
//    virtual void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph* graph) = 0;

//    virtual void update_graphs(GraphStore* another) = 0;

    virtual void update_graphs_sequential(GraphStore* another) = 0;

    virtual void update_graphs_parallel(GraphStore* another) = 0;

    void update_graphs(GraphStore* another, bool update_mode){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs starting...\n", LEVEL_LOG_FUNCTION);

    	if(update_mode){
	    	update_graphs_parallel(another); // in parallel
    	}
    	else{
			update_graphs_sequential(another); // sequential
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs finished.\n", LEVEL_LOG_FUNCTION);
    }

    virtual void printOutInfo() = 0;


//    virtual void clearEntryOnly() = 0;

//    virtual void clear() = 0;

    virtual void deserialize(const string& file) = 0;

    virtual void serialize(const string& file) = 0;

//    virtual void printOutInfo() = 0;

    virtual std::string toString(){
    	std::ostringstream strm;
		strm << "Graphstore<<<<\n============================================" << endl;
		toString_sub(strm);
		strm << "============================================" << endl;
		return strm.str();
    }


protected:

    virtual void print(std::ostream& str) = 0;

    virtual void toString_sub(std::ostringstream& strm) = 0;


    std::mutex mutex;

private:
    //	void add();



};



#endif /* COMP_GRAPHSTORE_GRAPHSTORE_H_ */
