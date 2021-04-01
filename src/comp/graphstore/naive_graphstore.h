/*
 * NaiveGraphStore.h
 *
 *  Created on: Jun 27, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_

#include <cachestate.h>
#include "../../utility/Logger.hpp"
#include "../../utility/FileUtil.hpp"
#include "graphstore.h"
//#include "cfg_compute.h"

using namespace std;

class NaiveGraphStore : public GraphStore {

public:
	NaiveGraphStore() : GraphStore (true, true) {

	}

	NaiveGraphStore(bool file_mode, bool buffered_m) : GraphStore (file_mode, buffered_m) {

	}


    ~NaiveGraphStore(){
    	for(auto it = cache_map.begin(); it != cache_map.end(); ++it){
    		delete it->second;
    	}
//    	clear();
    }

//    void init(CFG* cfg) {
//    	std::vector<CFGNode*> nodes = cfg->getNodes();
//    	for(auto & node: nodes){
//    		PEGraph_Pointer graph_pointer = node->getOutPointer();
//        	if(cache_map.find(graph_pointer) == map.end()){
//                cache_map[graph_pointer] = new PEGraph();
//        	}
//    	}
//    }


//    void load_onebyone(const string& file){
//    	//for debugging
//    	Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);
//
//	    std::ifstream fin;
//	    fin.open(file);
//	    if(!fin) {
//	        cout << "can't load file_graphs: " << file << endl;
////	        exit (EXIT_FAILURE);
//	    }
//	    else{
//			std::string line;
//			while (getline(fin, line)) {
//				if(line == ""){
//					continue;
//				}
//
//				std::stringstream stream(line);
//				std::string id;
//				stream >> id;
//				PEGraph_Pointer graph_pointer = atoi(id.c_str());
//				PEGraph* pegraph = new PEGraph();
//				pegraph->load_readable(stream);
//				//since the file is appended, we just use the recent updated pegraph
//				if (map.find(graph_pointer) != map.end()) {
//					delete map[graph_pointer];
//				}
//				map[graph_pointer] = pegraph;
//			}
//			fin.close();
//
//	    	//delete the old graphstore file
//	    	FileUtil::delete_file(file);
//	    }
//
//		//for debugging
//		Logger::print_thread_info_locked("load-readable finished.\n", LEVEL_LOG_FUNCTION);
//
//    }


    void loadGraphStore(const string& file, const string& file_graphs_in, Partition part, int mining_mode = -1, int support = 0, int length = 0) {
    	//graphstore file
    	this->deserialize(file);

    	//cout << "graphstore" << endl;
    	this->load_in_graphs(file_graphs_in);
        //cout << "graphstore_in" << endl;
    }

    void load_in_graphs(const string& file_graphs_in){
    	deserialize(file_graphs_in);
    }

    void serialize(const string& file){
		//for debugging
		Logger::print_thread_info_locked("serialize starting...\n", LEVEL_LOG_FUNCTION);

    	if(file_mode){
    	    cout << "wrong file_mode" << endl;
//    		ofstream myfile;
//    		myfile.open(file, std::ofstream::out);
//    		if (myfile.is_open()){
//    			for (auto& n : map) {
//    				//write a pegraph into file
//    		    	myfile << n.first << "\t";
//    		    	n.second->write_readable(myfile);
//    		    	myfile << "\n";
//    			}
//    			myfile.close();
//    		}
    	}
    	else{
    		FILE *f = fopen(file.c_str(),"wb");
    		if(f == NULL) {
    			cout << "can't write to file: " << file << endl;
    			exit(-1);
    		}
    		else{
    			if(buffered_mode){//cache-level
                    for (auto& n : cache_map) {
                        cachestate* state = n.second;
                        size_t bufsize = sizeof(PEGraph_Pointer) + state->compute_size_bytes();
                        fwrite((const void*)& bufsize, sizeof(size_t), 1, f);
                        char *buf = (char*)malloc(bufsize);
                        state->write_to_buf(buf, n.first);
                        fwrite(buf, bufsize, 1, f);
                        free(buf);
                    }
//    				for (auto& n : map) {
//    					PEGraph* pegraph = n.second;
//    					size_t bufsize = sizeof(PEGraph_Pointer) + pegraph->compute_size_bytes();
//    					fwrite((const void*)& bufsize, sizeof(size_t), 1, f);
//    					char *buf = (char*)malloc(bufsize);
//    					pegraph->write_to_buf(buf, n.first);
//    					fwrite(buf, bufsize, 1, f);
//    					free(buf);
//    				}
    			}
    			else{//field-level
    			    cout << "serialize field-level not realize" << endl;
//					for (auto& n : map) {
//						PEGraph_Pointer graph_pointer = n.first;
//						fwrite((const void*)& graph_pointer, sizeof(PEGraph_Pointer), 1, f);
//						n.second->write_unreadable(f);
//					}
    			}

				fclose(f);
    		}
    	}

		//for debugging
		Logger::print_thread_info_locked("serialize finished.\n", LEVEL_LOG_FUNCTION);
    }

    void deserialize(const string& file){
    	if(file_mode){
    	    cout << "wrong file_mode" << endl;
//    	    std::ifstream fin;
//    	    fin.open(file);
//    	    if(!fin) {
//    	        cout << "can't load graphs file: " << file << endl;
//    //	        exit (EXIT_FAILURE);
//    	    }
//    	    else{
//    			std::string line;
//    			while (getline(fin, line)) {
//    				if(line == ""){
//    					continue;
//    				}
//
//    				std::stringstream stream(line);
//    				std::string id;
//    				stream >> id;
//    				PEGraph_Pointer graph_pointer = atoi(id.c_str());
//    				PEGraph* pegraph = new PEGraph();
//    				pegraph->load_readable(stream);
//    				//since the file is appended, we just use the recent updated pegraph
//    				if (cache_map.find(graph_pointer) != cache_map.end()) {
//    					delete cache_map[graph_pointer];
//    				}
//                    cache_map[graph_pointer] = pegraph;
//    			}
//    			fin.close();
//
//    	    	//delete the old graphstore file
//    	    	FileUtil::delete_file(file);
//    	    }
    	}
    	else{
    		FILE *fp = fopen(file.c_str(),"rb");
    		if(!fp) {
    			cout << "can't load graphs file: " << file << endl;
//    			exit(-1);
    		}
    		else{
    			if(buffered_mode){//cachestate-level
    				size_t freadRes = 0; //clear warnings
    				size_t bufsize;
    				while(fread(&bufsize, sizeof(size_t), 1, fp) != 0) {
    					char *buf = (char*)malloc(bufsize);
    					freadRes = fread(buf, bufsize, 1, fp);
    					cachestate* state = new cachestate();
    					PEGraph_Pointer graph_pointer = state->read_from_buf(buf, bufsize);
    					free(buf);
						//since the file is appended, we just use the recent updated pegraph
						if (cache_map.find(graph_pointer) != cache_map.end()) {
							delete cache_map[graph_pointer];
						}
                        cache_map[graph_pointer] = state;
    				}
    			}
    			else{
    			    cout << "deserialize file-lever not reallize" << endl;
//					PEGraph_Pointer graph_pointer;
//					while(fread(&graph_pointer, sizeof(PEGraph_Pointer), 1, fp) != 0) {
//						PEGraph* pegraph = new PEGraph();
//						pegraph->load_unreadable(fp);
//						//since the file is appended, we just use the recent updated pegraph
//						if (map.find(graph_pointer) != map.end()) {
//							delete map[graph_pointer];
//						}
//						map[graph_pointer] = pegraph;
//					}
    			}

				fclose(fp);

				//delete the old graphstore file
				FileUtil::delete_file(file);
    		}
    	}
    }

    void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set){
		//for debugging
		Logger::print_thread_info_locked("store-in-graphs starting...\n", LEVEL_LOG_FUNCTION);

		if(file_mode){
		    cout << "store_in_graphs text mode not realize" << endl;
//			ofstream myfile;
//			myfile.open(file_graphs_in, std::ofstream::out | std::ofstream::app);
//			if (myfile.is_open()){
//				for (auto& n : set) {
//					auto pointer = n->getOutPointer();
//					PEGraph* graph = retrieve_shallow(pointer);
////					assert(graph != nullptr);
//					if(graph){
//						//write a pegraph into file
//						myfile << pointer << "\t";
//						graph->write_readable(myfile);
////						delete graph;
//						myfile << "\n";
//					}
//				}
//				myfile.close();
//			}
		}
		else{
    		FILE *f = fopen(file_graphs_in.c_str(),"ab");
    		if(f == NULL) {
    			cout << "can't write to file: " << file_graphs_in << endl;
    			exit(-1);
    		}
    		else{
    			if(buffered_mode){//pegraph-level
    				for (auto& n : set) {
						auto graph_pointer = n->getOutPointer();
						//PEGraph* pegraph = retrieve_shallow(graph_pointer);
                        cachestate* state = retrieve_shallow(graph_pointer);
						if(state){
							size_t bufsize = sizeof(PEGraph_Pointer) + state->compute_size_bytes();
							fwrite((const void*)& bufsize, sizeof(size_t), 1, f);
							char *buf = (char*)malloc(bufsize);
                            state->write_to_buf(buf, graph_pointer);
							fwrite(buf, bufsize, 1, f);
							free(buf);
						}
    				}
    			}
    			else{
    			    cout << "store_in_graphs file-lever not realize" << endl;
//					for (auto& n : set) {
//						auto graph_pointer = n->getOutPointer();
//						PEGraph* pegraph = retrieve_shallow(graph_pointer);
//						if(pegraph){
//							fwrite((const void*)& graph_pointer, sizeof(PEGraph_Pointer), 1, f);
//							pegraph->write_unreadable(f);
//						}
//					}
    			}
				fclose(f);
    		}
		}

		//for debugging
		Logger::print_thread_info_locked("store-in-graphs finished.\n", LEVEL_LOG_FUNCTION);
    }


//    //deep copy; locked version for asynchronous mode
//    PEGraph* retrieve_locked(PEGraph_Pointer graph_pointer){
//    	std::lock_guard<std::mutex> lockGuard(mutex);
//    	return retrieve(graph_pointer);
//    }

    cachestate* retrieve_shallow(PEGraph_Pointer graph_pointer) {
        cachestate* out;
        if (cache_map.find(graph_pointer) != cache_map.end()) {
            out = cache_map[graph_pointer];
        }
        else {
            out = nullptr;
        }
        return out;
    }

//    PEGraph* retrieve_shallow(PEGraph_Pointer graph_pointer){
////    	//for debugging
////    	Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);
//
//    	PEGraph* out;
//
//    	if(map.find(graph_pointer) != map.end()){
////    		Logger::print_thread_info_locked("retrieving +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
//    		out = map[graph_pointer];
//    	}
//    	else{
////    		out = new PEGraph();
//    		out = nullptr;
//    	}
//
//    	//for debugging
////    	Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);
//
//    	return out;
//    }


    //deep copy
    cachestate* retrieve(PEGraph_Pointer graph_pointer){
        cachestate* out;

        if (cache_map.find(graph_pointer) != cache_map.end()) {
            //out = new State(); // 初始化有问题
            //out->dcopy(map[graph_pointer]);
            out = cache_map[graph_pointer]->fork();
            //out = new State(map[graph_pointer]);
        }
        else {
            out = new cachestate();
            //out = nullptr;
        }
        return out;
    }

//    //deep copy; locked version for asynchronous mode
//    void update_locked(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//    	std::lock_guard<std::mutex> lockGuard(mutex);
//    	update(graph_pointer, pegraph);
//    }


    //shallow copy
    void update_shallow(PEGraph_Pointer graph_pointer, cachestate* pegraph) {
//    	//for debugging
//    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

//    	assert(map.find(graph_pointer) != map.end());
    	if(cache_map.find(graph_pointer) != cache_map.end()){
//    		Logger::print_thread_info_locked("deleting +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
			delete cache_map[graph_pointer];
    	}
        cache_map[graph_pointer] = pegraph;

    	//for debugging
//    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    //deep copy
//    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
////    	//for debugging
////    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);
//
////    	assert(map.find(graph_pointer) != map.end());
//    	if(cache_map.find(graph_pointer) != cache_map.end()){
////    		Logger::print_thread_info_locked("deleting +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
//			delete cache_map[graph_pointer];
//    	}
//        cache_map[graph_pointer] = new PEGraph(pegraph);
//
//    	//for debugging
////    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
//    }

    //shallow copy
//    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph* graph){
//    	std::lock_guard<std::mutex> lGuard (mtx);
//    	this->map[pointer] = graph;
//    }

    void addOneGraph_atomic(PEGraph_Pointer pointer, cachestate* graph) {
        std::lock_guard<std::mutex> lGuard (mtx);
        this->cache_map[pointer] = graph;
    }

//    static void update_parallel(NaiveGraphStore* current, NaiveGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
//    	vertexid_t id = -1;
//    	while(worklist->pop_atomic(id)){
//    		assert(current->cache_map.find(id) != current->cache_map.end());
//    		current->update(id, another->cache_map.at(id));
//    	}
//    }

//    void update_graphs_parallel(GraphStore* another){
//	    //initiate concurrent worklist
//	    Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
//	    NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
//	    for(auto& it: another_graphstore->cache_map){
//	        worklist->push_atomic(it.first);
//
//			//initialize the graphstore
//	        if(map.find(it.first) == map.end()){
//	        	map[it.first] = new PEGraph();
//	        }
//	    }
//
//		std::vector<std::thread> comp_threads;
//		for (unsigned int i = 0; i < NUM_THREADS; i++)
//			comp_threads.push_back(std::thread([=] {update_parallel(this, another_graphstore, worklist);}));
//
//		for (auto &t : comp_threads)
//			t.join();
//
//	    //clean
//	    delete(worklist);
//    }

//    void update_graphs_sequential(GraphStore* another){
//    	NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
//    	for(auto& it: another_graphstore->cache_map){
//    		update(it.first, it.second);
//    	}
//    }


    void update_graphs_shallow(GraphStore* another, bool update_mode){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs starting...\n", LEVEL_LOG_FUNCTION);

    	if(update_mode){
    		update_graphs_shallow_parallel(another); // in parallel
    	}
    	else{
    		update_graphs_shallow_sequential(another); // sequential
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_graphs_shallow_parallel(GraphStore* another){
	    //initiate concurrent worklist
	    Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	    NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
	    for(auto& it: another_graphstore->cache_map){
	        worklist->push_atomic(it.first);

			//initialize the graphstore
	        if(cache_map.find(it.first) == cache_map.end()){
                cache_map[it.first] = new cachestate();
	        }
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {update_shallow_parallel(this, another_graphstore, worklist);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);
    }

    static void update_shallow_parallel(NaiveGraphStore* current, NaiveGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
    		assert(current->cache_map.find(id) != current->cache_map.end());
    		current->update_shallow(id, another->cache_map.at(id));
    	}
    }


    void update_graphs_shallow_sequential(GraphStore* another){
    	NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
    	for(auto& it: another_graphstore->cache_map){
    		update_shallow(it.first, it.second);
    	}
    }





//    void update_graphs(GraphStore* another){
////    	update_graphs_sequential(another); // sequential
//    	update_graphs_parallel(another); // in parallel
//    }

//    void clearEntryOnly(){
//    	this->map.clear();
//    }

//    void clear(){
//    	for(auto it = cache_map.begin(); it != cache_map.end(); ){
//    		delete it->second;
//    		it = cache_map.erase(it);
//    	}
//    }

    void clear_shallow(){
//    	for(auto it = map.begin(); it != map.end(); ){
//    		it = map.erase(it);
//    	}
    	this->cache_map.clear();
    }


//    void printOutInfo(){
//    	int size_graphs = map.size();
//    	long size_edges = 0;
//
//    	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
//
//    	std::map<PEGraph_Pointer, int> sorted;
//
//    	for(auto it = map.begin(); it != map.end(); ++it){
////    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
////    		sorted[it->first] = it->second->getNumEdges();
//    		size_edges += it->second->getNumEdges();
//    	}
//
////    	cout << "\n\n";
////    	for(auto& it: sorted){
////    		cout << it.first << "\t" << it.second << endl;
////    	}
//
//    	cout << "Number of graphs: " << size_graphs << endl;
//    	cout << "Number of edges: " << size_edges << endl;
//    }


    std::unordered_map<PEGraph_Pointer, cachestate*>& getMap() {
    	return cache_map;
    }


    void getStatistics(int& size_graphs, long& size_graphitems, const std::unordered_set<PEGraph_Pointer>& mirrors){
    	for(auto it = cache_map.begin(); it != cache_map.end(); ++it){
    	    //cout << "id: " << it->first << endl;
    		if(mirrors.find(it->first) == mirrors.end()){
                for (int i = 0; i < CacheLinesPerSet; ++i) {
                    size_graphitems += it->second->IRs_Icache[i].size();
                    //if(it->second->IRs_Icache[i].size() != 0);
                    //cout << it->second->IRs_Icache[i].getStatisticssize() << endl;
                }
				size_graphs++;
    		}
    	}
    }


protected:
//    void print(std::ostream& str) {
//    	std::lock_guard<std::mutex> lGuard (mtx);
//    	str << "The number of graphs is: " << map.size() << "\n";
//    	for(auto it = map.begin(); it != map.end(); ++it){
//    		str << ">>>>" << it->first << " " << *(it->second) << endl;
//    	}
//    }
//
//    void toString_sub(std::ostringstream& str) {
//    	std::lock_guard<std::mutex> lGuard (mtx);
//    	str << "The number of graphs is: " << map.size() << "\n";
//    	for(auto it = map.begin(); it != map.end(); ++it){
//    		str << ">>>>" << it->first << " " << *(it->second) << endl;
//    	}
//    }


private:
	//std::unordered_map<PEGraph_Pointer, PEGraph*> map;
    std::unordered_map<PEGraph_Pointer, cachestate*> cache_map;



};



#endif /* COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_ */
