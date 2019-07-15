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

	static void do_worklist_asynchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons) {
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map* cfg = dynamic_cast<CFG_map*>(cfg_);
//	    std::vector<CFGNode*> nodes = cfg->getNodes();
	    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

	//    //for debugging
	//    StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist->push_atomic(*it);
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {compute_asynchronous(cfg, graphstore, worklist, grammar, singletons);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
	    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	}


	static void compute_asynchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist, Grammar* grammar, Singletons* singletons){
		CFGNode* cfg_node;
	    while(worklist->pop_atomic(cfg_node)){
	    	//for debugging
	//    	cout << "\nCFG Node under processing: " << *cfg_node << endl;
	    	Logger::print_thread_info_locked("----------------------- CFG Node "
	    			+ to_string(cfg_node->getCfgNodeId())
					+ " {" + cfg_node->getStmt()->toString()
					+ "} start processing -----------------------\n", LEVEL_LOG_CFGNODE);
	    	Logger::print_thread_info_locked(to_string((long)graphstore) + "\n", LEVEL_LOG_CFGNODE);

	        //merge
	    	std::vector<CFGNode*> preds = cfg->getPredesessors(cfg_node);
	//        //for debugging
	//    	StaticPrinter::print_vector(preds);
	        PEGraph* in = combine_asynchronous(graphstore, preds);

	        //for debugging
	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString() + "\n", LEVEL_LOG_PEG);

	        //transfer
	        PEGraph* out = CFGCompute::transfer(in, cfg_node->getStmt(), grammar, singletons);

	        //for debugging
	        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString() + "\n", LEVEL_LOG_PEG);

	        //update and propagate
	        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
	        PEGraph* old_out = graphstore->retrieve_locked(out_pointer);
	        bool isEqual = out->equals(old_out);
	        //for debugging
	        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);
	        if(!isEqual){
	            //update out
	            graphstore->update_locked(out_pointer, out);

	            //propagate
	            std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
	            for(auto it = successors.cbegin(); it != successors.cend(); ++it){
	                worklist->push_atomic(*it);
	            }
	        }

	        //clean out
	        delete old_out;
	        delete out;

	        //for debugging
	        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

	        //for debugging
	        Logger::print_thread_info_locked("1-> " + worklist->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }
	//    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	}

	static PEGraph* combine_asynchronous(GraphStore* graphstore, std::vector<CFGNode*>& preds){
		//for debugging
		Logger::print_thread_info_locked("combine starting...\n", LEVEL_LOG_FUNCTION);

		PEGraph* out;

	    if(preds.size() == 0){//entry node
	        //return an empty graph
	        out = new PEGraph();
	    }
	    else if(preds.size() == 1){
	        CFGNode* pred = preds[0];
	        PEGraph_Pointer out_pointer = pred->getOutPointer();
	        out = graphstore->retrieve_locked(out_pointer);
	    }
	    else{
	        out = new PEGraph();

	        for(auto it = preds.cbegin(); it != preds.cend(); it++){
	            CFGNode* pred = *it;
	            PEGraph_Pointer out_pointer = pred->getOutPointer();
	            PEGraph* out_graph = graphstore->retrieve_locked(out_pointer);
	            out->merge(out_graph);
	            delete out_graph;
	        }
	    }

		//for debugging
		Logger::print_thread_info_locked("combine finished.\n", LEVEL_LOG_FUNCTION);

		return out;
	}

};




#endif /* COMP_CFG_COMPUTE_ASYN_H_ */
