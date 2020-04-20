/*
 * cfg_compute_syn_itemset.h
 *
 *  Created on: Oct 6, 2019
 *      Author: dell
 */

#ifndef CFG_COMPUTE_SYN_ITEMSET_H_
#define CFG_COMPUTE_SYN_ITEMSET_H_

#include "cfg_compute_syn.h"

#define DIFF_MODE 1

using namespace std;

class CFGCompute_syn_itemset {

public:
	static void do_worklist_synchronous(CFG* cfg_, ItemsetGraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode){
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
//	    NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();
	    HybridGraphStore* tmp_graphs = new HybridGraphStore();

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        //for tuning
	        sum_compute.start();

	        std::vector<std::thread> comp_threads;
	        for (unsigned int i = 0; i < NUM_THREADS; i++)
	            comp_threads.push_back(std::thread( [=] {compute_synchronous(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphs, singletons, flag,
						timer);}));

	        for (auto &t : comp_threads)
	            t.join();

	        //for tuning
	        sum_compute.end();

	        //for tuning
	        sum_update.start();

//	        //for debugging
//	        tmp_graphs->printOutInfo();

	        //synchronize and communicate
	        graphstore->update_graphs_hybrid(tmp_graphs, update_mode);
	        //clear tmp_graphs
	        tmp_graphs->clear();


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

	    delete(tmp_graphs);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
	//    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	    graphstore->printOutInfo();

	    //for tuning
	    sum_compute.print();

	    timer->print();
	    delete timer;

	    sum_update.print();
	}


	static void compute_synchronous(CFG* cfg, ItemsetGraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
			Grammar* grammar, HybridGraphStore* tmp_graphs, Singletons* singletons, bool flag,
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
	        PEGraph* in = CFGCompute_syn::combine_synchronous(graphstore, preds);

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
	        if(DIFF_MODE){
				PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
				PEGraph* old_out = graphstore->retrieve(out_pointer);
				bool isEqual = out->equals(old_out);

//		        //for debugging
//		        if(old_out){
//					Logger::print_thread_info_locked("The old-out-PEG:\n" + old_out->toString(grammar) + "\n", LEVEL_LOG_PEG);
//		        }
//		        else{
//		        	Logger::print_thread_info_locked("The old-out-PEG:\n null \n", LEVEL_LOG_PEG);
//		        }

//		        //for debugging
//		        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", 1);

				if(!isEqual){
					ItemsetGraph* old_out_itemset = graphstore->retrieve_direct(out_pointer);

					HybridGraph* out_hybrid;
					if(old_out_itemset){
						out_hybrid = graphstore->constructHybridGraph(out, old_out, old_out_itemset);
					}
					else{
						out_hybrid = graphstore->convertToHybridGraph(out);
					}

		            //propagate
		        	if(cfg_node->getStmt()->getType() == TYPE::Callfptr){
		        		//to deal with function pointer callsite
			            std::vector<CFGNode*>* successors = cfg->getSuccessors(cfg_node);
			            if(successors){
							for(auto it = successors->cbegin(); it != successors->cend(); ++it){
								CFGNode* suc = *it;
								if(CFGCompute_syn::isFeasible(suc->getStmt(), cfg_node->getStmt(), out)){
									worklist_2->push_atomic(*it);
								}
							}
			            }
		        	}
		        	else{
						//propagate
						std::vector<CFGNode*>* successors = cfg->getSuccessors(cfg_node);
						if(successors){
							for(auto it = successors->cbegin(); it != successors->cend(); ++it){
								worklist_2->push_atomic(*it);
							}
						}
		        	}

					//store the new graph into tmp_graphstore
					tmp_graphs->addOneGraph_atomic(out_pointer, out_hybrid);
				}

				//clean out
				delete out;
				if(old_out){
					delete old_out;
				}
	        }
	        else{
				HybridGraph* out_hybrid = graphstore->convertToHybridGraph(out);
				PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
				ItemsetGraph* old_out = graphstore->retrieve_direct(out_pointer);
				bool isEqual = out_hybrid->equals(old_out);

//		        //for debugging
//		        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", 1);

				if(!isEqual){
		            //propagate
		        	if(cfg_node->getStmt()->getType() == TYPE::Callfptr){
		        		//to deal with function pointer callsite
			            std::vector<CFGNode*>* successors = cfg->getSuccessors(cfg_node);
			            if(successors){
							for(auto it = successors->cbegin(); it != successors->cend(); ++it){
								CFGNode* suc = *it;
								if(CFGCompute_syn::isFeasible(suc->getStmt(), cfg_node->getStmt(), out)){
									worklist_2->push_atomic(*it);
								}
							}
			            }
		        	}
		        	else{
						//propagate
						std::vector<CFGNode*>* successors = cfg->getSuccessors(cfg_node);
						if(successors){
							for(auto it = successors->cbegin(); it != successors->cend(); ++it){
								worklist_2->push_atomic(*it);
							}
						}
		        	}

					//store the new graph into tmp_graphstore
					tmp_graphs->addOneGraph_atomic(out_pointer, out_hybrid);
				}
				else{
					delete out_hybrid;
				}

				//clean out
				delete out;
	        }

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



#endif /* CFG_COMPUTE_SYN_ITEMSET_H_ */
