#include "peg_compute.h"

//PEGCompute::PEGCompute() = default;

long PEGCompute::startCompute_delete(ComputationSet &compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> &m) {
    long totalAddedEdges = 0;

    while (true) {
        computeOneIteration(compset, grammar);

        postProcessOneIteration(compset, true, &m);

        long realAddedEdgesPerIter = compset.getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if (!realAddedEdgesPerIter)
            break;
    }
    return totalAddedEdges;
}

long PEGCompute::startCompute_add(ComputationSet &compset, Grammar *grammar) {
    long totalAddedEdges = 0;

    while (true) {
        computeOneIteration(compset, grammar);

        postProcessOneIteration(compset, false);

        long realAddedEdgesPerIter = compset.getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if (!realAddedEdgesPerIter)
            break;
    }
    return totalAddedEdges;
}

void PEGCompute::computeOneIteration(ComputationSet &compset, Grammar *grammar) {
    auto vertexSet = compset.getVertices();
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        computeOneVertex(*it, compset, grammar);
    }
}

//void PEGCompute::computeOneIteration(ComputationSet &compset, Grammar *grammar) {
//    for(auto it = compset.getOlds().begin(); it!= compset.getOlds().end(); it++){
//       vertexid_t id = it->first;
//       bool oldEmpty = compset.oldEmpty(id);
//       if(!oldEmpty){
//
//       }
//
//    }
//    for(auto it = compset.getDeltas().begin(); it!= compset.getDeltas().end(); it++){
//    	vertexid_t id = it->first;
//
//
//    }
//}


long PEGCompute::computeOneVertex(vertexid_t index, ComputationSet &compset, Grammar *grammar) {
    bool oldEmpty = compset.oldEmpty(index);
    bool deltaEmpty = compset.deltaEmpty(index);

    //for debugging
    cout << "old is empty? " << oldEmpty << ", delta is empty? " << deltaEmpty << endl;

    // if this vertex has no edges, no need to merge.
    if (oldEmpty && deltaEmpty) return 0;

    // use array
    ContainersToMerge *containers = new myarray::ArraysToMerge();

    // find new edges to containers
    getEdgesToMerge(index, compset, oldEmpty, deltaEmpty, *containers, grammar);

    // merge and sort edges,remove duplicate edges.
    containers->merge();

    long newEdgesNum = containers->getNumEdges();
    cout << "number of new edges: " << newEdgesNum << endl;
    if (newEdgesNum)
        compset.setNews(index, newEdgesNum, containers->getEdgesFirstAddr(), containers->getLabelsFirstAddr());
    else
        compset.clearNews(index);

    containers->clear();
    delete containers;
    return newEdgesNum;
}

void PEGCompute::getEdgesToMerge(vertexid_t index, ComputationSet &compset, bool oldEmpty, bool deltaEmpty,
                                 ContainersToMerge &containers, Grammar *grammar) {
    // add s-rule edges
    if (!deltaEmpty){
        genS_RuleEdges_delta(index, compset, containers, grammar);
        genD_RuleEdges_delta(index, compset, containers, grammar);
    }
    if (!oldEmpty)
        genD_RuleEdges_old(index, compset, containers, grammar);
}

void PEGCompute::genS_RuleEdges_delta(vertexid_t index, ComputationSet &compset, ContainersToMerge &containers, Grammar *grammar) {
    vertexid_t numEdges = compset.getDeltasNumEdges(index); //## can we make sure that the deltas is uniqueness
    vertexid_t *edges = compset.getDeltasEdges(index);
    char *labels = compset.getDeltasLabels(index);

    char newLabel;
    bool added = false;
    for (vertexid_t i = 0; i < numEdges; ++i) {
        newLabel = grammar->checkRules(labels[i]);
        if (newLabel != (char) 127) {
            if (!added) {                            // ##这是个啥意思
                containers.addOneContainer();
                added = true;
            }
            containers.addOneEdge(edges[i], newLabel);
        }
    }
}

