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

bool CFGCompute::load(Partition* part, CFG* cfg, GraphStore* graphstore){
	return true;
}

void CFGCompute::do_worklist(CFG* cfg, GraphStore* graphstore){
    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Worklist<CFGNode*>();

    //initiate concurrent worklist
    std::vector<CFGNode*> nodes = cfg->getNodes();
    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
        worklist_1->push(*it);
    }

    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Worklist<CFGNode*>();
    while(!worklist_1->isEmpty()){
        std::vector<std::thread> comp_threads;
        for (unsigned int i = 0; i < num_threads; i++)
            comp_threads.push_back(std::thread( [=] {compute(cfg, graphstore, worklist_1, worklist_2);}));

        for (auto &t : comp_threads)
            t.join();

        assert(worklist_1->isEmpty());
        Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
        worklist_1 = worklist_2;
        worklist_2 = worklist_tmp;
        assert(worklist_2->isEmpty());
    }

    //clean
    delete(worklist_1);
    delete(worklist_2);
}


void CFGCompute::compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2){
    Grammar *grammar = new Grammar();
    /* TODO: load grammar from file
     * grammar->loadGrammar(filename);
     */

    CFGNode* cfg_node;
    while(worklist_1->pop_atomic(cfg_node)){
        //merge
    	std::vector<CFGNode*> preds = cfg->getPredesessors(cfg_node);
        PEGraph* in = combine(graphstore, preds);

        //transfer
        PEGraph* out = transfer(in, cfg_node->getStmt(),grammar, graphstore);

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
        PEGraph* old_out = graphstore->retrieve(out_pointer);
        if(!out->equals(old_out)){
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
    }
}



PEGraph* CFGCompute::combine(GraphStore* graphstore, std::vector<CFGNode*>& preds){
    //get the predecessors of node
//    std::vector<CFGNode*> preds = cfg->getPredesessors(node);

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
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar,vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_load(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_store(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    if(is_strong_update(stmt->getDst(),out,grammar, graphstore)) {
        strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, graphstore);
    }
    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_address(PEGraph* in, Stmt* stmt,Grammar *grammar, GraphStore* graphstore){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected, graphstore);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

bool CFGCompute::is_strong_update(vertexid_t x,PEGraph *out,Grammar *grammar, GraphStore* graphstore) {
    /* If there exists one and only one variable o,which
     * refers to a singleton memory location,such that x and o are memory alias
     */
    int numOfSingleTon = 0;
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i]) && graphstore->isSingleton(edges[i]))
            ++numOfSingleTon;
    }
    return (numOfSingleTon == 1);
}

bool isDeletable(vertexid_t src, vertexid_t dst, char label, std::set<vertexid_t> &vertices_changed, std::set<vertexid_t> &vertices_affected, Grammar *grammar){
	if(vertices_changed.find(src) != vertices_changed.end() || vertices_changed.find(dst) != vertices_changed.end()){
		return !grammar->isDereference_bidirect(label);
	}
	else if(vertices_affected.find(src) != vertices_affected.end() || vertices_affected.find(dst) != vertices_affected.end()){
		return !grammar->isDereference_bidirect(label) && !grammar->isAssign_bidirect(label);
	}

	return false;
}

/* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with that within vertices_changed,
 * and all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
 * * */
void findDeletedEdge(EdgeArray & edgesToDelete, vertexid_t src, std::set<vertexid_t> &vertices_changed, std::set<vertexid_t> &vertices_affected, Grammar *grammar, EdgeArray & deleted) {
	for (int i = 0; i < edgesToDelete.getSize(); ++i) {
		vertexid_t dst = edgesToDelete.getEdges()[i];
		char label = edgesToDelete.getLabels()[i];
		if(isDeletable(src, dst, label, vertices_changed, vertices_affected, grammar)){
			deleted.addOneEdge(dst, label);
		}
	}
}

void CFGCompute::strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices_changed,Grammar *grammar, std::set<vertexid_t> &vertices_affected, GraphStore* graphstore) {
    // vertices <- must_alias(x); put *x into this set as well
    must_alias(x,out,vertices_changed,grammar,vertices_affected, graphstore);

    /* foreach v in vertices do
     * delete all the direct incoming and outgoing a edges of v from OUT
     * delete all the incoming and outgoing M or V edges of v induced previously by the deleted a edges from OUT
     */
//    vertexid_t numVertices = out->getNumVertices();
    std::unordered_map<vertexid_t, EdgeArray> m;

    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); it++){
        int numEdges = out->getNumEdges(it->first);
        vertexid_t *edges = out->getEdges(it->first);
        label_t *labels = out->getLabels(it->first);
        for(int j = 0; j <numEdges; j++){
            if(isDirectAssignEdges(it->first,edges[j],labels[j],vertices_changed,grammar)) {
                // delete the direct incoming and outgoing assign edges
                if(m.find(it->first)==m.end()){
                    m[it->first] = EdgeArray();
                }
                m[it->first].addOneEdge(edges[j],labels[j]);
            }
        }
    }

    // execute the add edge operation. the oldsSet is out - m, the deltasSdge is m
    peg_compute_delete(out, grammar, m);

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
        vertexid_t src = it->first;

        /* delete all the ('a', '-a', 'V', 'M', and other temp labels) edges associated with that within vertices_changed, and
         * all the ('V', 'M', and other temp labels) edges associated with that within vertices_affected
         * */
        EdgeArray deletedArray = EdgeArray();
		findDeletedEdge(it->second, src, vertices_changed, vertices_affected, grammar, deletedArray);

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
}

