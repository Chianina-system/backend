/*
 * PEGraph.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_PEGRAPH_H_
#define COMP_PEGRAPH_H_

#include "../common/CommonLibs.hpp"
#include "edgearray.h"
#include "myalgorithm.h"
#include "../utility/Logger.hpp"
//#include "cfg_compute.h"
#include "graphstore/itemset_graph.h"
//#include "graphstore/delta_graph.h"

class PEGraph {


public:
    PEGraph();

    ~PEGraph() {
//    	cout << "deleting pegraph..." << endl;
    }

    PEGraph(PEGraph *p);

//    PEGraph(PEGraph *base, PEGraph *added, PEGraph *deleted);


	static void print_graph_map(std::ostream & strm, const std::unordered_map<vertexid_t, EdgeArray> graph) {
		int size = 0;
		for(auto& it : graph){
			strm << it.first << " -> " << it.second << endl;
			size += it.second.getSize();
		}
		strm << "------------------" << endl;
		strm << "size=" << size << endl;
	}

	int getNumEdges(){
		int size = 0;
		for(auto& it : graph){
			size += it.second.getSize();
		}
		return size;
	}

	bool equals(PEGraph *another);

//	bool equals(DeltaGraph *another);

    std::unordered_map<vertexid_t, EdgeArray> &getGraph();

    inline vertexid_t getNumVertices() { return graph.size(); }

    vertexid_t *getEdges(vertexid_t index) { return graph[index].getEdges(); }

    label_t *getLabels(vertexid_t index) { return graph[index].getLabels(); }

    int getNumEdges(vertexid_t index) { return graph[index].getSize(); }

    void setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels);

    void setEdgeArray(vertexid_t index, EdgeArray& array){
//    	assert(this->graph.find(index)==this->graph.end());
        this->graph[index] = array;
    }

    void setGraph(const std::unordered_map<vertexid_t, EdgeArray> &graph) {
        this->graph = graph;
    }

    void clearEdgeArray(vertexid_t index);

    void merge(PEGraph *graph_toMerge);

    void subtract(PEGraph * graph_toSubtract);

//    static PEGraph* merge(PEGraph* graph1, PEGraph* graph2){
//
//    }


//    void setGraph(std::unordered_map<vertexid_t, EdgeArray> &_graph);

//    static PEGraph * merge(PEGraph *graph_1, PEGraph *graph_2) {
//        //merge graph_1 and graph_2 together to generate a new graph
//        PEGraph *peGraph = new PEGraph();
//        std::unordered_map<vertexid_t, EdgeArray> mergedGraph = peGraph->getGraph();
//
//        for (auto it = graph_1->getGraph().begin(); it != graph_1->getGraph().end(); it++) {
//            if (graph_2->getGraph().find(it->first) != graph_2->getGraph().end()) {
//                // merge the edgearray with the same src in graph_1 and graph_2
//                int n1 = it->second.getSize();
//                int n2 = graph_2->getNumEdges(it->first);
//
//                vertexid_t *edges = new vertexid_t[n1 + n2];
//                char *labels = new char[n1 + n2];
//                int len = myalgo::unionTwoArray(edges, labels, n1, it->second.getEdges(), it->second.getLabels(), n2,
//                                      graph_2->getEdges(it->first), graph_2->getLabels(it->first));
//
//                mergedGraph[it->first] = EdgeArray();
//                mergedGraph[it->first].set(len, edges, labels);
//
//                delete[] edges;
//                delete[] labels;
//
//                graph_2->getGraph().erase(it->first);
//            }
//            else {
//                mergedGraph[it->first] = it->second;
//            }
//        }
//
//        for (auto it = graph_2->getGraph().begin(); it != graph_2->getGraph().end(); it++) {
//            mergedGraph[it->first] = it->second;
//        }
//
////        delete graph_1;
////        delete graph_2;
//
//        return peGraph;
//    }



    std::string toString(Grammar* grammar){
    	std::ostringstream strm;
    	strm << "PEGraph<<<<\n---------------------" << endl;
//    	toString_sub(strm);
    	int size = 0;
		for(auto& it : graph){
			strm << it.first << " -> " << it.second.toString(grammar) << endl;
			size += it.second.getSize();
		}
    	strm << "------------------" << endl;
    	strm << "size=" << size << endl;
    	strm << "---------------------" << endl;
    	return strm.str();
    }

    void write_readable(std::ofstream& os){
    	os << graph.size() << "\t";
    	for(auto& it: graph){
    		os << it.first << "\t";
    		it.second.write_readable(os);
    	}
    }

    void load_readable(std::stringstream& stream){
        std::string id;
		stream >> id;
		int size = atoi(id.c_str());
		for (int i = 0; i < size; ++i) {
			stream >> id;
			vertexid_t src = atoi(id.c_str());
			//load an edgearray
			this->graph[src] = EdgeArray();
			this->graph[src].load_readable(stream);
		}
    }

    bool isEmpty(){
    	return this->getNumEdges() == 0;
    }

    void write_unreadable(FILE* f){
    	unsigned int size = graph.size();
    	fwrite((const void*)& size, sizeof(unsigned int), 1, f);
		for (auto &it : graph) {
			fwrite((const void*)& it.first, sizeof(vertexid_t), 1, f);
			it.second.write_unreadable(f);
		}
    }

    void write_unreadable_pure(FILE* f){
		for (auto &it : graph) {
			fwrite((const void*)& it.first, sizeof(vertexid_t), 1, f);
			it.second.write_unreadable(f);
		}
    }

    void load_unreadable(FILE* fp){
    	unsigned int size;
    	size_t freadRes = 0; // clear warnings
    	freadRes = fread(&size, sizeof(unsigned int), 1, fp);
		for (unsigned i = 0; i < size; ++i) {
			vertexid_t src;
			freadRes = fread(&src, sizeof(vertexid_t), 1, fp);
			//load an edgearray
			this->graph[src] = EdgeArray();
			this->graph[src].load_unreadable(fp);
		}
    }

//    void load_unreadable_pure(FILE* fp){
//    	unsigned int size;
//    	size_t freadRes = 0; // clear warnings
//    	freadRes = fread(&size, sizeof(unsigned int), 1, fp);
//		for (unsigned i = 0; i < size; ++i) {
//			vertexid_t src;
//			freadRes = fread(&src, sizeof(vertexid_t), 1, fp);
//			//load an edgearray
//			this->graph[src] = EdgeArray();
//			this->graph[src].load_unreadable(fp);
//		}
//    }

    size_t compute_size_bytes(){
    	size_t size = 0;
    	for(auto& it: graph){
    		size += sizeof(vertexid_t);
    		size += it.second.get_size_bytes();
    	}
    	return size;
    }

    void write_to_buf(char* buf, PEGraph_Pointer pointer){
    	size_t offset = 0;
    	memcpy(buf, (char*)&pointer, sizeof(PEGraph_Pointer));
//    	*buf = pointer;
    	offset += sizeof(PEGraph_Pointer);
    	for(auto& it: graph){
    		//srcId
    		memcpy(buf + offset, (char*)&(it.first), sizeof(vertexid_t));
//    		*(buf + offset) = it.first;
    		offset += sizeof(vertexid_t);
    		//edgeArray
    		offset = it.second.write_to_buf(buf, offset);
    	}
    }

    size_t write_to_buf_pure(char* buf, size_t offset){
    	for(auto& it: graph){
    		//srcId
    		memcpy(buf + offset, (char*)&(it.first), sizeof(vertexid_t));
//    		*(buf + offset) = it.first;
    		offset += sizeof(vertexid_t);
    		//edgeArray
    		offset = it.second.write_to_buf(buf, offset);
    	}
    	return offset;
    }

    PEGraph_Pointer read_from_buf(char* buf, const size_t bufsize){
    	size_t offset = 0;
    	PEGraph_Pointer pointer = *((PEGraph_Pointer*)(buf));
    	offset += sizeof(PEGraph_Pointer);
    	while(offset < bufsize){
    		vertexid_t src = *((vertexid_t*)(buf + offset));
    		offset += sizeof(vertexid_t);
    		this->graph[src] = EdgeArray();
    		offset = this->graph[src].read_from_buf(buf, offset);
    	}
    	return pointer;
    }

//    PEGraph* read_from_buf_(char* buf, const size_t bufsize){
//    	size_t offset = 0;
//    	PEGraph* pointer = *((PEGraph*)(buf));
//    	offset += sizeof(PEGraph*);
//    	while(offset < bufsize){
//    		vertexid_t src = *((vertexid_t*)(buf + offset));
//    		offset += sizeof(vertexid_t);
//    		this->graph[src] = EdgeArray();
//    		offset = this->graph[src].read_from_buf(buf, offset);
//    	}
//    	return pointer;
//    }

    void read_from_buf_pure(char* buf, size_t bufsize){
    	size_t offset = 0;
    	while(offset < bufsize){
    		vertexid_t src = *((vertexid_t*)(buf + offset));
    		offset += sizeof(vertexid_t);
    		this->graph[src] = EdgeArray();
    		offset = this->graph[src].read_from_buf(buf, offset);
    	}
    }




private:
    std::unordered_map<vertexid_t, EdgeArray> graph;

	friend std::ostream & operator<<(std::ostream & strm, const PEGraph & peGraph) {
		strm << "PEGraph<<<<\n---------------------" << endl;
//		for(auto& it : peGraph.graph){
//			strm << it.first << " -> " << it.second << endl;
//		}
//		strm << "---------------------" << endl;
		print_graph_map(strm, peGraph.graph);
		strm << "---------------------" << endl;
		return strm;
	}

    void toString_sub(std::ostringstream& strm){
		for(auto& it : graph){
			strm << it.first << " -> " << it.second << endl;
		}
    }

};


#endif /* COMP_PEGRAPH_H_ */
