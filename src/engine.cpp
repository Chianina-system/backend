/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include "comp/cfg_compute_ooc_asyn.h"
#include "utility/ResourceManager.hpp"
using namespace std;

//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/newtest/inlined/";
//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/testExample/inlined/";
const string dir = "/home/dell/GraphFlow/GraphSSAInline/firefox/browser/";
const string file_total = dir + "total.txt";
const string file_entries = dir + "entry.txt";
const string file_cfg = dir + "final";
const string file_stmts = dir + "id_stmt_info.txt";
const string file_singletons = dir + "var_singleton_info.txt";
const string file_grammar = "/home/dell/Desktop/Ouroboros-dataset-master/rules_pointsto.txt";

/* function declaration */
void run_inmemory(int);
void run_ooc(int, int);


int main(int argc, char* argv[]) {
	if(argc != 2 && argc != 3){
		cout << "Usage: ./backend mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1)" << endl;
		return 0;
	}

	if(argc == 2){
		if(atoi(argv[1]) != 0){
			cout << "Usage: ./backend mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1)" << endl;
			return 0;
		}
	}

	ResourceManager rm;
	// get running time (wall time)
	auto start_fsm = std::chrono::high_resolution_clock::now();

	if(atoi(argv[1])){
		run_ooc(atoi(argv[2]), 1);
	}
	else{
		run_inmemory(1);
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
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();

//    //get the flag for adding self-loop edges
//    bool flag = context->getFlag(partition);
//	context->setFlag(partition);

    CFGCompute_ooc_syn::load(partition, cfg, graphstore, context);
    if(sync_mode){
		CFGCompute_ooc_syn::do_worklist_ooc_synchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false);
    }
    else{
    	CFGCompute_ooc_asyn::do_worklist_ooc_asynchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false);
    }
	CFGCompute_ooc_syn::pass(partition, cfg, graphstore, actives, context);

	delete cfg;
	delete graphstore;
	delete actives;

//	//for debugging
//	Logger::print_thread_info_locked("compute finished.\n", LEVEL_LOG_FUNCTION);
}

void readAllGraphs(NaiveGraphStore *graphstore, Context* context){
	for(unsigned int partition = 0; partition < context->getNumberPartitions(); ++ partition){
		const string filename_graphs = Context::file_graphstore + to_string(partition);
		graphstore->deserialize(filename_graphs);
	}
}

void loadMirrors(const string& file_mirrors_in, const string& file_mirrors_out, std::unordered_set<PEGraph_Pointer>& mirrors){
	//handle mirrors file
	std::ifstream fin;
	std::string line;
	fin.open(file_mirrors_in);
	if (!fin) {
		cout << "can't load file_mirrors_in: " << file_mirrors_in << endl;
	}
	else {
		while (getline(fin, line)) {
			if(line == ""){
				continue;
			}

			PEGraph_Pointer id = atoi(line.c_str());
			mirrors.insert(id);
		}
		fin.close();
	}

	fin.open(file_mirrors_out);
	if (!fin) {
		cout << "can't load file_mirrors_out: " << file_mirrors_out << endl;
	}
	else{
		while (getline(fin, line)) {
			if(line == ""){
				continue;
			}

			PEGraph_Pointer id = atoi(line.c_str());
			mirrors.insert(id);
		}
		fin.close();
	}
}

void printGraphstoreInfo(Context* context){
	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	long num_edges = 0;
	int num_graphs = 0;
	for(unsigned int partition = 0; partition < context->getNumberPartitions(); ++ partition){
		NaiveGraphStore *graphstore = new NaiveGraphStore();
		std::unordered_set<PEGraph_Pointer> mirrors;

		const string filename_graphs = Context::file_graphstore + to_string(partition);
		graphstore->deserialize(filename_graphs);

		const string filename_mirrors_in = Context::folder_mirrors_in + to_string(partition);
		const string filename_mirrors_out = Context::folder_mirrors_out + to_string(partition);
		loadMirrors(filename_mirrors_in, filename_mirrors_out, mirrors);

		auto map = graphstore->getMap();

    	int size_graphs = 0;
    	long size_edges = 0;

    	cout << "partition " << to_string(partition) << endl;
    	for(auto it = map.begin(); it != map.end(); ++it){
    		if(mirrors.find(it->first) == mirrors.end()){
				size_edges += it->second->getNumEdges();
				size_graphs++;
    		}
    	}

    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    	cout << endl;

    	delete graphstore;

    	num_edges += size_edges;
    	num_graphs += size_graphs;
	}

	cout << "\nTotal number of graphs: " << num_graphs << endl;
	cout << "Total number of edges: " << num_edges << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
}

void run_ooc(int num_partitions, int sync_mode){
	//preprocessing
	Context* context = new Context(num_partitions, file_total, file_cfg, file_stmts, file_entries, file_singletons, file_grammar);
	Preprocess::process(*context);

	//for debugging
	context->printOutPriorityInfo();

	//iterative computation
	Partition partition;
	while(context->schedule(partition)){
		compute_ooc(partition, context, sync_mode);

		//for debugging
		context->printOutPriorityInfo();
	}

	//for debugging
//	NaiveGraphStore *graphstore = new NaiveGraphStore();
//	readAllGraphs(graphstore, context);
//	graphstore->printOutInfo();
//	delete graphstore;
	printGraphstoreInfo(context);

	delete context;
}


void compute_inmemory(int sync_mode){
	CFG *cfg = new CFG_map();
	GraphStore *graphstore = new NaiveGraphStore();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();

	CFGCompute_syn::load(file_total, file_cfg, file_stmts, file_entries, cfg, file_singletons, singletons, graphstore, file_grammar, grammar);
	if(sync_mode){
		CFGCompute_syn::do_worklist_synchronous(cfg, graphstore, grammar, singletons, false);
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


