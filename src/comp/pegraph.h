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

class PEGraph {





public:
    PEGraph();

    ~PEGraph() {}

    PEGraph(PEGraph *p);

	static void print_graph_map(std::ostream & strm, const std::unordered_map<vertexid_t, EdgeArray> graph) {
		for(auto& it : graph){
			strm << it.first << " -> " << it.second << endl;
		}
	}

	bool equals(PEGraph *another);

    std::unordered_map<vertexid_t, EdgeArray> &getGraph();

    inline vertexid_t getNumVertices() { return graph.size(); }

    vertexid_t *getEdges(vertexid_t index) { return graph[index].getEdges(); }

    label_t *getLabels(vertexid_t index) { return graph[index].getLabels(); }

    int getNumEdges(vertexid_t index) { return graph[index].getSize(); }

    void setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels);

    void clearEdgeArray(vertexid_t index);

    void merge(PEGraph *graph_toMerge);


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

};


#endif /* COMP_PEGRAPH_H_ */
