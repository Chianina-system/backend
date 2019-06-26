/*
 * cfg_compute.cpp
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */


#include "cfg_compute.h"
#include "edgesToDelete.h"
#include "art.h"


bool CFGCompute::load(Partition* part, CFG* cfg, GraphStore* graphstore){

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
            comp_threads.push_back(std::thread( [=] {this->compute(cfg, graphstore, worklist_1, worklist_2);}));

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
        PEGraph* out = transfer(in, cfg_node->getStmt(),grammar);

//        // clean in
//        delete in;

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
            out = PEGraph::merge(out, out_graph);
        }

        return out;
    }

}

PEGraph* CFGCompute::transfer_copy(PEGraph* in, Stmt* stmt,Grammar *grammar){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar,vertices_affected);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_load(PEGraph* in, Stmt* stmt,Grammar *grammar){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_store(PEGraph* in, Stmt* stmt,Grammar *grammar){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    if(is_strong_update(stmt->getDst(),out,grammar)) {
        strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected);
    }
    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

PEGraph* CFGCompute::transfer_address(PEGraph* in, Stmt* stmt,Grammar *grammar){
//    PEGraph* out = new PEGraph(in);
	PEGraph* out = in;

    // the KILL set
    std::set<vertexid_t> vertices_changed;
    std::set<vertexid_t> vertices_affected;
    strong_update(stmt->getDst(),out,vertices_changed,grammar, vertices_affected);

    // the GEN set
    peg_compute_add(out,stmt,grammar);

    return out;
}

bool CFGCompute::is_strong_update(vertexid_t x,PEGraph *out,Grammar *grammar) {
    /* If there exists one and only one variable o,which
     * refers to a singleton memory location,such that x and o are memory alias
     */
    int numOfSingleTon = 0;
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i]) && out->isSingleton(edges[i]))
            ++numOfSingleTon;
    }
    return (numOfSingleTon == 1);
}

void CFGCompute::strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices_changed,Grammar *grammar, std::set<vertexid_t> &vertices_affected) {
    // vertices <- must_alias(x); put *x into this set as well
    must_alias(x,out,vertices_changed,grammar,vertices_affected);

    /* foreach v in vertices do
     * delete all the direct incoming and outgoing a edges of v from OUT
     * delete all the incoming and outgoing M or V edges of v induced previously by the deleted a edges from OUT
     */
//    vertexid_t numVertices = out->getNumVertices();
    std::unordered_map<int, EdgeArray*> m;

    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); it++){
        int numEdges = out->getNumEdges(it->first);
        vertexid_t *edges = out->getEdges(it->first);
        label_t *labels = out->getLabels(it->first);
        for(int j = 0; j <numEdges; j++){
            if(isDirectAssignEdges(it->first,edges[j],labels[j],vertices_changed,grammar)) {
                // delete the direct incoming and outgoing assign edges
                if(m.find(it->first)==m.end())
                    m[it->first] = new EdgeArray;
                m[it->first]->addOneEdge(edges[j],labels[j]);
            }
        }
    }

    // execute the add edge operation. the oldsSet is out - m, the deltasSdge is m
    peg_compute_delete(out, grammar, m);

    /* remove edges */
    for(auto it = m.begin(); it!= m.end(); it++){
        int src = it->first;
//        m[src]->merge();

        //get all the deleted edges
        findDeletedEdge(it->second, src, vertices_changed, vertices_affected);

        if(m[src]->getSize()){
            int n1 = out->getNumEdges(src);
            int n2 = m[src]->getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges,labels,n1,out->getEdges(src),out->getLabels(src),n2,m[src]->getEdges(),m[src]->getLabels());
            if(len)
                out->setEdgeArray(src,len,edges,labels);
            else
                out->clearEdgeArray(src);

            delete[] edges; delete[] labels;
            m[src]->clear();
        }
    }

//    for(auto it = out->getGraph().begin(); it!= out->getGraph().end(); it++){
//        int i = it->first;
//        if (m.find(i)!=m.end()){
//            m[i]->merge();
//            //m[i] only store the edges relevanted to vertices set
//            m[i] = m[i]->findDeletedEdge(i, vertices);
//            int len = 0; int n1 = out->getNumEdges(i); int n2 = m[i]->getRealNumEdges();
//            vertexid_t *edges = new vertexid_t[n1];
//            label_t *labels = new label_t[n1];
//            myalgo::minusTwoArray(len,edges,labels,n1,out->getEdges(i),out->getLabels(i),n2,m[i]->getEdges(),m[i]->getLabels());
//            if(len)
//                out->setEdgeArray(i,len,edges,labels);
//            else
//                out->clearEdgeArray(i);
//
//            delete[] edges; delete[] labels;
//            m[i]->clear();
//        }
//    }
}

bool CFGCompute::isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar) {
    if(!grammar->isAssign(label))
        return false;
    return ( (vertices.find(src) != vertices.end()) || (vertices.find(dst) != vertices.end()) );
}


