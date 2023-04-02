/*
 * cfg_compute_ooc_syn_naive.h
 *
 *  Created on: Oct 8, 2019
 *      Author: dell
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_NAIVE_H_
#define COMP_CFG_COMPUTE_OOC_SYN_NAIVE_H_

#include "cfg_compute_ooc_syn.h"

//using namespace std;

class CFGCompute_ooc_syn_naive {
public:

	static long do_worklist_ooc_synchronous(CFG* cfg_, NaiveGraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
			Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
		
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	  Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	  // initiate concurrent worklist
	  CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);

	  std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();
//	    auto tmp = &nodes;
//	    std::vector<CFGNode*> nodes = cfg->getNodes();


	  for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	    worklist_1->push_atomic(*it);
	  }

	  //initiate a temp graphstore to maintain all the updated graphs
	  NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();
	  long supersteps = 0;

	  Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	  while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        supersteps++;

          //cout << endl << "worklist size: " << worklist_1->size() << endl;

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

//            auto it = graphstore->getMap().begin();
//            while (it != graphstore->getMap().end()) {
//                //if (it->first == 90)
//                cout << "id: " << it->first << "    graphstore model miss: " << it->second->MissCount_Icache
//                << " node run count: " << it->second->count << endl;
//                it++;
//            }

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

	    return supersteps;
	}

  static void visitInstruction(int I, int bID, cachestate* predModel) {

    unsigned hit_Icache = predModel->AccessIC(I);  //只凭地址进行访问

    if (hit_Icache == 1) {
      // predModel->HitCount_Icache++;
    }
    else if (hit_Icache == 0) {
					
    }
		else {
      printf("error IR, invalid visit cache !!!!\n\n");
    }
  }

    static void propagate(CFG_map_outcore* cfg, CFGNode* cfg_node, std::vector<CFGNode*> *pSucs, Concurrent_Worklist<CFGNode *> *pWorklist, Concurrent_Worklist<CFGNode*> *actives) {
        if (pSucs) {
            for (auto it = pSucs->cbegin(); it != pSucs->cend(); ++it) {
                CFGNode *suc = *it;
                if (!cfg->isMirror(suc)) {
                    pWorklist->push_atomic(suc);
                }
                else{
                    actives->push_atomic(suc);
                }
            }
        }
    }


    static void ScanBlockIR(int bid, CFGNode *bb, cachestate* preModel) {
        vector<int> insts = bb->getAllContent();
        for (int t = 0; t < insts.size(); t++) {
            visitInstruction(insts[t], bid, preModel);
        }
    }

	static void compute_ooc(CFG_map_outcore* cfg, NaiveGraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

	  CFGNode* cfg_node;
		while(worklist_1->pop_atomic(cfg_node)){

			//for tuning
			diff_merge.start();

      cachestate* predModel = nullptr; // new in_state
      
			//merge
      std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
      if (preds != nullptr) {
        for (int p = 0; p < preds->size(); p++) {
          CFGNode* it = preds->at(p);
          if (graphstore->getMap().find(it->getCfgNodeId()) != graphstore->getMap().end()) { //pred不在graphstore中
            cachestate* tmp = graphstore->retrieve_shallow(it->getCfgNodeId());
						if (predModel != nullptr){
							predModel->merge(tmp); 
						}
            else{
							predModel = tmp->fork(); //深拷贝
						}
          }
        }
      }

      if (predModel == nullptr) predModel = new cachestate(); // special for entries

      ScanBlockIR(cfg_node->getOutPointer(), cfg_node, predModel);

      cachestate* old = graphstore->retrieve_shallow(cfg_node->getCfgNodeId());
      int out_pointer = cfg_node->getOutPointer();

      if (!old) { // can not find old in_state in graphstore
				if(predModel->size() != 0){ // can propagate
					tmp_graphstore->addOneGraph_atomic(out_pointer, predModel); // add new in_state
					propagate(cfg, cfg_node, cfg->getSuccessors(cfg_node), worklist_2, actives);
				}else{
					delete predModel;
				}
      } else {
				if (predModel->CacheConsistent(old)) {
          delete predModel;
        } else { // can propagate
					tmp_graphstore->addOneGraph_atomic(out_pointer, predModel); // add new in_state
					propagate(cfg, cfg_node, cfg->getSuccessors(cfg_node), worklist_2, actives);
        }
    }

			//for tuning
			diff_merge.end();
			timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());
			
			//for tuning
			diff_transfer.start();
			diff_transfer.end();
			timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

			diff_propagate.start();

			//for tuning
			diff_propagate.end();
	    timer->getPropagateSum()->add_locked(diff_propagate.getClockDiff(), diff_propagate.getTimeDiff());
	  }



	}


};


#endif /* COMP_CFG_COMPUTE_OOC_SYN_NAIVE_H_ */
