/*
 * cfg_compute.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */


#include "cfg_compute_syn.h"


void CFGCompute_syn::do_worklist_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode){
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
    NaiveGraphStore* tmp_graphstore = new NaiveGraphStore();

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
        graphstore->update_graphs(tmp_graphstore, update_mode);
        tmp_graphstore->clear();

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

//        graphstore->printOutInfo();
    }

    //clean
    delete(worklist_1);
    delete(worklist_2);

    delete(tmp_graphstore);

    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
//    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
    graphstore->printOutInfo();

    //for tuning
    sum_compute.print();

    timer->print();
    delete timer;

    sum_update.print();
}


void CFGCompute_syn::compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
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
//    	Logger::print_thread_info_locked("----------------------- CFG Node "
//    			+ to_string(cfg_node->getCfgNodeId())
//				+ " " + cfg_node->getStmt()->toString()
//				+ " start processing -----------------------\n", LEVEL_LOG_CFGNODE);

		//for tuning
		diff_merge.start();

		//merge
    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
        PEGraph* in = combine_synchronous(graphstore, preds, cfg_node, grammar);

        //for tuning
        diff_merge.end();
        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

//        //for debugging
//        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


        //for tuning
        diff_transfer.start();

        //transfer
        PEGraph* out = transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

        //for tuning
        diff_transfer.end();
        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

//        //for debugging
//        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


        //for tuning
        diff_propagate.start();

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
        PEGraph* old_out = graphstore->retrieve(out_pointer);
        bool isEqual = out->equals(old_out);

//        //for debugging
//        if(old_out){
//			Logger::print_thread_info_locked("The old-out-PEG:\n" + old_out->toString(grammar) + "\n", LEVEL_LOG_PEG);
//        }
//        else{
//        	Logger::print_thread_info_locked("The old-out-PEG:\n null \n", LEVEL_LOG_PEG);
//        }

//        //for debugging
//        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);

        if(!isEqual){
            //propagate
			propagate(cfg_node, cfg, out, grammar, worklist_2);

            //store the new graph into tmp_graphstore
            dynamic_cast<NaiveGraphStore*>(tmp_graphstore)->addOneGraph_atomic(out_pointer, out);
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
//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

//        //for debugging
//        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
    }

	//for debugging
	Logger::print_thread_info_locked("compute-synchronous finished.\n", LEVEL_LOG_FUNCTION);
}

void collect_associated_variables_summary(std::unordered_set<vertexid_t> &ids, vertexid_t *args, int len, vertexid_t ret, PEGraph *graph, Grammar *grammar) {
	vector<vertexid_t> worklist;
	for (int i = 0; i < len; i++) {
		ids.insert(args[i]);
		worklist.push_back(args[i]);
	}
	if (ret != -1) {
		ids.insert(ret);
		worklist.push_back(ret);
	}
	while (!worklist.empty()) {
		vertexid_t id = worklist.back();
		worklist.pop_back();
		if (graph->getGraph().find(id) != graph->getGraph().end()) {
			for (int i = 0; i < graph->getNumEdges(id); i++) {
				vertexid_t dst = graph->getEdges(id)[i];
				label_t label = graph->getLabels(id)[i];
				if (grammar->isDereference(label)) {
					ids.insert(dst);
					worklist.push_back(dst);
					break;
				}
			}
		}
	}
}

PEGraph* extractSubGraph_left_summary(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar){
	if(len == 0 && ret == -1){
		return graph;
	}

	std::unordered_set<vertexid_t> ids;
	collect_associated_variables_summary(ids, args, len, ret, graph, grammar);

    /* remove edges associated with ids and deref_ids */
	for(auto it = ids.begin(); it != ids.end(); ++it){
		vertexid_t id = *it;
		if(graph->getGraph().find(id) != graph->getGraph().end()){
			EdgeArray deletedArray = EdgeArray();
			for(int i = 0; i < graph->getNumEdges(id); i++){
				vertexid_t dst = graph->getEdges(id)[i];
				label_t label = graph->getLabels(id)[i];
				if(ids.find(dst) != ids.end()){
					deletedArray.addOneEdge(dst, label);
				}
			}

			if(deletedArray.getSize() != 0){
	            int n1 = graph->getNumEdges(id);
	            auto *edges = new vertexid_t[n1];
	            auto *labels = new label_t[n1];
	            int len = myalgo::minusTwoArray(edges, labels, n1, graph->getEdges(id), graph->getLabels(id), deletedArray.getSize(), deletedArray.getEdges(), deletedArray.getLabels());
	            if(len){
	            	graph->setEdgeArray(id,len,edges,labels);
	            }
	            else{
	            	graph->clearEdgeArray(id);
	            }

	            delete[] edges;
	            delete[] labels;
			}
		}
	}

	return graph;

}

PEGraph* extractSubGraph_summary(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar){
	if(len == 0 && ret == -1){
		delete graph;
		return new PEGraph();
	}

	std::unordered_set<vertexid_t> ids;
	collect_associated_variables_summary(ids, args, len, ret, graph, grammar);

	PEGraph* out = new PEGraph();
    /* extract edges associated with ids and deref_ids */
	for(auto it = ids.begin(); it != ids.end(); ++it){
		vertexid_t id = *it;
		if(graph->getGraph().find(id) != graph->getGraph().end()){
			EdgeArray deletedArray = EdgeArray();
			for(int i = 0; i < graph->getNumEdges(id); i++){
				vertexid_t dst = graph->getEdges(id)[i];
				label_t label = graph->getLabels(id)[i];
				if(ids.find(dst) != ids.end()){
					deletedArray.addOneEdge(dst, label);
				}
			}

			if(deletedArray.getSize() != 0){
				out->setEdgeArray(id, deletedArray);
			}
		}
	}

	delete graph;
	return out;
}

void collect_associated_variables(std::unordered_set<vertexid_t> &ids, vertexid_t *args, int len, vertexid_t ret, PEGraph *graph, Grammar *grammar) {
	//for debugging
	Logger::print_thread_info_locked("collect-associated-variables starting...\n", LEVEL_LOG_FUNCTION);

	vector<vertexid_t> worklist;
	for (int i = 0; i < len; i++) {
		ids.insert(args[i]);
		worklist.push_back(args[i]);
	}
	if (ret != -1) {
		ids.insert(ret);
		worklist.push_back(ret);
	}
	while (!worklist.empty()) {
		vertexid_t id = worklist.back();
		worklist.pop_back();
		if (graph->getGraph().find(id) != graph->getGraph().end()) {
			for (int i = 0; i < graph->getNumEdges(id); i++) {
				vertexid_t dst = graph->getEdges(id)[i];
				if(ids.find(dst) == ids.end()){
					ids.insert(dst);
					worklist.push_back(dst);
				}
			}
		}
	}

	//for debugging
	Logger::print_thread_info_locked("collect-associated-variables finished.\n", LEVEL_LOG_FUNCTION);
}

void collect_associated_variables_all(std::unordered_set<vertexid_t> &ids, vertexid_t *args, int len, vertexid_t ret, PEGraph *graph, Grammar *grammar) {
	//for debugging
	Logger::print_thread_info_locked("collect-associated-variables-all starting...\n", LEVEL_LOG_FUNCTION);

	vector<vertexid_t> worklist;
	for (int i = 0; i < len; i++) {
		ids.insert(args[i]);
		worklist.push_back(args[i]);
	}
	if (ret != -1) {
		ids.insert(ret);
		worklist.push_back(ret);
	}

	for(auto it = graph->getGraph().begin(); it!= graph->getGraph().end(); ){
		vertexid_t src = it->first;
		if(ids.find(src) == ids.end()){
			ids.insert(src);
			worklist.push_back(src);
		}
	}

	while (!worklist.empty()) {
		vertexid_t id = worklist.back();
		worklist.pop_back();
		if (graph->getGraph().find(id) != graph->getGraph().end()) {
			for (int i = 0; i < graph->getNumEdges(id); i++) {
				vertexid_t dst = graph->getEdges(id)[i];
				if(ids.find(dst) == ids.end()){
					ids.insert(dst);
					worklist.push_back(dst);
				}
			}
		}
	}

	//for debugging
	Logger::print_thread_info_locked("collect-associated-variables-all finished.\n", LEVEL_LOG_FUNCTION);
}

