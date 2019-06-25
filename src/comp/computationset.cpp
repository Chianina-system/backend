#include "computationset.h"
#include <unistd.h>
#include <cstdlib>

ComputationSet::ComputationSet() {
//    firstVid = numVertices = 0;
}

//void ComputationSet::init_add(PEGraph *out, Stmt *stmt) {
//    // TODO: after adding assign edge based on stmt,numVertices may increase.
//
//    // OldsV <- (),NewsV <- (),DeltasV <- (out,stmt)
//    int i = stmt->getSrc();
//    int len = 0;
//    int n1 = out->getNumEdges(i);
//    int n2 = 1;
//    vertexid_t *stmt_edge = new vertexid_t[n2];
//    label_t *stmt_label = new label_t[n2];
//    *stmt_edge = stmt->getDst();
//    *stmt_label = 'A'; //TODO: this label means "ASSIGN"
//
//    vertexid_t *edges = new vertexid_t[n1 + n2];
//    label_t *labels = new label_t[n1 + n2];
//    myalgo::unionTwoArray(len, edges, labels, n1, out->getEdges(i), out->getLabels(i), n2, stmt_edge,
//                          stmt_label);
//    Deltas[i] = EdgeArray();
//    Deltas[i].set(len, edges, labels);
//    delete[] edges;
//    delete[] labels;
//    delete[] stmt_edge;
//    delete[] stmt_label;
//}

void ComputationSet::init_add(PEGraph *out, Stmt *stmt) {
    // Deltas <- stmt
    vertexid_t *stmt_edge = new vertexid_t[1];
    label_t *stmt_label = new label_t[1];
    *stmt_edge = stmt->getDst();
    *stmt_label = 'A'; //TODO: this label means "ASSIGN"
    Deltas[stmt->getSrc()] = EdgeArray();
    Deltas[stmt->getSrc()].set(1, stmt_edge, stmt_label);
    delete[] stmt_edge;
    delete[] stmt_label;

    //OldsV <- out
    for(auto it = out->getGraph().begin(); it != out->getGraph().end(); it++){
        Olds[it->first] = EdgeArray();
        Olds[it->first].set(it->second.getSize(), it->second.getEdges(), it->second.getLabels());
    }
}

    // Olds <- out - m, Deltas <- m, News <- empty
void ComputationSet::init_delete(PEGraph *out, std::unordered_map<int, EdgesToDelete *> &m) {
	// Deltas <- m
    for(auto & it : m){
    	Deltas[it.first] = EdgeArray();
        Deltas[it.first].set(it.second->getSize(), it.second->getEdges(), it.second->getLabels());
    }

    //Olds <- out - m
    for(auto it = out->getGraph().begin(); it != out->getGraph().end(); it++){
    	if(m.find(it->first) != m.end()){
            int n1 = it->second.getSize();
            int n2 = Deltas[it->first].getSize();
            auto *edges = new vertexid_t[n1];
            auto *labels = new label_t[n1];
            int len = myalgo::minusTwoArray(edges, labels, n1, it->second.getEdges(), it->second.getLabels(), n2, Deltas[it->first].getEdges(), Deltas[it->first].getLabels());
            if (len){
            	Olds[it->first] = EdgeArray();
                Olds[it->first].set(len, edges, labels);
            }

            delete[] edges;
            delete[] labels;
    	}
    	else{
			Olds[it->first] = EdgeArray();
			Olds[it->first].set(it->second.getSize(), it->second.getEdges(), it->second.getLabels());
    	}
    }

}

//void ComputationSet::clear() {
//    if (numVertices) {
//        if (Olds) {
//            for (int i = 0; i < numVertices; ++i)
//                Olds->getEdgeArray(i)->clear();
//            delete[] Olds;
//            Olds = nullptr;
//        }
//        if (Deltas) {
//            for (int i = 0; i < numVertices; ++i)
//                Deltas->getEdgeArray(i)->clear();
//            delete[] Deltas;
//            Deltas = nullptr;
//        }
//        if (News) {
//            for (int i = 0; i < numVertices; ++i)
//                News->getEdgeArray(i)->clear();
//            delete[] News;
//            News = nullptr;
//        }
//        numVertices = 0;
//    }
//}

long ComputationSet::getDeltasTotalNumEdges() {
    long num = 0;
    for (auto it = Deltas.begin(); it != Deltas.end();it++){
        num += getDeltasNumEdges((*it).first);
    }
    return num;
}

void ComputationSet::setOlds(vertexid_t index, int numEdges, vertexid_t *edges, char *labels) {
	if(Olds.find(index) == Olds.end())
        Olds[index] = EdgeArray();
    Olds[index].set(numEdges,edges,labels);
}

void ComputationSet::setDeltas(vertexid_t index, int numEdges, vertexid_t *edges, char *labels) {
    if(Deltas.find(index) == Deltas.end())
        Deltas[index] = EdgeArray();
    Deltas[index].set(numEdges,edges,labels);
}

void ComputationSet::setNews(vertexid_t index, int numEdges, vertexid_t *edges, char *labels) {
    if(News.find(index) == News.end())
        News[index] = EdgeArray();
    News[index].set(numEdges,edges,labels);
}

void ComputationSet::clearOlds(vertexid_t index) {
	Olds.erase(index);
}

void ComputationSet::clearDeltas(vertexid_t index) {
    Deltas.erase(index);
}

void ComputationSet::clearNews(vertexid_t index) {
	if(News.find(index) != News.end())
		News.erase(index);
}

std::set<vertexid_t> ComputationSet::getVertices() {
    std::set<vertexid_t> vertexSet;
    for(auto it = this->getOlds().begin(); it!= this->getOlds().end(); it++){
        vertexSet.insert(it->first);
    }
    for(auto it = this->getDeltas().begin(); it!= this->getDeltas().end(); it++){
        vertexSet.insert(it->first);
    }
    for(auto it = this->getNews().begin(); it!= this->getNews().end(); it++){
        vertexSet.insert(it->first);
    }
    return vertexSet;
}

const std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getOlds() const {
    return Olds;
}

const std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getDeltas() const {
    return Deltas;
}

const std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getNews() const {
    return News;
}

void ComputationSet::clear() {

}
