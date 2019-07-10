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

/* function declaration */
void preprocess();
Partition schedule();
void compute(Partition partition, Grammar* grammar);
void compute_inmemory();


int main() {

	return 0;
}



void compute(Partition partition, Grammar* grammar){
	CFG *cfg = new CFG_map_outcore();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();

    CFGCompute_ooc::load(partition, cfg, singletons, graphstore);
    CFGCompute_ooc::do_worklist_ooc(cfg, graphstore, grammar, singletons, actives);
	CFGCompute_ooc::pass(partition, cfg, graphstore, actives);

	delete cfg;
	delete graphstore;
	delete actives;
	delete singletons;

}

void run(){
	/* TODO: load grammar from file grammar->loadGrammar(filename) */
	Grammar* grammar = new Grammar();
	grammar->loadGrammar("");

	//preprocessing
	preprocess();

	//iterative computation
	while(true){
		Partition partition = schedule();

		compute(partition, grammar);
	}

	delete grammar;
}



void compute_inmemory(){
	CFG *cfg = new CFG_map();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();

	CFGCompute::load("/home/zqzuo/Desktop/inlined/final", "/home/zqzuo/Desktop/inlined/id_stmt_info.txt", cfg,
			"/home/zqzuo/Desktop/inlined/var_singleton_info.txt", singletons, graphstore, "/home/zqzuo/Desktop/inlined/rules_pointsto.txt", grammar);
	CFGCompute::do_worklist_synchronous(cfg, graphstore, grammar, singletons);
//	CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons);

	delete cfg;
	delete graphstore;
	delete grammar;

}

void preprocess(){

}

Partition schedule(){
	return 0;
}

