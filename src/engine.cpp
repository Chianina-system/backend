/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include "comp/cfg_compute_ooc_asyn.h"
#include "comp/cfg_compute_ooc_syn_naive.h"
#include "comp/cfg_compute_ooc_syn_itemset.h"
#include "comp/cfg_compute_ooc_syn_delta.h"
#include "utility/ResourceManager.hpp"

//using namespace std;

//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/newtest/inlined/";
//const string dir = "/home/dell/Desktop/Ouroboros-dataset-master/testExample/inlined/";
//const string dir = "/home/dell/GraphFlow/GraphSSAInline/firefox/browser/";
//const string file_total = dir + "total.txt";
//const string file_entries = dir + "entry.txt";
//const string file_cfg = dir + "final";
//const string file_stmts = dir + "id_stmt_info.txt";
//const string file_singletons = dir + "var_singleton_info.txt";
//const string file_grammar = "/home/dell/GraphFlow/Ouroboros-dataset-master/rules_pointsto.txt";
const string file_grammar = "/home/decxu/Documents/analysis_data/simple/rules_pointsto.txt";

/* function declaration */
void run_inmemory(int, bool, bool, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons);
void run_ooc(int, int, int, bool, bool, int, bool, int, bool, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons);


int main(int argc, char* argv[]) {
	if(argc != 9 && argc != 15){
		cout << "Usage: ./backend file_total file_entries file_cfg file_stmts file_singletons "
				  << "graphstore_mode(0: naive; 1: itemset) update_mode(0: sequential; 1: parallel) "
						<< "computation_mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1) file_mode(0: binary; 1: text) buffered_mode(0: default; 1: user-specified) "
						<< "mining_mode(0: eclat; 1: apriori; 2: fp-growth) support_threshold length_threshold" << endl;
		return 0;
	}

	if(argc == 9){
		if(atoi(argv[8]) != 0){
			cout << "Usage: ./backend file_total file_entries file_cfg file_stmts file_singletons "
					<< "graphstore_mode(0: naive; 1: itemset) update_mode(0: sequential; 1: parallel) "
						<< "computation_mode(0: in-memory; 1: out-of-core) num_partitions(if mode == 1) file_mode(0: binary; 1: text) buffered_mode(0: default; 1: user-specified) "
						<< "mining_mode(0: eclat; 1: apriori; 2: fp-growth) support_threshold length_threshold" << endl;
			return 0;
		}
	}

	const string file_total = argv[1]; // 总共的节点数
	const string file_entries = argv[2];
	const string file_cfg = argv[3];
	const string file_stmts = argv[4];
	const string file_singletons = argv[5];

	ResourceManager rm;

	// get running time (wall time)
	auto start_fsm = std::chrono::high_resolution_clock::now();

	// computation_mode设置为1时，开启核外计算
	if(atoi(argv[8])){
		run_ooc(atoi(argv[12]), atoi(argv[13]), atoi(argv[14]), (bool)atoi(argv[11]), (bool)atoi(argv[10]), atoi(argv[6]), (bool)atoi(argv[7]), atoi(argv[9]), true, file_total, file_entries, file_cfg, file_stmts, file_singletons);
	}
	else{
		run_inmemory(atoi(argv[6]), (bool)atoi(argv[7]), true, file_total, file_entries, file_cfg, file_stmts, file_singletons);
	}

	auto end_fsm = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff_fsm = end_fsm - start_fsm;
	std::cout << "Running time: " << diff_fsm.count() << " s\n";

	//print out resource usage
	std::cout << "\n\n";
	std::cout << "------------------------------ resource usage ------------------------------" << std::endl;
	std::cout << rm.result() << std::endl;
	std::cout << "------------------------------ resource usage ------------------------------" << std::endl;
	std::cout << "\n\n";

	return 0;
}


