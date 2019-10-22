/*
 * itemset_graphstore.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_

#include "graphstore.h"
#include "edge.h"
#include "itemset_graph.h"
#include "fptree.hpp"


using namespace std;

const string inputFile = "../lib/file/input_itemsets_";
const string outFile = "../lib/file/out_itemsets_";


class ItemsetGraphStore : public GraphStore {

public:
	ItemsetGraphStore() : GraphStore (true, true) {

	}

	ItemsetGraphStore(bool file_mode, bool buffered_m) : GraphStore (file_mode, buffered_m) {

	}

    ~ItemsetGraphStore(){
    	//delete ItemsetGraph in graphs
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		delete it->second;
    	}

    	//delete ItemsetGraph in intToItemset
    	for(auto it = intToItemset.begin(); it != intToItemset.end(); ++it){
    		delete *it;
    	}

//    	intToEdge.clear();
//    	edgeToInt.clear();
    }


    PEGraph* retrieve(PEGraph_Pointer graph_pointer) {
//		//for debugging
//		Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

		PEGraph *out;

		if (graphs.find(graph_pointer) != graphs.end()) {
//			out = new PEGraph(graphs[graph_pointer]);
			out = convertToPEGraph(graphs[graph_pointer]);
		}
		else {
			out = nullptr;
		}

//		//for debugging
//		Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

		return out;
    }

    ItemsetGraph* retrieve_direct(PEGraph_Pointer graph_pointer) {
//		//for debugging
//		Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

    	ItemsetGraph *out;

		if (graphs.find(graph_pointer) != graphs.end()) {
//			out = new PEGraph(graphs[graph_pointer]);
			out = graphs[graph_pointer];
		}
		else {
			out = nullptr;
		}

//		//for debugging
//		Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

		return out;
    }

    static bool isItemset(int id_edge){
    	return id_edge < 0;
    }


    PEGraph* convertToPEGraph(ItemsetGraph* graph){
    	//collect all the edges
    	vector<int> edges;
    	vector<ItemsetGraph*> itemset_graphs;
    	itemset_graphs.push_back(graph);
    	while(!itemset_graphs.empty()){
    		ItemsetGraph* g = itemset_graphs.back();
    		itemset_graphs.pop_back();
			for(unsigned int i = 0; i < g->getLength(); ++i){
				int id_edge = g->getEdgeId(i);
				if(isItemset(id_edge)){
					itemset_graphs.push_back(intToItemset[getItemsetIndex(id_edge)]);
				}
				else{
					edges.push_back(id_edge);
				}
			}
    	}
//    	cout << edges.size() << endl;
//    	cout << "size of intToEdge: " << intToEdge.size() << endl;

    	//construct pegraph
    	PEGraph* peg = new PEGraph();
    	for(auto it = edges.begin(); it != edges.end(); ++it){
    		int edge_id = *it;
    		Edge edge = intToEdge[edge_id];

//    		//for debugging
//    		cout << edge << endl;

    		if(peg->getGraph().find(edge.getSrcId()) == peg->getGraph().end()){
    			peg->getGraph()[edge.getSrcId()] = EdgeArray();
    		}
    		peg->getGraph()[edge.getSrcId()].addOneEdge(edge.getDstId(), edge.getLabel());
    	}
    	//sort all the edgeArrays in peg
    	for(auto it = peg->getGraph().begin(); it != peg->getGraph().end(); ++ it){
			(*it).second.sort();
    	}

//    	cout << *peg << endl;
//    	cout << peg->getGraph().size() << endl;
    	return peg;
    }

    void update_convert_locked(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph_locked(pegraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph(pegraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_hybrid(PEGraph_Pointer graph_pointer, HybridGraph* hybridgraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph_hybrid(hybridgraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_convert_hybrid_locked(PEGraph_Pointer graph_pointer, HybridGraph* pegraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph_hybrid_locked(pegraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void loadGraphStore(const string& file, const string& file_in, Partition part, int mining_mode, int support, int length) {
		//graphstore file
		this->deserialize(file);

//		cout << "deserialize ended." << endl;

		//load in-graphs
		this->load_in_graphs(file_in);

//		cout << "load_in_graphs ended." << endl;

		//construct itemset base
		if(mining_mode == 0){
			this->constructItemsetBase_eclat(part, support, length);
		}
		else if(mining_mode == 1){
			this->constructItemsetBase_apriori(part, support, length);
		}
		else if(mining_mode == 2){
			this->constructItemsetBase_fpgrowth(part, support, length);
		}
		else{
			cout << "wrong mining option!" << endl;
			exit(-1);
		}

//		cout << "construct itemset base." << endl;
    }

    void serialize(const string& file){
    	if(serialize_peg_mode){
    		if(file_mode){
				ofstream myfile;
				myfile.open(file, std::ofstream::out);
				if (myfile.is_open()){
					for (auto& n : graphs) {
						//write a pegraph into file
						myfile << n.first << "\t";
						PEGraph* out = convertToPEGraph(n.second);
						out->write_readable(myfile);
						delete out;
						myfile << "\n";
					}
					myfile.close();
				}
    		}
    		else{
        		FILE *f = fopen(file.c_str(),"wb");
        		if(f == NULL) {
        			cout << "can't write to file: " << file << endl;
        			exit(-1);
        		}
        		else{
        			if(buffered_mode){//pegraph-level
        				for (auto& n : graphs) {
        					PEGraph* out = convertToPEGraph(n.second);
        					size_t bufsize = sizeof(PEGraph_Pointer) + out->compute_size_bytes();
        					fwrite((const void*)& bufsize, sizeof(size_t), 1, f);
        					char *buf = (char*)malloc(bufsize);
        					out->write_to_buf(buf, n.first);
        					delete out;
        					fwrite(buf, bufsize, 1, f);
        					free(buf);
        				}
        			}
        			else{
						for (auto& n : graphs) {
							PEGraph_Pointer graph_pointer = n.first;
							fwrite((const void*)& graph_pointer, sizeof(PEGraph_Pointer), 1, f);
							PEGraph* out = convertToPEGraph(n.second);
							out->write_unreadable(f);
							delete out;
						}
        			}
    				fclose(f);
        		}
    		}
    	}
    	else{//direct written in the itemset format
   			store_itemsetGraphstore(file);
    	}
    }

    void deserialize(const string& file){
    	if(serialize_peg_mode){
    		load_naiveGraphStore(file);
    	}
    	else{
   			load_itemsetGraphstore(file);
    	}
    }

    void load_naiveGraphStore(const string& file){
    	load_in_graphs(file);
    }

    long get_binary_size(FILE *fp){
    	fseek(fp, 0, SEEK_END);
    	auto size = ftell(fp);
    	return size;
    }

    //load the graphstore in the itemset format
    void load_itemsetGraphstore(const string& file){
    	if(file_mode){
			cout << "wrong file w/r mode while loading itemsetGraphStore!" << endl;
			exit(-1);
    	}
    	else{
    		if(buffered_mode){//pegraph-level
    			//read intToEdge
//    			cout << "load intToEdge" << endl;
        		string file_intToEdge = file + ".intToEdge";
				int fp_intToEdge = open(file_intToEdge.c_str(), O_RDONLY);
        		if(!(fp_intToEdge > 0)) {
        			cout << "can't load graphs file: " << file_intToEdge << endl;
    //    			exit(-1);
        		}
        		else{
					long inttoedge_file_size = io_manager::get_filesize(fp_intToEdge);
					char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					long real_io_size = get_real_io_size(IO_SIZE, Edge::getSizeof());
					int streaming_counter = inttoedge_file_size / real_io_size + 1;

					long offset_read = 0;

					for(int counter = 0; counter < streaming_counter; counter++) {
						long valid_io_size = 0;
						if (counter == streaming_counter - 1)
							valid_io_size = inttoedge_file_size - real_io_size * (streaming_counter - 1);
						else
							valid_io_size = real_io_size;

						io_manager::read_from_file(fp_intToEdge, buf, valid_io_size, offset_read);
						offset_read += valid_io_size;

						for(long offset = 0; offset < valid_io_size; offset += Edge::getSizeof()) {
							Edge edge(*((vertexid_t*)(buf + offset)), *((vertexid_t*)(buf + offset + sizeof(vertexid_t))), *((label_t*)(buf + offset + sizeof(vertexid_t) * 2)));
							this->intToEdge.push_back(edge);
						}
					}
					free(buf);

					close(fp_intToEdge);
    		    	FileUtil::delete_file(file_intToEdge);
        		}


				//read edgeToInt
//        		cout << "load edgeToInt" << endl;
        		string file_edgeToInt = file + ".edgeToInt";
        		int fp_edgetoint = open(file_edgeToInt.c_str(), O_RDONLY);
        		if(!(fp_edgetoint > 0)) {
        			cout << "can't load graphs file: " << file_edgeToInt << endl;
    //    			exit(-1);
        		}
        		else{
        			long edgetoint_file_size = io_manager::get_filesize(fp_edgetoint);
					char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					long real_io_size = get_real_io_size(IO_SIZE, Edge::getSizeof() + sizeof(int));
					int streaming_counter = edgetoint_file_size / real_io_size + 1;
					long offset_read = 0;

					for(int counter = 0; counter < streaming_counter; counter++) {
						long valid_io_size = 0;
						if (counter == streaming_counter - 1)
							valid_io_size = edgetoint_file_size - real_io_size * (streaming_counter - 1);
						else
							valid_io_size = real_io_size;

						io_manager::read_from_file(fp_edgetoint, buf, valid_io_size, offset_read);
						offset_read += valid_io_size;

						for(long offset = 0; offset < valid_io_size; offset += Edge::getSizeof() + sizeof(int)) {
							Edge edge(*((vertexid_t*)(buf + offset)), *((vertexid_t*)(buf + offset + sizeof(vertexid_t))), *((label_t*)(buf + offset + sizeof(vertexid_t) * 2)));
							int index = *((int*)(buf + offset + Edge::getSizeof()));
							this->edgeToInt[edge] = index;
						}
					}
					free(buf);

        			close(fp_edgetoint);
    		    	FileUtil::delete_file(file_edgeToInt);
        		}

        		//read itemsetGraphs
//        		cout << "load itemsetGraphs" << endl;
				string file_itemsetgraphs = file + ".itemsetGraphs";
				FILE *fp_itemsetgraphs = fopen(file_itemsetgraphs.c_str(),"rb");
				if(!fp_itemsetgraphs) {
					cout << "can't load graphs file: " << file_itemsetgraphs << endl;
	//    			exit(-1);
				}
				else{
					PEGraph_Pointer graph_pointer;
					while(fread(&graph_pointer, sizeof(PEGraph_Pointer), 1, fp_itemsetgraphs) != 0) {
						ItemsetGraph* graph = new ItemsetGraph();
						graph->load_unreadable(fp_itemsetgraphs);
						graphs[graph_pointer] = graph;
					}
					fclose(fp_itemsetgraphs);
					FileUtil::delete_file(file_itemsetgraphs);
				}


				//read intToItemset
//				cout << "load intToItemset" << endl;
				string file_intToItemset = file + ".intToItemset";
				FILE *fp_inttoitemset = fopen(file_intToItemset.c_str(),"rb");
				if(!fp_inttoitemset) {
					cout << "can't load graphs file: " << file_intToItemset << endl;
	//    			exit(-1);
				}
				else{
					unsigned int len;
					while(fread(&len, sizeof(unsigned int), 1, fp_inttoitemset) != 0) {
						ItemsetGraph* graph = new ItemsetGraph(len);
						fread(graph->getEdgeIds(), sizeof(int) * len, 1, fp_inttoitemset);
						this->intToItemset.push_back(graph);
					}
					fclose(fp_inttoitemset);
					FileUtil::delete_file(file_intToItemset);
				}

    		}
    		else{//default-buffered
    			//read intToEdge
				string file_intToEdge = file + ".intToEdge";
				FILE *fp_intToEdge = fopen(file_intToEdge.c_str(),"rb");
				if(!fp_intToEdge) {
					cout << "can't load graphs file: " << file_intToEdge << endl;
	//    			exit(-1);
				}
				else{
					char* buf_edge = (char*) malloc(Edge::getSizeof());
					while(fread(buf_edge, Edge::getSizeof(), 1, fp_intToEdge) != 0) {
						Edge edge(*((vertexid_t*)(buf_edge)), *((vertexid_t*)(buf_edge + sizeof(vertexid_t))), *((label_t*)(buf_edge + sizeof(vertexid_t) * 2)));
						this->intToEdge.push_back(edge);
					}
					free(buf_edge);
					fclose(fp_intToEdge);
					FileUtil::delete_file(file_intToEdge);
				}

				//read edgeToInt
				string file_edgeToInt = file + ".edgeToInt";
				FILE *fp_edgetoint = fopen(file_edgeToInt.c_str(),"rb");
				if(!fp_edgetoint) {
					cout << "can't load graphs file: " << file_edgeToInt << endl;
	//    			exit(-1);
				}
				else{
					char* buf_edge_int = (char*) malloc(Edge::getSizeof() + sizeof(int));
					while (fread(buf_edge_int, Edge::getSizeof() + sizeof(int), 1, fp_edgetoint) != 0) {
						Edge edge(*((vertexid_t*) (buf_edge_int)), *((vertexid_t*) (buf_edge_int + sizeof(vertexid_t))), *((label_t*) (buf_edge_int + sizeof(vertexid_t) * 2)));
						int index = *((int*)(buf_edge_int + Edge::getSizeof()));
						this->edgeToInt[edge] = index;
					}
					free(buf_edge_int);
					fclose(fp_edgetoint);
					FileUtil::delete_file(file_edgeToInt);
				}

				//read itemsetGraphs
				string file_itemsetgraphs = file + ".itemsetGraphs";
				FILE *fp_itemsetgraphs = fopen(file_itemsetgraphs.c_str(),"rb");
				if(!fp_itemsetgraphs) {
					cout << "can't load graphs file: " << file_itemsetgraphs << endl;
	//    			exit(-1);
				}
				else{
					PEGraph_Pointer graph_pointer;
					while(fread(&graph_pointer, sizeof(PEGraph_Pointer), 1, fp_itemsetgraphs) != 0) {
						ItemsetGraph* graph = new ItemsetGraph();
						graph->load_unreadable(fp_itemsetgraphs);
						graphs[graph_pointer] = graph;
					}
					fclose(fp_itemsetgraphs);
					FileUtil::delete_file(file_itemsetgraphs);
				}

				//read intToItemset
				string file_intToItemset = file + ".intToItemset";
				FILE *fp_inttoitemset = fopen(file_intToItemset.c_str(),"rb");
				if(!fp_inttoitemset) {
					cout << "can't load graphs file: " << file_intToItemset << endl;
	//    			exit(-1);
				}
				else{
					unsigned int len;
					while(fread(&len, sizeof(unsigned int), 1, fp_inttoitemset) != 0) {
						ItemsetGraph* graph = new ItemsetGraph(len);
						fread(graph->getEdgeIds(), sizeof(int) * len, 1, fp_inttoitemset);
						this->intToItemset.push_back(graph);
					}
					fclose(fp_inttoitemset);
					FileUtil::delete_file(file_intToItemset);
				}

    		}

    	}
    }


    void store_itemsetGraphstore(const string& file){
    	if(file_mode){
			cout << "wrong file w/r mode while storing itemsetGraphStore!" << endl;
			exit(-1);
    	}
    	else{
    		if(buffered_mode){//pegraph-level
				//write intToEdge
//    			cout << "write intToEdge" << endl;
//				string file_intToEdge = file + ".intToEdge";
//				FILE * f_intToEdge = fopen(file_intToEdge.c_str(), "wb");
//				if(f_intToEdge == NULL) {
//					cout << "can't write to file: " << file_intToEdge << endl;
//					exit(-1);
//				}
//				else{
//					size_t bufsize = this->intToEdge.size() * (Edge::getSizeof());
//					char* buf = (char*) malloc(bufsize);
//					size_t offset = 0;
//					for(auto it = this->intToEdge.begin(); it != this->intToEdge.end(); ++it){
//						Edge edge = *it;
//						offset = edge.write_to_buf(buf, offset);
//					}
//					fwrite(buf, bufsize, 1, f_intToEdge);
//					free(buf);
//					fclose(f_intToEdge);
//				}
				string file_intToEdge = file + ".intToEdge";
				FILE *f_intToEdge = fopen(file_intToEdge.c_str(), "wb");
				if (f_intToEdge == NULL) {
					cout << "can't write to file: " << file_intToEdge << endl;
					exit(-1);
				}
				else {
//					char * buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					char* buf = (char*) malloc(IO_SIZE);
					long real_io_size = get_real_io_size(IO_SIZE, Edge::getSizeof());
					long offset = 0;
					for (auto it = this->intToEdge.begin(); it != this->intToEdge.end(); ++it) {
						Edge edge = *it;
						offset = edge.write_to_buf(buf, offset);

				    	if(offset == real_io_size){
							fwrite(buf, real_io_size, 1, f_intToEdge);
							offset = 0;
				    	}
					}
					//write the remaining part to file
					fwrite(buf, offset, 1, f_intToEdge);

					free(buf);
					fclose(f_intToEdge);
				}


				//write edgeToInt
//				cout << "write edgeToInt" << endl;
//				string file_edgeToInt = file + ".edgeToInt";
//				FILE * f_edgeToInt = fopen(file_edgeToInt.c_str(),"wb");
//				if(f_edgeToInt == NULL) {
//					cout << "can't write to file: " << file_edgeToInt << endl;
//					exit(-1);
//				}
//				else{
//					size_t bufsize = this->edgeToInt.size() * ((Edge::getSizeof()) + sizeof(int));
//					char* buf = (char*) malloc(bufsize);
//					size_t offset = 0;
//					for(auto it = this->edgeToInt.begin(); it != this->edgeToInt.end(); ++it){
//						Edge edge = it->first;
//						int index = it->second;
//						offset = edge.write_to_buf(buf, offset);
//						memcpy(buf + offset, (char*)& index, sizeof(int));
//						offset += sizeof(int);
//					}
//					fwrite(buf, bufsize, 1, f_edgeToInt);
//					free(buf);
//					fclose(f_edgeToInt);
//				}
				string file_edgeToInt = file + ".edgeToInt";
				FILE * f_edgeToInt = fopen(file_edgeToInt.c_str(),"wb");
				if(f_edgeToInt == NULL) {
					cout << "can't write to file: " << file_edgeToInt << endl;
					exit(-1);
				}
				else{
//					char * buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					char* buf = (char*) malloc(IO_SIZE);
					long real_io_size = get_real_io_size(IO_SIZE, Edge::getSizeof() + sizeof(int));
					long offset = 0;
					for(auto it = this->edgeToInt.begin(); it != this->edgeToInt.end(); ++it){
						Edge edge = it->first;
						int index = it->second;
						offset = edge.write_to_buf(buf, offset);
						memcpy(buf + offset, (char*)& index, sizeof(int));
						offset += sizeof(int);

				    	if(offset == real_io_size){
							fwrite(buf, real_io_size, 1, f_edgeToInt);
							offset = 0;
				    	}
					}
					fwrite(buf, offset, 1, f_edgeToInt);

					free(buf);
					fclose(f_edgeToInt);
				}


				//write graphs
//				cout << "write itemsetGraphs" << endl;
//				string file_itemsetgraphs = file + ".itemsetGraphs";
//				FILE * f_itemsetGraphs = fopen(file_itemsetgraphs.c_str(),"wb");
//				if(f_itemsetGraphs == NULL) {
//					cout << "can't write to file: " << file_itemsetgraphs << endl;
//					exit(-1);
//				}
//				else{
//					size_t bufsize = 0;
//					for(auto it : this->graphs){
//						bufsize += sizeof(PEGraph_Pointer) + sizeof(unsigned int) + it.second->getLength() * sizeof(int);
//					}
//					char* buf = (char*) malloc(bufsize);
//					size_t offset = 0;
//					for(auto it = this->graphs.begin(); it != this->graphs.end(); ++it){
//						PEGraph_Pointer pointer = it->first;
//						ItemsetGraph* graph = it->second;
//						memcpy(buf + offset, (char*)& pointer, sizeof(PEGraph_Pointer));
//						offset += sizeof(PEGraph_Pointer);
//						offset = graph->write_to_buf(buf, offset);
//					}
//					fwrite(buf, bufsize, 1, f_itemsetGraphs);
//					free(buf);
//					fclose(f_itemsetGraphs);
//				}
				string file_itemsetgraphs = file + ".itemsetGraphs";
				FILE * f_itemsetGraphs = fopen(file_itemsetgraphs.c_str(),"wb");
				if(f_itemsetGraphs == NULL) {
					cout << "can't write to file: " << file_itemsetgraphs << endl;
					exit(-1);
				}
				else{
//					char * buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					char* buf = (char*) malloc(IO_SIZE);
					long offset = 0;
					for(auto it = this->graphs.begin(); it != this->graphs.end(); ++it){
						PEGraph_Pointer pointer = it->first;
						ItemsetGraph *graph = it->second;
						long bufsize = sizeof(PEGraph_Pointer) + sizeof(unsigned int) + graph->getLength() * sizeof(int);

						if(bufsize >= IO_SIZE){
							fwrite((const void*)& pointer, sizeof(PEGraph_Pointer), 1, f_itemsetGraphs);
							graph->write_unreadable(f_itemsetGraphs);
						}
						else{
							if(bufsize > IO_SIZE - offset){
								fwrite(buf, offset, 1, f_itemsetGraphs);
								offset = 0;
							}
							memcpy(buf + offset, (char*) &pointer, sizeof(PEGraph_Pointer));
							offset += sizeof(PEGraph_Pointer);
							offset = graph->write_to_buf(buf, offset);
						}
					}
					fwrite(buf, offset, 1, f_itemsetGraphs);

					free(buf);
					fclose(f_itemsetGraphs);
				}


				//write itemsets
//				cout << "write intToItems" << endl;
//				string file_intToItemset = file + ".intToItemset";
//				FILE * f_intToItemset = fopen(file_intToItemset.c_str(), "wb");
//				if(f_intToItemset == NULL) {
//					cout << "can't write to file: " << file_intToItemset << endl;
//					exit(-1);
//				}
//				else{
//					size_t bufsize = 0;
//					for(auto it : this->intToItemset){
//						bufsize += sizeof(unsigned int) + it->getLength() * sizeof(int);
//					}
//					char* buf = (char*) malloc(bufsize);
//					size_t offset = 0;
//					for(auto it = this->intToItemset.begin(); it != this->intToItemset.end(); ++it){
//						ItemsetGraph* graph = *it;
//						offset = graph->write_to_buf(buf, offset);
//					}
//					fwrite(buf, bufsize, 1, f_intToItemset);
//					free(buf);
//					fclose(f_intToItemset);
//				}
				string file_intToItemset = file + ".intToItemset";
				FILE * f_intToItemset = fopen(file_intToItemset.c_str(), "wb");
				if(f_intToItemset == NULL) {
					cout << "can't write to file: " << file_intToItemset << endl;
					exit(-1);
				}
				else{
//					char * buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
					char* buf = (char*) malloc(IO_SIZE);
					long offset = 0;
					for(auto it = this->intToItemset.begin(); it != this->intToItemset.end(); ++it){
						ItemsetGraph* graph = *it;
						long bufsize = sizeof(unsigned int) + graph->getLength() * sizeof(int);

						if(bufsize >= IO_SIZE){
							graph->write_unreadable(f_intToItemset);
						}
						else{
							if(bufsize > IO_SIZE - offset){
								fwrite(buf, offset, 1, f_intToItemset);
								offset = 0;
							}
							offset = graph->write_to_buf(buf, offset);
						}
					}
					fwrite(buf, offset, 1, f_intToItemset);

					free(buf);
					fclose(f_intToItemset);
				}

    		}
    		else{//default-buffered
				//write intToEdge
				string file_intToEdge = file + ".intToEdge";
				FILE * f_intToEdge = fopen(file_intToEdge.c_str(), "wb");
				if(f_intToEdge == NULL) {
					cout << "can't write to file: " << file_intToEdge << endl;
					exit(-1);
				}
				else{
					for(auto it = this->intToEdge.begin(); it != this->intToEdge.end(); ++it){
						Edge edge = *it;
						edge.write_unreadable(f_intToEdge);
					}
					fclose(f_intToEdge);
				}

				//write edgeToInt
				string file_edgeToInt = file + ".edgeToInt";
				FILE * f_edgeToInt = fopen(file_edgeToInt.c_str(),"wb");
				if(f_edgeToInt == NULL) {
					cout << "can't write to file: " << file_edgeToInt << endl;
					exit(-1);
				}
				else{
					for(auto it = this->edgeToInt.begin(); it != this->edgeToInt.end(); ++it){
						Edge edge = it->first;
						edge.write_unreadable(f_edgeToInt);
						int index = it->second;
						fwrite((const void*)& index, sizeof(int), 1, f_edgeToInt);
					}
					fclose(f_edgeToInt);
				}

				//write graphs
				string file_itemsetgraphs = file + ".itemsetGraphs";
				FILE * f_itemsetGraphs = fopen(file_itemsetgraphs.c_str(),"wb");
				if(f_itemsetGraphs == NULL) {
					cout << "can't write to file: " << file_itemsetgraphs << endl;
					exit(-1);
				}
				else{
					for(auto it = this->graphs.begin(); it != this->graphs.end(); ++it){
						PEGraph_Pointer pointer = it->first;
						fwrite((const void*)& pointer, sizeof(PEGraph_Pointer), 1, f_itemsetGraphs);
						ItemsetGraph* graph = it->second;
						graph->write_unreadable(f_itemsetGraphs);
					}
					fclose(f_itemsetGraphs);
				}

				//write itemsets
				string file_intToItemset = file + ".intToItemset";
				FILE * f_intToItemset = fopen(file_intToItemset.c_str(), "wb");
				if(f_intToItemset == NULL) {
					cout << "can't write to file: " << file_intToItemset << endl;
					exit(-1);
				}
				else{
					for(auto it = this->intToItemset.begin(); it != this->intToItemset.end(); ++it){
						ItemsetGraph* graph = *it;
						graph->write_unreadable(f_intToItemset);
					}
					fclose(f_intToItemset);
				}
    		}

    	}
    }

    void load_in_graphs(const string& file){
		if(file_mode){
		    std::ifstream fin;
		    fin.open(file);
		    if(!fin) {
		        cout << "can't load graphs file: " << file << endl;
		    }
		    else{
				std::string line;
				while (getline(fin, line)) {
					if(line == ""){
						continue;
					}

					std::stringstream stream(line);
					std::string id;
					stream >> id;
					PEGraph_Pointer graph_pointer = atoi(id.c_str());
					PEGraph* pegraph = new PEGraph();
					pegraph->load_readable(stream);
					//since the file is appended, we just use the recent updated pegraph
					ItemsetGraph *graph = convertToSetGraph(pegraph);
					delete pegraph;
					if (graphs.find(graph_pointer) != graphs.end()) {
						delete graphs[graph_pointer];
					}
					graphs[graph_pointer] = graph;
				}
				fin.close();

		    	//delete the old graphstore file
		    	FileUtil::delete_file(file);
		    }
		}
		else{
    		FILE *fp = fopen(file.c_str(),"rb");
    		if(!fp) {
    			cout << "can't load graphs file: " << file << endl;
//    			exit(-1);
    		}
    		else{
    			if(buffered_mode){//pegraph-level
    				size_t freadRes = 0; //clear warnings
    				size_t bufsize;
    				while(fread(&bufsize, sizeof(size_t), 1, fp) != 0) {
    					char *buf = (char*)malloc(bufsize);
    					freadRes = fread(buf, bufsize, 1, fp);
    					PEGraph* pegraph = new PEGraph();
    					PEGraph_Pointer graph_pointer = pegraph->read_from_buf(buf, bufsize);
    					free(buf);
    					ItemsetGraph *graph = convertToSetGraph(pegraph);
    					delete pegraph;
						//since the file is appended, we just use the recent updated pegraph
						if (graphs.find(graph_pointer) != graphs.end()) {
							delete graphs[graph_pointer];
						}
						graphs[graph_pointer] = graph;
    				}
    			}
    			else{
					PEGraph_Pointer graph_pointer;
					while(fread(&graph_pointer, sizeof(PEGraph_Pointer), 1, fp) != 0) {
						PEGraph* pegraph = new PEGraph();
						pegraph->load_unreadable(fp);
						ItemsetGraph *graph = convertToSetGraph(pegraph);
						delete pegraph;
						//since the file is appended, we just use the recent updated pegraph
						if (graphs.find(graph_pointer) != graphs.end()) {
							delete graphs[graph_pointer];
						}
						graphs[graph_pointer] = graph;
					}
    			}

				fclose(fp);

				//delete the old graphstore file
				FileUtil::delete_file(file);
    		}
		}
    }

    /**
     * can only store the mirror_in graphs in PEGraph format since different partition maintains different items
     */
    void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set){
		if (file_mode) {
			ofstream myfile;
			myfile.open(file_graphs_in, std::ofstream::out | std::ofstream::app);
			if (myfile.is_open()) {
				for (auto &n : set) {
					auto pointer = n->getOutPointer();
					PEGraph *graph = retrieve(pointer);
					if (graph) {
						//write a pegraph into file
						myfile << pointer << "\t";
						graph->write_readable(myfile);
						delete graph;
						myfile << "\n";
					}
				}
				myfile.close();
			}
		}
		else {
			FILE *f = fopen(file_graphs_in.c_str(), "ab");
			if (f == NULL) {
				cout << "can't write to file: " << file_graphs_in << endl;
				exit(-1);
			}
			else {
    			if(buffered_mode){//pegraph-level
    				for (auto& n : set) {
						auto graph_pointer = n->getOutPointer();
						PEGraph* pegraph = retrieve(graph_pointer);
						if(pegraph){
							size_t bufsize = sizeof(PEGraph_Pointer) + pegraph->compute_size_bytes();
							fwrite((const void*)& bufsize, sizeof(size_t), 1, f);
							char *buf = (char*)malloc(bufsize);
							pegraph->write_to_buf(buf, graph_pointer);
							delete pegraph;
							fwrite(buf, bufsize, 1, f);
							free(buf);
						}
    				}
    			}
    			else{
					for (auto &n : set) {
						auto graph_pointer = n->getOutPointer();
						PEGraph *graph = retrieve(graph_pointer);
						if (graph) {
							fwrite((const void*) &graph_pointer, sizeof(PEGraph_Pointer), 1, f);
							graph->write_unreadable(f);
							delete graph;
						}
					}
    			}

				fclose(f);
			}
		}
    }


