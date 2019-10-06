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
//const string dir = "/home/dell/GraphFlow/GraphSSAInline/firefox/browser/";
//const string file_total = dir + "total.txt";
//const string file_entries = dir + "entry.txt";
//const string file_cfg = dir + "final";
//const string file_stmts = dir + "id_stmt_info.txt";
//const string file_singletons = dir + "var_singleton_info.txt";
const string file_grammar = "/home/dell/Desktop/Ouroboros-dataset-master/rules_pointsto.txt";


/* function declaration */
void run_inmemory(int, bool, bool, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons);
void run_ooc(int, bool, int, bool, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons);


int main(int argc, char* argv[]) {
	if(argc != 9 && argc != 10){
		cout << "Usage: ./backend file_total file_entries file_cfg file_stmts file_singletons "
				  << "graphstore_mode(0: naive; 1: itemset) update_mode(0: sequential; 1: parallel) computation_mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1)" << endl;
		return 0;
	}

	if(argc == 9){
		if(atoi(argv[8]) != 0){
			cout << "Usage: ./backend file_total file_entries file_cfg file_stmts file_singletons "
					<< "graphstore_mode(0: naive; 1: itemset) update_mode(0: sequential; 1: parallel) computation_mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1)" << endl;
			return 0;
		}
	}

	const string file_total = argv[1];
	const string file_entries = argv[2];
	const string file_cfg = argv[3];
	const string file_stmts = argv[4];
	const string file_singletons = argv[5];

	ResourceManager rm;
	// get running time (wall time)
	auto start_fsm = std::chrono::high_resolution_clock::now();

	if(atoi(argv[8])){
		run_ooc(atoi(argv[6]), (bool)atoi(argv[7]), atoi(argv[9]), true, file_total, file_entries, file_cfg, file_stmts, file_singletons);
	}
	else{
		run_inmemory(atoi(argv[6]), (bool)atoi(argv[7]), true, file_total, file_entries, file_cfg, file_stmts, file_singletons);
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



void compute_ooc(Partition partition, Context* context, bool sync_mode, int graphstore_mode, bool update_mode,
		Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
	//for debugging
	Logger::print_thread_info_locked("----------------------- Partition " + to_string(partition) + " starting -----------------------\n", LEVEL_LOG_MAIN);

	CFG *cfg = new CFG_map_outcore();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();
//	GraphStore *graphstore = new NaiveGraphStore();
	GraphStore* graphstore;
	if(graphstore_mode){
		graphstore = new ItemsetGraphStore();
	}
	else{
		graphstore = new NaiveGraphStore();
	}

//    //get the flag for adding self-loop edges
//    bool flag = context->getFlag(partition);
//	context->setFlag(partition);

	//for tuning
	timer_ooc->getLoadSum()->start();

    CFGCompute_ooc_syn::load(partition, cfg, graphstore, context);

    //for tuning
    timer_ooc->getLoadSum()->end();


    //for tuning
    timer_ooc->getComputeSum()->start();

    if(sync_mode){
		CFGCompute_ooc_syn::do_worklist_ooc_synchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false, update_mode, timer_ooc, timer);
    }
    else{
    	CFGCompute_ooc_asyn::do_worklist_ooc_asynchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false);
    }

    //for tuning
    timer_ooc->getComputeSum()->end();


    //for tuning
    timer_ooc->getPassSum()->start();

    CFGCompute_ooc_syn::pass(partition, cfg, graphstore, actives, context);

    //for tuning
    timer_ooc->getPassSum()->end();

	delete cfg;
	delete graphstore;
	delete actives;

	//for debugging
	Logger::print_thread_info_locked("----------------------- Partition " + to_string(partition) + " finished -----------------------\n", LEVEL_LOG_MAIN);
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

void run_ooc(int graphstore_mode, bool update_mode, int num_partitions, bool sync_mode, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	//for performance tuning
	Timer_sum sum_preprocess("preprocess");
	Timer_sum sum_compute("compute-ooc");
	Timer_wrapper_ooc* timer_ooc = new Timer_wrapper_ooc();
	Timer_wrapper_inmemory* timer = new Timer_wrapper_inmemory();

	//for tuning
	sum_preprocess.start();

	//preprocessing
	Context* context = new Context(num_partitions, file_total, file_cfg, file_stmts, file_entries, file_singletons, file_grammar);
	Preprocess::process(*context);

	//for tuning
	sum_preprocess.end();

//	//for debugging
//	context->printOutPriorityInfo();

	//for tuning
	sum_compute.start();

	//iterative computation
	Partition partition;
	while(context->schedule(partition)){
		compute_ooc(partition, context, sync_mode, graphstore_mode, update_mode, timer_ooc, timer);

//		//for debugging
//		context->printOutPriorityInfo();
	}

	//for tuning
	sum_compute.end();

	//for debugging
//	NaiveGraphStore *graphstore = new NaiveGraphStore();
//	readAllGraphs(graphstore, context);
//	graphstore->printOutInfo();
//	delete graphstore;
	printGraphstoreInfo(context);

	delete context;

	//for tuning
	sum_preprocess.print();

	timer_ooc->print();
	delete timer_ooc;

	timer->print();
	delete timer;

	sum_compute.print();
}


void compute_inmemory(int graphstore_mode, bool update_mode, bool sync_mode, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	CFG *cfg = new CFG_map();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();
	GraphStore* graphstore;
	if(graphstore_mode){
		graphstore = new ItemsetGraphStore();
	}
	else{
		graphstore = new NaiveGraphStore();
	}

	CFGCompute_syn::load(file_total, file_cfg, file_stmts, file_entries, cfg, file_singletons, singletons, graphstore, file_grammar, grammar);
	if(sync_mode){
		CFGCompute_syn::do_worklist_synchronous(cfg, graphstore, grammar, singletons, false, update_mode);
	}
	else{
		CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons, false);
	}

	delete cfg;
	delete graphstore;
	delete grammar;
	delete singletons;
}

void run_inmemory(int graphstore_mode, bool update_mode, bool sync_mode, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	compute_inmemory(graphstore_mode, update_mode, sync_mode, file_total, file_entries, file_cfg, file_stmts, file_singletons);
}


