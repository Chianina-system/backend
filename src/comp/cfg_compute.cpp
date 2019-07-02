/*
 * cfg_compute.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */


#include "cfg_compute.h"
#include "edgesToDelete.h"
#include "art.h"
#include "cfg_map.h"
#include "naive_graphstore.h"
#include "../utility/StaticPrinter.h"
#include "../utility/Logger.hpp"

bool CFGCompute::load(Partition* part, CFG* cfg, GraphStore* graphstore){
	return true;
}

void CFGCompute::do_worklist(CFG* cfg, GraphStore* graphstore, Grammar* grammar){
	Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", level_log_1);

    Concurrent_Worklist* worklist_1 = new Concurrent_Workset();

//    //for debugging
//    cout << cfg << endl;

    //initiate concurrent worklist
    std::vector<CFGNode*> nodes = cfg->getNodes();

//    //for debugging
//    cout << nodes.size();
//    StaticPrinter::print_vector(nodes);

    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
//    	//for debugging
//    	cout << **it << endl;
        worklist_1->push_atomic(*it);
    }


    Concurrent_Worklist* worklist_2 = new Concurrent_Workset();
    while(!worklist_1->isEmpty()){
        std::vector<std::thread> comp_threads;
        for (unsigned int i = 0; i < num_threads; i++)
            comp_threads.push_back(std::thread( [=] {compute(cfg, graphstore, worklist_1, worklist_2, grammar);}));

        for (auto &t : comp_threads)
            t.join();

        assert(worklist_1->isEmpty());
        Concurrent_Worklist* worklist_tmp = worklist_1;
        worklist_1 = worklist_2;
        worklist_2 = worklist_tmp;
        assert(worklist_2->isEmpty());

        //for debugging
        Logger::print_thread_info_locked("-----------------------------------------------------------------------------------------------------------------------------\n\n\n", level_log_1);
    }

    //clean
    delete(worklist_1);
    delete(worklist_2);

    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", level_log_1);
}


void CFGCompute::compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist* worklist_1, Concurrent_Worklist* worklist_2, Grammar* grammar){
    while(CFGNode* cfg_node = worklist_1->pop_atomic()){
    	//for debugging
//    	cout << "\nCFG Node under processing: " << *cfg_node << endl;
    	Logger::print_thread_info_locked("----------------------- CFG Node "
    			+ to_string(cfg_node->getCfgNodeId())
				+ " {" + cfg_node->getStmt()->toString()
				+ "} start processing -----------------------\n", level_log_cfgnode);

        //merge
    	std::vector<CFGNode*> preds = cfg->getPredesessors(cfg_node);
//        //for debugging
//    	StaticPrinter::print_vector(preds);
        PEGraph* in = combine(graphstore, preds);

        //for debugging
        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString() + "\n", level_log_peg);

        //transfer
        PEGraph* out = transfer(in, cfg_node->getStmt(), grammar, graphstore);

        //for debugging
        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString() + "\n", level_log_peg);

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
//        //for debugging
//        cout << *graphstore << endl;
        PEGraph* old_out = graphstore->retrieve(out_pointer);
//        //for debugging
//        cout << *graphstore << endl;

        bool isEqual = out->equals(old_out);
        //for debugging
        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", level_log);
        if(!isEqual){
            //update out
            graphstore->update(out_pointer, out);

            //propagate
            std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
            for(auto it = successors.cbegin(); it != successors.cend(); ++it){
                worklist_2->push_atomic(*it);
            }
        }

        //clean out
        delete old_out;
        delete out;

        //for debugging
        Logger::print_thread_info_locked(graphstore->toString() + "\n", level_log_graphstore);
        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", level_log_cfgnode);

        //for debugging
        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", level_log_worklist);
    }
}



PEGraph* CFGCompute::combine(GraphStore* graphstore, std::vector<CFGNode*>& preds){
    if(preds.size() == 0){//entry node
        //return an empty graph
        return new PEGraph();
    }
    else if(preds.size() == 1){
        CFGNode* pred = preds[0];
        PEGraph_Pointer out_pointer = pred->getOutPointer();
        PEGraph* out_graph = graphstore->retrieve(out_pointer);

        return out_graph;
    }
    else{
        PEGraph* out = new PEGraph();

        for(auto it = preds.cbegin(); it != preds.cend(); it++){
            CFGNode* pred = *it;
            PEGraph_Pointer out_pointer = pred->getOutPointer();
            PEGraph* out_graph = graphstore->retrieve(out_pointer);
//            out = PEGraph::merge(out, out_graph);
            out->merge(out_graph);
            delete out_graph;
        }

        return out;
    }
}

PEGraph* CFGCompute::transfer_copy(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
	//for debugging
	Logger::print_thread_info_locked("transfer-copy starting...\n", level_log_function);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

	//for debugging
	Logger::print_thread_info_locked("transfer-copy finished.\n", level_log_function);

    return out;
}