PEGraph* extractSubGraph(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar){
	if(len == 0 && ret == -1){
		delete graph;
		return new PEGraph();
	}

	std::unordered_set<vertexid_t> ids;
	collect_associated_variables(ids, args, len, ret, graph, grammar);

    /* keep edges associated with ids */
    for(auto it = graph->getGraph().begin(); it!= graph->getGraph().end(); ){
        if(it->second.isEmpty()){
        	graph->getGraph().erase(it++);
        	continue;
        }

        vertexid_t src = it->first;
        if(ids.find(src) == ids.end()){
        	graph->getGraph().erase(it++);
        }
        else{
        	it++;
        }
    }

	return graph;
}

PEGraph* extractSubGraph_left(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar){
	if(len == 0 && ret == -1){
		return graph;
	}

	std::unordered_set<vertexid_t> ids;
	collect_associated_variables(ids, args, len, ret, graph, grammar);

    /* remove edges associated with ids */
    for(auto it = graph->getGraph().begin(); it!= graph->getGraph().end(); ){
        if(it->second.isEmpty()){
        	graph->getGraph().erase(it++);
        	continue;
        }

        vertexid_t src = it->first;
        if(ids.find(src) != ids.end()){
        	graph->getGraph().erase(it++);
        }
        else{
        	it++;
        }
    }

	return graph;
}

PEGraph* extractSubGraph_exit(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar, std::vector<CFGNode*>* preds, GraphStore* graphstore){
	if(len == 0 && ret == -1){
		delete graph;
		return new PEGraph();
	}

	std::unordered_set<vertexid_t> ids;

	PEGraph* pred_graph = nullptr;
	for(auto it = preds->cbegin(); it != preds->cend(); it++){
		CFGNode* pred = *it;
		if(pred->getStmt() && (pred->getStmt()->getType() == TYPE::Call || pred->getStmt()->getType() == TYPE::Callfptr)){
			//for debugging
//			cout << "call node: " << *pred << endl;
			if(pred->getStmt()->getType() == TYPE::Call){
				CallStmt* callstmt = (CallStmt*)(pred->getStmt());
				assert(len == callstmt->getLength() && ret == callstmt->getRet());
				for(int i = 0; i < len; i++){
					assert(args[i] == callstmt->getArgs()[i]);
				}
			}
			else if(pred->getStmt()->getType() == TYPE::Callfptr){
				CallfptrStmt* callfptrstmt = (CallfptrStmt*)(pred->getStmt());
				assert(len == callfptrstmt->getLength() && ret == callfptrstmt->getRet());
				for(int i = 0; i < len; i++){
					assert(args[i] == callfptrstmt->getArgs()[i]);
				}
			}

			PEGraph_Pointer out_pointer = pred->getOutPointer();
			pred_graph = graphstore->retrieve(out_pointer);
			if(pred_graph){
				collect_associated_variables(ids, args, len, ret, pred_graph, grammar);
				delete pred_graph;
			}
			break;
		}
	}

	PEGraph* fromExit = new PEGraph();
    /* extract edges associated with ids */
	for(auto it = ids.begin(); it != ids.end(); ++it){
		vertexid_t id = *it;
		if(graph->getGraph().find(id) != graph->getGraph().end()){
			EdgeArray edges = EdgeArray();
			for(int i = 0; i < graph->getNumEdges(id); i++){
				vertexid_t dst = graph->getEdges(id)[i];
				label_t label = graph->getLabels(id)[i];
				if(ids.find(dst) != ids.end()){
					edges.addOneEdge(dst, label);
				}
			}

			if(edges.getSize() != 0){
				fromExit->setEdgeArray(id, edges);
			}
		}
	}

//	//for debugging
//	//check the subsumption between the extracted to entry and the extracted from exit
//	if(pred_graph){
//        //for debugging
//        Logger::print_thread_info_locked("The call PEG after transformation:\n" + pred_graph->toString(grammar) + "\n", LEVEL_LOG_PEG);
//
//		PEGraph* toEntry = extractSubGraph(pred_graph, args, len, ret, grammar);
//        //for debugging
//        Logger::print_thread_info_locked("The toEntry PEG after transformation:\n" + toEntry->toString(grammar) + "\n", LEVEL_LOG_PEG);
//
//        //for debugging
//        Logger::print_thread_info_locked("The exit PEG after transformation:\n" + graph->toString(grammar) + "\n", LEVEL_LOG_PEG);
//        //for debugging
//        Logger::print_thread_info_locked("The fromExit PEG after transformation:\n" + fromExit->toString(grammar) + "\n", LEVEL_LOG_PEG);
//
//
//        toEntry->subtract(fromExit);
//        //for debugging
//        Logger::print_thread_info_locked("The toEntry PEG after subtraction:\n" + toEntry->toString(grammar) + "\n", LEVEL_LOG_PEG);
//		assert(toEntry->isEmpty());
//		delete toEntry;
//	}
	delete graph;
	return fromExit;
}

PEGraph* extractSubGraph_all_exit(PEGraph* graph, vertexid_t* args, int len, vertexid_t ret, Grammar *grammar, std::vector<CFGNode*>* preds, GraphStore* graphstore){
	if(len == 0 && ret == -1){
		delete graph;
		return new PEGraph();
	}

	std::unordered_set<vertexid_t> ids;

	for(auto it = preds->cbegin(); it != preds->cend(); it++){
		CFGNode* pred = *it;
		if(pred->getStmt() && (pred->getStmt()->getType() == TYPE::Call || pred->getStmt()->getType() == TYPE::Callfptr)){
			PEGraph_Pointer out_pointer = pred->getOutPointer();
			PEGraph* pred_graph = graphstore->retrieve(out_pointer);
			if(pred_graph){
				collect_associated_variables_all(ids, args, len, ret, pred_graph, grammar);
				delete pred_graph;
			}
			break;
		}
	}

	PEGraph* out = new PEGraph();
    /* extract edges associated with ids */
	for(auto it = ids.begin(); it != ids.end(); ++it){
		vertexid_t id = *it;
		if(graph->getGraph().find(id) != graph->getGraph().end()){
			EdgeArray edges = EdgeArray();
			for(int i = 0; i < graph->getNumEdges(id); i++){
				vertexid_t dst = graph->getEdges(id)[i];
				label_t label = graph->getLabels(id)[i];
				if(ids.find(dst) != ids.end()){
					edges.addOneEdge(dst, label);
				}
			}

			if(edges.getSize() != 0){
				out->setEdgeArray(id, edges);
			}
		}
	}

	delete graph;
	return out;
}

