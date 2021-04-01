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
	    NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();
	    long supersteps = 0;

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        supersteps++;

            cout << endl << "worklist size: " << worklist_1->size() << endl;

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
            predModel->HitCount_Icache++;
        }
        else if (hit_Icache == 0) {
            predModel->MissCount_Icache++;
            //missNum_Icache++;
        }
        else {
            printf("error IR, invalid visit cache !!!!\n\n");
        }
    }

    static bool IsFEdge(CFG *pCfg, CFGNode *pNode, CFGNode *cfg_node) {
        std::vector<suc*>* tmp = pCfg->getSuccessors(pNode);
        std::string edgeType = "";
        for(auto iter : *tmp)
        {
            if (iter->node->getCfgNodeId() == cfg_node->getCfgNodeId()) {
                edgeType = iter->type;
                break;
            }
        }
        if (edgeType == "") {
            cout << "edge type error";
            exit(EXIT_FAILURE);
        }
        if (edgeType == "FEdge") {
            return true;
        } else {
            return false;
        }
    }
    static bool IsBackEdge(CFG *pCfg, CFGNode *pNode, CFGNode *cfg_node) {
        std::vector<suc*>* tmp = pCfg->getSuccessors(pNode);
        std::string edgeType = "";
        for(auto iter : *tmp)
        {
            if (iter->node->getCfgNodeId() == cfg_node->getCfgNodeId()) {
                edgeType = iter->type;
                break;
            }
        }
        if (edgeType == "") {
            cout << "edge type error";
            exit(EXIT_FAILURE);
        }
        if (edgeType == "BackEdge") {
            return true;
        } else {
            return false;
        }
    }

    static void propagate(CFG_map_outcore* cfg, CFGNode* cfg_node, vector<suc* > *pSucs, Concurrent_Worklist<CFGNode *> *pWorklist, Concurrent_Worklist<CFGNode*> *actives) {
        //cout << "successor number: " << pSucs->size() << endl;
        //mtx.lock();
        if (pSucs) {
            for (auto it = pSucs->cbegin(); it != pSucs->cend(); ++it) {
                if (IsBackEdge(cfg, cfg_node, (*it)->node)) {
                    (*it)->num ++;
                    if ((*it)->num <= bound) {
                        //if (it->first->getOutPointer() != cfg_node->getOutPointer()) {
                        if ((*it)->node->getOutPointer() != cfg_node->getOutPointer()) {
                            if (!cfg->isMirror((*it)->node)) {
                                pWorklist->push_atomic((*it)->node);
							}
							else{
								actives->push_atomic((*it)->node);
							}

                            //pWorklist->push_atomic((*it)->node);
                        }
                    }
                }
                else if (IsFEdge(cfg, cfg_node, (*it)->node)) {
                    (*it)->num ++;
                    if ((*it)->num <= bound/2) {
                        //if (it->first->getOutPointer() != cfg_node->getOutPointer()) {
                        if ((*it)->node->getOutPointer() != cfg_node->getOutPointer()) {
                            if (!cfg->isMirror((*it)->node)) {
                                pWorklist->push_atomic((*it)->node);
                            }
                            else{
                                actives->push_atomic((*it)->node);
                            }
                            //pWorklist->push_atomic((*it)->node);
                        }
                    }
                }
                else {
                    //cout << "is not b" << cfg_node->getOutPointer() << " -> " << it->first->getOutPointer() << endl;
                    if ((*it)->node->getOutPointer() != cfg_node->getOutPointer()) {
                        //if (it->first->getOutPointer() != cfg_node->getOutPointer()) {
                        //pWorklist->push_atomic(it->first);
                        if (!cfg->isMirror((*it)->node)) {
                            pWorklist->push_atomic((*it)->node);
                        }
                        else{
                            actives->push_atomic((*it)->node);
                        }
                        //pWorklist->push_atomic((*it)->node);
                    }
                }


                //pWorklist->push_atomic(*it);

//            if (it->first->getOutPointer() != cfg_node->getOutPointer())
//            {
//                cout << cfg_node->getOutPointer() << " -> " << it->first->getOutPointer() << endl;
//                pWorklist->push_atomic(it->first);
//            }
                // cout << cfg_node->getOutPointer() << " -> " << (*it)->node->getOutPointer() << " " << (*it)->num << endl;
            }
        }
//    cout << "backLoopCount.size(): " << backLoopCount->size() << endl;
//    for (auto i : *backLoopCount) {
//        cout << i.first.first << " -> " << i.first.second << "num: " << i.second << endl;
//    }
        //   mtx.unlock();
    }


    static void ScanBlockIR(int bid, CFGNode *bb, cachestate* preModel) {
        //cout << "   ...scan IR list....    " << bid << endl;
        vector<int> insts = bb->getAllContent();
        for (int t = 0; t < insts.size(); t++) {
            //this->cacheChanged = false;
            //cout << "\n\n_______    _________\n\n";
            visitInstruction(insts[t], bid, preModel);  //访问当前block中的IR, 计算cache的miss/hit数
            //cout << "\n\n____________________\n\n";
        }
        //cout << " ....exit IRs...." << endl;
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
//		    cout << "node id: " << cfg_node->getCfgNodeId() << endl;
//            for ()
//		    cfg_node->getAllContent()
//	    	//for debugging
//	    	Logger::print_thread_info_locked("----------------------- CFG Node "
//	    			+ to_string(cfg_node->getCfgNodeId())
//					+ " " + cfg_node->getStmt()->toString()
//					+ " start processing -----------------------\n", LEVEL_LOG_CFGNODE);

			//for tuning
			diff_merge.start();

            cachestate* predModel = nullptr;

	        //merge
	    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);

            int num = 0;
            if (preds != nullptr) {
                num = preds->size();
            }
            else predModel = new cachestate(64, 512, 4);

            bool allIsNull = true;
            for (int p = 0; p < num; p++) {
                CFGNode* it = preds->at(p);
                if (graphstore->retrieve_shallow(it->getCfgNodeId()) != nullptr) {
                    allIsNull = false;
                    break;
                }
            }
            if (allIsNull) predModel = new cachestate(64, 512, 4);


            for (int p = 0; p < num; p++) {
                CFGNode* it = preds->at(p);

                //if (this->cacheTrace.find(it.getID()) == this->cacheTrace.end()) {//pred不在WL中
                //case1: it是当前block的前继，在以往的WL扫描中未访问过
                if (graphstore->getMap().find(it->getCfgNodeId()) == graphstore->getMap().end()) {//pred不在graphstore中
                    continue;
                }
                else {
                    //predModel->merge(dynamic_cast<State*>(graphstore->retrieve(it->getCfgNodeId()))); //前继合并后的model不为空，可直接merge(直接与空merge会出错)
                    //pair<int , int> p1(it->getOutPointer(), cfg_node->getOutPointer());
                    //if (IsBackEdge(cfg, it, cfg_node) && backLoopCount[p1] >= 5)
                    //    continue;
                    //else{
                    if (predModel != nullptr)
                        //predModel->merge(this->cacheTrace[it.getID()]); //合并
                        predModel->merge(dynamic_cast<cachestate*>(graphstore->retrieve(it->getCfgNodeId()))); //前继合并后的model不为空，可直接merge(直接与空merge会出错)
                    else
                        //predModel = this->cacheTrace[it.getID()]->fork(); //深拷贝
                        predModel = dynamic_cast<cachestate*>(graphstore->retrieve_shallow(it->getCfgNodeId()))->fork(); //深拷贝
                    // }
                }
            }

            ScanBlockIR(cfg_node->getOutPointer(), cfg_node, predModel);

            cachestate* old = graphstore->retrieve_shallow(cfg_node->getCfgNodeId());
            int out_pointer = cfg_node->getOutPointer();

            if (!old) {
                //cout << "predModel: " << predModel << endl;
                predModel->count = 1;
                tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
                //cout << "cfg_node " << *cfg_node << endl;
                //cout << "succssor number: " << cfg->getSuccessors(cfg_node) << endl;
                if (predModel->count < node_bound)  propagate(cfg, cfg_node, cfg->getSuccessors(cfg_node), worklist_2, actives);
                //cout << "worklist_2 size:" << worklist_2->size() << endl;
            } else {
                if (predModel->CacheConsistent(graphstore->retrieve_shallow(out_pointer))) {
                    predModel->count = old->count + 1;
                    tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
//                if (predModel->MissCount_Icache == graphstore->retrieve_shallow(out_pointer)->MissCount_Icache)
//                {
//                    tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
//                }
//                else
//                {
//                    unsigned miss1 = predModel->MissCount_Icache;
//                    unsigned miss2 = graphstore->retrieve_shallow(out_pointer)->MissCount_Icache;
//                    graphstore->retrieve_shallow(out_pointer)->MissCount_Icache = miss1 > miss2 ? miss1 : miss2;
//                    tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
//                    propagate(cfg, cfg_node, cfg->getSuccessors(cfg_node), worklist_2);
//                }
                    //tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
                    // cout << cfg_node->getOutPointer() << endl;

                    //     skipTransfer = true;
                } else {
                    //cout << cfg_node->getOutPointer() << " -> " << endl;
                    predModel->count = old->count + 1;
                    tmp_graphstore->addOneGraph_atomic(out_pointer, predModel);
                    if (predModel->count < node_bound)  propagate(cfg, cfg_node, cfg->getSuccessors(cfg_node), worklist_2, actives);
                }
            }



	//        //for debugging
	//    	StaticPrinter::print_vector(preds);
	        //PEGraph* in = CFGCompute_syn::combine_synchronous(graphstore, preds, cfg_node, grammar);

	        //for tuning
	        diff_merge.end();
	        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_transfer.start();

	        //transfer
	        //PEGraph* out = CFGCompute_syn::transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

	        //for tuning
	        diff_transfer.end();
	        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_propagate.start();

	        //update and propagate
//	        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
//	        PEGraph* old_out = nullptr;//graphstore->retrieve_shallow(out_pointer);
//	        bool isEqual = out->equals(old_out);

	        //for debugging
//	        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", 1);

//	        if(!isEqual){
//	            //propagate
//	        	//CFGCompute_ooc_syn::propagate(cfg_node, cfg, out, grammar, worklist_2, actives);
//
//	            //store the new graph into tmp_graphstore
////	            tmp_graphstore->addOneGraph_atomic(out_pointer, out);
//	            dynamic_cast<NaiveGraphStore*>(tmp_graphstore)->addOneGraph_atomic(out_pointer, out);
//	        }
//	        else{
//				delete out;
//	        }

//	        //clean out
//	        if(old_out){
//	        	delete old_out;
//	        }

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


#endif /* COMP_CFG_COMPUTE_OOC_SYN_NAIVE_H_ */
