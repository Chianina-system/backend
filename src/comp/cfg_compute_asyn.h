/*
 * cfg_compute_asynchronous.h
 *
 *  Created on: Jul 5, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_ASYN_H_
#define COMP_CFG_COMPUTE_ASYN_H_

#include "cfg_compute.h"

using namespace std;

class CFGCompute_asyn {

public:
	static void do_worklist_asynchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons); //worklist algorithm in parallel



private:

	static void compute_asynchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Grammar* grammar, Singletons* singletons);

	static PEGraph* combine_asynchronous(GraphStore* graphstore, std::vector<CFGNode*>& preds);


};




#endif /* COMP_CFG_COMPUTE_ASYN_H_ */
