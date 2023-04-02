/*
 * graphstore.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_GRAPHSTORE_H_

//#include "../pegraph.h"
#include <cachestate.h>
#include "../cfg.h"
#include "../concurrent_worklist/concurrent_workset.h"
//#include "peGraphPointer.h"
//#include "cfg_compute.h"
#include "hybrid_graphstore.h"
#include "buffer.h"
#include "../io_manager.hpp"

using namespace std;

class GraphStore{

	friend std::ostream & operator<<(std::ostream & strm, GraphStore& graphstore) {
		strm << "Graphstore<<<<\n============================================" << endl;
		//graphstore.print(strm);
		strm << "============================================" << endl;
		return strm;
	}

public:
    GraphStore(bool f_mode, bool buffered_m){
    	file_mode = f_mode;
    	buffered_mode = buffered_m;
    }

    virtual ~GraphStore(){};



    virtual void loadGraphStore(const string& file, const string& file_in, Partition part, int mining_mode, int support, int length) = 0;

    virtual std::unordered_map<PEGraph_Pointer, cachestate*>& getMap() = 0;

    virtual void deserialize(const string& file) = 0;

    virtual void serialize(const string& file) = 0;

    virtual void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set) = 0;

    virtual std::string toString(){
    	std::ostringstream strm;
		strm << "Graphstore<<<<\n============================================" << endl;
		//toString_sub(strm);
		strm << "============================================" << endl;
		return strm.str();
    }

    virtual void getStatistics(int& size_graphs,long& size_graphitems, const std::unordered_set<PEGraph_Pointer>& mirrors) = 0;


    virtual void addOneGraph_atomic(int i, cachestate *pCachestate) = 0;

protected:

    //virtual void print(std::ostream& str) = 0;

    //virtual void toString_sub(std::ostringstream& strm) = 0;


    std::mutex mtx;
    bool file_mode;
    bool buffered_mode;

//	void add();



};



#endif /* COMP_GRAPHSTORE_GRAPHSTORE_H_ */
