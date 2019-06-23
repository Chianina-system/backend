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

    const std::unordered_map<vertexid_t, bool> &getIsSingletonArray() const;

    bool equals(PEGraph *another);

    const std::unordered_map<vertexid_t, EdgeArray> &getGraph() const;

    inline vertexid_t getNumVertices() { return graph.size(); }

    inline bool isSingleton(vertexid_t vid) { return isSingletonArray[vid]; }


    vertexid_t *getEdges(vertexid_t index) { return graph[index].getEdges(); }

    label_t *getLabels(vertexid_t index) { return graph[index].getLabels(); }

    int getNumEdges(vertexid_t index) { return graph[index].getSize(); }

    void setEdgeArray(vertexid_t index, int numEdges, vertexid_t *edges, label_t *labels);

    void clearEdgeArray(vertexid_t index);

    static PEGraph *merge(PEGraph *graph_1, PEGraph *graph_2) {
        //merge graph_1 and graph_2 together to generate a new graph
        PEGraph *peGraph = new PEGraph;
        std::unordered_map<vertexid_t, EdgeArray> mergeGraph;

        for (auto it = graph_1->getGraph().begin(); it != graph_1->getGraph().end(); it++) {
            if (graph_2->getGraph().find(it->first) != graph_2->getGraph().end()) {
                // merge the edgearray with the same src in graph_1 and graph_2
                int len = 0;
                int n1 = it->second.getSize();
                int n2 = graph_2->getNumEdges(it->first);

                vertexid_t *edges = new vertexid_t[n1 + n2];
                char *labels = new char[n1 + n2];
                myalgo::unionTwoArray(len, edges, labels, n1, it->second.getEdges(), it->second.getLabels(), n2,
                                      graph_2->getEdges(it->first), graph_2->getLabels(it->first));
                EdgeArray edgeArray;
                edgeArray.set(len, edges, labels);
                mergeGraph[it->first] = edgeArray;
                delete[] edges;
                delete[] labels;
            } else {
                mergeGraph[it->first] = it->second;
            }
        }

        for (auto it = graph_2->getGraph().begin(); it != graph_2->getGraph().end(); it++) {
            if (graph_1->getGraph().find(it->first) == graph_1->getGraph().end()) {
                mergeGraph[it->first] = it->second;
            }
        }
        delete graph_1;
        delete graph_2;
        peGraph->setGraph(mergeGraph);
        return peGraph;
    }

    void setGraph(const std::unordered_map<vertexid_t, EdgeArray> &_graph);

private:
    std::unordered_map<vertexid_t, EdgeArray> graph;
    std::unordered_map<vertexid_t, bool> isSingletonArray;
};


#endif /* COMP_PEGRAPH_H_ */
