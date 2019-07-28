#ifndef COMP_COMPUTATIONSET_H
#define COMP_COMPUTATIONSET_H
#include "edgearray.h"
#include "../common/CommonLibs.hpp"
#include "pegraph.h"
#include "myalgorithm.h"
#include "edgesToDelete.h"
#include "../utility/Logger.hpp"
#include "grammar.h"
#include "stmt/stmt.h"

class ComputationSet {

	friend std::ostream & operator<<(std::ostream & strm, const ComputationSet & compset) {
		strm << "\nComputationSet<<<<\n---------------------" << endl;
		strm << "Olds:\n";
		PEGraph::print_graph_map(strm, compset.Olds);
		strm << "Deltas:\n";
		PEGraph::print_graph_map(strm, compset.Deltas);
		strm << "News:\n";
		PEGraph::print_graph_map(strm, compset.News);
		strm << "---------------------" << endl;
		return strm;
	}

private:
    std::unordered_map<vertexid_t, EdgeArray> Olds;
    std::unordered_map<vertexid_t, EdgeArray> Deltas;
    std::unordered_map<vertexid_t, EdgeArray> News;


public:
    ComputationSet(){}
    ~ComputationSet(){};
//    void clear();

    void init_add(PEGraph *out, std::unordered_map<vertexid_t, EdgeArray>* m, const bool isConservative);

    void init_delete(PEGraph *out, std::unordered_map<vertexid_t, EdgeArray>* m);

    // getters and setters
    inline bool oldEmpty(vertexid_t index) {return Olds.find(index)==Olds.end();}
    inline bool deltaEmpty(vertexid_t index) {return Deltas.find(index)==Deltas.end();}
    inline bool newEmpty(vertexid_t index) {return News.find(index)==News.end();}

    inline vertexid_t getOldsNumEdges(vertexid_t index) {return Olds[index].getSize();}
    inline vertexid_t getDeltasNumEdges(vertexid_t index) {return Deltas[index].getSize();}
    inline vertexid_t getNewsNumEdges(vertexid_t index) {return News[index].getSize();}
    long getDeltasTotalNumEdges();

    inline char* getOldsLabels(vertexid_t index) {return Olds[index].getLabels();}
    inline char* getDeltasLabels(vertexid_t index) {return Deltas[index].getLabels();}
    inline char* getNewsLabels(vertexid_t index) {return News[index].getLabels();}

    inline vertexid_t* getOldsEdges(vertexid_t index) {return Olds[index].getEdges();}
    inline vertexid_t* getDeltasEdges(vertexid_t index) {return Deltas[index].getEdges();}
    inline vertexid_t* getNewsEdges(vertexid_t index) {return News[index].getEdges();}


    void setOlds(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);
    void setDeltas(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);
    void setNews(vertexid_t index,int numEdges,vertexid_t *edges,char *labels);


    std::unordered_map<vertexid_t, EdgeArray> &getOlds() ;
    std::unordered_map<vertexid_t, EdgeArray> &getDeltas() ;
    std::unordered_map<vertexid_t, EdgeArray> &getNews() ;

//    void clearOlds(vertexid_t index);
//    void clearDeltas(vertexid_t index);
//    void clearNews(vertexid_t index);

    std::unordered_set<vertexid_t > getVertices();

};
#endif