PEGraph* getPartial(Stmt* current, Stmt* pred, PEGraph* pred_graph, Grammar *grammar, std::vector<CFGNode*>* preds, GraphStore* graphstore){
	//for debugging
	Logger::print_thread_info_locked("get-partial starting...\n", LEVEL_LOG_FUNCTION);

	PEGraph* out;

	if(!pred_graph){
		out = pred_graph;
	}
	else if(pred && pred->getType() == TYPE::Callfptr){
   		CallfptrStmt* callfptrstmt = (CallfptrStmt*)(pred);
    	if(current->getType() == TYPE::Return){
    		if(callfptrstmt->getLength() == 0){
	    		out = pred_graph;
    		}
    		else {
//				out = extractSubGraph_left(pred_graph, callfptrstmt->getArgs(), callfptrstmt->getLength(), callfptrstmt->getRet(), grammar);
//				out = extractSubGraph_left_summary(pred_graph, callfptrstmt->getArgs(), callfptrstmt->getLength(), callfptrstmt->getRet(), grammar);
    			out = pred_graph;
    		}
    	}
    	else {//other entry node in callee
			out = extractSubGraph(pred_graph, callfptrstmt->getArgs(), callfptrstmt->getLength(), callfptrstmt->getRet(), grammar);
//    		out = pred_graph;
    	}
    }
    else if(pred && pred->getType() == TYPE::Call){
   		CallStmt* callstmt = (CallStmt*)(pred);
    	if(current->getType() == TYPE::Return){
    		if(callstmt->getLength() == 0){
				out = pred_graph;
    		}
    		else{
//				out = extractSubGraph_left(pred_graph, callstmt->getArgs(), callstmt->getLength(), callstmt->getRet(), grammar);
//				out = extractSubGraph_left_summary(pred_graph, callstmt->getArgs(), callstmt->getLength(), callstmt->getRet(), grammar);
    			out = pred_graph;
    		}
    	}
    	else {//other entry node in callee
			out = extractSubGraph(pred_graph, callstmt->getArgs(), callstmt->getLength(), callstmt->getRet(), grammar);
//    		out = pred_graph;
    	}
    }
    else if(current->getType() == TYPE::Return){
    	ReturnStmt* returnstmt = (ReturnStmt*)(current);
    	if(returnstmt->getLength() == 0){
    		delete pred_graph;
    		out = new PEGraph();
    	}
    	else{
			out = extractSubGraph_exit(pred_graph, returnstmt->getArgs(), returnstmt->getLength(), returnstmt->getRet(), grammar, preds, graphstore);
//	    	out = extractSubGraph_summary(pred_graph, returnstmt->getArgs(), returnstmt->getLength(), returnstmt->getRet(), grammar);
    	}
    }
    else{
    	out = pred_graph;
    }

	//for debugging
	Logger::print_thread_info_locked("get-partial finished.\n", LEVEL_LOG_FUNCTION);

	return out;
}

//PEGraph* getPartial_summary(Stmt* current, Stmt* pred, PEGraph* pred_graph, Grammar *grammar){
//	if(!pred_graph){
//		return pred_graph;
//	}
//
//    if(pred && pred->getType() == TYPE::Callfptr){
//   		CallfptrStmt* callfptrstmt = (CallfptrStmt*)(pred);
//    	if(current->getType() == TYPE::Return){
//			PEGraph* toCaller = extractSubGraph_left_summary(pred_graph, callfptrstmt->getArgs(), callfptrstmt->getLength(), callfptrstmt->getRet(), grammar);
//			return toCaller;
//    	}
//    	else {//other entry node in callee
//			PEGraph* toCallee = extractSubGraph_summary(pred_graph, callfptrstmt->getArgs(), callfptrstmt->getLength(), callfptrstmt->getRet(), grammar);
//			return toCallee;
//    	}
//    }
//    else if(pred && pred->getType() == TYPE::Call){
//   		CallStmt* callstmt = (CallStmt*)(pred);
//    	if(current->getType() == TYPE::Return){
//			PEGraph* toCaller = extractSubGraph_left_summary(pred_graph, callstmt->getArgs(), callstmt->getLength(), callstmt->getRet(), grammar);
//			return toCaller;
//    	}
//    	else {//other entry node in callee
//			PEGraph* toCallee = extractSubGraph_summary(pred_graph, callstmt->getArgs(), callstmt->getLength(), callstmt->getRet(), grammar);
//			return toCallee;
//    	}
//    }
//    else if(current->getType() == TYPE::Return){
//    	ReturnStmt* returnstmt = (ReturnStmt*)(current);
//		PEGraph* toCaller = extractSubGraph_summary(pred_graph, returnstmt->getArgs(), returnstmt->getLength(), returnstmt->getRet(), grammar);
//		return toCaller;
//    }
//    else{
//    	return pred_graph;
//    }
//}

//PEGraph* getPartial_naive(Stmt* current, Stmt* pred, PEGraph* pred_graph, Grammar *grammar){
//	if(!pred_graph){
//		return pred_graph;
//	}
//
//    if(pred && pred->getType() == TYPE::Callfptr){
//   		CallfptrStmt* callfptrstmt = (CallfptrStmt*)(pred);
//    	if(current->getType() == TYPE::Return && callfptrstmt->getLength() > 0){
//			delete pred_graph;
//			return nullptr;
//    	}
//    }
//    if(pred && pred->getType() == TYPE::Call){
//   		CallStmt* callstmt = (CallStmt*)(pred);
//    	if(current->getType() == TYPE::Return && callstmt->getLength() > 0){
//			delete pred_graph;
//			return nullptr;
//    	}
//    }
//    return pred_graph;
//}


