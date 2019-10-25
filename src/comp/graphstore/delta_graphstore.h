/*
 * delta_graphstore.h
 *
 *  Created on: Oct 20, 2019
 *      Author: zzq
 */

#ifndef COMP_GRAPHSTORE_DELTA_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_DELTA_GRAPHSTORE_H_

#include "delta_graph.h"
#include "graphstore.h"

using namespace std;

class DeltaGraphStore : public GraphStore {

public:
	DeltaGraphStore() : GraphStore (true, true) {

	}

	DeltaGraphStore(bool file_mode, bool buffered_m) : GraphStore (file_mode, buffered_m) {

	}

	~DeltaGraphStore(){
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		delete it->second;
    	}

    	for(auto it = bases_set.begin(); it != bases_set.end(); ++it){
    		delete *it;
    	}
	}


	DeltaGraph* retrieve_shallow(PEGraph_Pointer graph_pointer){
//		//for debugging
//		Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

		DeltaGraph *out;

		if (graphs.find(graph_pointer) != graphs.end()) {
			out = graphs[graph_pointer];
		} else {
			out = nullptr;
		}

//		//for debugging
//		Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

		return out;
	}


    //deep copy
    PEGraph* retrieve(PEGraph_Pointer graph_pointer){
//    	//for debugging
//    	Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

    	PEGraph* out;

    	if(graphs.find(graph_pointer) != graphs.end()){
//    		out = new PEGraph(graphs[graph_pointer]->getBase(), graphs[graph_pointer]->getAdded(), graphs[graph_pointer]->getDeleted());
    		out = convertToPEGraph(graphs[graph_pointer]);
    	}
    	else{
    		out = nullptr;
    	}

    	//for debugging
//    	Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

    	return out;
    }

    PEGraph* convertToPEGraph(DeltaGraph* delta){
    	PEGraph* base = delta->getBase();
    	PEGraph* added = delta->getAdded();
    	PEGraph* deleted = delta->getDeleted();

    	//base
    	PEGraph* out = new PEGraph(base);
    	out->merge(added);
    	out->subtract(deleted);

    	return out;
    }

    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {

    }

    DeltaGraph* convertToDeltaGraph(PEGraph* pegraph, DeltaGraph* old_out, std::vector<CFGNode*>* preds){
    	DeltaGraph* pred_graph;
        if(!preds || preds->size() == 0){//entry node
            //return an empty graph
        	pred_graph = NULL;
        }
        else if(preds->size() == 1){
            CFGNode* pred = preds->at(0);
            PEGraph_Pointer out_pointer = pred->getOutPointer();
            pred_graph = retrieve_shallow(out_pointer);
            if(!pred_graph || pred_graph->isNaive()){
            	pred_graph = NULL;
            }
        }
        else{
            for(auto it = preds->cbegin(); it != preds->cend(); it++){
                CFGNode* pred = *it;
                PEGraph_Pointer out_pointer = pred->getOutPointer();
                pred_graph = retrieve_shallow(out_pointer);
                if(!pred_graph || pred_graph->isNaive()){
                	pred_graph = NULL;
                }
                else{
                	break;
                }
            }
        }


//    	if(old_out == NULL){
//    		if(pred_graph == NULL){//predecessors are naive
//				return new DeltaGraph(new PEGraph(pegraph));
//    		}
//    		else{
//    			return constructDeltaGraph(pegraph, pred_graph);
//    		}
//    	}
//    	else if(old_out->isNaive()){
//    		if(pred_graph == NULL){//predecessors are naive
//				return new DeltaGraph(new PEGraph(pegraph));
//    		}
//    		else{
//    			return constructDeltaGraph(pegraph, pred_graph);
//    		}
//    	}
//    	else{
//    		return constructDeltaGraph(pegraph, old_out);
//    	}

		if(pred_graph == NULL){//predecessors are naive
			return new DeltaGraph(new PEGraph(pegraph));
		}
		else{
			return constructDeltaGraph(pegraph, pred_graph);
		}


    }

    DeltaGraph* constructDeltaGraph(PEGraph* pegraph, DeltaGraph* delta_graph){
    	assert(!(delta_graph->isNaive()) && this->bases_set.find(delta_graph->getBase()) != this->bases_set.end());
    	PEGraph* base = delta_graph->getBase();
    	PEGraph* added = new PEGraph();
    	PEGraph* deleted = new PEGraph();

    	for (auto it = base->getGraph().begin(); it != base->getGraph().end(); it++) {
    		if (pegraph->getGraph().find(it->first) != pegraph->getGraph().end()) {
    			int n2 = pegraph->getNumEdges(it->first);
    			int n1 = it->second.getSize();

    			//get added
    			vertexid_t *edges_added = new vertexid_t[n2];
    			char *labels_added = new char[n2];
    			int len_added = myalgo::minusTwoArray(edges_added, labels_added,
    					n2, pegraph->getEdges(it->first), pegraph->getLabels(it->first),
    					n1, it->second.getEdges(), it->second.getLabels());

    			added->getGraph()[it->first].set(len_added, edges_added, labels_added);

    			delete[] edges_added;
    			delete[] labels_added;


    			//get deleted
    			vertexid_t *edges_deleted = new vertexid_t[n1];
    			char *labels_deleted = new char[n1];
    			int len_deleted = myalgo::minusTwoArray(edges_deleted, labels_deleted,
    					n1, it->second.getEdges(), it->second.getLabels(),
						n2, pegraph->getEdges(it->first), pegraph->getLabels(it->first));

    			deleted->getGraph()[it->first].set(len_deleted, edges_deleted, labels_deleted);

    			delete[] edges_deleted;
    			delete[] labels_deleted;
    		}
    		else{
    			deleted->getGraph()[it->first] = it->second;
    		}
    	}

    	for (auto it = pegraph->getGraph().begin(); it != pegraph->getGraph().end(); it++) {
    		if (base->getGraph().find(it->first) != base->getGraph().end()) {

    		}
    		else{
    			added->getGraph()[it->first] = it->second;
    		}
    	}

    	//conditions for deltaGraph construction
    	int num_peg = pegraph->getNumEdges();
    	int num_base = base->getNumEdges();
    	int num_added = added->getNumEdges();
    	int num_deleted = deleted->getNumEdges();

    	if(num_peg <= num_added + num_deleted){
    		delete added;
    		delete deleted;
    		return new DeltaGraph(new PEGraph(pegraph));
    	}

    	if(num_base < num_added){
    		delete added;
    		delete deleted;
    		return new DeltaGraph(new PEGraph(pegraph));
    	}


    	DeltaGraph* out = new DeltaGraph(base, added, deleted, false);
    	return out;
    }

    //shallow copy
    void addOneGraph_atomic(PEGraph_Pointer pointer, DeltaGraph* graph){
    	std::lock_guard<std::mutex> lGuard (mtx);
    	this->graphs[pointer] = graph;
    }

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
	    DeltaGraphStore* another_graphstore = dynamic_cast<DeltaGraphStore*>(another);
	    for(auto& it: another_graphstore->graphs){
	        worklist->push_atomic(it.first);

			//initialize the graphstore
	        if(graphs.find(it.first) == graphs.end()){
	        	graphs[it.first] = new DeltaGraph();
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

    static void update_shallow_parallel(DeltaGraphStore* current, DeltaGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
    		assert(current->graphs.find(id) != current->graphs.end());
    		current->update_shallow(id, another->graphs.at(id));
    	}
    }


    void update_graphs_shallow_sequential(GraphStore* another){
    	DeltaGraphStore* another_graphstore = dynamic_cast<DeltaGraphStore*>(another);
    	for(auto& it: another_graphstore->graphs){
    		update_shallow(it.first, it.second);
    	}
    }

    //shallow copy
    void update_shallow(PEGraph_Pointer graph_pointer, DeltaGraph* graph) {
//    	//for debugging
//    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

//    	assert(map.find(graph_pointer) != map.end());
    	if(graphs.find(graph_pointer) != graphs.end()){
			delete graphs[graph_pointer];
    	}
    	graphs[graph_pointer] = graph;

    	//for debugging
//    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }


    void loadGraphStore(const string& file, const string& file_in, Partition part, int mining_mode, int support, int length) {
		//graphstore file
		this->deserialize(file);

//		cout << "deserialize ended." << endl;

		//load in-graphs
		this->load_in_graphs(file_in);

//		cout << "load_in_graphs ended." << endl;

		//construct base
		this->constructDeltaGraphBases();
    }

    void update_graphs_sequential(GraphStore* another) {

    }

    void update_graphs_parallel(GraphStore* another) {

    }

    void deserialize(const string& file) {
    	load_deltaGraphstore(file);
    }

    void load_deltaGraphstore(const string& file) {
    	if(file_mode){
			cout << "wrong file w/r mode while loading deltaGraphStore!" << endl;
			exit(-1);
    	}
    	else{
    		if(buffered_mode){//pegraph-level
    			unordered_map<PEGraph*, PEGraph*> map;

    			//read bases
    			string file_bases = file + ".bases";
        		FILE *fp_bases = fopen(file_bases.c_str(),"rb");
        		if(!fp_bases) {
        			cout << "can't load graphs file: " << file_bases << endl;
    //    			exit(-1);
        		}
        		else{
    				size_t freadRes = 0; //clear warnings
    				size_t bufsize;
    				while(fread(&bufsize, sizeof(size_t), 1, fp_bases) != 0) {
    					char *buf = (char*)malloc(bufsize);
    					freadRes = fread(buf, bufsize, 1, fp_bases);

    					PEGraph* pointer = *((PEGraph**)(buf));
//    					PEGraph* pointer;
//    					freadRes = fread(&pointer, sizeof(PEGraph*), 1, fp_bases);

    					PEGraph* pegraph = new PEGraph();
    					pegraph->read_from_buf_pure(buf + sizeof(PEGraph*), bufsize - sizeof(PEGraph*));
    					free(buf);

    					this->bases_set.insert(pegraph);
    					map[pointer] = pegraph;
    				}
        			fclose(fp_bases);
        			FileUtil::delete_file(file_bases);
        		}

        		//read graphs
    			string file_graphs = file + ".graphs";
        		FILE *fp_graphs = fopen(file_graphs.c_str(),"rb");
        		if(!fp_graphs) {
        			cout << "can't load graphs file: " << file_graphs << endl;
    //    			exit(-1);
        		}
        		else{
    				size_t freadRes = 0; //clear warnings
    				PEGraph_Pointer graph_pointer;
    				while(fread(&graph_pointer, sizeof(PEGraph_Pointer), 1, fp_graphs) != 0) {
    					//read base
    					bool is_naive;
    					freadRes = fread(&is_naive, sizeof(bool), 1, fp_graphs);

    					PEGraph* base;
    					size_t size_base;
    					freadRes = fread(&size_base, sizeof(size_t), 1, fp_graphs);
    					if(!is_naive){
    						PEGraph* pointer;
    						freadRes = fread(&pointer, sizeof(PEGraph*), 1, fp_graphs);
    						assert(map.find(pointer) != map.end());
    						base = map[pointer];
    					}
    					else{
        					char *buf_base = (char*)malloc(size_base);
        					freadRes = fread(buf_base, size_base, 1, fp_graphs);
        					base = new PEGraph();
        					base->read_from_buf_pure(buf_base, size_base);
        					free(buf_base);
    					}

    					//read added
    					size_t size_added;
    					freadRes = fread(&size_added, sizeof(size_t), 1, fp_graphs);
    					char *buf_added = (char*)malloc(size_added);
    					freadRes = fread(buf_added, size_added, 1, fp_graphs);
    					PEGraph* added = new PEGraph();
    					added->read_from_buf_pure(buf_added, size_added);
    					free(buf_added);

    					//read deleted
    					size_t size_deleted;
    					freadRes = fread(&size_deleted, sizeof(size_t), 1, fp_graphs);
    					char *buf_deleted = (char*)malloc(size_deleted);
    					freadRes = fread(buf_deleted, size_deleted, 1, fp_graphs);
    					PEGraph* deleted = new PEGraph();
    					deleted->read_from_buf_pure(buf_deleted, size_deleted);
    					free(buf_deleted);

    					//put into graphs
    					DeltaGraph* graph = new DeltaGraph(base, added, deleted, is_naive);
    					this->graphs[graph_pointer] = graph;
    				}
    				fclose(fp_graphs);
    				FileUtil::delete_file(file_graphs);
        		}

    		}
    		else {
    			cout << "wrong file w/r mode while loading deltaGraphStore!" << endl;
    			exit(-1);
    		}
    	}
    }

    void serialize(const string& file) {
    	store_deltaGraphstore(file);
    }

    void store_deltaGraphstore(const string& file) {
       	if(file_mode){
    		cout << "wrong file w/r mode while storing deltaGraphStore!" << endl;
    		exit(-1);
        }
    	else{
    		if(buffered_mode){//pegraph-level
				//write bases
				string file_bases = file + ".bases";
				FILE * f_bases = fopen(file_bases.c_str(),"wb");
				if(f_bases == NULL) {
					cout << "can't write to file: " << file_bases << endl;
					exit(-1);
				}
				else{
					char* buf = (char*) malloc(IO_SIZE);
					long offset = 0;
					for(auto it = this->bases_set.begin(); it != this->bases_set.end(); ++it){
						PEGraph* graph = *it;
						size_t size = sizeof(PEGraph*) + graph->compute_size_bytes();

						long bufsize = sizeof(size_t) + size;

						if(bufsize >= IO_SIZE){
							fwrite((const void*)& size, sizeof(size_t), 1, f_bases);
							fwrite((const void*)& graph, sizeof(PEGraph*), 1, f_bases);
							graph->write_unreadable_pure(f_bases);
						}
						else{
							if(bufsize > IO_SIZE - offset){
								fwrite(buf, offset, 1, f_bases);
								offset = 0;
							}
							memcpy(buf + offset, (char*) &size, sizeof(size_t));
							offset += sizeof(size_t);
							memcpy(buf + offset, (char*) &graph, sizeof(PEGraph*));
							offset += sizeof(PEGraph*);
							offset = graph->write_to_buf_pure(buf, offset);
						}
					}
					fwrite(buf, offset, 1, f_bases);

					free(buf);
					fclose(f_bases);
				}



				//write graphs
				string file_graphs = file + ".graphs";
				FILE * f_graphs = fopen(file_graphs.c_str(),"wb");
				if(f_graphs == NULL) {
					cout << "can't write to file: " << file_graphs << endl;
					exit(-1);
				}
				else{
					char* buf = (char*) malloc(IO_SIZE);
					long offset = 0;
					for(auto it = this->graphs.begin(); it != this->graphs.end(); ++it){
						PEGraph_Pointer pointer = it->first;
						DeltaGraph *graph = it->second;
						bool is_naive = graph->isNaive();

						PEGraph* base = graph->getBase();
						size_t size_base;
						if(!is_naive){
							size_base = sizeof(PEGraph*);
						}
						else{
							size_base = base->compute_size_bytes();
						}

						PEGraph* added = graph->getAdded();
						size_t size_added = added->compute_size_bytes();

						PEGraph* deleted = graph->getDeleted();
						size_t size_deleted = deleted->compute_size_bytes();

						long bufsize = sizeof(PEGraph_Pointer)
								+ sizeof(bool) + sizeof(size_t) + size_base
								+ sizeof(size_t) + size_added
								+ sizeof(size_t) + size_deleted
								;

						if(bufsize >= IO_SIZE){
							fwrite((const void*)& pointer, sizeof(PEGraph_Pointer), 1, f_graphs);
							fwrite((const void*)& is_naive, sizeof(bool), 1, f_graphs);
							fwrite((const void*)& size_base, sizeof(size_t), 1, f_graphs);
							if(!is_naive){
								fwrite((const void*)& base, sizeof(PEGraph*), 1, f_graphs);
							}
							else{
								base->write_unreadable_pure(f_graphs);
							}

							fwrite((const void*)& size_added, sizeof(size_t), 1, f_graphs);
							added->write_unreadable_pure(f_graphs);

							fwrite((const void*)& size_deleted, sizeof(size_t), 1, f_graphs);
							deleted->write_unreadable_pure(f_graphs);
						}
						else{
							if(bufsize > IO_SIZE - offset){
								fwrite(buf, offset, 1, f_graphs);
								offset = 0;
							}

							memcpy(buf + offset, (char*) &pointer, sizeof(PEGraph_Pointer));
							offset += sizeof(PEGraph_Pointer);

							memcpy(buf + offset, (char*) &is_naive, sizeof(bool));
							offset += sizeof(bool);

							memcpy(buf + offset, (char*) &size_base, sizeof(size_t));
							offset += sizeof(size_t);
							if(!is_naive){
								memcpy(buf + offset, (char*) & base, sizeof(PEGraph*));
								offset += sizeof(PEGraph*);
							}
							else{
								offset = base->write_to_buf_pure(buf, offset);
							}

							memcpy(buf + offset, (char*) &size_added, sizeof(size_t));
							offset += sizeof(size_t);
							offset = added->write_to_buf_pure(buf, offset);

							memcpy(buf + offset, (char*) &size_deleted, sizeof(size_t));
							offset += sizeof(size_t);
							offset = deleted->write_to_buf_pure(buf, offset);

						}
					}
					fwrite(buf, offset, 1, f_graphs);

					free(buf);
					fclose(f_graphs);
				}


    		}
    		else{//default-buffered
        		cout << "wrong file w/r mode while storing deltaGraphStore!" << endl;
        		exit(-1);
    		}
    	}

    }

    bool isDeltaBase(PEGraph* base){
    	return this->bases_set.find(base) != this->bases_set.end();
    }

    void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set) {
		//for debugging
		Logger::print_thread_info_locked("store-in-graphs starting...\n", LEVEL_LOG_FUNCTION);

		if(file_mode){
			cout << "wrong file w/r mode while storing deltaGraphs!" << endl;
			exit(-1);
		}
		else{
    		if(buffered_mode){//pegraph-level
				FILE *f = fopen(file_graphs_in.c_str(),"ab");
				if(f == NULL) {
					cout << "can't write to file: " << file_graphs_in << endl;
					exit(-1);
				}
				else{
    				for (auto& n : set) {
						auto graph_pointer = n->getOutPointer();
						DeltaGraph* graph = retrieve_shallow(graph_pointer);
						if(graph){
							PEGraph* base = graph->getBase();
							size_t size_base = base->compute_size_bytes();

							PEGraph* added = graph->getAdded();
							size_t size_added = added->compute_size_bytes();

							PEGraph* deleted = graph->getDeleted();
							size_t size_deleted = deleted->compute_size_bytes();

							size_t bufsize = sizeof(PEGraph_Pointer)
									+ sizeof(size_t) + size_base
									+ sizeof(size_t) + size_added
									+ sizeof(size_t) + size_deleted;
							fwrite((const void*)& bufsize, sizeof(size_t), 1, f);

							char *buf = (char*)malloc(bufsize);
							size_t offset = 0;

							memcpy(buf + offset, (char*)&graph_pointer, sizeof(PEGraph_Pointer));
							offset += sizeof(PEGraph_Pointer);

							memcpy(buf + offset, (char*)& size_base, sizeof(size_t));
							offset += sizeof(size_t);
							offset = base->write_to_buf_pure(buf, offset);

							memcpy(buf + offset, (char*)& size_added, sizeof(size_t));
							offset += sizeof(size_t);
							offset = added->write_to_buf_pure(buf, offset);

							memcpy(buf + offset, (char*)& size_deleted, sizeof(size_t));
							offset += sizeof(size_t);
							offset = deleted->write_to_buf_pure(buf, offset);

							fwrite(buf, bufsize, 1, f);
							free(buf);
						}
    				}
					fclose(f);
    			}
    		}
    		else {
				cout << "wrong file w/r mode while storing deltaGraphs!" << endl;
				exit(-1);
			}
		}

		//for debugging
		Logger::print_thread_info_locked("store-in-graphs finished.\n", LEVEL_LOG_FUNCTION);
    }

    void load_in_graphs(const string& file) {
    	if(file_mode){
			cout << "wrong file w/r mode while loading deltaGraphs!" << endl;
			exit(-1);
    	}
    	else{
    		if(buffered_mode){//pegraph-level
				FILE *fp = fopen(file.c_str(),"rb");
				if(!fp) {
					cout << "can't load graphs file: " << file << endl;
	//    			exit(-1);
				}
				else{
    				size_t freadRes = 0; //clear warnings
    				size_t bufsize;
    				while(fread(&bufsize, sizeof(size_t), 1, fp) != 0) {
    					char *buf = (char*)malloc(bufsize);
    					freadRes = fread(buf, bufsize, 1, fp);

    			    	size_t offset = 0;

    			    	PEGraph_Pointer graph_pointer = *((PEGraph_Pointer*)(buf));
    			    	offset += sizeof(PEGraph_Pointer);

    			    	DeltaGraph* graph = new DeltaGraph();

    			    	size_t size_base = *((size_t*)(buf + offset));
    			    	offset += sizeof(size_t);

    					graph->getBase()->read_from_buf_pure(buf + offset, size_base);
    					offset += size_base;

    			    	size_t size_added = *((size_t*)(buf + offset));
    			    	offset += sizeof(size_t);

    					graph->getAdded()->read_from_buf_pure(buf + offset, size_added);
    					offset += size_added;

    			    	size_t size_deleted = *((size_t*)(buf + offset));
    			    	offset += sizeof(size_t);

    					graph->getDeleted()->read_from_buf_pure(buf + offset, size_deleted);
    					offset += size_deleted;

    					free(buf);

						//since the file is appended, we just use the recent updated pegraph
						if (graphs.find(graph_pointer) != graphs.end()) {
							delete graphs[graph_pointer];
						}
						graphs[graph_pointer] = graph;
    				}

					fclose(fp);

					//delete the old graphstore file
					FileUtil::delete_file(file);
    			}
    		}
			else{
				cout << "wrong file w/r mode while loading deltaGraphs!" << endl;
				exit(-1);
			}
    	}
    }


    void printOutInfo() {
    	int size_graphs = graphs.size();
    	long size_edges = 0;
    	long size_items = 0;

    	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		long size_base = it->second->getBase()->getNumEdges();
    		long size_added = it->second->getAdded()->getNumEdges();
    		long size_deleted = it->second->getDeleted()->getNumEdges();
    		cout << it->first << ":\t" << size_base << ", " << size_added << ", " << size_deleted << "\n";
    		size_edges += size_base + size_added - size_deleted;
    		size_items += (it->second->isNaive() ? size_base : 1) + size_added + size_deleted;
    	}

    	cout << endl << endl;

    	for(auto it: bases_set){
    		cout << it << ":\t" << it->getNumEdges() << "\n";
    		size_items += it->getNumEdges();
    	}

    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    	cout << "Number of items: " << size_items << endl;
    }


    void getStatistics(int& size_graphs, long& size_edges, long& size_graphitems, long& size_baseitems, const std::unordered_set<PEGraph_Pointer>& mirrors) {
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		if(mirrors.find(it->first) == mirrors.end()){
        		long size_base = it->second->getBase()->getNumEdges();
        		long size_added = it->second->getAdded()->getNumEdges();
        		long size_deleted = it->second->getDeleted()->getNumEdges();

        		size_graphitems += (it->second->isNaive() ? size_base : 1) + size_added + size_deleted;
				size_edges += size_base + size_added - size_deleted;
				size_graphs++;
    		}
    	}

    	for(auto it: bases_set){
    		size_baseitems += it->getNumEdges();
    	}
    }

    void clear_shallow(){
    	assert(bases_set.empty());
    	this->graphs.clear();
    }

    void constructDeltaGraphBases(){

    }

	std::unordered_set<PEGraph*>& getBasesSet() {
		return bases_set;
	}

protected:

    void print(std::ostream& str) {

    }

    void toString_sub(std::ostringstream& strm) {

    }


private:
	std::unordered_set<PEGraph*> bases_set;

	std::unordered_map<PEGraph_Pointer, DeltaGraph*> graphs;



};


#endif /* COMP_GRAPHSTORE_DELTA_GRAPHSTORE_H_ */