void PEGCompute::genD_RuleEdges_old(vertexid_t index, ComputationSet &compset, ContainersToMerge &containers, Grammar *grammar) {
    vertexid_t numEdges_src_old = compset.getOldsNumEdges(index);
    vertexid_t* edges_src_old = compset.getOldsEdges(index);
    char* labels_src_old = compset.getOldsLabels(index);

    for (vertexid_t i_src = 0; i_src < numEdges_src_old; ++i_src) {
    	vertexid_t dstId = edges_src_old[i_src];
    	char dstVal = labels_src_old[i_src];

    	if(compset.getDeltas().find(dstId) == compset.getDeltas().end()){
    		continue;
    	}

        vertexid_t numEdges_delta = compset.getDeltasNumEdges(dstId);
        vertexid_t* edges_delta = compset.getDeltasEdges(dstId);
        char* labels_delta = compset.getDeltasLabels(dstId);

        char newVal;
        bool added = false;
        for (vertexid_t i = 0; i < numEdges_delta; ++i) {
            newVal = grammar->checkRules(dstVal, labels_delta[i]);
            if (newVal != (char) 127) {
                if (!added) {
                    containers.addOneContainer();
                    added = true;
                }
                containers.addOneEdge(edges_delta[i], newVal);
            }
        }
    }
}

void PEGCompute::genD_RuleEdges_delta(vertexid_t index, ComputationSet &compset, ContainersToMerge &containers, Grammar *grammar) {
    vertexid_t numEdges_src_delta = compset.getDeltasNumEdges(index);
    vertexid_t* edges_src_delta = compset.getDeltasEdges(index);
    char* labels_src_delta = compset.getDeltasLabels(index);

    for (vertexid_t i_src = 0; i_src < numEdges_src_delta; ++i_src) {
    	vertexid_t dstId = edges_src_delta[i_src];
    	char dstVal = labels_src_delta[i_src];

		//delta * delta
    	if(compset.getDeltas().find(dstId) != compset.getDeltas().end()){
			vertexid_t numEdges_delta = compset.getDeltasNumEdges(dstId);
			vertexid_t* edges_delta = compset.getDeltasEdges(dstId);
			char* labels_delta = compset.getDeltasLabels(dstId);

			char newVal;
			bool added = false;
			for (vertexid_t i = 0; i < numEdges_delta; ++i) {
				newVal = grammar->checkRules(dstVal, labels_delta[i]);
				if (newVal != (char) 127) {
					if (!added) {
						containers.addOneContainer();
						added = true;
					}
					containers.addOneEdge(edges_delta[i], newVal);
				}
			}
    	}

		//delta * old
    	if(compset.getOlds().find(dstId) != compset.getOlds().end()){
    		vertexid_t numEdges_old = compset.getOldsNumEdges(dstId);
    		vertexid_t* edges_old = compset.getOldsEdges(dstId);
			char* labels_old = compset.getOldsLabels(dstId);
			char newVal;
			bool added = false;
			for (vertexid_t i = 0; i < numEdges_old; ++i) {
				newVal = grammar->checkRules(dstVal, labels_old[i]);
				if (newVal != (char) 127) {
					if (!added) {
						containers.addOneContainer();
						added = true;
					}
					containers.addOneEdge(edges_old[i], newVal);
				}
			}
    	}

    }
}



//void PEGCompute::checkEdges(vertexid_t dstInd, char dstVal, ComputationSet &compset, ContainersToMerge &containers,
//                            bool isOld, Grammar *grammar) {
//    vertexid_t numEdges;
//    vertexid_t *edges;
//    char *labels;
//    numEdges = compset.getDeltasNumEdges(dstInd);
//    edges = compset.getDeltasEdges(dstInd);
//    labels = compset.getDeltasLabels(dstInd);
//
//    char newVal;
//    bool added = false;
//    for (vertexid_t i = 0; i < numEdges; ++i) {
//        newVal = grammar->checkRules(dstVal, labels[i]);
//        if (newVal != (char) 127) {
//            if (!added) {
//                containers.addOneContainer();
//                added = true;
//            }
//            containers.addOneEdge(edges[i], newVal);
//        }
//    }
//
//    if (!isOld) {
//        numEdges = compset.getOldsNumEdges(dstInd);
//        edges = compset.getOldsEdges(dstInd);
//        labels = compset.getOldsLabels(dstInd);
//        added = false;
//        for (vertexid_t i = 0; i < numEdges; ++i) {
//            newVal = grammar->checkRules(dstVal, labels[i]);
//            if (newVal != (char) 127) {
//                if (!added) {
//                    containers.addOneContainer();
//                    added = true;
//                }
//                containers.addOneEdge(edges[i], newVal);
//            }
//        }
//    }
//}

