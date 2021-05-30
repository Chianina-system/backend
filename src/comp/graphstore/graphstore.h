/*
 * graphstore.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_GRAPHSTORE_H_

//#include "../pegraph.h"
#include "../cfg.h"
#include "../concurrent_worklist/concurrent_workset.h"
//#include "peGraphPointer.h"
//#include "cfg_compute.h"
#include "hybrid_graphstore.h"
#include "buffer.h"
#include "../io_manager.hpp"

using namespace std;

class GraphStore{

	

public:
    
    virtual void loadGraphStore(const string& file, const string& file_in, Partition part, int mining_mode, int support, int length) = 0;

    virtual void deserialize(const string& file) = 0;
    
    virtual void serialize(const string& file) = 0;

    virtual void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set) = 0;

protected:


    std::mutex mtx;
    bool file_mode;
    bool buffered_mode;



};



#endif /* COMP_GRAPHSTORE_GRAPHSTORE_H_ */



