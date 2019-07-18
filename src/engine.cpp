/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include "comp/cfg_compute_ooc_asyn.h"
using namespace std;

const string dir = "/home/zqzuo/Desktop/Ouroboros-dataset-master/httpd/";
const string file_cfg = dir + "final";
const string file_stmts = dir + "id_stmt_info.txt";
const string file_singletons = dir + "var_singleton_info.txt";
const string file_grammar = "/home/zqzuo/Desktop/Ouroboros-dataset-master/rules_pointsto.txt";

/* function declaration */
void compute(Partition partition, Context* context);
void compute_inmemory();
void run();


int main() {
	run();
//	compute_inmemory();
	return 0;
}



void compute(Partition partition, Context* context){
	CFG *cfg = new CFG_map_outcore();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();

    //get the flag for adding self-loop edges
    bool flag = context->getFlag(partition);
	context->setFlag(partition);

    CFGCompute_ooc::load(partition, cfg, singletons, graphstore, context);
    CFGCompute_ooc::do_worklist_ooc(cfg, graphstore, context->getGrammar(), singletons, actives, flag);
//    CFGCompute_ooc_asyn::do_worklist_ooc_asynchronous(cfg, graphstore, context->getGrammar(), singletons, actives, flag);
	CFGCompute_ooc::pass(partition, cfg, graphstore, actives, context);

	delete cfg;
	delete graphstore;
	delete actives;
	delete singletons;
}

void run(){
	//preprocessing
	int num_partitions = 5000;
	long totol_nodes = 2120914;
	Context* context = new Context(num_partitions, totol_nodes, file_cfg, file_stmts, file_singletons, file_grammar);
	Preprocess::process(*context);

	//iterative computation
	Partition partition;
	while(context->schedule(partition)){
		compute(partition, context);
	}

	delete context;
}



void compute_inmemory(){
	CFG *cfg = new CFG_map();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();


	CFGCompute::load(file_cfg, file_stmts, cfg, file_singletons, singletons, graphstore, file_grammar, grammar);
//	CFGCompute::do_worklist_synchronous(cfg, graphstore, grammar, singletons, true);
	CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons, true);

	delete cfg;
	delete graphstore;
	delete grammar;
	delete singletons;

}




