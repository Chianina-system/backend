/*
 * cfg_compute_syn_naive.h
 *
 *  Created on: Oct 6, 2019
 *      Author: dell
 */

#ifndef CFG_COMPUTE_SYN_CACHE_NAIVE_H_
#define CFG_COMPUTE_SYN_CACHE_NAIVE_H_

#include "cfg_compute_syn_cache.h"


using namespace std;

class CFGCompute_syn_cache_naive {

public:
	static void do_worklist_synchronous(CFG* cfg_, NaiveGraphStore_cache* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode){
		//for performance tuning
		Timer_sum sum_compute("compute-synchronous");
		Timer_sum sum_update("update-graphs");
		Timer_wrapper_inmemory* timer = new Timer_wrapper_inmemory();

		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map* cfg = dynamic_cast<CFG_map*>(cfg_);
	   //std::vector<CFGNode*> nodes = cfg->getNodes();
	    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

	    //for debugging
	    //StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist_1->push_atomic(*it);
	    }

	    //initiate a temp graphstore to maintain all the updated graphs
	    NaiveGraphStore_cache* tmp_graphstore = new NaiveGraphStore_cache();

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        //for tuning
	        sum_compute.start();

	        std::vector<std::thread> comp_threads;
	        for (unsigned int i = 0; i < NUM_THREADS; i++)
	            comp_threads.push_back(std::thread( [=] {compute_synchronous(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphstore, singletons, flag,
						timer);}));

	        for (auto &t : comp_threads)
	            t.join();

	        //for tuning
	        sum_compute.end();

	        //for tuning
	        sum_update.start();

	        //synchronize and communicate
	        graphstore->update_graphs_shallow(tmp_graphstore, update_mode);
	        tmp_graphstore->clear_shallow();

	        //for tuning
	        sum_update.end();

	        //update worklists
	        assert(worklist_1->isEmpty());
	        Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
	        worklist_1 = worklist_2;
	        worklist_2 = worklist_tmp;
	        assert(worklist_2->isEmpty());

	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- finished ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);
	    }

	    //clean
	    delete(worklist_1);
	    delete(worklist_2);

	    delete(tmp_graphstore);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
	    //Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	    //graphstore->printOutInfo();

	    //for tuning
	    sum_compute.print();

	    timer->print();
	    delete timer;

	    sum_update.print();
	}


	static void compute_synchronous(CFG* cfg, NaiveGraphStore_cache* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
			Grammar* grammar, GraphStore_cache* tmp_graphstore, Singletons* singletons, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

		//for debugging
		Logger::print_thread_info_locked("compute-synchronous starting...\n", LEVEL_LOG_FUNCTION);

	    CFGNode* cfg_node;
		while(worklist_1->pop_atomic(cfg_node)){

			//for tuning
			diff_merge.start();

			//merge
	    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
	        //PEGraph* in = CFGCompute_syn::combine_synchronous(Graphstore_cache, preds, cfg_node, grammar);

	        //for tuning
	        diff_merge.end();
	        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

	        //for debugging
	        //Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_transfer.start();

	        //transfer
	        PEGraph* out;// = CFGCompute_syn::transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

	        //for tuning
	        diff_transfer.end();
	        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

	        //for debugging
	        //Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_propagate.start();

	        //update and propagate
	        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
	        PEGraph* old_out = nullptr;//graphstore->retrieve_shallow(out_pointer);
	        bool isEqual = out->equals(old_out);


	        //for debugging
	        // Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);

	        if(!isEqual){
	            //propagate
	        	CFGCompute_syn_cache::propagate(cfg_node, cfg, out, grammar, worklist_2);

	            //store the new graph into tmp_graphstore
	            //dynamic_cast<NaiveGraphStore*>(tmp_graphstore)->addOneGraph_atomic(out_pointer, out);
	        }
	        else{
				delete out;
	        }


	        //for tuning
	        diff_propagate.end();
	        timer->getPropagateSum()->add_locked(diff_propagate.getClockDiff(), diff_propagate.getTimeDiff());

	    }

		//for debugging
		Logger::print_thread_info_locked("compute-synchronous finished.\n", LEVEL_LOG_FUNCTION);
	}



};



#endif /* CFG_COMPUTE_SYN_CACHE_NAIVE_H_ */