//    void load_onebyone(const string& file){
//    	//for debugging
//    	Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);
//
//	    std::ifstream fin;
//	    fin.open(file);
//	    if(!fin) {
//	        cout << "can't load file_graphs: " << file << endl;
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
//				ItemsetGraph *graph = convertToSetGraph(pegraph);
//				delete pegraph;
//				if (graphs.find(graph_pointer) != graphs.end()) {
//					delete graphs[graph_pointer];
//				}
//				graphs[graph_pointer] = graph;
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

//    ItemsetGraph* convertToSetGraph(PEGraph* pegraph){
//    	set<int> edges_set;
//    	for(auto& it : pegraph->getGraph()){
//    		vertexid_t src = it.first;
//    		int size = it.second.getSize();
//    		vertexid_t* dsts = it.second.getEdges();
//    		label_t* labels = it.second.getLabels();
//    		for(int i = 0; i < size; ++i){
//    			Edge edge(src, dsts[i], labels[i]);
//    			addNewEdgeToEdgeset(edge, edges_set);
//    		}
//		}
//
//    	//TODO: replace edge set with frequent itemset
//    	compressEdges(edges_set);
//    	ItemsetGraph *graph = new ItemsetGraph(edges_set);
//    	return graph;
//    }

    ItemsetGraph* convertToSetGraph(PEGraph* pegraph){
    	int len = pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    ItemsetGraph* convertToSetGraph_hybrid(HybridGraph* hybridgraph){
    	PEGraph* pegraph = hybridgraph->getPeGraph();
    	ItemsetGraph* itemsetgraph = hybridgraph->getItemsetGraph();
    	int len = itemsetgraph->getLength() + pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	//first copy the original itemsetgraph into the array
    	myArray->insert(itemsetgraph->getEdgeIds(), itemsetgraph->getLength());

    	//then add the pegraph into the array
    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    ItemsetGraph* convertToSetGraph_locked(PEGraph* pegraph){
    	int len = pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset_locked(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    ItemsetGraph* convertToSetGraph_hybrid_locked(HybridGraph* hybridgraph){
    	PEGraph* pegraph = hybridgraph->getPeGraph();
    	ItemsetGraph* itemsetgraph = hybridgraph->getItemsetGraph();
    	int len = itemsetgraph->getLength() + pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	//first copy the original itemsetgraph into the array
    	myArray->insert(itemsetgraph->getEdgeIds(), itemsetgraph->getLength());

    	//then add the pegraph into the array
    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset_locked(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    void addNewEdgeToEdgeset_locked(Edge& edge, MyArray* myArray){
    	std::lock_guard<std::mutex> lGuard(mtx);
    	addNewEdgeToEdgeset(edge, myArray);
    }

    void addNewEdgeToEdgeset(Edge& edge, MyArray* myArray){
		if(edgeToInt.find(edge) != edgeToInt.end()){
			myArray->insert(edgeToInt[edge]);
		}
		else{
	    	intToEdge.push_back(edge);
	    	int id = intToEdge.size() - 1;
	    	edgeToInt[edge] = id;
	    	myArray->insert(id);
		}
    }


    HybridGraph* convertToHybridGraph(PEGraph* pegraph){
    	int len = pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);
    	PEGraph* diff_peGraph = new PEGraph();

    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();

    		EdgeArray edgeArray;

    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				myArray->insert(edgeToInt[edge]);
    			}
    			else{
    				edgeArray.addOneEdge(dsts[i], labels[i]);
    			}
    		}

    		if(!edgeArray.isEmpty()){
    			diff_peGraph->setEdgeArray(src, edgeArray);
    		}
		}

    	//sort the array
//    	sort(myArray->getData(), myArray->getData() + myArray->getLength());
    	myalgo::insertSortArray(myArray->getData(), myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	HybridGraph *graph = new HybridGraph(myArray, diff_peGraph);
    	delete myArray;
    	return graph;
    }


    //for debugging
    static void printArray(int* array, unsigned int len){
    	for(unsigned int i = 0; i < len; i++){
    		cout << array[i] << ",";
    	}
    	cout << endl;
    }

    HybridGraph* constructHybridGraph(PEGraph* out, PEGraph* old_out, ItemsetGraph* old_out_itemset){
    	assert(old_out != nullptr && old_out_itemset != nullptr);

    	//get the keys of out
    	unsigned int out_len = out->getNumVertices();
    	vertexid_t* out_keys = new vertexid_t[out_len];
    	unsigned int i = 0;
    	for(auto it = out->getGraph().begin(); it != out->getGraph().end(); ++it){
    		out_keys[i++] = it->first;
    	}
    	sort(out_keys, out_keys + out_len);

//    	//for debugging
//    	cout << "kyes in out: " << endl;
//    	printArray(out_keys, out_len);

    	//get the keys of old_out
    	unsigned int old_out_len = old_out->getNumVertices();
    	vertexid_t* old_out_keys = new vertexid_t[old_out_len];
    	unsigned int j = 0;
    	for(auto it = old_out->getGraph().begin(); it != old_out->getGraph().end(); ++it){
    		old_out_keys[j++] = it->first;
    	}
    	sort(old_out_keys, old_out_keys + old_out_len);

//    	//for debugging
//    	cout << "kyes in old-out: " << endl;
//    	printArray(old_out_keys, old_out_len);

    	//compute the deleted and added keys
    	vector<vertexid_t> added;
    	vector<vertexid_t> both;
    	vector<vertexid_t> deleted;
    	i = 0, j = 0;
    	while(i < out_len && j < old_out_len){
    		if(out_keys[i] < old_out_keys[j]){
    			//push to added
    			added.push_back(out_keys[i]);
    			i++;
    		}
    		else if(out_keys[i] == old_out_keys[j]){
    			//push to both
    			both.push_back(out_keys[i]);
    			j++;
    			i++;

    		}
    		else {//out_keys[i] > old_out_keys[j]
    			//push to deleted
    			deleted.push_back(old_out_keys[j]);
    			j++;
    		}
    	}
    	while(i < out_len){
    		added.push_back(out_keys[i]);
    		i++;
    	}
    	while(j < old_out_len){
			deleted.push_back(old_out_keys[j]);
			j++;
    	}

    	//clean
    	delete[] out_keys;
    	delete[] old_out_keys;

//    	//for debugging
//    	cout << "srcIds-added: " << endl;
//    	printArray(added.data(), added.size());
//    	cout << "srcIds-deleted: " << endl;
//    	printArray(deleted.data(), deleted.size());
//    	cout << "srcIds-both: " << endl;
//    	printArray(both.data(), both.size());


    	vector<int> myArray_added;
    	PEGraph* added_peGraph = new PEGraph();
    	vector<int> myArray_deleted;

    	//get the added edges
    	for(auto it = added.begin(); it != added.end(); ++it){
    		vertexid_t src = *it;
    		int size = out->getNumEdges(src);
    		vertexid_t* dsts = out->getEdges(src);
    		label_t* labels = out->getLabels(src);

    		EdgeArray edgeArray;

    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				myArray_added.push_back(edgeToInt[edge]);
    			}
    			else{
    				edgeArray.addOneEdge(dsts[i], labels[i]);
    			}
    		}

    		if(!edgeArray.isEmpty()){
    			added_peGraph->setEdgeArray(src, edgeArray);
    		}
		}

    	//get the deleted edges
    	for(auto it = deleted.begin(); it != deleted.end(); ++it){
    		vertexid_t src = *it;
    		int size = old_out->getNumEdges(src);
    		vertexid_t* dsts = old_out->getEdges(src);
    		label_t* labels = old_out->getLabels(src);

    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			assert(edgeToInt.find(edge) != edgeToInt.end());
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				myArray_deleted.push_back(edgeToInt[edge]);
    			}
    		}
		}

    	//for both
    	for(auto it = both.begin(); it != both.end(); ++it){
    		EdgeArray edgeArray;

			vertexid_t src = *it;

			unsigned int size = out->getNumEdges(src);
			vertexid_t *dsts = out->getEdges(src);
			label_t *labels = out->getLabels(src);

			unsigned int old_size = old_out->getNumEdges(src);
			vertexid_t *old_dsts = old_out->getEdges(src);
			label_t *old_labels = old_out->getLabels(src);

			unsigned int i = 0; j = 0;
	    	while(i < size && j < old_size){
	    		int value = myalgo::myCompare(dsts[i], labels[i], old_dsts[j], old_labels[j]);
	    		if(value < 0){
	    			//push to added array and added EdgeArray
	    			Edge edge(src, dsts[i], labels[i]);
	    			if(edgeToInt.find(edge) != edgeToInt.end()){
	    				myArray_added.push_back(edgeToInt[edge]);
	    			}
	    			else{
	    				edgeArray.addOneEdge(dsts[i], labels[i]);
	    			}

	    			i++;
	    		}
	    		else if(value == 0){
	    			j++;
	    			i++;
	    		}
	    		else {//value > 0
	    			//push to deleted array
	    			Edge edge(src, old_dsts[j], old_labels[j]);
	    			assert(edgeToInt.find(edge) != edgeToInt.end());
	    			if(edgeToInt.find(edge) != edgeToInt.end()){
	    				myArray_deleted.push_back(edgeToInt[edge]);
	    			}

	    			j++;
	    		}
	    	}
	    	while(i < size){
    			//push to added array and added EdgeArray
    			Edge edge(src, dsts[i], labels[i]);
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				myArray_added.push_back(edgeToInt[edge]);
    			}
    			else{
    				edgeArray.addOneEdge(dsts[i], labels[i]);
    			}

	    		i++;
	    	}
	    	while(j < old_size){
    			//push to deleted array
    			Edge edge(src, old_dsts[j], old_labels[j]);
    			assert(edgeToInt.find(edge) != edgeToInt.end());
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				myArray_deleted.push_back(edgeToInt[edge]);
    			}

				j++;
	    	}

    		if(!edgeArray.isEmpty()){
    			added_peGraph->setEdgeArray(src, edgeArray);
    		}

    	}

    	//sort the array
