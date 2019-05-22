/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_H_
#define COMP_CFG_COMPUTE_H_

#include "graphstore.h"
#include "cfg.h"
#include "concurrent_worklist.h"

class CFGCompute{

public:
	unsigned int num_threads;

	void load(Partition* part);

	void do_worklist(CFG* cfg, GraphStore* graphstore); //worklist algorithm in parallel



private:
//	CFG* cfg;
//	GraphStore* graphstore;




	void compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2);


//	void peg_compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist, CFGNode* cfg_node);

	PEGraph* combine(CFGNode* cfg_node);

	PEGraph* transfer(PEGraph* in, Stmt* stmt);





};


#endif /* COMP_CFG_COMPUTE_H_ */