long compute_ooc(Partition partition, Context* context, bool sync_mode, int graphstore_mode, bool update_mode, bool file_mode, bool buffered_mode,
		int mining_mode, int support, int length,
		Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
	//for debugging
	Logger::print_thread_info_locked("----------------------- Partition " + to_string(partition) + " starting -----------------------\n", LEVEL_LOG_MAIN);

	CFG *cfg = new CFG_map_outcore();
    Concurrent_Worklist<CFGNode*>* actives = new Concurrent_Workset<CFGNode*>();
//	GraphStore *graphstore = new NaiveGraphStore();
	GraphStore* graphstore;
	if(graphstore_mode == 1){
	    cout << "wrong graohstore_mode" << endl;
		//graphstore = new ItemsetGraphStore(file_mode, buffered_mode);
	}
	else if(graphstore_mode == 0){
		graphstore = new NaiveGraphStore(file_mode, buffered_mode);
	}
	else if(graphstore_mode == 2){
        cout << "wrong graohstore_mode" << endl;
		//graphstore = new DeltaGraphStore(file_mode, buffered_mode);
	}
	else{
		cout << "wrong graphstore mode!" << endl;
		exit(-1);
	}

//    //get the flag for adding self-loop edges
//    bool flag = context->getFlag(partition);
//	context->setFlag(partition);

	//for tuning
	timer_ooc->getLoadSum()->start();

    CFGCompute_ooc_syn::load(partition, cfg, graphstore, context, file_mode, mining_mode, support, length);

    //for tuning
    timer_ooc->getLoadSum()->end();


    //for tuning
    timer_ooc->getComputeSum()->start();

    long supersteps = 0;
    if(sync_mode){
//		CFGCompute_ooc_syn::do_worklist_ooc_synchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false, update_mode, timer_ooc, timer);
		if(graphstore_mode == 1){
			//supersteps = CFGCompute_ooc_syn_itemset::do_worklist_ooc_synchronous(cfg, dynamic_cast<ItemsetGraphStore*> (graphstore), context->getGrammar(), context->getSingletons(), actives, false, update_mode, timer_ooc, timer);
		}
		else if(graphstore_mode == 0){
			supersteps = CFGCompute_ooc_syn_naive::do_worklist_ooc_synchronous(cfg, dynamic_cast<NaiveGraphStore*> (graphstore), context->getGrammar(), context->getSingletons(), actives, false, update_mode, timer_ooc, timer);
		}
		else if(graphstore_mode == 2){
			//CFGCompute_ooc_syn_delta::do_worklist_ooc_synchronous(cfg, dynamic_cast<DeltaGraphStore*> (graphstore), context->getGrammar(), context->getSingletons(), actives, false, update_mode, timer_ooc, timer);
		}
		else{
			cout << "wrong graphstore mode!" << endl;
			exit(-1);
		}
    }
    else{
    	//CFGCompute_ooc_asyn::do_worklist_ooc_asynchronous(cfg, graphstore, context->getGrammar(), context->getSingletons(), actives, false);
    }

    //for tuning
    timer_ooc->getComputeSum()->end();

    //for tuning
    timer_ooc->getCompressSum()->start();

    if(sync_mode){
		if(graphstore_mode == 1){
            cout << "wrong graphstore mode!" << endl;
			//start compressing graphs
			//dynamic_cast<ItemsetGraphStore*> (graphstore)->compressGraphStore(partition, support, length);
		}
    }

    //for tuning
    timer_ooc->getCompressSum()->end();

    //for tuning
    timer_ooc->getPassSum()->start();

    // 属于其他partition的节点在此partition更新，因此需要加入到其他partition的actives中，并且为相应的partition提高优先级
    CFGCompute_ooc_syn::pass(partition, cfg, graphstore, actives, context, file_mode);

    //assert(1 == 0);

    //for tuning
    timer_ooc->getPassSum()->end();


    //for tuning
    timer_ooc->getCleanSum()->start();

//    int num = 0;
//    auto it = graphstore->getMap().begin();
//    while (it != graphstore->getMap().end()) {
//        //if (it->first == 90)
//        cout << "id: " << it->first << "    graphstore model miss: " << it->second->MissCount_Icache << endl;
//        if (it->second->MissCount_Icache > num) num = it->second->MissCount_Icache;
//        it++;
//    }
//    cout << "MissCount_Icache: " << num << endl;

	delete cfg;
	delete graphstore;
	delete actives;

	//for tuning
	timer_ooc->getCleanSum()->end();

	//for debugging
	Logger::print_thread_info_locked("----------------------- Partition " + to_string(partition) + " finished -----------------------\n", LEVEL_LOG_MAIN);
	return supersteps;
}

void readAllGraphs(NaiveGraphStore *graphstore, Context* context){
	for(unsigned int partition = 0; partition < context->getNumberPartitions(); ++ partition){
		const string filename_graphs = Context::file_graphstore + to_string(partition);
		graphstore->deserialize(filename_graphs);
	}
}

