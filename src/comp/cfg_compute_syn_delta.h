/*
 * cfg_compute_syn_delta.h
 *
 *  Created on: Oct 20, 2019
 *      Author: zzq
 */

#ifndef CFG_COMPUTE_SYN_DELTA_H_
#define CFG_COMPUTE_SYN_DELTA_H_

#include "cfg_compute_syn.h"


using namespace std;

class CFGCompute_syn_delta {

public:
	static void do_worklist_synchronous(CFG* cfg_, DeltaGraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode){
		//for performance tuning
		Timer_sum sum_compute("compute-synchronous");
		Timer_sum sum_update("update-graphs");
		Timer_wrapper_inmemory* timer = new Timer_wrapper_inmemory();

		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map* cfg = dynamic_cast<CFG_map*>(cfg_);
	//    std::vector<CFGNode*> nodes = cfg->getNodes();
	    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

	//    //for debugging
	//    StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist_1->push_atomic(*it);
	    }

	    //initiate a temp graphstore to maintain all the updated graphs
	    DeltaGraphStore* tmp_graphstore = new DeltaGraphStore();

	    int supersteps = 0;

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        supersteps++;

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


	        //start deltaGraph processing
	        startDeltaGraphs(supersteps, worklist_1, graphstore, cfg);
//	        startDeltaGraphs(supersteps, nodes, graphstore);

	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- finished ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	//        std::set<CFGNode*>* list = dynamic_cast<Concurrent_Workset<CFGNode*>*>(worklist_1)->getSet();
	//        if(list->size() < 100){
	//			Logger::print_thread_info_locked(StaticPrinter::toString_set(*list), LEVEL_LOG_MAIN);
	//        }
	//        Logger::print_thread_info_locked("worklist size :" + to_string(list->size()) + "\n", LEVEL_LOG_MAIN);
	//        delete list;

//	        graphstore->printOutInfo();
	    }

	    //clean
	    delete(worklist_1);
	    delete(worklist_2);

	    delete(tmp_graphstore);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
	//    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	    graphstore->printOutInfo();
	    cout << "Number of supersteps: " << supersteps << endl;

	    //for tuning
	    sum_compute.print();

	    timer->print();
	    delete timer;

	    sum_update.print();
	}

	static void startDeltaGraphs(int supersteps, Concurrent_Worklist<CFGNode*>* worklist, DeltaGraphStore* graphstore, CFG* cfg){
		if(supersteps > 0){
			Concurrent_Workset<CFGNode*>* workset = dynamic_cast<Concurrent_Workset<CFGNode*>*>(worklist);
			for(auto it = workset->getSet().begin(); it != workset->getSet().end(); ++it){
				CFGNode* cfg_node = *it;
				std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
				for(auto p = preds->begin(); p != preds->end(); ++p){
					CFGNode* pred_node = *p;

					PEGraph_Pointer out_pointer = pred_node->getOutPointer();
					DeltaGraph* old_out = graphstore->retrieve_shallow(out_pointer);
					if(old_out){
						if(old_out->isNaive() && old_out->getBase()->getNumEdges() >= 50){
							old_out->setNaive(false);
							graphstore->getBasesSet().insert(old_out->getBase());
	//						return;
						}
					}
				}

			}
		}
	}

//	static void startDeltaGraphs(int supersteps, Concurrent_Worklist<CFGNode*>* worklist, DeltaGraphStore* graphstore){
//		if(supersteps > 0){
//			Concurrent_Workset<CFGNode*>* workset = dynamic_cast<Concurrent_Workset<CFGNode*>*>(worklist);
//			for(auto it = workset->getSet().begin(); it != workset->getSet().end(); ++it){
//				CFGNode* cfg_node = *it;
//				PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
//				DeltaGraph* old_out = graphstore->retrieve_shallow(out_pointer);
//				if(old_out){
//					if(old_out->isNaive() && old_out->getBase()->getNumEdges() >= 50){
//						old_out->setNaive(false);
//						graphstore->getBasesSet().insert(old_out->getBase());
//						return;
//					}
//				}
//			}
//		}
//	}

//	static void startDeltaGraphs(int supersteps, std::vector<CFGNode*>& nodes, DeltaGraphStore* graphstore){
//		if(supersteps > 8){
//			for(auto it = nodes.begin(); it != nodes.end(); ++it){
//				CFGNode* cfg_node = *it;
//				PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
//				DeltaGraph* old_out = graphstore->retrieve_shallow(out_pointer);
//				if(old_out){
//					if(old_out->isNaive() && old_out->getBase()->getNumEdges() >= 0){
//						old_out->setNaive(false);
//						graphstore->getBasesSet().insert(old_out->getBase());
//					}
//				}
//			}
//		}
//	}



	static void compute_synchronous(CFG* cfg, DeltaGraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
			Grammar* grammar, DeltaGraphStore* tmp_graphstore, Singletons* singletons, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

		//for debugging
		Logger::print_thread_info_locked("compute-synchronous starting...\n", LEVEL_LOG_FUNCTION);

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
	        PEGraph* in = CFGCompute_syn::combine_synchronous(graphstore, preds, cfg_node, grammar);

	        //for tuning
	        diff_merge.end();
	        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_transfer.start();

	        //transfer
	        PEGraph* out = CFGCompute_syn::transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

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
				CFGCompute_syn::propagate(cfg_node, cfg, out, grammar, worklist_2);

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

	        //for debugging
	//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	//        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

	//        //for debugging
	//        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }

		//for debugging
		Logger::print_thread_info_locked("compute-synchronous finished.\n", LEVEL_LOG_FUNCTION);
	}

};



#endif /* CFG_COMPUTE_SYN_DELTA_H_ */
