/*
 * cfg_compute.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */


#include "cfg_compute.h"


void CFGCompute::do_worklist(CFG* cfg, GraphStore* graphstore){
	Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Worklist<CFGNode*>();

	//initiate concurrent worklist
	std::vector<CFGNode*> nodes = cfg->getNodes();
	for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
		worklist_1->push(*it);
	}

	Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Worklist<CFGNode*>();
	while(!worklist_1->isEmpty()){
		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < num_threads; i++)
			comp_threads.push_back(std::thread( [=] {this->compute(cfg, graphstore, worklist_1, worklist_2);}));

		for (auto &t : comp_threads)
			t.join();

		assert(worklist_1->isEmpty());
		Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
		worklist_1 = worklist_2;
		worklist_2 = worklist_tmp;
		assert(worklist_2->isEmpty());
	}

	//clean
	delete(worklist_1);
	delete(worklist_2);

}


void CFGCompute::compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2){
	CFGNode* cfg_node;
	while(worklist_1->pop_atomic(cfg_node)){
		//merge
		PEGraph* in = combine(cfg_node);

		//transfer
		PEGraph* out = transfer(in, cfg_node->getStmt());

		//update and propagate
		PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
		PEGraph* old_out = graphstore->retrieve(out_pointer);
		if(!out->equals(old_out)){
			//update out
			graphstore->update(out_pointer, out);

			//propagate
			std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
			for(auto it = successors.cbegin(); it != successors.cend(); ++it){
				worklist_2->push_atomic(*it);
			}
		}
	}
}



PEGraph* CFGCompute::combine(CFGNode* node){

}


PEGraph* CFGCompute::transfer(PEGraph* in, Stmt* stmt){

}