bool CFGCompute::isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar) {
    if(!grammar->isAssign(label))
        return false;
    return ( (vertices.find(src) != vertices.end()) || (vertices.find(dst) != vertices.end()) );
}


void CFGCompute::must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices_changed,Grammar *grammar, std::set<vertexid_t> &vertices_affected, GraphStore* graphstore) {
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

//void CFGCompute::must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar) {
//    /* if there exists one and only one variable o,which
//     * refers to a singleton memory location,such that x and
//     * y are both memory aliases of o,then x and y are Must-alias
//     */
//    vertexid_t o ;
//    int numOfSingleTon = 0;
////    int numEdges = out->getNumEdges(x);
//    vertexid_t *edges = out->getEdges(x);
//    label_t *labels = out->getLabels(x);
//
//    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); it++){
//        vertexid_t i= it->first;
//        if(grammar->isMemory(labels[i]) && out->isSingleton(edges[i])) {
//            ++numOfSingleTon;
//            o = edges[i];
//        }
//    }
//
//    if(numOfSingleTon != 1)
//        return;
//
//    int numEdges = out->getNumEdges(o);
//    edges = out->getEdges(o);
//    labels = out->getLabels(o);
//
//    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); it++){
//        int i = it->first;
//        if(grammar->isMemory(labels[i]) && (edges[i] != x) && is_strong_update(edges[i],out,grammar))
//            vertices.insert(edges[i]);
//    }
//}

void CFGCompute::peg_compute_delete(PEGraph *out, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>& m) {
    // add assgin edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_delete(out, m);

    // start GEN
    PEGCompute::startCompute_delete(*compset,grammar, m);

    // clean
    delete compset;
}

void CFGCompute::peg_compute_add(PEGraph *out,Stmt *stmt,Grammar *grammar) {
    // add assign edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_add(out,stmt);

    // start GEN
    PEGCompute::startCompute_add(*compset,grammar);

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
}

// 使用tab键分割
bool CFGCompute::load(const string& file_cfg, const string& file_stmt, const string& file_singleton, CFG *cfg, GraphStore *graphstore) {
//    // handle the stmt file
//    std::ifstream fin;
//    CFG_map* cfgMap = dynamic_cast<CFG_map* > (cfg);
////    CFG * cfgMap = cfg;
//
//    fin.open(file_stmt);
//    if(!fin) {
//        cout << "can't load file_stmt " << endl;
//        return false;
//    }
//
//    std::map<int, CFGNode*> m;
//
//    std::string line;
//    while (getline(fin, line) && line != "") {
//    	std::cout << line << "\n";
//
//        std::stringstream stream(line);
//        std::string stmt_id, type, dst, src, added;
//        stream >> stmt_id >> type >> dst >> src >> added;
//
//        std::cout << stmt_id << "," << type << "," << dst << "," << src << "," << added << "\n";
//
//        TYPE t;
//        if(type == "assign"){
//            t = TYPE::Assign;
//        }
//        if(type == "load"){
//            t = TYPE::Load;
//        }
//        if(type == "store"){
//            t = TYPE ::Store;
//        }
//        if(type == "alloca"){
//            t = TYPE ::Alloca;
//        }
//
//        Stmt* stmt = new Stmt(t, atoi(src.c_str()), atoi(dst.c_str()), atoi(added.c_str()));
//        CFGNode* cfgNode = new CFGNode(atoi(stmt_id.c_str()), stmt);
//        m[atoi(stmt_id.c_str())] = cfgNode;
//
//        //add cfgnode into cfg
//        cfgMap->addOneNode(cfgNode);
//    }
//    fin.close();
//
//
//    //handle the cfg.txt
////    std::ifstream fin;
//    fin.open(file_cfg);
//    if(!fin) {
//        cout << "can't load file_cfg: " << file_cfg << endl;
//        return false;
//    }
//
//    while (getline(fin, line) && line != "") {
//        std::stringstream stream(line);
//        std::string pred_id, succ_id;
//        stream >> pred_id >> succ_id;
//
////        cfgMap->addOneNode(m[atoi(pred_id.c_str())]);
////        cfgMap->addOneNode(m[atoi(succ_id.c_str())]);
//        cfgMap->addOneSucc(m[atoi(pred_id.c_str())], m[atoi(succ_id.c_str())]);
//        cfgMap->addOnePred(m[atoi(succ_id.c_str())], m[atoi(pred_id.c_str())]);
//    }
//    fin.close();
//
//    std::cout << *cfgMap;

	cfg = new CFG_map(file_cfg, file_stmt);

//    //handle the singleton.txt
//    std::ifstream fin;
//    fin.open(file_singleton);
//    if(!fin) {
//        cout << "can't load file_singleton: " << file_singleton << endl;
//        return false;
//    }
//    ART* art = dynamic_cast<ART* > (graphstore);
//    std::string line;
//    while (getline(fin, line) && line != "") {
//        std::stringstream stream(line);
//        std::string id;
//        stream >> id;
//        art->addOneSingleton(atoi(id.c_str()));
//    }
//    fin.close();
	graphstore = new NaiveGraphStore(file_singleton);


    return true;
}