void PEGCompute::postProcessOneIteration(ComputationSet &compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m) {
	// oldsV <- {oldsV,deltasV}
	for (auto it = compset.getOlds().begin(); it != compset.getOlds().end(); it++) {
		vertexid_t id_old = it->first;
		if(compset.getDeltas().find(id_old) != compset.getDeltas().end()){
			int n1 = compset.getOldsNumEdges(id_old);
			int n2 = compset.getDeltasNumEdges(id_old);
			vertexid_t *edges = new vertexid_t[n1 + n2];
			char *labels = new char[n1 + n2];
			int len = myalgo::unionTwoArray(edges, labels, n1,
					compset.getOldsEdges(id_old), compset.getOldsLabels(id_old), n2,
					compset.getDeltasEdges(id_old), compset.getDeltasLabels(id_old));
			compset.setOlds(id_old, len, edges, labels);
			delete[] edges;
			delete[] labels;

			compset.clearDeltas(id_old);
		}
	}
	for (auto it = compset.getDeltas().begin(); it != compset.getDeltas().end(); it++) {
		vertexid_t id_delta = it->first;
		//the left in deltas doesn't exist in olds
		assert(compset.getOlds().find(id_delta) == compset.getOlds().end());
		compset.setOlds(id_delta, compset.getDeltasNumEdges(id_delta), compset.getDeltasEdges(id_delta), compset.getDeltasLabels(id_delta));

		compset.clearDeltas(id_delta);
	}
	assert(compset.getDeltas().empty());

    // deltasV <- newsV - oldsV, newsV <= empty set
    for (auto it = compset.getNews().begin(); it != compset.getNews().end(); it++) {
        vertexid_t i_new = it->first;
        int n1;
        int n2;
        vertexid_t *edges;
        char *labels;
        n1 = compset.getNewsNumEdges(i_new);
        n2 = compset.getOldsNumEdges(i_new);
        edges = new vertexid_t[n1];
        labels = new char[n1];
        int len = myalgo::minusTwoArray(edges, labels,
                                        n1, compset.getNewsEdges(i_new), compset.getNewsLabels(i_new),
                                        n2, compset.getOldsEdges(i_new), compset.getOldsLabels(i_new));

		if (len){
			compset.setDeltas(i_new, len, edges, labels);

            if (isDelete) {
				mergeToDeletedGraph(i_new, m, compset);
            }
        }

		delete[] edges;
		delete[] labels;

		compset.clearNews(i_new);
	}
}

void PEGCompute::mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet& compset) {
	if(m->find(i_new) != m->end()){
		int n1 = m->at(i_new).getSize();
		int n2 = compset.getDeltasNumEdges(i_new);
		vertexid_t* edges = new vertexid_t[n1 + n2];
		char* labels = new char[n1 + n2];
		int len_union = myalgo::unionTwoArray(edges, labels,
				n1, m->at(i_new).getEdges(), m->at(i_new).getLabels(),
				n2, compset.getDeltasEdges(i_new), compset.getDeltasLabels(i_new));
		m->at(i_new).set(len_union, edges, labels);

		delete[] edges;
		delete[] labels;
	}
	else{
		(*m)[i_new] = EdgeArray();
//		m->insert(std::make_pair<vertexid_t, EdgeArray>(i_new, EdgeArray()));
		m->at(i_new).set(compset.getDeltasNumEdges(i_new), compset.getDeltasEdges(i_new), compset.getDeltasLabels(i_new));
	}
}

