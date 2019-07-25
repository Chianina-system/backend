/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include "utility/ResourceManager.hpp"
#include "comp/cfg_compute_ooc_asyn.h"
using namespace std;

//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/newtest/inlined/";
//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/testExample/inlined/";
const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/httpd/";
const string file_cfg = dir + "final";
const string file_stmts = dir + "id_stmt_info.txt";
const string file_singletons = dir + "var_singleton_info.txt";
const string file_grammar = "/home/dell/Desktop/Ouroboros-dataset-master/rules_pointsto.txt";

/* function declaration */
void run_inmemory(int);
void run_ooc(int);


int main(int argc, char* argv[]) {
	if(argc != 3){
		cout << "Usage: ./backend ooc_mode(0: in-memory; 1: out-of-core) sync_mode(0: asynchronous; 1: synchronous)" << endl;
		return 0;
	}

	ResourceManager rm;
	// get running time (wall time)
	auto start_fsm = std::chrono::high_resolution_clock::now();

	if(atoi(argv[1])){
		run_ooc(atoi(argv[2]));
	}
	else{
		run_inmemory(atoi(argv[2]));
	}

	auto end_fsm = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff_fsm = end_fsm - start_fsm;
	std::cout << "Running time : " << diff_fsm.count() << " s\n";

	//print out resource usage
	std::cout << "\n\n";
	std::cout << "------------------------------ resource usage ------------------------------" << std::endl;
	std::cout << rm.result() << std::endl;
	std::cout << "------------------------------ resource usage ------------------------------" << std::endl;
	std::cout << "\n\n";

	return 0;
}



void compute_ooc(Partition partition, Context* context, int sync_mode){
//	//for debugging
//	Logger::print_thread_info_locked("compute starting...\n", LEVEL_LOG_FUNCTION);

	CFG *cfg = new CFG_map_outcore();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();

    //get the flag for adding self-loop edges
    bool flag = context->getFlag(partition);

    CFGCompute_ooc::load(partition, cfg, singletons, graphstore, context);
    if(sync_mode){
		CFGCompute_ooc::do_worklist_ooc_synchronous(cfg, graphstore, context->getGrammar(), singletons, actives, flag);
    }
    else{
    	CFGCompute_ooc_asyn::do_worklist_ooc_asynchronous(cfg, graphstore, context->getGrammar(), singletons, actives, flag);
    }
	CFGCompute_ooc::pass(partition, cfg, graphstore, actives, context);

	context->setFlag(partition);
	delete cfg;
	delete graphstore;
	delete actives;
	delete singletons;

//	//for debugging
//	Logger::print_thread_info_locked("compute finished.\n", LEVEL_LOG_FUNCTION);
}

void run_ooc(int sync_mode){
	//preprocessing
	int num_partitions = 1;
	long totol_nodes = 2693933;
	Context* context = new Context(num_partitions, totol_nodes, file_cfg, file_stmts, file_singletons, file_grammar);
	Preprocess::process(*context);

	//iterative computation
	Partition partition;
	while(context->schedule(partition)){
		compute_ooc(partition, context, sync_mode);
	}

	delete context;
}


void compute_inmemory(int sync_mode){
	CFG *cfg = new CFG_map();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();

	CFGCompute::load(file_cfg, file_stmts, cfg, file_singletons, singletons, graphstore, file_grammar, grammar);
	if(sync_mode){
		CFGCompute::do_worklist_synchronous(cfg, graphstore, grammar, singletons, false);
	}
	else{
		CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons, false);
	}

	delete cfg;
	delete graphstore;
	delete grammar;
	delete singletons;
}

void run_inmemory(int sync_mode){
	compute_inmemory(sync_mode);
}


