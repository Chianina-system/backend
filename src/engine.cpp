/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

//#include <iostream>
#include "comp/cfg_compute_asyn.h"
#include "comp/cfg_compute_ooc.h"
//#include "../preproc/preprocess.h"
using namespace std;

const string file_cfg = "/home/zqzuo/Desktop/inlined/final";
const string file_stmts = "/home/zqzuo/Desktop/inlined/id_stmt_info.txt";
const string file_singletons = "/home/zqzuo/Desktop/inlined/var_singleton_info.txt";
const string file_grammar = "/home/zqzuo/Desktop/inlined/rules_pointsto.txt";

/* function declaration */
void preprocess();
void compute(Partition partition, Context* context);
void compute_inmemory();


int main() {

	return 0;
}



void compute(Partition partition, Context* context){
	CFG *cfg = new CFG_map_outcore();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();

    CFGCompute_ooc::load(partition, cfg, singletons, graphstore, context);
    CFGCompute_ooc::do_worklist_ooc(cfg, graphstore, context->getGrammar(), singletons, actives);
	CFGCompute_ooc::pass(partition, cfg, graphstore, actives, context);

	delete cfg;
	delete graphstore;
	delete actives;
	delete singletons;
}

void run(){
	//preprocessing
	Context* context = new Context(2, 9);
	Preprocess::process(*context);

	//iterative computation
	while(Partition partition = context->schedule() != -1){
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
	CFGCompute::do_worklist_synchronous(cfg, graphstore, grammar, singletons);
//	CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons);

	delete cfg;
	delete graphstore;
	delete grammar;
	delete singletons;
}

void preprocess(){

}



