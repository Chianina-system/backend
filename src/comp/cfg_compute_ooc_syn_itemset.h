/*
 * cfg_compute_ooc_syn_itemset.h
 *
 *  Created on: Oct 8, 2019
 *      Author: dell
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_ITEMSET_H_
#define COMP_CFG_COMPUTE_OOC_SYN_ITEMSET_H_

#include "cfg_compute_ooc_syn.h"

using namespace std;

class CFGCompute_ooc_syn_itemset {
public:

	static long do_worklist_ooc_synchronous(CFG* cfg_, ItemsetGraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
			Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
	    std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();
//	    std::vector<CFGNode*> nodes = cfg->getNodes();

	//    //for debugging
	//    StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist_1->push_atomic(*it);
	    }

	    //initiate a temp graphstore to maintain all the updated graphs
//	    NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();
	    HybridGraphStore* tmp_graphs = new HybridGraphStore();

	    long supersteps = 0;

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        supersteps++;

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->start();

	        std::vector<std::thread> comp_threads;
	        for (unsigned int i = 0; i < NUM_THREADS; i++)
	            comp_threads.push_back(std::thread( [=] {compute_ooc(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphs, singletons, actives, flag, timer);}));

	        for (auto &t : comp_threads)
	            t.join();

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->end();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->start();

	        //synchronize and communicate
//	        graphstore->update_graphs(tmp_graphstore, update_mode);
//	        tmp_graphstore->clear();
	        graphstore->update_graphs_hybrid(tmp_graphs, update_mode);
	        //clear tmp_graphs
	        tmp_graphs->clear();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->end();

	        //update worklists
	        assert(worklist_1->isEmpty());
	        Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
	        worklist_1 = worklist_2;
	        worklist_2 = worklist_tmp;
	        assert(worklist_2->isEmpty());

//	        //start compressing via frequent itemsets
//	        startItemsetGraphs(supersteps, worklist_1, graphstore, cfg);

	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- finished ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);
	    }

	    //clean
	    delete(worklist_1);
	    delete(worklist_2);

	    delete(tmp_graphs);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
//	    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	    dynamic_cast<NaiveGraphStore*>(graphstore)->printOutInfo();
	    return supersteps;
	}

	static void startItemsetGraphs(int supersteps, Concurrent_Worklist<CFGNode*>* worklist, ItemsetGraphStore* graphstore, CFG* cfg){
		if(supersteps > 0){
			Concurrent_Workset<CFGNode*>* workset = dynamic_cast<Concurrent_Workset<CFGNode*>*>(worklist);
			for(auto it = workset->getSet().begin(); it != workset->getSet().end(); ++it){
				CFGNode* cfg_node = *it;
				std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
				if(!preds){
					continue;
				}
				for(auto p = preds->begin(); p != preds->end(); ++p){
					CFGNode* pred_node = *p;

					PEGraph_Pointer pred_pointer = pred_node->getOutPointer();
					ItemsetGraph* pred_graph = graphstore->retrieve_direct(pred_pointer);

					if(pred_graph && !pred_graph->isEmpty()){
						graphstore->compressItemsetGraph(pred_graph);
					}
				}

			}
		}
	}


	static void compute_ooc(CFG_map_outcore* cfg, ItemsetGraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, HybridGraphStore* tmp_graphs, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

	    CFGNode* cfg_node;
		while(worklist_1->pop_atomic(cfg_node)){
//	//    	//for debugging
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
		        	CFGCompute_ooc_syn::propagate(cfg_node, cfg, out, grammar, worklist_2, actives);

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
					CFGCompute_ooc_syn::propagate(cfg_node, cfg, out, grammar, worklist_2, actives);

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

//	        //for debugging
//	//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);
//
//	        //for debugging
//	        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }
	}

//	static PEGraph* combine_synchronous(ItemsetGraphStore* graphstore, std::vector<CFGNode*>* preds){
//		//for debugging
//		Logger::print_thread_info_locked("combine starting...\n", LEVEL_LOG_FUNCTION);
//
//		PEGraph* out;
//
//	    if(!preds || preds->size() == 0){//entry node
//	        //return an empty graph
//	        out = new PEGraph();
//	    }
//	    else if(preds->size() == 1){
//	        CFGNode* pred = preds->at(0);
//	        PEGraph_Pointer out_pointer = pred->getOutPointer();
//	        out = graphstore->retrieve_update(out_pointer);
//	        if(!out){
//	        	out = new PEGraph();
//	        }
//	    }
//	    else{
//	        out = new PEGraph();
//
//	        for(auto it = preds->cbegin(); it != preds->cend(); it++){
//	            CFGNode* pred = *it;
//	            PEGraph_Pointer out_pointer = pred->getOutPointer();
//	            PEGraph* out_graph = graphstore->retrieve_update(out_pointer);
//	            if(!out_graph){
//	            	continue;
//	            }
//	           	out->merge(out_graph);
//	           	delete out_graph;
//	        }
//	    }
//
//		//for debugging
//		Logger::print_thread_info_locked("combine finished.\n", LEVEL_LOG_FUNCTION);
//
//		return out;
//	}


};



#endif /* COMP_CFG_COMPUTE_OOC_SYN_ITEMSET_H_ */