//void PEGCompute::postProcessOneIteration_add(ComputationSet &compset) {
//    // oldsV <- {oldsV,deltasV}
//    auto vertexSet = compset.getVertices();
//    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
//        int i= *it;
//        bool oldEmpty = compset.oldEmpty(i);
//        bool deltaEmpty = compset.deltaEmpty(i);
//        if (oldEmpty) {
//            if (deltaEmpty)
//                compset.clearOlds(i);
//            else {
//                compset.setOlds(i, compset.getDeltasNumEdges(i), compset.getDeltasEdges(i), compset.getDeltasLabels(i));
//            }
//        } else {
//            if (!deltaEmpty) {
//                int n1 = compset.getOldsNumEdges(i);
//                int n2 = compset.getDeltasNumEdges(i);
//                vertexid_t *edges = new vertexid_t[n1 + n2];
//                char *labels = new char[n1 + n2];
//                int len = myalgo::unionTwoArray(edges, labels, n1, compset.getOldsEdges(i), compset.getOldsLabels(i), n2,
//                                      compset.getDeltasEdges(i), compset.getDeltasLabels(i));
//                compset.setOlds(i, len, edges, labels);
//                delete[] edges;
//                delete[] labels;
//            }
//        }
//        compset.clearDeltas(i);
//    }
//
//    // deltasV <- newsV - oldsV, newsV <= empty set
//    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
//        int i = *it;
//        bool newEmpty = compset.newEmpty(i);
//        if (!newEmpty) {
//            int n1 = compset.getNewsNumEdges(i);
//            int n2 = compset.getOldsNumEdges(i);
//            vertexid_t *edges = new vertexid_t[n1];
//            char *labels = new char[n1];
//            int len = myalgo::minusTwoArray(edges, labels, n1, compset.getNewsEdges(i), compset.getNewsLabels(i), n2,
//                                  compset.getOldsEdges(i), compset.getOldsLabels(i));
//            if (len)
//                compset.setDeltas(i, len, edges, labels);
//            else
//                compset.clearDeltas(i);
//            delete[] edges;
//            delete[] labels;
//        } else
//            compset.clearDeltas(i);
//        compset.clearNews(i);
//    }
//}

//void PEGCompute::postProcessOneIteration_delete(ComputationSet &compset, std::unordered_map<int, EdgesToDelete *> &m) {
//    // oldsV <- {oldsV,deltasV}
//    auto vertexSet = compset.getVertices();
//    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
//        int i= *it;
//        bool oldEmpty = compset.oldEmpty(i);
//        bool deltaEmpty = compset.deltaEmpty(i);
//        if (oldEmpty) {
//            if (deltaEmpty)
//                compset.clearOlds(i);
//            else {
//                compset.setOlds(i, compset.getDeltasNumEdges(i), compset.getDeltasEdges(i), compset.getDeltasLabels(i));
//            }
//        } else {
//            if (!deltaEmpty) {
//                int n1 = compset.getOldsNumEdges(i);
//                int n2 = compset.getDeltasNumEdges(i);
//                vertexid_t *edges = new vertexid_t[n1 + n2];
//                char *labels = new char[n1 + n2];
//                int len = myalgo::unionTwoArray(edges, labels, n1, compset.getOldsEdges(i), compset.getOldsLabels(i), n2,
//                                      compset.getDeltasEdges(i), compset.getDeltasLabels(i));
//                compset.setOlds(i, len, edges, labels);
//                delete[] edges;
//                delete[] labels;
//            }
//        }
//        compset.clearDeltas(i);
//    }
//
//    // deltasV <- newsV - oldsV, newsV <= empty set
//    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
//        int i= *it;
//        bool newEmpty = compset.newEmpty(i);
//        if (!newEmpty) {
//            int n1 = compset.getNewsNumEdges(i);
//            int n2 = compset.getOldsNumEdges(i);
//            vertexid_t *edges = new vertexid_t[n1];
//            char *labels = new char[n1];
//            int len = myalgo::minusTwoArray(edges, labels, n1, compset.getNewsEdges(i), compset.getNewsLabels(i), n2,
//                                  compset.getOldsEdges(i), compset.getOldsLabels(i));
//
////            m[i]->addOneEdge(edges, labels);
//            m[i]->set(len, edges, labels);
//
//            if (len)
//                compset.setDeltas(i, len, edges, labels);
//            else
//                compset.clearDeltas(i);
//            delete[] edges;
//            delete[] labels;
//        } else
//            compset.clearDeltas(i);
//        compset.clearNews(i);
//    }
//}
