/*
 * cfg_compute_ooc.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_H_
#define COMP_CFG_COMPUTE_OOC_H_


#include "cfg_compute.h"
using namespace std;

class CFGCompute_ooc {

public:
	static void do_worklist_ooc(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives); //worklist algorithm in parallel



private:

	static void compute_ooc(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives);



};




#endif /* COMP_CFG_COMPUTE_OOC_H_ */
