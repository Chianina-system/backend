/*
 * cfg_compute_asynchronous.h
 *
 *  Created on: Jul 5, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_ASYN_H_
#define COMP_CFG_COMPUTE_ASYN_H_

//#include "graphstore.h"
//#include "cfg_map.h"
//#include "naive_graphstore.h"
//#include "concurrent_workset.h"
//#include "concurrent_workqueue.h"
//#include "grammar.h"
//#include "computationset.h"
//#include "singletons.h"
//#include "../utility/StaticPrinter.h"
//#include "../utility/Logger.hpp"
//#include "peg_compute.h"
//#include "peg_compute_parallel.h"
#include "cfg_compute.h"

using namespace std;

class CFGCompute_asyn {

public:
	static void do_worklist_asynchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons); //worklist algorithm in parallel



private:
	static void compute_asynchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist* worklist_1, Grammar* grammar, Singletons* singletons);

	static PEGraph* combine_asynchronous(GraphStore* graphstore, std::vector<CFGNode*>& preds);


};




#endif /* COMP_CFG_COMPUTE_ASYN_H_ */
