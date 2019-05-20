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

class CFGCompute{

public:

	void load(Partition* part);

	void do_worklist(); //worklist algorithm in parallel



private:
	CFG* cfg;
	GraphStore* graphstore;


	void peg_compute(CFGNode* cfg_node){

		PEGraph* in = combine(cfg_node);
		PEGraph* out = transfer(in, cfg_node->getStmt());

		graphstore->update(cfg_node->getOutPointer(), out);

	}

	PEGraph* combine(CFGNode* cfg_node);

	PEGraph* transfer(PEGraph* in, Stmt* stmt);





};


#endif /* COMP_CFG_COMPUTE_H_ */