PEGraph* CFGCompute_syn::combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds, CFGNode* cfg_node, Grammar *grammar){
	//for debugging
	Logger::print_thread_info_locked("combine starting...\n", LEVEL_LOG_FUNCTION);

	PEGraph* out;

//	cout << "size of preds: " << (preds != nullptr ? preds->size() : 0) << endl;
    if(!preds || preds->size() == 0){//entry node
        //return an empty graph
        out = new PEGraph();
    }
    else if(preds->size() == 1){
        CFGNode* pred = preds->at(0);
        PEGraph_Pointer out_pointer = pred->getOutPointer();
        out = graphstore->retrieve(out_pointer);

        //simplify the message passed through calls
//        cout << "current node: " << *cfg_node << "; pred node: " << *pred << endl;
        out = getPartial(cfg_node->getStmt(), pred->getStmt(), out, grammar, preds, graphstore);
//        out = getPartial_summary(cfg_node->getStmt(), pred->getStmt(), out, grammar);
//        out = getPartial_naive(cfg_node->getStmt(), pred->getStmt(), out, grammar);

        if(!out){
        	out = new PEGraph();
        }
    }
    else{
        out = new PEGraph();

        for(auto it = preds->cbegin(); it != preds->cend(); it++){
            CFGNode* pred = *it;
            PEGraph_Pointer out_pointer = pred->getOutPointer();
            PEGraph* out_graph = graphstore->retrieve(out_pointer);

            //get partial peg
//            cout << "current node: " << *cfg_node << "; pred node: " << *pred << endl;
            out_graph = getPartial(cfg_node->getStmt(), pred->getStmt(), out_graph, grammar, preds, graphstore);
//            out_graph = getPartial_summary(cfg_node->getStmt(), pred->getStmt(), out_graph, grammar);
//            out_graph = getPartial_naive(cfg_node->getStmt(), pred->getStmt(), out_graph, grammar);

            if(!out_graph){
            	continue;
            }
           	out->merge(out_graph);
           	delete out_graph;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("combine finished.\n", LEVEL_LOG_FUNCTION);

	return out;
}


PEGraph* CFGCompute_syn::transfer_phi(PEGraph* in, PhiStmt* stmt,Grammar *grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_update;
	Timer_diff diff_add;

	//for debugging
	Logger::print_thread_info_locked("transfer-phi starting...\n", LEVEL_LOG_FUNCTION);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

	//for tuning
	diff_update.start();

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
//    strong_update(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, singletons);
    strong_update_simplify(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, singletons);

    //for tuning
    diff_update.end();
    timer->getStrongupdateSum()->add_locked(diff_update.getClockDiff(), diff_update.getTimeDiff());


    //for tuning
    diff_add.start();

    // the GEN set
    peg_compute_add(out,stmt,grammar, flag, timer);

    //for tuning
    diff_add.end();
    timer->getAddSum()->add_locked(diff_add.getClockDiff(), diff_add.getTimeDiff());

	//for debugging
	Logger::print_thread_info_locked("transfer-phi finished.\n", LEVEL_LOG_FUNCTION);

    return out;
}

PEGraph* CFGCompute_syn::transfer_copy(PEGraph* in, AssignStmt* stmt,Grammar *grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_update;
	Timer_diff diff_add;

	//for debugging
	Logger::print_thread_info_locked("transfer-copy starting...\n", LEVEL_LOG_FUNCTION);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

	//for tuning
	diff_update.start();

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
//    strong_update(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, singletons);
    strong_update_simplify(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, singletons);

    //for tuning
    diff_update.end();
    timer->getStrongupdateSum()->add_locked(diff_update.getClockDiff(), diff_update.getTimeDiff());


    //for tuning
    diff_add.start();

    // the GEN set
    peg_compute_add(out,stmt,grammar, flag, timer);

    //for tuning
    diff_add.end();
    timer->getAddSum()->add_locked(diff_add.getClockDiff(), diff_add.getTimeDiff());

	//for debugging
	Logger::print_thread_info_locked("transfer-copy finished.\n", LEVEL_LOG_FUNCTION);

    return out;
}

PEGraph* CFGCompute_syn::transfer_load(PEGraph* in, LoadStmt* stmt,Grammar *grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_update;
	Timer_diff diff_add;

	//for debugging
	Logger::print_thread_info_locked("transfer-load starting...\n", LEVEL_LOG_FUNCTION);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

	//for tuning
	diff_update.start();

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
//    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, singletons);
    strong_update_simplify(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, singletons);

    //for tuning
    diff_update.end();
    timer->getStrongupdateSum()->add_locked(diff_update.getClockDiff(), diff_update.getTimeDiff());


    //for tuning
    diff_add.start();

    // the GEN set
    peg_compute_add(out,stmt,grammar, flag, timer);

    //for tuning
    diff_add.end();
    timer->getAddSum()->add_locked(diff_add.getClockDiff(), diff_add.getTimeDiff());

	//for debugging
	Logger::print_thread_info_locked("transfer-load finished.\n", LEVEL_LOG_FUNCTION);

    return out;
}

PEGraph* CFGCompute_syn::transfer_store(PEGraph* in, StoreStmt* stmt,Grammar *grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_update;
	Timer_diff diff_add;

	//for debugging
	Logger::print_thread_info_locked("transfer-store starting...\n", LEVEL_LOG_FUNCTION);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

	//for tuning
	diff_update.start();

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;

    if(out->getGraph().find(stmt->getDst()) != out->getGraph().end()){
		if(is_strong_update_dst(stmt->getDst(), out, grammar, singletons)) {
//			strong_update_store_dst(stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);
			strong_update_store_dst_simplify(stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);
		}
    }
    else{
        if(is_strong_update_aux(stmt->getAux(), out, grammar, singletons)) {
//            strong_update_store_aux(stmt->getAux(), stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);
            strong_update_store_aux_simplify(stmt->getAux(), stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);
        }
    }

    //for tuning
    diff_update.end();
    timer->getStrongupdateSum()->add_locked(diff_update.getClockDiff(), diff_update.getTimeDiff());


    //for tuning
    diff_add.start();

    // the GEN set
    peg_compute_add(out,stmt,grammar, flag, timer);

    //for tuning
    diff_add.end();
    timer->getAddSum()->add_locked(diff_add.getClockDiff(), diff_add.getTimeDiff());

	//for debugging
	Logger::print_thread_info_locked("transfer-store finished.\n", LEVEL_LOG_FUNCTION);

    return out;
}

PEGraph* CFGCompute_syn::transfer_address(PEGraph* in, AllocStmt* stmt,Grammar *grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer){
	//for performance tuning
	Timer_diff diff_update;
	Timer_diff diff_add;

	//for debugging
	Logger::print_thread_info_locked("transfer-alloc starting...\n", LEVEL_LOG_FUNCTION);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

	//for tuning
	diff_update.start();

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
//	strong_update(stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);
	strong_update_simplify(stmt->getDst(), out, vertices_changed, grammar, vertices_affected, singletons);

    //for tuning
    diff_update.end();
    timer->getStrongupdateSum()->add_locked(diff_update.getClockDiff(), diff_update.getTimeDiff());


    //for tuning
    diff_add.start();

    // the GEN set
    peg_compute_add(out, stmt, grammar, flag, timer);

    //for tuning
    diff_add.end();
    timer->getAddSum()->add_locked(diff_add.getClockDiff(), diff_add.getTimeDiff());

    //for debugging
   	Logger::print_thread_info_locked("transfer-alloc finished.\n", LEVEL_LOG_FUNCTION);

    return out;
}

bool CFGCompute_syn::is_strong_update_dst(vertexid_t x, PEGraph *out, Grammar *grammar, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("is-strong-update starting...\n", LEVEL_LOG_FUNCTION);

    /* If there exists one and only one variable o,which
     * refers to a singleton memory location,such that x and o are memory alias
     */
	assert(out->getGraph().find(x) != out->getGraph().end());

    int numOfSingleTon = 0;
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i]) && singletons->isSingleton(edges[i]))
            ++numOfSingleTon;
    }

	//for debugging
	Logger::print_thread_info_locked("is-strong-update finished.\n", LEVEL_LOG_FUNCTION);

    return (numOfSingleTon == 1);

}

bool CFGCompute_syn::is_strong_update_aux(vertexid_t aux, PEGraph *out, Grammar *grammar, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("is-strong-update-store starting...\n", LEVEL_LOG_FUNCTION);

    /* If there exists one and only one variable o,which
     * refers to a singleton memory location,such that x points to o
     */
	if(out->getGraph().find(aux) == out->getGraph().end()){
		//for debugging
		Logger::print_thread_info_locked("is-strong-update-store finished.\n", LEVEL_LOG_FUNCTION);
		return false;
	}

    int numOfSingleTon = 0;
    int numEdges = out->getNumEdges(aux);
    vertexid_t * edges = out->getEdges(aux);
    label_t *labels = out->getLabels(aux);

    for(int i = 0; i < numEdges; ++i) {
        if(grammar->isPointsTo(labels[i]) && singletons->isSingleton(edges[i])){
//        	cout << grammar->isPointsTo(labels[i]) << endl;
            ++numOfSingleTon;
        }
    }

//    //for debugging
//    cout << out->getGraph()[aux].toString(grammar) << endl;
//    cout << "number of singletons:\t" << numOfSingleTon << endl;

	//for debugging
	Logger::print_thread_info_locked("is-strong-update-store finished.\n", LEVEL_LOG_FUNCTION);

    return (numOfSingleTon == 1);
}

bool isDeletable(vertexid_t src, vertexid_t dst, char label, std::set<vertexid_t> &vertices_changed, std::set<vertexid_t> &vertices_affected, Grammar *grammar){
	//don't delete self-loop edges
	if(src == dst && grammar->isEruleLabel(label)){
		return false;
	}

	//delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with that within vertices_changed
	if(vertices_changed.find(src) != vertices_changed.end() || vertices_changed.find(dst) != vertices_changed.end()){
		return !grammar->isDereference_bidirect(label);
	}

	//delete all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
//	else if(vertices_affected.find(src) != vertices_affected.end() || vertices_affected.find(dst) != vertices_affected.end()){
//		return !grammar->isDereference_bidirect(label) && !grammar->isAssign_bidirect(label);
//	}

	return false;
}

/* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with that within vertices_changed,
 * and all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
 * * */
void findDeletedEdges(EdgeArray & edgesToDelete, vertexid_t src, std::set<vertexid_t> &vertices_changed, std::set<vertexid_t> &vertices_affected, Grammar *grammar, EdgeArray & deleted) {
	for (int i = 0; i < edgesToDelete.getSize(); ++i) {
		vertexid_t dst = edgesToDelete.getEdges()[i];
		char label = edgesToDelete.getLabels()[i];
		if(isDeletable(src, dst, label, vertices_changed, vertices_affected, grammar)){
			deleted.addOneEdge(dst, label);
		}
	}

	//		//seems not necessary since the original EdgeArray in edgesToDelete is already sorted
	//		deleted.merge();
}

void CFGCompute_syn::getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m) {
	//get all the direct assign-bidirection edges into m
	//TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
	for (auto it = out->getGraph().begin(); it != out->getGraph().end(); it++) {
		int numEdges = out->getNumEdges(it->first);
		vertexid_t* edges = out->getEdges(it->first);
		label_t* labels = out->getLabels(it->first);
		for (int j = 0; j < numEdges; j++) {
			if (isDirectAssignEdges(it->first, edges[j], labels[j], vertices_changed, grammar)) {
				// delete the direct incoming and outgoing assign edges
				if (m->find(it->first) == m->end()) {
					(*m)[it->first] = EdgeArray();
				}
				(*m)[it->first].addOneEdge(edges[j], labels[j]);
			}
		}
//		//seems not necessary since the original EdgeArray in out is already sorted
//		(*m)[it->first].merge();
	}


}

void CFGCompute_syn::strong_update_store_aux_simplify(vertexid_t aux, vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update-store starting...\n", LEVEL_LOG_FUNCTION);

	assert(out->getGraph().find(x) == out->getGraph().end());
	assert(out->getGraph().find(aux) != out->getGraph().end());

    // vertices <- must_alias(x); put *x into this set as well
	must_alias_store_aux(aux, x, out, vertices_changed, grammar, vertices_affected, singletons);


    /* remove edges */
    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); ){
    	if(it->second.isEmpty()){
    		it++;
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		cout << it->second.toString(grammar) << endl;
//		cout << deletedArray.toString(grammar) << endl << endl;

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
                out->setEdgeArray(src,len,edges,labels);
                it++;
            }
            else{
//                out->clearEdgeArray(src);
            	out->getGraph().erase(it++);
            }

            delete[] edges;
            delete[] labels;
        }
        else{
        	it++;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update-store finished.\n", LEVEL_LOG_FUNCTION);
}

void CFGCompute_syn::strong_update_store_aux(vertexid_t aux, vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update-store starting...\n", LEVEL_LOG_FUNCTION);

	assert(out->getGraph().find(x) == out->getGraph().end());
	assert(out->getGraph().find(aux) != out->getGraph().end());

    // vertices <- must_alias(x); put *x into this set as well
	must_alias_store_aux(aux, x, out, vertices_changed, grammar, vertices_affected, singletons);

    //get all the direct assign edges into m
    //TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
    std::unordered_map<vertexid_t, EdgeArray> m;
	getDirectAssignEdges(out, vertices_changed, grammar, &m);

//	//for debugging
//	if(!m.empty()){
//		cout << m.size() << endl;
//		for(auto& it: m){
//			cout << it.first << endl;
//			cout << it.second.toString(grammar) << endl;
//		}
//		cout << "\n";
//	}

	if(m.empty()){
		//for debugging
		Logger::print_thread_info_locked("strong-update-store finished.\n", LEVEL_LOG_FUNCTION);
		return;
	}

    // execute the edge addition operation. the oldsSet is out - m, the deltasSet is m
    peg_compute_delete(out, grammar, &m);

//    //for debugging
//	cout << m.size() << endl;
//	for(auto& it: m){
//		cout << it.first << endl;
//		cout << it.second.toString(grammar) << endl;
//	}
//	cout << "\n";

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
    	if(it->second.isEmpty()){
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		cout << it->second.toString(grammar) << endl;
//		cout << deletedArray.toString(grammar) << endl << endl;

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
                out->setEdgeArray(src,len,edges,labels);
            }
            else{
                out->clearEdgeArray(src);
            }

            delete[] edges;
            delete[] labels;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update-store finished.\n", LEVEL_LOG_FUNCTION);
}


