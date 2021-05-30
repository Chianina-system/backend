/*
 * cfg_compute_ooc_syn_naive.h
 *
 *  Created on: Oct 8, 2019
 *      Author: dell
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_ALIAS_DELTA_H_
#define COMP_CFG_COMPUTE_OOC_SYN_ALIAS_DELTA_H_

#include "cfg_compute_ooc_syn_alias.h"

using namespace std;

class CFGCompute_ooc_syn_alias_delta {
public:

	static void do_worklist_ooc_synchronous(CFG* cfg_, DeltaGraphStore_alias* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
			Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
	    std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();
//	    std::vector<CFGNode*> nodes = cfg->getNodes();

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist_1->push_atomic(*it);
	    }

	    //initiate a temp graphstore to maintain all the updated graphs
	    DeltaGraphStore_alias* tmp_graphstore = new DeltaGraphStore_alias();

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->start();

	        std::vector<std::thread> comp_threads;
	        for (unsigned int i = 0; i < NUM_THREADS; i++)
	            comp_threads.push_back(std::thread( [=] {compute_ooc(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphstore, singletons, actives, flag, timer);}));

	        for (auto &t : comp_threads)
	            t.join();

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->end();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->start();

	        //synchronize and communicate
//	        graphstore->update_graphs(tmp_graphstore, update_mode);
//	        tmp_graphstore->clear();
	        graphstore->update_graphs_shallow(tmp_graphstore, update_mode);
	        tmp_graphstore->clear_shallow();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->end();

	        //update worklists
	        assert(worklist_1->isEmpty());
	        Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
	        worklist_1 = worklist_2;
	        worklist_2 = worklist_tmp;
	        assert(worklist_2->isEmpty());

	        //start deltaGraph processing
	        CFGCompute_syn_alias_delta::startDeltaGraphs(1, worklist_1, graphstore, cfg);

	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- finished ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);
	    }

	    //clean
	    delete(worklist_1);
	    delete(worklist_2);

	    delete(tmp_graphstore);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
//	    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	    dynamic_cast<NaiveGraphStore*>(graphstore)->printOutInfo();
	}


	static void compute_ooc(CFG_map_outcore* cfg, DeltaGraphStore_alias* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, DeltaGraphStore_alias* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

	    CFGNode* cfg_node;
		while(worklist_1->pop_atomic(cfg_node)){
//	    	//for debugging
//	    	Logger::print_thread_info_locked("----------------------- CFG Node "
//	    			+ to_string(cfg_node->getCfgNodeId())
//					+ " " + cfg_node->getStmt()->toString()
//					+ " start processing -----------------------\n", LEVEL_LOG_CFGNODE);

			//for tuning
			diff_merge.start();

	        //merge
	    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
	//        //for debugging
	//    	StaticPrinter::print_vector(preds);
	        PEGraph* in = CFGCompute_syn_alias::combine_synchronous(graphstore, preds, cfg_node, grammar);

	        //for tuning
	        diff_merge.end();
	        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_transfer.start();

	        //transfer
	        PEGraph* out = CFGCompute_syn_alias::transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

	        //for tuning
	        diff_transfer.end();
	        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_propagate.start();

	        //update and propagate
			PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
			DeltaGraph* old_out = graphstore->retrieve_shallow(out_pointer);
			DeltaGraph* out_delta = graphstore->convertToDeltaGraph(out, old_out, preds);
			bool isEqual = out_delta->equals(old_out, out);

//		        //for debugging
//		        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", 1);

			if(!isEqual){
	            //propagate
				CFGCompute_ooc_syn_alias::propagate(cfg_node, cfg, out, grammar, worklist_2, actives);

				//store the new graph into tmp_graphstore
				tmp_graphstore->addOneGraph_atomic(out_pointer, out_delta);
			}
			else{
				delete out_delta;
			}

			//clean out
			delete out;

	        //for tuning
	        diff_propagate.end();
	        timer->getPropagateSum()->add_locked(diff_propagate.getClockDiff(), diff_propagate.getTimeDiff());

//	        //for debugging
//	//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);
//
//	        //for debugging
//	        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }
	}


};


#endif /* COMP_CFG_COMPUTE_OOC_SYN_ALIAS_NAIVE_H_ */


