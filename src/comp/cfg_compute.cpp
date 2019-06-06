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
		PEGraph* in = combine(cfg, graphstore, cfg_node);

		//transfer
		PEGraph* out = transfer(in, cfg_node->getStmt());

		//clean in
		delete in;

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

		//clean out
		delete out;
	}
}



PEGraph* CFGCompute::combine(const CFG* cfg, const GraphStore* graphstore, const CFGNode* node){
	//get the predecessors of node
	std::vector<CFGNode*> preds = cfg->getPredesessors(node);

	if(preds.size() == 0){//entry node
		//return an empty graph
		return new PEGraph();
	}
	else if(preds.size() == 1){
		CFGNode* pred = preds[0];
		PEGraph_Pointer out_pointer = pred->getOutPointer();
		PEGraph* out_graph = graphstore->retrieve(out_pointer);
		return out_graph;
	}
	else{

		for(auto it = preds.cbegin(); it != preds.cend(); it++){
			CFGNode* pred = *it;
			PEGraph_Pointer out_pointer = pred->getOutPointer();
			PEGraph* out_graph = graphstore->retrieve(out_pointer);
		}

	}


}


//PEGraph* CFGCompute::transfer(PEGraph* in, Stmt* stmt){
//
//}


PEGraph* CFGCompute::transfer_copy(PEGraph* in, Stmt* stmt){

}

PEGraph* CFGCompute::transfer_load(PEGraph* in, Stmt* stmt){

}

PEGraph* CFGCompute::transfer_store(PEGraph* in, Stmt* stmt){

}

PEGraph* CFGCompute::transfer_address(PEGraph* in, Stmt* stmt){

}