void CFGCompute::must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices_changed,Grammar *grammar, std::set<vertexid_t> &vertices_affected) {
	/* if there exists one and only one variable o,which
	 * refers to a singleton memory location,such that x and
	 * y are both memory aliases of o,then x and y are Must-alias
	 */
    int numEdges = out->getNumEdges(x);
    vertexid_t * edges = out->getEdges(x);
    label_t *labels = out->getLabels(x);

    int numOfSingleTon = 0;
    for(int i = 0;i < numEdges;++i) {
        if(grammar->isMemoryAlias(labels[i])){
        	vertices_changed.insert(edges[i]);
        	if(out->isSingleton(edges[i])){
        		++numOfSingleTon;
        	}
        }
    }

    //check the number of singletons
    if(numOfSingleTon == 0){
    	perror("invalid number of singletons!");
    }
    else if(numOfSingleTon == 1){

    }
    else{
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

void CFGCompute::peg_compute_delete(PEGraph *out, Grammar *grammar, std::unordered_map<int, EdgeArray*>& m) {
    // add assgin edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_delete(out, m);

    // start GEN
    PEGCompute pegCompute;
    pegCompute.startCompute_delete(*compset,grammar, m);

//    // KILL fininshed, compset -> out
//    vertexid_t numVertices = out->getNumVertices();

    // clean
    delete compset;
}

void CFGCompute::peg_compute_add(PEGraph *out,Stmt *stmt,Grammar *grammar) {
    // add assign edge based on stmt, (out,assign edge) -> compset
    ComputationSet *compset = new ComputationSet();
    compset->init_add(out,stmt);

    // start GEN
    PEGCompute pegCompute;
    pegCompute.startCompute_add(*compset,grammar);

    // GEN finished, compset -> out
//    vertexid_t numVertices = out->getNumVertices();
//    for(vertexid_t i = 0;i < numVertices;++i) {
//        if(compset->getOldsNumEdges(i))
//            out->setEdgeArray(i,compset->getOldsNumEdges(i),compset->getOldsEdges(i),compset->getOldsLabels(i));
//        else
//            out->clearEdgeArray(i);
//    }
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

//void CFGCompute::initComputationSet_add(ComputationSet &compset,PEGraph *out,Stmt *stmt) {
//    compset.init_add(out,stmt);
//}
//
//void CFGCompute::initComputationSet_delete(ComputationSet &compset, PEGraph *out, std::unordered_map<int, EdgesToDelete *> &m) {
//    compset.init_delete(out, m);
//}

void CFGCompute::findDeletedEdge(EdgeArray *edgesToDelete, int src, std::set<vertexid_t> &vertices, std::set<vertexid_t> &vertices_affected) {
    // src is in the vertices set
    if (vertices.find(src) != vertices.end()) {
        return ;
    } else {
        EdgeArray* _edgesToDelete = new EdgeArray;
        for (int i = 0; i < edgesToDelete->getSize(); ++i) {
            if (vertices.find(edgesToDelete->getEdges()[i]) != vertices.end()) {
                _edgesToDelete->addOneEdge(edgesToDelete->getEdges()[i], edgesToDelete->getLabels()[i]);
            }
        }
//        _edgesToDelete->setRealNumEdges( _edgesToDelete->getSize() ) ;
//        edgesToDelete->clear();
        edgesToDelete->set(_edgesToDelete->getSize(), _edgesToDelete->getEdges(), _edgesToDelete->getLabels() );
//        edgesToDelete->setRealNumEdges(_edgesToDelete->getSize() );
        _edgesToDelete->clear();
    }
}

// 使用tab键分割
bool CFGCompute::load(const string& file_cfg, const string& file_stmt,const string& file_singleton,  CFG *cfg, GraphStore *graphstore) {
    // handle the stmt file
    std::ifstream fin;
    CFG_map* cfgMap = dynamic_cast<CFG_map* > (cfg);

    fin.open(file_stmt);
    if(!fin) {
        cout << "can't load file_stmt " << endl;
        return false;
    }

    std::map<int, CFGNode*> m;

    std::string line;
    while (getline(fin, line)) {
        std::stringstream stream(line);
        std::string stmt_id, type, dst, src, added;
        stream >> stmt_id >> type >> dst >> src >> added;

        TYPE t;
        if(type == "assign"){
            t = TYPE::Assign;
        }
        if(type == "load"){
            t = TYPE::Load;
        }
        if(type == "store"){
            t = TYPE ::Store;
        }
        if(type == "alloca"){
            t = TYPE ::Alloca;
        }

        Stmt* stmt = new Stmt(t, atoi(src.c_str()), atoi(dst.c_str()), atoi(added.c_str()));
        CFGNode* cfgNode = new CFGNode(atoi(stmt_id.c_str()), stmt);
        m[atoi(stmt_id.c_str())] = cfgNode;
    }

    //handle the cfg.txt
    fin.open(file_cfg);
    if(!fin) {
        cout << "can't load file_cfg: " << file_cfg << endl;
        return false;
    }

    while (getline(fin, line)) {
        std::stringstream stream(line);
        std::string pred, succ;
        stream >> pred >> succ;

        cfgMap->addOneNode(m[atoi(pred.c_str())]);
        cfgMap->addOneNode(m[atoi(succ.c_str())]);
        cfgMap->addOneSucc(m[atoi(pred.c_str())], m[atoi(succ.c_str())]);
        cfgMap->addOnePred(m[atoi(succ.c_str())], m[atoi(pred.c_str())]);
    }


    //handle the singleton.txt
    fin.open(file_singleton);
    if(!fin) {
        cout << "can't load file_singleton: " << file_cfg << endl;
        return false;
    }
    ART* art = dynamic_cast<ART* > (graphstore);
    while (getline(fin, line)) {
        std::stringstream stream(line);
        std::string id;
        stream >> id;
        art->addOneSingleton(atoi(id.c_str()));
    }

    return true;
}