void CFGCompute_syn::strong_update_store_dst_simplify(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update starting...\n", LEVEL_LOG_FUNCTION);

//	if(out->getGraph().find(x) == out->getGraph().end()){
//		//for debugging
//		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
//		return;
//	}

    // vertices <- must_alias(x); put *x into this set as well
	must_alias_store_dst(x, out, vertices_changed, grammar, vertices_affected, singletons);


    /* remove edges */
    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); ){
    	if(it->second.isEmpty()){
    		it++;
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		if(deletedArray.getSize() != 0){
//			cout << deletedArray << endl;
//		}

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
				out->setEdgeArray(src,len,edges,labels);
				it++;
            }
            else{
//                out->clearEdgeArray(src);
            	out->getGraph().erase(it++);
            }

            delete[] edges;
            delete[] labels;
        }
        else{
        	it++;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
}

void CFGCompute_syn::strong_update_store_dst(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update starting...\n", LEVEL_LOG_FUNCTION);

//	if(out->getGraph().find(x) == out->getGraph().end()){
//		//for debugging
//		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
//		return;
//	}

    // vertices <- must_alias(x); put *x into this set as well
	must_alias_store_dst(x, out, vertices_changed, grammar, vertices_affected, singletons);

    //get all the direct assign edges into m
    //TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
    std::unordered_map<vertexid_t, EdgeArray> m;
	getDirectAssignEdges(out, vertices_changed, grammar, &m);

//	//for debugging
//	if(!m.empty()){
//		cout << m.size() << endl;
//		cout << "\n\n\n";
//	}

	if(m.empty()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
		return;
	}

    // execute the edge addition operation. the oldsSet is out - m, the deltasSet is m
    peg_compute_delete(out, grammar, &m);

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
    	if(it->second.isEmpty()){
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		if(deletedArray.getSize() != 0){
//			cout << deletedArray << endl;
//		}

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
				out->setEdgeArray(src,len,edges,labels);
            }
            else{
                out->clearEdgeArray(src);
            }

            delete[] edges;
            delete[] labels;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
}

void CFGCompute_syn::strong_update_simplify(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update starting...\n", LEVEL_LOG_FUNCTION);

	if(out->getGraph().find(x) == out->getGraph().end()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
		return;
	}

    // vertices <- must_alias(x); put *x into this set as well
	must_alias(x, out, vertices_changed, grammar, vertices_affected, singletons);


    /* remove edges */
	for(auto it = out->getGraph().begin(); it != out->getGraph().end(); ){
    	if(it->second.isEmpty()){
    		it++;
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		if(deletedArray.getSize() != 0){
//			cout << deletedArray << endl;
//		}

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
				out->setEdgeArray(src,len,edges,labels);
				it++;
            }
            else{
//                out->clearEdgeArray(src);
            	out->getGraph().erase(it++);
            }

            delete[] edges;
            delete[] labels;
        }
        else{
        	it++;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
}

void CFGCompute_syn::strong_update(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	//for debugging
	Logger::print_thread_info_locked("strong-update starting...\n", LEVEL_LOG_FUNCTION);

	if(out->getGraph().find(x) == out->getGraph().end()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
		return;
	}

    // vertices <- must_alias(x); put *x into this set as well
	must_alias(x, out, vertices_changed, grammar, vertices_affected, singletons);

    //get all the direct assign edges into m
    //TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
    std::unordered_map<vertexid_t, EdgeArray> m;
	getDirectAssignEdges(out, vertices_changed, grammar, &m);

//	//for debugging
//	if(!m.empty()){
//		cout << m.size() << endl;
//		cout << "\n\n\n";
//	}

	if(m.empty()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
		return;
	}

    // execute the edge addition operation. the oldsSet is out - m, the deltasSet is m
    peg_compute_delete(out, grammar, &m);

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
    	if(it->second.isEmpty()){
    		continue;
    	}

        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

//		//for debugging
//		if(deletedArray.getSize() != 0){
//			cout << deletedArray << endl;
//		}

        if(deletedArray.getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = deletedArray.getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, out->getEdges(src), out->getLabels(src), n2, deletedArray.getEdges(), deletedArray.getLabels());
            if(len){
                out->setEdgeArray(src,len,edges,labels);
            }
            else{
                out->clearEdgeArray(src);
            }

            delete[] edges;
            delete[] labels;
        }
    }

	//for debugging
	Logger::print_thread_info_locked("strong-update finished.\n", LEVEL_LOG_FUNCTION);
}

bool CFGCompute_syn::isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar) {
//	if(!grammar->isAssign(label))
    if(!grammar->isAssign_bidirect(label))
        return false;
    return ( (vertices.find(src) != vertices.end()) || (vertices.find(dst) != vertices.end()) );
}

void CFGCompute_syn::must_alias_store_aux(vertexid_t aux, vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	assert(out->getGraph().find(x) == out->getGraph().end());
	assert(out->getGraph().find(aux) != out->getGraph().end());

	/* if there exists one and only one variable o,which
	 * refers to a singleton memory location,such that x and
	 * y are both memory aliases of o,then x and y are Must-alias
	 */
	std::unordered_set<vertexid_t> set1;

	{
		int numEdges = out->getNumEdges(aux);
		vertexid_t *edges = out->getEdges(aux);
		label_t *labels = out->getLabels(aux);

		for (int i = 0; i < numEdges; ++i) {
			if (grammar->isPointsTo(labels[i])
					&& singletons->isSingleton(edges[i])) {
				set1.insert(edges[i]);
			}
		}
	}
	assert(set1.size() == 1);


	//compute all the must-alias expressions
	int numEdges = out->getNumEdges(aux);
	vertexid_t *edges = out->getEdges(aux);
	label_t *labels = out->getLabels(aux);

	for (int i = 0; i < numEdges; ++i) {
		if (grammar->isPointsTo(labels[i])) {
			std::unordered_set<vertexid_t> set2;

			vertexid_t candidate = edges[i];
			int numEdges = out->getNumEdges(candidate);
			vertexid_t *edges = out->getEdges(candidate);
			label_t *labels = out->getLabels(candidate);

			for (int i = 0; i < numEdges; ++i) {
				if (grammar->isMemoryAlias(labels[i]) && singletons->isSingleton(edges[i])) {
					set2.insert(edges[i]);
				}
			}

			if(set2.size() == 1 && *(set2.begin()) == *(set1.begin())){
				vertices_changed.insert(candidate);
			}
		}
	}

	vertices_changed.insert(x);


	//add *x into vertices as well
	for(auto it = vertices_changed.begin(); it != vertices_changed.end(); ++it){
		vertexid_t x = *it;

	    int numEdges = out->getNumEdges(x);
	    vertexid_t * edges = out->getEdges(x);
	    label_t *labels = out->getLabels(x);

	    for(int i = 0;i < numEdges;++i) {
	        if(grammar->isDereference(labels[i])){
	        	vertices_changed.insert(edges[i]);
	        }

	        if(grammar->isDereference_reverse(labels[i])){
	        	vertices_affected.insert(edges[i]);
	        }
	    }
	}
}

void CFGCompute_syn::must_alias_store_dst(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	/* if there exists one and only one variable o,which
	 * refers to a singleton memory location,such that x and
	 * y are both memory aliases of o,then x and y are Must-alias
	 */
	std::unordered_set<vertexid_t> set1;

	assert(!singletons->isSingleton(x));

	{
		int numEdges = out->getNumEdges(x);
		vertexid_t *edges = out->getEdges(x);
		label_t *labels = out->getLabels(x);

		for (int i = 0; i < numEdges; ++i) {
			if (grammar->isMemoryAlias(labels[i])
					&& singletons->isSingleton(edges[i])) {
				set1.insert(edges[i]);
			}
		}
	}
	assert(set1.size() == 1);


	//compute all the must-alias expressions
	int numEdges = out->getNumEdges(x);
	vertexid_t *edges = out->getEdges(x);
	label_t *labels = out->getLabels(x);

	for (int i = 0; i < numEdges; ++i) {
		if (grammar->isMemoryAlias(labels[i])) {
			std::unordered_set<vertexid_t> set2;

			vertexid_t candidate = edges[i];
			int numEdges = out->getNumEdges(candidate);
			vertexid_t *edges = out->getEdges(candidate);
			label_t *labels = out->getLabels(candidate);

			for (int i = 0; i < numEdges; ++i) {
				if (grammar->isMemoryAlias(labels[i]) && singletons->isSingleton(edges[i])) {
					set2.insert(edges[i]);
				}
			}

			if(set2.size() == 1 && *(set2.begin()) == *(set1.begin())){
				vertices_changed.insert(candidate);
			}
		}
	}

	vertices_changed.insert(x);


	//add *x into vertices as well
	for (auto it = vertices_changed.begin(); it != vertices_changed.end(); ++it) {
		vertexid_t x = *it;

		int numEdges = out->getNumEdges(x);
		vertexid_t *edges = out->getEdges(x);
		label_t *labels = out->getLabels(x);

		for (int i = 0; i < numEdges; ++i) {
			if (grammar->isDereference(labels[i])) {
				vertices_changed.insert(edges[i]);
			}

			if (grammar->isDereference_reverse(labels[i])) {
				vertices_affected.insert(edges[i]);
			}
		}
	}

}

void CFGCompute_syn::must_alias(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, Singletons* singletons) {
	/* if there exists one and only one variable o,which
	 * refers to a singleton memory location,such that x and
	 * y are both memory aliases of o,then x and y are Must-alias
	 */
	std::unordered_set<vertexid_t> set1;

//	assert(singletons->isSingleton(x));
	set1.insert(x);


	//compute all the must-alias expressions
	int numEdges = out->getNumEdges(x);
	vertexid_t *edges = out->getEdges(x);
	label_t *labels = out->getLabels(x);

	for (int i = 0; i < numEdges; ++i) {
		if (grammar->isMemoryAlias(labels[i])) {
			std::unordered_set<vertexid_t> set2;

			vertexid_t candidate = edges[i];
			int numEdges = out->getNumEdges(candidate);
			vertexid_t *edges = out->getEdges(candidate);
			label_t *labels = out->getLabels(candidate);

			for (int i = 0; i < numEdges; ++i) {
				if (grammar->isMemoryAlias(labels[i]) && singletons->isSingleton(edges[i])) {
					set2.insert(edges[i]);
				}
			}

			if(set2.size() == 1 && *(set2.begin()) == *(set1.begin())){
				vertices_changed.insert(candidate);
			}
		}
	}

	vertices_changed.insert(x);


	//add *x into vertices as well
	for (auto it = vertices_changed.begin(); it != vertices_changed.end(); ++it) {
		vertexid_t x = *it;

		int numEdges = out->getNumEdges(x);
		vertexid_t *edges = out->getEdges(x);
		label_t *labels = out->getLabels(x);

		for (int i = 0; i < numEdges; ++i) {
			if (grammar->isDereference(labels[i])) {
				vertices_changed.insert(edges[i]);
			}

			if (grammar->isDereference_reverse(labels[i])) {
				vertices_affected.insert(edges[i]);
			}
		}
	}


	//	//x must be a singleton variable
//	assert(singletons->isSingleton(x));
//
//	/* if there exists one and only one variable o,which
//	 * refers to a singleton memory location,such that x and
//	 * y are both memory aliases of o,then x and y are Must-alias
//	 */
//    int numEdges = out->getNumEdges(x);
//    vertexid_t * edges = out->getEdges(x);
//    label_t *labels = out->getLabels(x);
//
//    int numOfSingleTon = 0;
//	if(singletons->isSingleton(x)){
//		++numOfSingleTon;
//	}
//    for(int i = 0;i < numEdges;++i) {
//        if(grammar->isMemoryAlias(labels[i])){
//        	vertices_changed.insert(edges[i]);
//        	if(singletons->isSingleton(edges[i])){
//        		++numOfSingleTon;
//        	}
//        }
//    }
//
//    //check the number of singletons
//    if(numOfSingleTon == 0){
//    	perror("invalid number of singletons!");
//    	vertices_changed.clear();
//    }
////    else if(numOfSingleTon == 1){
////
////    }
//    else if(numOfSingleTon > 1){
//    	vertices_changed.clear();
//    }
//	vertices_changed.insert(x);
//
//	//add *x into vertices as well
//	for(auto it = vertices_changed.begin(); it != vertices_changed.end(); ++it){
//		vertexid_t x = *it;
//
//	    int numEdges = out->getNumEdges(x);
//	    vertexid_t * edges = out->getEdges(x);
//	    label_t *labels = out->getLabels(x);
//
//	    for(int i = 0;i < numEdges;++i) {
//	        if(grammar->isDereference(labels[i])){
//	        	vertices_changed.insert(edges[i]);
//	        }
//
//	        if(grammar->isDereference_reverse(labels[i])){
//	        	vertices_affected.insert(edges[i]);
//	        }
//	    }
//	}
}

void CFGCompute_syn::peg_compute_delete(PEGraph *out, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m) {
	//for debugging
	Logger::print_thread_info_locked("peg-compute-delete starting...\n", LEVEL_LOG_FUNCTION);

    // add assgin edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_delete(out, m);

    // start GEN
    long number_deleted = 0;
    if(IS_PEGCOMPUTE_PARALLEL_DELETE){
		number_deleted = PEGCompute_parallel::startCompute_delete(compset, grammar, m);
    }
    else{
    	number_deleted = PEGCompute::startCompute_delete(compset, grammar, m);
    }
//    Logger::print_thread_info_locked("number of edges deleted: " + std::to_string(number_deleted) + "\n", LEVEL_LOG_INFO);

    // clean
    delete compset;

	//for debugging
	Logger::print_thread_info_locked("peg-compute-delete finished.\n", LEVEL_LOG_FUNCTION);
}

void removeExistingEdges(const EdgeArray& edges_src, vertexid_t src, PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>* m) {
	//remove the existing edges
	int n1 = edges_src.getSize();
	auto* edges = new vertexid_t[n1];
	auto* labels = new label_t[n1];
	int len = myalgo::minusTwoArray(edges, labels, edges_src.getSize(),
			edges_src.getEdges(), edges_src.getLabels(), out->getNumEdges(src),
			out->getEdges(src), out->getLabels(src));
	if (len) {
		(*m)[src] = EdgeArray();
		(*m)[src].set(len, edges, labels);
	}
	delete[] edges;
	delete[] labels;
}

//bool containsSelfLoopEdges(PEGraph *out, Stmt *stmt, Grammar *grammar){
//	vertexid_t src = stmt->getSrc();
//
//	vertexid_t dst = stmt->getDst();
//
//	vertexid_t aux = stmt->getAux();
//}

void CFGCompute_syn::getDirectAddedEdges_phi(PEGraph *out, Stmt *stmt_, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
	PhiStmt* stmt = dynamic_cast<PhiStmt*>(stmt_);

    //'a', '-a', 'd', '-d', and self-loop edges
	int length = stmt->getLength();
	vertexid_t* srcs = stmt->getSrcs();

	vertexid_t dst = stmt->getDst();
	EdgeArray edges_dst = EdgeArray();

	for(int i = 0; i < length; i++){
		vertexid_t src = srcs[i];
		EdgeArray edges_src = EdgeArray();

		//'a', '-a'
		edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
		edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

//		//self-loop edges
//	    if(!flag){
//			for(int i = 0; i < grammar->getNumErules(); ++i){
//				char label = grammar->getErule(i);
//				edges_src.addOneEdge(src, label);
//			}
//	    }

	    //merge and sort
	    edges_src.merge();

	    //remove the existing edges
		removeExistingEdges(edges_src, src, out, m);
	}

	//self-loop edges
    if(!flag){
//    	Logger::print_thread_info_locked("adding self-loop edges...\n", 1);

		for(int i = 0; i < grammar->getNumErules(); ++i){
			char label = grammar->getErule(i);
			edges_dst.addOneEdge(dst, label);
		}
    }

	edges_dst.merge();
	removeExistingEdges(edges_dst, dst, out, m);
}

void getDirectAddedEdges_alloc(PEGraph *out, AllocStmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
    //'a', '-a', 'd', '-d', and self-loop edges
    vertexid_t src = stmt->getSrc();
    EdgeArray edges_src = EdgeArray();

    vertexid_t dst = stmt->getDst();
    EdgeArray edges_dst = EdgeArray();

    vertexid_t aux = stmt->getAux();
    EdgeArray edges_aux = EdgeArray();

    //'a', '-a'
    edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
    edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

    //'d', '-d'
	edges_src.addOneEdge(aux, grammar->getLabelValue("d"));
	edges_aux.addOneEdge(src, grammar->getLabelValue("-d"));

	//self-loop edges
    if(!flag){
//    	Logger::print_thread_info_locked("adding self-loop edges...\n", 1);

		for(int i = 0; i < grammar->getNumErules(); ++i){
			char label = grammar->getErule(i);
			edges_src.addOneEdge(src, label);
			edges_dst.addOneEdge(dst, label);
			edges_aux.addOneEdge(aux, label);
		}
    }

    //merge and sort
    edges_src.merge();
    edges_dst.merge();
	edges_aux.merge();

    //remove the existing edges
	removeExistingEdges(edges_src, src, out, m);
	removeExistingEdges(edges_dst, dst, out, m);
   	removeExistingEdges(edges_aux, aux, out, m);
}

void getDirectAddedEdges_store(PEGraph *out, StoreStmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
    //'a', '-a', 'd', '-d', and self-loop edges
    vertexid_t src = stmt->getSrc();
    EdgeArray edges_src = EdgeArray();

    vertexid_t dst = stmt->getDst();
    EdgeArray edges_dst = EdgeArray();

    vertexid_t aux = stmt->getAux();
    EdgeArray edges_aux = EdgeArray();

    //'a', '-a'
    edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
    edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

    //'d', '-d'
	edges_aux.addOneEdge(dst, grammar->getLabelValue("d"));
	edges_dst.addOneEdge(aux, grammar->getLabelValue("-d"));

	//self-loop edges
    if(!flag){
//    	Logger::print_thread_info_locked("adding self-loop edges...\n", 1);

		for(int i = 0; i < grammar->getNumErules(); ++i){
			char label = grammar->getErule(i);
			edges_src.addOneEdge(src, label);
			edges_dst.addOneEdge(dst, label);
			edges_aux.addOneEdge(aux, label);
		}
    }

    //merge and sort
    edges_src.merge();
    edges_dst.merge();
	edges_aux.merge();

    //remove the existing edges
	removeExistingEdges(edges_src, src, out, m);
	removeExistingEdges(edges_dst, dst, out, m);
   	removeExistingEdges(edges_aux, aux, out, m);
}

void getDirectAddedEdges_load(PEGraph *out, LoadStmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
    //'a', '-a', 'd', '-d', and self-loop edges
    vertexid_t src = stmt->getSrc();
    EdgeArray edges_src = EdgeArray();

    vertexid_t dst = stmt->getDst();
    EdgeArray edges_dst = EdgeArray();

    vertexid_t aux = stmt->getAux();
    EdgeArray edges_aux = EdgeArray();

    //'a', '-a'
    edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
    edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

    //'d', '-d'
    edges_aux.addOneEdge(src, grammar->getLabelValue("d"));
    edges_src.addOneEdge(aux, grammar->getLabelValue("-d"));

	//self-loop edges
    if(!flag){
//    	Logger::print_thread_info_locked("adding self-loop edges...\n", 1);

		for(int i = 0; i < grammar->getNumErules(); ++i){
			char label = grammar->getErule(i);
			edges_src.addOneEdge(src, label);
			edges_dst.addOneEdge(dst, label);
			edges_aux.addOneEdge(aux, label);
		}
    }

    //merge and sort
    edges_src.merge();
    edges_dst.merge();
	edges_aux.merge();

    //remove the existing edges
	removeExistingEdges(edges_src, src, out, m);
	removeExistingEdges(edges_dst, dst, out, m);
   	removeExistingEdges(edges_aux, aux, out, m);
}

void getDirectAddedEdges_assign(PEGraph *out, AssignStmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
    //'a', '-a', and self-loop edges
    vertexid_t src = stmt->getSrc();
    EdgeArray edges_src = EdgeArray();

    vertexid_t dst = stmt->getDst();
    EdgeArray edges_dst = EdgeArray();

    //'a', '-a'
    edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
    edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

	//self-loop edges
    if(!flag){
//    	Logger::print_thread_info_locked("adding self-loop edges...\n", 1);

		for(int i = 0; i < grammar->getNumErules(); ++i){
			char label = grammar->getErule(i);
			edges_src.addOneEdge(src, label);
			edges_dst.addOneEdge(dst, label);
		}
    }

    //merge and sort
    edges_src.merge();
    edges_dst.merge();

    //remove the existing edges
	removeExistingEdges(edges_src, src, out, m);
	removeExistingEdges(edges_dst, dst, out, m);
}

void CFGCompute_syn::getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag){
//    switch(stmt->getType()){
    TYPE t = stmt->getType();
    if(t == TYPE::Alloca){
    	AllocStmt* stmt_alloc = dynamic_cast<AllocStmt*>(stmt);
    	getDirectAddedEdges_alloc(out, stmt_alloc, grammar, m, flag);
    }
    else if(t == TYPE::Store){
    	StoreStmt* stmt_store = dynamic_cast<StoreStmt*>(stmt);
    	getDirectAddedEdges_store(out, stmt_store, grammar, m, flag);
    }
    else if(t == TYPE::Load){
    	LoadStmt* stmt_load = dynamic_cast<LoadStmt*>(stmt);
    	getDirectAddedEdges_load(out, stmt_load, grammar, m, flag);
    }
    else if(t == TYPE::Assign){
    	AssignStmt* stmt_assign = dynamic_cast<AssignStmt*>(stmt);
    	getDirectAddedEdges_assign(out, stmt_assign, grammar, m, flag);
    }
    else{
    	cout << "wrong stmt type!!!" << endl;
    	exit(EXIT_FAILURE);
    }

}