void loadMirrors(const string& file_mirrors_call, const string& file_mirrors_shallow, std::unordered_set<PEGraph_Pointer>& mirrors, bool file_mode){
	//handle mirrors file
	if(file_mode){
		std::ifstream fin;
		std::string line;
		fin.open(file_mirrors_call);
		if (!fin) {
			cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
		}
		else {
			while (getline(fin, line)) {
				if(line == ""){
					continue;
				}

				CFGNode* cfgNode_mirror = new CFGNode(line);
				mirrors.insert(cfgNode_mirror->getCfgNodeId());
				delete cfgNode_mirror;
			}
			fin.close();
		}

		fin.open(file_mirrors_shallow);
		if (!fin) {
			cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
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
	else{
		//handle mirrors file
		FILE* fp_mirrors_call = fopen(file_mirrors_call.c_str(), "rb");
		if(!fp_mirrors_call) {
			cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
//    			exit(-1);
		}
		else{
			size_t freadRes = 0; //clear warnings
			size_t bufsize;
			while(fread(&bufsize, sizeof(size_t), 1, fp_mirrors_call) != 0) {
				char *buf = (char*)malloc(bufsize);
				freadRes = fread(buf, bufsize, 1, fp_mirrors_call);

				CFGNode* cfgNode = new CFGNode();
				cfgNode->read_from_buf(buf, bufsize);
				mirrors.insert(cfgNode->getCfgNodeId());

				free(buf);
			}
			fclose(fp_mirrors_call);
		}

		int fp_mirrors_shallow = open(file_mirrors_shallow.c_str(), O_RDONLY);
		if(!(fp_mirrors_shallow > 0)) {
			cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
//    			exit(-1);
		}
		else{
			long mirrors_shallow_file_size = io_manager::get_filesize(fp_mirrors_shallow);
			char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
			long real_io_size = get_real_io_size(IO_SIZE, sizeof(vertexid_t));
			int streaming_counter = mirrors_shallow_file_size / real_io_size + 1;

			long offset_read = 0;

			for(int counter = 0; counter < streaming_counter; counter++) {
				long valid_io_size = 0;
				if (counter == streaming_counter - 1)
					valid_io_size = mirrors_shallow_file_size - real_io_size * (streaming_counter - 1);
				else
					valid_io_size = real_io_size;

				io_manager::read_from_file(fp_mirrors_shallow, buf, valid_io_size, offset_read);
				offset_read += valid_io_size;

				for(long offset = 0; offset < valid_io_size; offset += sizeof(vertexid_t)) {
					PEGraph_Pointer id = *((vertexid_t*)(buf + offset));
					mirrors.insert(id);
				}
			}
			free(buf);

			close(fp_mirrors_shallow);
		}
	}
}

long retrieve_alias(PEGraph* in, vertexid_t x, Grammar* grammar){
	long num = 0;
	int numEdges = in->getNumEdges(x);
//	vertexid_t *edges = in->getEdges(x);
	label_t *labels = in->getLabels(x);

	for (int i = 0; i < numEdges; ++i) {
		if (grammar->isMemoryAlias(labels[i]) || grammar->isValueAlias(labels[i])) {
			num++;
		}
	}

	return num;
}

void printGraphstoreInfo_more(Context* context, int graphstore_mode, bool file_mode, bool buffered_mode, int mining_mode, int support, int length){
	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	long num_edges = 0;
	long num_vertices = 0;
	int num_graphs = 0;
	long num_graphitems = 0;
	long num_baseitems = 0;

	long num_load_alias = 0;
	long num_store_alias = 0;
	long num_load = 0;
	long num_store = 0;

	long num_load_alias_weighted = 0;
	long num_store_alias_weighted = 0;
	long num_load_weighted = 0;
	long num_store_weighted = 0;

//	long num_assign_alias = 0;
//	long num_phi_alias = 0;
//	long num_store_pointsto = 0;
//	long num_load_pointsto = 0;

	for(unsigned int partition = 0; partition < context->getNumberPartitions(); ++ partition){
		CFG_map_outcore *cfg = new CFG_map_outcore();
		GraphStore* graphstore;
		if(graphstore_mode == 1){
            cout << "wrong graohstore_mode" << endl;
			//graphstore = new ItemsetGraphStore(file_mode, buffered_mode);
		}
		else if(graphstore_mode == 0){
			graphstore = new NaiveGraphStore(file_mode, buffered_mode);
		}
		else if(graphstore_mode == 2){
            cout << "wrong graohstore_mode" << endl;
			//graphstore = new DeltaGraphStore(file_mode, buffered_mode);
		}
		else{
			cout << "wrong graphstore mode!" << endl;
			exit(-1);
		}

		//read in the times info of each variable occurred
		string file_call_times = context->getFileSingletons().substr(0, context->getFileSingletons().find_last_of('v')) + "id_num.txt";
		cout << file_call_times << endl;
		unordered_map<vertexid_t, int> map;
		ifstream myfile(file_call_times);
		if (myfile.is_open()) {
			string line;
			while (getline(myfile, line)) {
				if(line == ""){
					continue;
				}

				std::stringstream stream(line);
				std::string id, times;
				stream >> id >> times;

				map[atoi(id.c_str())] = atoi(times.c_str());
			}
			myfile.close();
		}


		CFGCompute_ooc_syn::load(partition, cfg, graphstore, context, file_mode, mining_mode, support, length);
    	for(CFGNode* node : cfg->getNodes()){
    		if(!cfg->isMirror(node)){
//    			PEGraph_Pointer out_pointer = node->getOutPointer();
//    			PEGraph* out = graphstore->retrieve(out_pointer);

    	        //merge
    	    	std::vector<CFGNode*>* preds = cfg->getPredesessors(node);
    	        PEGraph* in = CFGCompute_syn::combine_synchronous(graphstore, preds, node, context->getGrammar());

    			Stmt* stmt = node->getStmt();
    			TYPE type = stmt->getType();
    			if(type == TYPE::Load){
	            	vertexid_t load_exp = ((LoadStmt*)stmt)->getSrc();
	            	num_load_alias += retrieve_alias(in, load_exp, context->getGrammar());
	            	num_load++;

	            	num_load_alias_weighted += retrieve_alias(in, load_exp, context->getGrammar()) * map[load_exp];
	            	num_load_weighted += map[load_exp];
    			}
    			else if(type == TYPE::Store){
	            	vertexid_t store_exp = ((StoreStmt*)stmt)->getDst();
	            	num_store_alias += retrieve_alias(in, store_exp, context->getGrammar());
	            	num_store++;

	            	num_store_alias_weighted += retrieve_alias(in, store_exp, context->getGrammar()) * map[store_exp];
	            	num_store_weighted += map[store_exp];
    			}

    			delete in;
    		}
    	}
    	delete cfg;

		std::unordered_set<PEGraph_Pointer> mirrors;
		const string filename_mirrors_in = Context::folder_mirrors_call + to_string(partition);
		const string filename_mirrors_out = Context::folder_mirrors_shallow + to_string(partition);
		loadMirrors(filename_mirrors_in, filename_mirrors_out, mirrors, file_mode);

    	int size_graphs = 0;
    	long size_vertices = 0;
    	long size_edges = 0;
    	long size_graphitems = 0;
    	long size_baseitems = 0;
    	//graphstore->getStatistics(size_graphs, size_vertices, size_edges, size_graphitems, size_baseitems, mirrors);



    	cout << "partition " << to_string(partition) << endl;
    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of vertices: " << size_vertices << endl;
    	cout << "Number of edges: " << size_edges << endl;
    	cout << "Number of total items: " << size_graphitems + size_baseitems << endl;
    	cout << "Number of graph items: " << size_graphitems << endl;
    	cout << "Number of base items: " << size_baseitems << endl;
    	cout << endl;

    	delete graphstore;


    	num_edges += size_edges;
    	num_vertices += size_vertices;
    	num_graphs += size_graphs;
    	num_graphitems += size_graphitems;
    	num_baseitems += size_baseitems;
	}

	cout << "\nTotal number of graphs: " << num_graphs << endl;
	cout << "Total number of vertices: " << num_vertices << endl;
	cout << "Total number of edges: " << num_edges << endl;
	cout << "Total number of items: " << num_graphitems + num_baseitems << endl;
	cout << "Total number of graph items: " << num_graphitems << endl;
	cout << "Total number of base items: " << num_baseitems << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	cout << endl;
	cout << "Total number of loads: " << num_load << endl;
	cout << "Total number of load-alias: " << num_load_alias << endl;
	cout << "Total number of stores: " << num_store << endl;
	cout << "Total number of store-alias: " << num_store_alias << endl;
	cout << endl;
	cout << "Total number of loads-weighted: " << num_load_weighted << endl;
	cout << "Total number of load-alias-weighted: " << num_load_alias_weighted << endl;
	cout << "Total number of stores-weighted: " << num_store_weighted << endl;
	cout << "Total number of store-alias-weighted: " << num_store_alias_weighted << endl;

}

void printGraphstoreInfo(Context* context, int graphstore_mode, bool file_mode, bool buffered_mode){
	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	long num_edges = 0;
	long num_vertices = 0;
	int num_graphs = 0;
	long num_graphitems = 0;
	long num_baseitems = 0;
	for(unsigned int partition = 0; partition < context->getNumberPartitions(); ++ partition){
//		NaiveGraphStore *graphstore = new NaiveGraphStore();
		GraphStore* graphstore;
		if(graphstore_mode == 1){
            cout << "wrong graohstore_mode" << endl;
			//graphstore = new ItemsetGraphStore(file_mode, buffered_mode);
		}
		else if(graphstore_mode == 0){
			graphstore = new NaiveGraphStore(file_mode, buffered_mode);
		}
		else if(graphstore_mode == 2){
            cout << "wrong graohstore_mode" << endl;
			//graphstore = new DeltaGraphStore(file_mode, buffered_mode);
		}
		else{
			cout << "wrong graphstore mode!" << endl;
			exit(-1);
		}

		std::unordered_set<PEGraph_Pointer> mirrors;

		const string filename_graphs = Context::file_graphstore + to_string(partition);
		graphstore->deserialize(filename_graphs);

		const string filename_mirrors_in = Context::folder_mirrors_call + to_string(partition);
		const string filename_mirrors_out = Context::folder_mirrors_shallow + to_string(partition);
		loadMirrors(filename_mirrors_in, filename_mirrors_out, mirrors, file_mode);

    	int size_graphs = 0;
    	long size_vertices = 0;
    	long size_edges = 0;
    	long size_graphitems = 0;
    	long size_baseitems = 0;
    	//graphstore->getStatistics(size_graphs, size_vertices, size_edges, size_graphitems, size_baseitems, mirrors);

    	cout << "partition " << to_string(partition) << endl;
    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    	cout << "Number of total items: " << size_graphitems + size_baseitems << endl;
    	cout << "Number of graph items: " << size_graphitems << endl;
    	cout << "Number of base items: " << size_baseitems << endl;
    	cout << endl;

    	delete graphstore;

    	num_edges += size_edges;
    	num_vertices += size_vertices;
    	num_graphs += size_graphs;
    	num_graphitems += size_graphitems;
    	num_baseitems += size_baseitems;
	}

	cout << "\nTotal number of graphs: " << num_graphs << endl;
	cout << "Total number of vertices: " << num_vertices << endl;
	cout << "Total number of edges: " << num_edges << endl;
	cout << "Total number of items: " << num_graphitems + num_baseitems << endl;
	cout << "Total number of graph items: " << num_graphitems << endl;
	cout << "Total number of base items: " << num_baseitems << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
}

void clean_mining_files(int num_partitions){
	for(int part = 0; part < num_partitions; part++){
		string input_file = inputFile + to_string(part);
		string output_file = outFile + to_string(part);
		FileUtil::delete_file(input_file);
		FileUtil::delete_file(output_file);
	}
}


void run_ooc(int mining_mode, int support, int length, bool buffered_mode, bool file_mode, int graphstore_mode, bool update_mode, int num_partitions, bool sync_mode,
		const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	//for performance tuning
	Timer_sum sum_preprocess("preprocess");
	Timer_sum sum_compute("compute-ooc");
	Timer_wrapper_ooc* timer_ooc = new Timer_wrapper_ooc();
	Timer_wrapper_inmemory* timer = new Timer_wrapper_inmemory();

	//for tuning
	sum_preprocess.start();

//	char *p = "0x40118f";
//	int nv = 0;
//	sscanf(p, "%x", &nv);
//	cout << "0x40118f is " << nv << endl;
	//preprocessing
	//cout << "sizeof(std::vector<IR>): " << sizeof(std::vector<IR>) << endl;
	Context* context = new Context(num_partitions, file_total, file_cfg, file_stmts, file_entries, file_singletons, file_grammar);
    /* 将输入文件处理为适合核外处理的输入文件集，即单个输入文件分为多个文件
     * partition A ：边在cfg_A中（包含所有src或者dst属于A的节点的边），属于A的节点信息在stmt_A中，
     * 同时不属于A但在cfg_A中的节点信息保存在mirrors_call_A和mirrors_shallow_A中
     */
    //cout << "sizeof(TYPE):" << sizeof(TYPE) << endl;
	Preprocess::process(*context, file_mode);


	//for tuning
	sum_preprocess.end();

//	//for debugging
//	context->printOutPriorityInfo();

	//for tuning
	sum_compute.start();

	//iterative computation
	int iterations = 0;
	long supersteps = 0;
	Partition partition;
	// 优先级最高的partition先拿出来进行计算
	while(context->schedule(partition)){
		cout << "Partition: " << partition << endl;
		iterations++;
        //assert(1 == 0);
		// sync_mode恒为true
		supersteps += compute_ooc(partition, context, sync_mode, graphstore_mode, update_mode, file_mode, buffered_mode, mining_mode, support, length, timer_ooc, timer);

//		//for debugging
//		context->printOutPriorityInfo();
	}

	//for tuning
	sum_compute.end();

//	//for debugging
//	NaiveGraphStore *graphstore = new NaiveGraphStore();
//	readAllGraphs(graphstore, context);
//	graphstore->printOutInfo();
//	delete graphstore;
//	printGraphstoreInfo(context, graphstore_mode, file_mode, buffered_mode);
//	printGraphstoreInfo_more(context, graphstore_mode, file_mode, buffered_mode, mining_mode, support, length);

	//delete all the files for itemset mining
	clean_mining_files(num_partitions);
	delete context;

	//for tuning
	//sum_preprocess.print();
	//sum_compute.print();

	//timer_ooc->print();
	delete timer_ooc;

	//timer->print();
	delete timer;

	cout << "Number of iterations: " << iterations << endl;
	cout << "Number of supersteps: " << supersteps << endl;
}


void compute_inmemory(int graphstore_mode, bool update_mode, bool sync_mode, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	CFG *cfg = new CFG_map();
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();
	GraphStore* graphstore;
	if(graphstore_mode == 1){
        cout << "wrong graohstore_mode" << endl;
		//graphstore = new ItemsetGraphStore();
	}
	else if (graphstore_mode == 0){
		graphstore = new NaiveGraphStore();
	}
	else if (graphstore_mode == 2){
        cout << "wrong graohstore_mode" << endl;
		//graphstore = new DeltaGraphStore();
	}
	else{
		cout << "wrong graphstore mode!" << endl;
		exit(-1);
	}

	CFGCompute_syn::load(file_total, file_cfg, file_stmts, file_entries, cfg, file_singletons, singletons, graphstore, file_grammar, grammar);
	if(sync_mode){
//		CFGCompute_syn::do_worklist_synchronous(cfg, graphstore, grammar, singletons, false, update_mode);
		if(graphstore_mode == 1){
            cout << "wrong graohstore_mode" << endl;
			//CFGCompute_syn_itemset::do_worklist_synchronous(cfg, dynamic_cast<ItemsetGraphStore*> (graphstore), grammar, singletons, false, update_mode);
		}
		else if (graphstore_mode == 0){
			CFGCompute_syn_naive::do_worklist_synchronous(cfg, dynamic_cast<NaiveGraphStore*> (graphstore), grammar, singletons, false, update_mode);
		}
		else if (graphstore_mode == 2){
            cout << "wrong graohstore_mode" << endl;
			//CFGCompute_syn_delta::do_worklist_synchronous(cfg, dynamic_cast<DeltaGraphStore*> (graphstore), grammar, singletons, false, update_mode);
		}
		else{
			cout << "wrong graphstore mode!" << endl;
			exit(-1);
		}
	}
	else{
		//CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons, false);
	}

	delete cfg;
	delete graphstore;
	delete grammar;
	delete singletons;
}

void run_inmemory(int graphstore_mode, bool update_mode, bool sync_mode, const string& file_total, const string& file_entries, const string& file_cfg, const string& file_stmts, const string& file_singletons){
	compute_inmemory(graphstore_mode, update_mode, sync_mode, file_total, file_entries, file_cfg, file_stmts, file_singletons);
}


