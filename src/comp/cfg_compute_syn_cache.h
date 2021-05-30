/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_SYN_CACHE_H_
#define COMP_CFG_COMPUTE_SYN_CACHE_H_

//#include "../preproc/grammar.h"
//#include "../preproc/singletons.h"
//#include "cfg_map.h"
//#include "cfg_map_outcore.h"
//#include "computationset.h"
//#include "../utility/StaticPrinter.h"
//#include "../utility/Logger.hpp"
//#include "concurrent_worklist/concurrent_workqueue.h"
//#include "concurrent_worklist/concurrent_workset.h"
//#include "graphstore/graphstore.h"
//#include "graphstore/naive_graphstore.h"
//#include "graphstore/itemset_graphstore.h"
//#include "graphstore/delta_graphstore.h"
//#include "peg_compute.h"
//#include "peg_compute_parallel.h"
//#include "../utility/timer_sum.hpp"
//#include "../utility/timer_diff.hpp"
//#include "../utility/timer_sum_sync.hpp"
//#include "../utility/Timer_wrapper.h"
//#include "stmt/stmt_callfptr.h"

#include "cfg_compute_syn.h"

using namespace std;

class CFGCompute_syn_cache {

public:

    //static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode); //worklist algorithm in parallel

    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag,
    		Timer_wrapper_inmemory* timer = nullptr){
                return nullptr;
    }

    //static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds, CFGNode* cfg_node, Grammar* grammar);

	static void propagate(CFGNode *cfg_node, CFG *cfg, PEGraph *out, Grammar *grammar, Concurrent_Worklist<CFGNode*> *worklist_2) {
		//propagate
	}


    static void do_worklist_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode){
	//for performance tuning
	Timer_sum sum_compute("compute-synchronous");
	Timer_sum sum_update("update-graphs");
	Timer_wrapper_inmemory* timer = new Timer_wrapper_inmemory();

	Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

    //initiate concurrent worklist
    CFG_map_cache* cfg = dynamic_cast<CFG_map_cache*>(cfg_);
    //std::vector<CFGNode*> nodes = cfg->getNodes();
    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

    //for debugging
    //StaticPrinter::print_vector(nodes);

    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
        worklist_1->push_atomic(*it);
    }

    //initiate a temp graphstore to maintain all the updated graphs
    ///NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();
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
        //        graphstore->update_graphs(tmp_graphstore, update_mode);
        //        tmp_graphstore->clear();

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
    //    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
    //graphstore->printOutInfo();

    //for tuning
    sum_compute.print();

    timer->print();
    delete timer;

    sum_update.print();
}


	static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
		Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, bool flag,
		Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_merge;
	Timer_diff diff_transfer;
	Timer_diff diff_propagate;

	//for debugging
	Logger::print_thread_info_locked("compute-synchronous starting...\n", LEVEL_LOG_FUNCTION);

    CFGNode* cfg_node;
	while(worklist_1->pop_atomic(cfg_node)){
    //    	//for debugging


		//for tuning
		diff_merge.start();

		//merge
    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
        //PEGraph* in = CFGCompute_syn::combine_synchronous(graphstore, preds, cfg_node, grammar);
        PEGraph* in = nullptr;
        
        //for tuning
        diff_merge.end();
        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

        //for debugging
        //Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


        //for tuning
        diff_transfer.start();

        //transfer
        PEGraph* out = transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

        //for tuning
        diff_transfer.end();
        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

        //for debugging
        //Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


        //for tuning
        diff_propagate.start();

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
        PEGraph* old_out;// = graphstore->retrieve(out_pointer);
        bool isEqual = out->equals(old_out);

        
        if(!isEqual){
            //propagate
			propagate(cfg_node, cfg, out, grammar, worklist_2);

            //store the new graph into tmp_graphstore
           // dynamic_cast<NaiveGraphStore*>(tmp_graphstore)->addOneGraph_atomic(out_pointer, out);
        }
        else{
			delete out;
        }

        //clean out
        if(old_out){
        	delete old_out;
        }

        //for tuning
        diff_propagate.end();
        timer->getPropagateSum()->add_locked(diff_propagate.getClockDiff(), diff_propagate.getTimeDiff());

        //for debugging
      //  Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
      //  Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

        //for debugging
        //Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
    }

	//for debugging
	Logger::print_thread_info_locked("compute-synchronous finished.\n", LEVEL_LOG_FUNCTION);
}


private:
    //static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
    //		Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, bool flag,
	//		Timer_wrapper_inmemory* timer);

};


#endif /* COMP_CFG_COMPUTE_SYN_CACHE_H_ */