PEGraph* CFGCompute::transfer_load(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
	//for debugging
	Logger::print_thread_info_locked("transfer-load starting...\n", level_log_function);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

	//for debugging
	Logger::print_thread_info_locked("transfer-load finished.\n", level_log_function);

    return out;
}

PEGraph* CFGCompute::transfer_store(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
	//for debugging
	Logger::print_thread_info_locked("transfer-store starting...\n", level_log_function);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    if(is_strong_update(stmt->getDst(),out,grammar, graphstore)) {
        strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, graphstore);
    }

//    //for debugging
//    cout << *out << endl;

    // the GEN set
    peg_compute_add(out,stmt,grammar);

	//for debugging
	Logger::print_thread_info_locked("transfer-store finished.\n", level_log_function);

    return out;
}

PEGraph* CFGCompute::transfer_address(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
	//for debugging
	Logger::print_thread_info_locked("transfer-alloc starting...\n", level_log_function);

//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
	strong_update(stmt->getDst(), out, vertices_changed, grammar, vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out, stmt, grammar);

    //for debugging
   	Logger::print_thread_info_locked("transfer-alloc finished.\n", level_log_function);

    return out;
}

bool CFGCompute::is_strong_update(vertexid_t x, PEGraph *out, Grammar *grammar, GraphStore* graphstore) {
	//for debugging
	Logger::print_thread_info_locked("is-strong-update starting...\n", level_log_function);

    /* If there exists one and only one variable o,which
     * refers to a singleton memory location,such that x and o are memory alias
     */
	if(out->getGraph().find(x) == out->getGraph().end()){
		//for debugging
		Logger::print_thread_info_locked("is-strong-update finished.\n", level_log_function);
		return false;
	}

    int numOfSingleTon = 0;
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i]) && graphstore->isSingleton(edges[i]))
            ++numOfSingleTon;
    }

	//for debugging
	Logger::print_thread_info_locked("is-strong-update finished.\n", level_log_function);

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
	else if(vertices_affected.find(src) != vertices_affected.end() || vertices_affected.find(dst) != vertices_affected.end()){
		return !grammar->isDereference_bidirect(label) && !grammar->isAssign_bidirect(label);
	}

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
}

void CFGCompute::getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>& m) {
	//get all the direct assign-bidirection edges into m
	//TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
	for (auto it = out->getGraph().begin(); it != out->getGraph().end(); it++) {
		int numEdges = out->getNumEdges(it->first);
		vertexid_t* edges = out->getEdges(it->first);
		label_t* labels = out->getLabels(it->first);
		for (int j = 0; j < numEdges; j++) {
			if (isDirectAssignEdges(it->first, edges[j], labels[j], vertices_changed, grammar)) {
				// delete the direct incoming and outgoing assign edges
				if (m.find(it->first) == m.end()) {
					m[it->first] = EdgeArray();
				}
				m[it->first].addOneEdge(edges[j], labels[j]);
			}
		}
	}


}

void CFGCompute::strong_update(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, GraphStore* graphstore) {
	//for debugging
	Logger::print_thread_info_locked("strong-update starting...\n", level_log_function);

	if(out->getGraph().find(x) == out->getGraph().end()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", level_log_function);
		return;
	}

    // vertices <- must_alias(x); put *x into this set as well
	must_alias(x, out, vertices_changed, grammar, vertices_affected, graphstore);

    //get all the direct assign edges into m
    //TODO: can be optimized by checking only the adjacent list of vertex in vertices_changed?
    std::unordered_map<vertexid_t, EdgeArray> m;
	getDirectAssignEdges(out, vertices_changed, grammar, m);

	if(m.empty()){
		//for debugging
		Logger::print_thread_info_locked("strong-update finished.\n", level_log_function);
		return;
	}

    // execute the edge addition operation. the oldsSet is out - m, the deltasSet is m
    peg_compute_delete(out, grammar, m);

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with a vertex within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdges(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

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
	Logger::print_thread_info_locked("strong-update finished.\n", level_log_function);
}

bool CFGCompute::isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar) {
    if(!grammar->isAssign_bidirect(label))
        return false;
    return ( (vertices.find(src) != vertices.end()) || (vertices.find(dst) != vertices.end()) );
}


void CFGCompute::must_alias(vertexid_t x, PEGraph *out, std::set<vertexid_t> &vertices_changed, Grammar *grammar, std::set<vertexid_t> &vertices_affected, GraphStore* graphstore) {
	/* if there exists one and only one variable o,which
	 * refers to a singleton memory location,such that x and
	 * y are both memory aliases of o,then x and y are Must-alias
	 */
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    int numOfSingleTon = 0;
	if(graphstore->isSingleton(x)){
		++numOfSingleTon;
	}
    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i])){
        	vertices_changed.insert(edges[i]);
        	if(graphstore->isSingleton(edges[i])){
        		++numOfSingleTon;
        	}
        }
    }

    //check the number of singletons
    if(numOfSingleTon == 0){
    	perror("invalid number of singletons!");
    }
