#include "computationset.h"
#include <unistd.h>
#include <cstdlib>

//ComputationSet::ComputationSet() {
////    firstVid = numVertices = 0;
//}

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

void ComputationSet::init_add(PEGraph *out, std::unordered_map<vertexid_t, EdgeArray> &m, const bool isConservative) {
	//for debugging
	Logger::print_thread_info_locked("init-add starting...\n", 0);

	//first get a fixed point over all the old edges
	if(isConservative){
		// Deltas <- {m, out}
		for (auto it = out->getGraph().begin(); it != out->getGraph().end(); it++) {
			vertexid_t id_old = it->first;
			if(m.find(id_old) != m.end()){
				int n1 = out->getNumEdges(id_old);
				int n2 = m[id_old].getSize();
				vertexid_t *edges = new vertexid_t[n1 + n2];
				char *labels = new char[n1 + n2];

				int len = myalgo::unionTwoArray(edges, labels, n1,
						out->getEdges(id_old), out->getLabels(id_old), n2,
						m[id_old].getEdges(), m[id_old].getLabels());

				setDeltas(id_old, len, edges, labels);

				delete[] edges;
				delete[] labels;

				m.erase(id_old);
			}
			else{
				setDeltas(it->first, it->second.getSize(), it->second.getEdges(), it->second.getLabels());
			}
		}

		for (auto it = m.begin(); it != m.end(); ) {
			setDeltas(it->first, it->second.getSize(), it->second.getEdges(), it->second.getLabels());

			it = m.erase(it);
		}
	}
	else{
		// Deltas <- m
		for(auto & it : m){
			setDeltas(it.first, it.second.getSize(), it.second.getEdges(), it.second.getLabels());
		}
		m.clear();

		//OldsV <- out
		for(auto it = out->getGraph().begin(); it != out->getGraph().end(); it++){
	//        Olds[it->first] = EdgeArray();
	//        Olds[it->first].set(it->second.getSize(), it->second.getEdges(), it->second.getLabels());
			setOlds(it->first, it->second.getSize(), it->second.getEdges(), it->second.getLabels());
		}
	}

//    //for debugging
//    cout << "-----------------------------------------------------------------------------\n";
//    cout << this->getDeltasEdges(2)[0]<< endl;
//    cout << *this << endl;

	//for debugging
	Logger::print_thread_info_locked("init-add finished.\n", 0);
}

    // Olds <- out - m, Deltas <- m, News <- empty
void ComputationSet::init_delete(PEGraph *out, std::unordered_map<vertexid_t, EdgeArray> &m) {
	// Deltas <- m
    for(auto & it : m){
    	setDeltas(it.first, it.second.getSize(), it.second.getEdges(), it.second.getLabels());
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
//            	Olds[it->first] = EdgeArray();
//                Olds[it->first].set(len, edges, labels);
            	setOlds(it->first, len, edges, labels);
            }

            delete[] edges;
            delete[] labels;
    	}
    	else{
//			Olds[it->first] = EdgeArray();
//			Olds[it->first].set(it->second.getSize(), it->second.getEdges(), it->second.getLabels());
			setOlds(it->first, it->second.getSize(), it->second.getEdges(), it->second.getLabels());
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
//	//for debugging
//	Logger::print_thread_info_locked("set-deltas starting...\n");

    if(Deltas.find(index) == Deltas.end())
        Deltas[index] = EdgeArray();
    Deltas[index].set(numEdges,edges,labels);

//	//for debugging
//	Logger::print_thread_info_locked("set-deltas finished.\n");
}

void ComputationSet::setNews(vertexid_t index, int numEdges, vertexid_t *edges, char *labels) {
    if(News.find(index) == News.end())
        News[index] = EdgeArray();
    News[index].set(numEdges,edges,labels);
}

//void ComputationSet::clearOlds(vertexid_t index) {
//	if(Olds.find(index) != Olds.end())
//		Olds.erase(index);
//}
//
//void ComputationSet::clearDeltas(vertexid_t index) {
//	if(Deltas.find(index) != Deltas.end())
//		Deltas.erase(index);
//}
//
//void ComputationSet::clearNews(vertexid_t index) {
//	if(News.find(index) != News.end())
//		News.erase(index);
//}

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

std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getOlds()  {
    return Olds;
}

std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getDeltas()  {
    return Deltas;
}

std::unordered_map<vertexid_t, EdgeArray> &ComputationSet::getNews()  {
    return News;
}

//void ComputationSet::clear() {
//
//}