//    	sort(myArray_added.begin(), myArray_added.end());
    	myalgo::insertSortArray(myArray_added.data(), myArray_added.size());
//    	sort(myArray_deleted.begin(), myArray_deleted.end());
    	myalgo::insertSortArray(myArray_deleted.data(), myArray_deleted.size());

//    	//for debugging
//    	cout << "old-out-itemset: " << endl;
//    	cout << old_out_itemset->toString() << endl;
//    	cout << "edges-added: " << endl;
//    	printArray(myArray_added.data(), myArray_added.size());
//    	cout << "edges-deleted: " << endl;
//    	printArray(myArray_deleted.data(), myArray_deleted.size());

    	MyArray* myArray = new MyArray(old_out_itemset->getLength() + myArray_added.size() - myArray_deleted.size());
    	//construct myArray
    	unsigned int a = 0, o = 0, d = 0;
    	while(a < myArray_added.size() && o < old_out_itemset->getLength()){
    		int diff = myArray_added[a] - old_out_itemset->getEdgeId(o);
    		if(diff < 0){
    			myArray->insert(myArray_added[a]);
    			a++;
    		}
    		else if(diff > 0){
    			if(d >= myArray_deleted.size() || myArray_deleted[d] != old_out_itemset->getEdgeId(o)){
					myArray->insert(old_out_itemset->getEdgeId(o));
    			}
    			else{
    				d++;
    			}
    			o++;
    		}
    		else {//diff == 0
    			cout << "wrong condition!" << endl;
    			exit(EXIT_FAILURE);
    		}
    	}
    	if(a < myArray_added.size()){
    		myArray->insert(myArray_added.data() + a, myArray_added.size() - a);
    	}
    	while(o < old_out_itemset->getLength()){
			if(d >= myArray_deleted.size()){
				myArray->insert(old_out_itemset->getEdgeIds() + o, old_out_itemset->getLength() - o);
				break;
			}

			if(myArray_deleted[d] != old_out_itemset->getEdgeId(o)){
				myArray->insert(old_out_itemset->getEdgeId(o));
			}
			else{
				d++;
			}
			o++;
    	}

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	HybridGraph *graph = new HybridGraph(myArray, added_peGraph);
    	delete myArray;
    	return graph;

    }

    /*
     *encode or compress the graph representation by replacing certain frequent subsets of edge ids with the unique itemset ids
     */
    void compressEdges(MyArray* myArray){
    	for(unsigned i = 0; i < intToItemset.size(); i++){
    		if(myArray->getLength() < intToItemset.at(intToItemset.size() - 1)->getLength()){
    			break;
    		}
    		subsume(myArray, intToItemset[i], getItemsetId(i));
    	}
    }

    static int getItemsetId(unsigned index){
    	return 0 - (index + 1);
    }

    static unsigned getItemsetIndex(int id_itemset){
    	return 0 - (id_itemset + 1);
    }

    bool subsume(MyArray* myArray, ItemsetGraph* itemset, int id_itemset){
    	//first, determine if the itemset is a subset of edges_set; note that both are sorted
    	unsigned int i = 0, j = 0;
    	while(j < myArray->getLength() && i < itemset->getLength()){
    		if(myArray->getData(j) < itemset->getEdgeId(i)){
    			j++;
    		}
    		else if(myArray->getData(j) == itemset->getEdgeId(i)){
    			j++;
    			i++;
    		}
    		else {//if(myArray->getData(j) > itemset->getEdgeId(i))
    			return false;
    		}
    	}
    	if(i < itemset->getLength()){
    		return false;
    	}

    	//second, if so, replace the itemset in edges_set with the itemset id
    	unsigned new_len = myArray->getLength() - itemset->getLength() + 1;
    	MyArray* new_array = new MyArray(new_len);

    	i = 0, j = 0;
    	bool flag = true;
    	while(j < myArray->getLength() && i < itemset->getLength()){
    		if(myArray->getData(j) < itemset->getEdgeId(i)){
    			if(flag){
					if(myArray->getData(j) < id_itemset){
						new_array->insert(myArray->getData(j));
					}
					else{
						new_array->insert(id_itemset);
						new_array->insert(myArray->getData(j));
						flag = false;
					}
    			}
    			else{
    				new_array->insert(myArray->getData(j));
    			}
    			j++;
    		}
    		else if(myArray->getData(j) == itemset->getEdgeId(i)){
    			j++;
    			i++;
    		}
    		else {//if(myArray->getData(j) > itemset->getEdgeId(i))
                cout << "wrong condition!" << outFile << endl;
                exit(EXIT_FAILURE);
    		}
    	}
    	assert(i == itemset->getLength());

    	//copy all the remaining data in myArray to new_array
    	if(flag){
			while(j < myArray->getLength()){
				if(flag){
					if(myArray->getData(j) < id_itemset) {
						new_array->insert(myArray->getData(j));
					}
					else{
						new_array->insert(id_itemset);
						new_array->insert(myArray->getData(j));
						flag = false;
					}
				}
				else{
//					copy(myArray->getData() + j, myArray->getData() + myArray->getLength(), new_array->getData() + new_array->getIndex());
					new_array->insert(myArray->getData() + j, myArray->getLength() - j);
					break;
				}
				j++;
			}

			if(flag){
				new_array->insert(id_itemset);
			}
    	}
    	else{
//    		copy(myArray->getData() + j, myArray->getData() + myArray->getLength(), new_array->getData() + new_array->getIndex());
    		new_array->insert(myArray->getData() + j, myArray->getLength() - j);
    	}

    	//update myArray
    	myArray->shallowCopy(new_array);
    	delete new_array;
    	return true;
    }