//    else if(numOfSingleTon == 1){
//
//    }
    else if(numOfSingleTon > 1){
    	vertices_changed.clear();
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

void CFGCompute::peg_compute_delete(PEGraph *out, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>& m) {
    // add assgin edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_delete(out, m);

    // start GEN
    long number_deleted = PEGCompute::startCompute_delete(compset,grammar, m);
    Logger::print_thread_info_locked("number of edges deleted: " + std::to_string(number_deleted) + "\n", level_log);

    // clean
    delete compset;
}

void CFGCompute::removeExistingEdges(const EdgeArray& edges_src, vertexid_t src, PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>& m) {
	//remove the existing edges
	int n1 = edges_src.getSize();
	auto* edges = new vertexid_t[n1];
	auto* labels = new label_t[n1];
	int len = myalgo::minusTwoArray(edges, labels, edges_src.getSize(),
			edges_src.getEdges(), edges_src.getLabels(), out->getNumEdges(src),
			out->getEdges(src), out->getLabels(src));
	if (len) {
		m[src] = EdgeArray();
		m[src].set(len, edges, labels);
	}
	delete[] edges;
	delete[] labels;
}

void CFGCompute::getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>& m){
    //'a', '-a', 'd', '-d', and self-loop edges
    vertexid_t src = stmt->getSrc();
    EdgeArray edges_src = EdgeArray();

    vertexid_t dst = stmt->getDst();
    EdgeArray edges_dst = EdgeArray();

    vertexid_t added = stmt->getAdded();
    EdgeArray edges_added = EdgeArray();

    //'a', '-a'
    edges_src.addOneEdge(dst, grammar->getLabelValue("a"));
    edges_dst.addOneEdge(src, grammar->getLabelValue("-a"));

    //'d', '-d'
    switch(stmt->getType()){
    case TYPE::Alloca:
    	edges_src.addOneEdge(added, grammar->getLabelValue("d"));
    	edges_added.addOneEdge(src, grammar->getLabelValue("-d"));
    	break;
    case TYPE::Store:
    	edges_added.addOneEdge(dst, grammar->getLabelValue("d"));
    	edges_dst.addOneEdge(added, grammar->getLabelValue("-d"));
    	break;
    case TYPE::Load:
    	edges_added.addOneEdge(src, grammar->getLabelValue("d"));
    	edges_src.addOneEdge(added, grammar->getLabelValue("-d"));
    	break;
    default:
    	break;
    }

    //self-loop edges
    for(int i = 0; i < grammar->getNumErules(); ++i){
        char label = grammar->getErule(i);
    	edges_src.addOneEdge(src, label);
    	edges_dst.addOneEdge(dst, label);
    	if(stmt->isValidAdded()){
    		edges_added.addOneEdge(added, label);
    	}
    }

    //merge and sort
    edges_src.merge();
    edges_dst.merge();
    if(stmt->isValidAdded()){
		edges_added.merge();
    }

    //remove the existing edges
	removeExistingEdges(edges_src, src, out, m);
	removeExistingEdges(edges_dst, dst, out, m);
    if(stmt->isValidAdded()){
    	removeExistingEdges(edges_added, added, out, m);
    }
}

void CFGCompute::peg_compute_add(PEGraph *out, Stmt *stmt, Grammar *grammar) {
	//for debugging
	Logger::print_thread_info_locked("peg-compute-add starting...\n", level_log_function);

	bool isConservative = false;

	std::unordered_map<vertexid_t, EdgeArray> m;
	getDirectAddedEdges(out, stmt, grammar, m);
	if(m.empty() && !isConservative){// no new edges directly added
		return;
	}

    // add assign edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_add(out, m, isConservative);

//    //for debugging
//    cout << *compset << endl;

    // start GEN
    PEGCompute::startCompute_add(compset, grammar);
//    Logger::print_thread_info_locked("number of edges added: " + to_string(number_added) + "\n");

//    //for debugging
//    cout << *compset << endl;

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

	//for debugging
	Logger::print_thread_info_locked("peg-compute-add finished.\n", level_log_function);
}

// 使用tab键分割
bool CFGCompute::load(const string& file_cfg, const string& file_stmt, CFG *cfg, const string& file_singleton, GraphStore *graphstore, const string& file_grammar, Grammar * grammar) {
	cfg->loadCFG(file_cfg, file_stmt);
	cout << *cfg;

	graphstore->loadGraphStore(file_singleton);
	cout << *graphstore << endl;

    /* TODO: load grammar from file
     * grammar->loadGrammar(filename);
     */
    grammar->loadGrammar(file_grammar.c_str());



    return true;
}