void CFGCompute_syn::peg_compute_add(PEGraph *out, Stmt *stmt, Grammar *grammar, bool flag, Timer_wrapper_inmemory* timer) {
	//for performance tuning
	Timer_diff diff_direct;
	Timer_diff diff_init;
	Timer_diff diff_compute;
	Timer_diff diff_post;

	//for debugging
	Logger::print_thread_info_locked("peg-compute-add starting...\n", LEVEL_LOG_FUNCTION);

	bool isConservative = true;

	//for tuning
	diff_direct.start();

	std::unordered_map<vertexid_t, EdgeArray> m;
	if(stmt->getType() == TYPE::Phi){
		getDirectAddedEdges_phi(out, stmt, grammar, &m, flag);
	}
	else{
		getDirectAddedEdges(out, stmt, grammar, &m, flag);
	}

	//for tuning
	diff_direct.end();
	timer->getAddDirectSum()->add_locked(diff_direct.getClockDiff(), diff_direct.getTimeDiff());

	if(m.empty() && !isConservative){// no new edges directly added
		return;
	}

	//for tuning
	diff_init.start();

    // add assign edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_add(out, &m, isConservative);

    //for tuning
    diff_init.end();
    timer->getAddInitSum()->add_locked(diff_init.getClockDiff(), diff_init.getTimeDiff());

//    //for debugging
//    cout << *compset << endl;

    //for tuning
    diff_compute.start();

    // start GEN
    if(IS_PEGCOMPUTE_PARALLEL_ADD){
		PEGCompute_parallel::startCompute_add(compset, grammar);
    }
    else{
	    PEGCompute::startCompute_add(compset, grammar, timer);
    }

    //for tuning
    diff_compute.end();
    timer->getAddComputeSum()->add_locked(diff_compute.getClockDiff(), diff_compute.getTimeDiff());

    //    Logger::print_thread_info_locked("number of edges added: " + to_string(number_added) + "\n");

//    //for debugging
//    cout << *compset << endl;

    //for tuning
    diff_post.start();

    // GEN finished, compset -> out
    auto olds = compset->getOlds();
    for(auto it = olds.begin(); it != olds.end(); ++it){
    	vertexid_t id = it->first;
    	if(out->getGraph().find(id) == out->getGraph().end()){
    		out->setEdgeArray(id, it->second.getSize(), it->second.getEdges(), it->second.getLabels());
    	}
    	else if(out->getNumEdges(id) != it->second.getSize()){
			out->setEdgeArray(id, it->second.getSize(), it->second.getEdges(), it->second.getLabels());
    	}
    }

    // clean
	delete compset;

	//for tuning
	diff_post.end();
	timer->getAddPostprocessSum()->add_locked(diff_post.getClockDiff(), diff_post.getTimeDiff());

	//for debugging
	Logger::print_thread_info_locked("peg-compute-add finished.\n", LEVEL_LOG_FUNCTION);
}

//// 使用tab键分割
//bool CFGCompute::load(const string& file_cfg, const string& file_stmt, CFG *cfg, const string& file_singleton, Singletons* singletons, GraphStore *graphstore, const string& file_grammar, Grammar * grammar) {
//	cfg->loadCFG(file_cfg, file_stmt);
//	cout << *cfg;
//
//	graphstore->loadGraphStore(file_singleton);
//	cout << *graphstore << endl;
//
//	singletons->loadSingletonSet(file_singleton);
//	cout << *singletons << endl;
//
//    /* TODO: load grammar from file
//     * grammar->loadGrammar(filename);
//     */
//    grammar->loadGrammar(file_grammar.c_str());
//
//    return true;
//}






