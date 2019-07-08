/*
 * cfg_compute_ooc.cpp
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#include "cfg_compute_ooc.h"


void CFGCompute_ooc::do_worklist_ooc(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives){
	Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

    //initiate concurrent worklist
    std::vector<CFGNode*> nodes = cfg->getNodes();
//    std::vector<CFGNode*> nodes = cfg->getEntryNodes();

//    //for debugging
//    StaticPrinter::print_vector(nodes);

    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
        worklist_1->push_atomic(*it);
    }

    //initiate a temp graphstore to maintain all the updated graphs
    GraphStore* tmp_graphstore = new NaiveGraphStore();

    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
    while(!worklist_1->isEmpty()){
        //for debugging
        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

        std::vector<std::thread> comp_threads;
        for (unsigned int i = 0; i < NUM_THREADS_CFGCOMPUTE; i++)
            comp_threads.push_back(std::thread( [=] {compute_ooc(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphstore, singletons, actives);}));

        for (auto &t : comp_threads)
            t.join();

        //synchronize and communicate
//        update_GraphStore(graphstore, tmp_graphstore);
        graphstore->update_graphs(tmp_graphstore);
        tmp_graphstore->clear();

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
    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
}



void CFGCompute_ooc::compute_ooc(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives){
    CFGNode* cfg_node;
	while(worklist_1->pop_atomic(cfg_node)){
//    	//for debugging
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
        PEGraph* in = CFGCompute::combine_synchronous(graphstore, preds);

        //for debugging
        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString() + "\n", LEVEL_LOG_PEG);

        //transfer
        PEGraph* out = CFGCompute::transfer(in, cfg_node->getStmt(), grammar, singletons);

        //for debugging
        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString() + "\n", LEVEL_LOG_PEG);

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
        PEGraph* old_out = graphstore->retrieve(out_pointer);
        bool isEqual = out->equals(old_out);
        //for debugging
        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);
        if(!isEqual){
            //propagate
            std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
            for(auto it = successors.cbegin(); it != successors.cend(); ++it){
                worklist_2->push_atomic(*it);
            }

            //store the new graph into tmp_graphstore
            tmp_graphstore->addOneGraph(out_pointer, out);
        }
        else{
			delete out;
        }

        //clean out
        delete old_out;

        //for debugging
//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

        //for debugging
        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
    }
}




