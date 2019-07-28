/*
 * cfg_compute_ooc_asyn.h
 *
 *  Created on: Jul 15, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_ASYN_H_
#define COMP_CFG_COMPUTE_OOC_ASYN_H_

#include "cfg_compute_asyn.h"
#include "cfg_compute_ooc_syn.h"

class CFGCompute_ooc_asyn {

public:

	static void do_worklist_ooc_asynchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag) {
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
	    std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();
//	    std::vector<CFGNode*> nodes = cfg->getNodes();
	//    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

	//    //for debugging
	//    StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist->push_atomic(*it);
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {compute_ooc_asynchronous(cfg, graphstore, worklist, grammar, singletons, actives, flag);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);

	    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
//	    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	    dynamic_cast<NaiveGraphStore*>(graphstore)->printOutInfo();
	}


private:
	static void compute_ooc_asynchronous(CFG_map_outcore* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist, Grammar* grammar, Singletons* singletons,
			Concurrent_Worklist<CFGNode*>* actives, bool flag){
		CFGNode* cfg_node;
	    while(worklist->pop_atomic(cfg_node)){
//	    	//for debugging
//	    	Logger::print_thread_info_locked("----------------------- CFG Node "
//	    			+ to_string(cfg_node->getCfgNodeId())
//					+ " {" + cfg_node->getStmt()->toString()
//					+ "} start processing -----------------------\n", LEVEL_LOG_CFGNODE);

	        //merge
	    	std::vector<CFGNode*> preds = cfg->getPredesessors(cfg_node);
	//        //for debugging
	//    	StaticPrinter::print_vector(preds);
	        PEGraph* in = CFGCompute_asyn::combine_asynchronous(graphstore, preds);

//	        //for debugging
//	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString() + "\n", LEVEL_LOG_PEG);

	        //transfer
	        PEGraph* out = CFGCompute_syn::transfer(in, cfg_node->getStmt(), grammar, singletons, flag);

//	        //for debugging
//	        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString() + "\n", LEVEL_LOG_PEG);

	        //update and propagate
	        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
	        PEGraph* old_out = graphstore->retrieve_locked(out_pointer);
	        bool isEqual = out->equals(old_out);

//	        //for debugging
//	        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);

	        if(!isEqual){
	            //update out
	            graphstore->update_locked(out_pointer, out);

	            //propagate
	            std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
	            for(auto it = successors.cbegin(); it != successors.cend(); ++it){
//	                worklist->push_atomic(*it);
	                if(!cfg->isMirror(*it)){
	                	worklist->push_atomic(*it);
	                }
	//                else if(cfg->isInMirror(*it)){
	//                	worklist_2->push_atomic(*it);
	//                }
	                else{
	                	actives->push_atomic(*it);
	                }
	            }
	        }

	        //clean out
	        if(old_out){
				delete old_out;
	        }
	        delete out;

//	        //for debugging
//	        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);
//
//	        //for debugging
//	        Logger::print_thread_info_locked("1-> " + worklist->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }
	//    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
	}


};


#endif /* COMP_CFG_COMPUTE_OOC_ASYN_H_ */