//    //shallow copy
//    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph* graph) {
//    	cout << "shouldn't call this function!" << endl;
//    	exit (EXIT_FAILURE);
//    }

    static void update_parallel(ItemsetGraphStore* current, NaiveGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
    	//for debugging
    	Logger::print_thread_info_locked("update-parallel starting...\n", LEVEL_LOG_FUNCTION);

    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
//    		//for debugging
//    		Logger::print_thread_info_locked("graph id = " + to_string(id) + "\n", LEVEL_LOG_FUNCTION);

    		assert(current->graphs.find(id) != current->graphs.end());
    		current->update_convert_locked(id, another->getMap().at(id));
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-parallel finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_graphs_parallel(GraphStore* another){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel starting...\n", LEVEL_LOG_FUNCTION);

	    //initiate concurrent worklist
	    Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	    NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
	    for(auto& it: another_graphstore->getMap()){
	        worklist->push_atomic(it.first);

			//initialize the graphstore
	        if(graphs.find(it.first) == graphs.end()){
	        	graphs[it.first] = new ItemsetGraph();
	        }
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {update_parallel(this, another_graphstore, worklist);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel finished.\n", LEVEL_LOG_FUNCTION);
    }

    static void update_hybrid_parallel(ItemsetGraphStore* current, HybridGraphStore* tmp_graphs, Concurrent_Worklist<vertexid_t>* worklist){
    	//for debugging
    	Logger::print_thread_info_locked("update-parallel starting...\n", LEVEL_LOG_FUNCTION);

    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
//    		//for debugging
//    		Logger::print_thread_info_locked("graph id = " + to_string(id) + "\n", LEVEL_LOG_FUNCTION);

    		assert(current->graphs.find(id) != current->graphs.end());
    		current->update_convert_hybrid_locked(id, tmp_graphs->getGraphs().at(id));
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-parallel finished.\n", LEVEL_LOG_FUNCTION);
    }


    void update_graphs_sequential(GraphStore* another){
    	NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
    	for(auto& it: another_graphstore->getMap()){
    		update(it.first, it.second);
    	}
    }

    void update_graphs_hybrid(HybridGraphStore* tmp_graphs, bool update_mode){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs starting...\n", LEVEL_LOG_FUNCTION);

    	if(update_mode){
	    	update_graphs_hybrid_parallel(tmp_graphs); // in parallel
    	}
    	else{
			update_graphs_hybrid_sequential(tmp_graphs); // sequential
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_graphs_hybrid_sequential(HybridGraphStore* tmp_graphs){
    	for(auto& it: tmp_graphs->getGraphs()){
    		update_hybrid(it.first, it.second);
    	}
    }

    void update_graphs_hybrid_parallel(HybridGraphStore* tmp_graphs){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel starting...\n", LEVEL_LOG_FUNCTION);

	    //initiate concurrent worklist
	    Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	    for(auto& it: tmp_graphs->getGraphs()){
	        worklist->push_atomic(it.first);

			//initialize the graphstore
	        if(graphs.find(it.first) == graphs.end()){
	        	graphs[it.first] = new ItemsetGraph();
	        }
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {update_hybrid_parallel(this, tmp_graphs, worklist);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel finished.\n", LEVEL_LOG_FUNCTION);
    }

//    void clearEntryOnly() {
//
//    }

//    void clear() {
//    	for(auto it = graphs.begin(); it != graphs.end(); ){
//    		delete it->second;
//    		it = graphs.erase(it);
//    	}
//    }

    void printOutInfo(){
    	int size_graphs = graphs.size();
    	long size_edges = 0;

    	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
//    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
    		size_edges += it->second->getNumEdges(this->intToItemset);
    	}

    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    }


    void getStatistics(int& size_graphs, long& size_edges, long& size_items, const std::unordered_set<PEGraph_Pointer>& mirrors){
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		if(mirrors.find(it->first) == mirrors.end()){
    			size_items += it->second->getLength();
				size_edges += it->second->getNumEdges(this->intToItemset);
				size_graphs++;
    		}
    	}
    }


    /*
     * write graphs into a file for itemset mining later where
     * each graph as a line separated by " "
     */
    void writeToFile(const string& input_file) {
		ofstream myfile;
		myfile.open(input_file, std::ofstream::out);
//		//for debugging
//		cout << inputFile << endl;
//		cout << myfile.is_open() << endl;

		if (myfile.is_open()) {
			for (auto &n : graphs) {
				if(!n.second->isEmpty()){
//					for(unsigned int i = 0; i < n.second->getLength(); i++){
//						myfile << n.second->getEdgeId(i) << " ";
//					}
//					myfile << "\n";


			        std::stringstream buffer;
			        for(unsigned int i = 0; i < n.second->getLength(); i++){
			        	char aLine[32];
			        	sprintf(aLine, "%d ", n.second->getEdgeId(i));
			        	buffer << aLine;
			        }
			        buffer << "\n";
			        myfile << buffer.str();
				}
			}

			myfile.close();
		}
    }

    //
    void readFromFile(const string& output_file) {
        ifstream myfile;
        myfile.open(output_file);
        if (!myfile) {
            cout << "can't load file: " << output_file << endl;
//            exit(EXIT_FAILURE);
        }
        else{
			//get all the frequent itemset and the corresponding frequency info
			multimap<int, ItemsetGraph*> frequency_graph_map;
			string line;
			while (getline(myfile, line)) {
				if(line == ""){
					continue;
				}

				set<int> itemset;
				int frequency;
				std::stringstream stream(line);
				do{
					std::string id = "";
					stream >> id;

					if(id == ""){
						continue;
					}

					if(id[0] != '('){
						itemset.insert(atoi(id.c_str()));
					}
					else {
						string word = "";
						for(auto c: id){
							if(c != '(' && c != ')'){
								word = word + c;
							}
						}
						frequency = atoi(word.c_str());
					}
				}
				while(stream);

				//transfer itemset into array
				ItemsetGraph* g = new ItemsetGraph(itemset);
				//sort itemsets in the ascending order of frequency
				frequency_graph_map.insert(std::pair<int, ItemsetGraph*>(frequency, g));
			}
			myfile.close();


	//		//filter to obtain top-k disjoint frequent itemsets
	//		int k = 5;
	//		get_disjoint_itemset(frequency_graph_map, k);

			for(auto it = frequency_graph_map.cbegin(); it != frequency_graph_map.cend(); ++ it){
				intToItemset.push_back((*it).second);

//				//for debugging
//				cout << *((*it).second) << endl;
			}
        }
    }


    /*
     *mine a set of frequent itemsets to construct the itemset base for encoding
     *precondition: the current intToItemset is empty
     *              or would like to reconstruct the base from scratch
     */
    void constructItemsetBase_eclat(Partition part, int support, int length){
    	string input_file = inputFile + to_string(part);
    	string output_file = outFile + to_string(part);

//    	//if input_file exists, meaning that the itemset mining for this partition has been done before, then return directly
//    	if(FileUtil::file_exists(input_file)){
//    		return;
//    	}

    	if(!graphs.empty()){
			writeToFile(input_file);

			std::string option = "-tc -s" + to_string(support) + " -m" + to_string(length);
			std::string command = "../lib/eclat " + option + " " + input_file + " " + output_file;
			system(command.c_str());

			//put the resulting frequent itemsets into intToItemset while by the ascending order of frequency
			readFromFile(output_file);
    	}
    }


    void constructItemsetBase_apriori(Partition part, int support, int length){

    }


//    void constructItemsetBase_fpgrowth(Partition part, int support, int length){
//
//    }


    /*
     *update the existing itemset base by adding extra frequent itemsets
     */
    void updateItemsetBase(){
    	//TODO

    }

    /*
     * reconstruct the entire itemset base
     */
    void reconstructItemsetBase(){

    }

	unordered_map<PEGraph_Pointer, ItemsetGraph*>& getGraphs() {
		return graphs;
	}


	bool isSubset(const std::set<Item> &s1, const std::set<Item> &s2)
	{
    	for (auto iter=s1.begin();iter!=s1.end();iter++) {
        	if (s2.find(*iter)==s2.end())
            	return false;
    	}
    	return true;
	}

	static bool comparePattern(const Pattern &p1, const Pattern &p2)
	{
		if (p1.second==p2.second)
        	return (p1.first).size()<(p2.first).size();
    	return p1.second>p2.second;
	}

    void constructItemsetBase_fpgrowth(Partition part, int support, int length){
		if (!graphs.empty()) {
			std::vector<Transaction> transactions;
			for (auto &n : graphs) {
				if (!n.second->isEmpty()) {
					Transaction tmp;
					for (unsigned int i = 0; i < n.second->getLength(); i++) {
						tmp.push_back(n.second->getEdgeId(i));
					}
					transactions.push_back(tmp);
				}
			}
			if (transactions.size()>0) {
				uint64_t minimum_support_threshold = uint64_t(std::floor(transactions.size()*(double(support)/100)));
				const FPTree fptree{ transactions, minimum_support_threshold };
				std::set<Pattern> patterns = fptree_growth( fptree );

				for (auto iter=patterns.begin();iter!=patterns.end();) {
					if (((*iter).first).size()<length) {
						iter=patterns.erase(iter);
					} else {
						iter++;
					}
				}
				std::vector<Pattern> patterns_v(patterns.begin(), patterns.end());
				sort(patterns_v.begin(), patterns_v.end(), comparePattern);
				for (int i=0;i<patterns_v.size();i++) {
        			for (int j=i+1;j<patterns_v.size();j++) {
            			if (patterns_v[j].second!=patterns_v[i].second)
                			break;
            			if ((patterns_v[j].first).size()>(patterns_v[i].first).size() && isSubset(patterns_v[i].first,patterns_v[j].first)) {
                			patterns_v.erase(patterns_v.begin()+i);
                			i--;
                			break;
            			}
        			}
    			}

				for (int i=0;i<patterns_v.size();i++) {
					ItemsetGraph* g = new ItemsetGraph(patterns_v[i].first);
					intToItemset.push_back(g);
				}
			}
		}
    }


protected:
    void print(std::ostream& str) {
    	std::lock_guard<std::mutex> lGuard (mtx);
    	str << "The number of graphs is: " << graphs.size() << "\n";
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

    void toString_sub(std::ostringstream& str) {
    	std::lock_guard<std::mutex> lGuard (mtx);
    	str << "The number of graphs is: " << graphs.size() << "\n";
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

private:
	//private members
	vector<Edge> intToEdge;
	unordered_map<Edge, int> edgeToInt;

	unordered_map<PEGraph_Pointer, ItemsetGraph*> graphs;

	//a vector storing all the ItemsetGraph
	vector<ItemsetGraph*> intToItemset;


};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_ */
