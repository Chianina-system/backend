#include "peg_compute_parallel.h"

long PEGCompute_parallel::startCompute_delete(ComputationSet *compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> *m) {
    long totalAddedEdges = 0;

    auto vertexSet = compset->getVertices();
    initiateComputationSet(vertexSet, compset, m);

    while (true) {
        computeOneIteration(compset, grammar);

        postProcessOneIteration(compset, true, m);

        long realAddedEdgesPerIter = compset->getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if (!realAddedEdgesPerIter)
            break;
    }
    return totalAddedEdges;
}

void PEGCompute_parallel::initiateComputationSet(std::unordered_set<vertexid_t>& vertexSet, ComputationSet *compset, std::unordered_map<vertexid_t, EdgeArray> *m){
	for(vertexid_t index: vertexSet){
		//for olds
		if(compset->oldEmpty(index)){
			compset->getOlds()[index] = EdgeArray();
		}

		if(compset->deltaEmpty(index)){
			compset->getDeltas()[index] = EdgeArray();
		}

		if(compset->newEmpty(index)){
			compset->getNews()[index] = EdgeArray();
		}

		if(m){
			(*m)[index] = EdgeArray();
		}
	}
}

long PEGCompute_parallel::startCompute_add(ComputationSet *compset, Grammar *grammar) {
	//for debugging
	Logger::print_thread_info_locked("start-compute_add starting...\n", LEVEL_LOG_FUNCTION);

    long totalAddedEdges = 0;

//    //for debugging
//    cout << "-----------------------------------------------------------------------------\n";
//    cout << compset->getDeltasEdges(2)[0]<< endl;
//    cout << *compset << endl;

    auto vertexSet = compset->getVertices();
    initiateComputationSet(vertexSet, compset);

    while (true) {
        computeOneIteration(compset, grammar);

//        //for debugging
//        cout << *compset << endl;
//        cout << "-----------------------------------------------------------------------------\n";

        postProcessOneIteration(compset, false);

//        //for debugging
//        cout << *compset << endl;
//        cout << "-----------------------------------------------------------------------------\n";

        long realAddedEdgesPerIter = compset->getDeltasTotalNumEdges();
        totalAddedEdges += realAddedEdgesPerIter;
        if (!realAddedEdgesPerIter)
            break;
    }

	//for debugging
	Logger::print_thread_info_locked("start-compute_add finished.\n", LEVEL_LOG_FUNCTION);

    return totalAddedEdges;
}

void PEGCompute_parallel::initiateTasks(ComputationSet* compset, Concurrent_Worklist<vertexid_t>* worklist) {
	//initiate worklist
	for (auto it = compset->getOlds().begin(); it != compset->getOlds().end(); it++) {
		worklist->push(it->first);
	}
	for (auto it = compset->getDeltas().begin(); it != compset->getDeltas().end(); it++) {
		worklist->push(it->first);
	}
	for (auto it = compset->getNews().begin(); it != compset->getNews().end(); it++) {
		worklist->push(it->first);
	}
}

void PEGCompute_parallel::computeOneIteration(ComputationSet *compset, Grammar *grammar) {
	//for debugging
	Logger::print_thread_info_locked("compute-one-iteration starting...\n", LEVEL_LOG_FUNCTION);

	//initiate worklist
	Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	initiateTasks(compset, worklist);

	std::vector<std::thread> comp_threads;
	for (unsigned int i = 0; i < NUM_THREADS - NUM_THREADS_CFGCOMPUTE; i++)
		comp_threads.push_back(std::thread([=] {computeOneVertex(compset, grammar, worklist);}));

	for (auto &t : comp_threads)
		t.join();


	delete(worklist);

	//for debugging
	Logger::print_thread_info_locked("compute-one-iteration finished.\n", LEVEL_LOG_FUNCTION);
}


void PEGCompute_parallel::postProcessOneIteration(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m) {
	//for debugging
	Logger::print_thread_info_locked("postprocess-one-iteration starting...\n", LEVEL_LOG_FUNCTION);

	//initiate worklist
	Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	initiateTasks(compset, worklist);

	std::vector<std::thread> comp_threads;
	for (unsigned int i = 0; i < NUM_THREADS - NUM_THREADS_CFGCOMPUTE; i++)
		comp_threads.push_back(std::thread([=] {postProcessOneVertex(compset, isDelete, m, worklist);}));

	for (auto &t : comp_threads)
		t.join();

    delete(worklist);

	//for debugging
	Logger::print_thread_info_locked("postprocess-one-iteration finished.\n", LEVEL_LOG_FUNCTION);
}

void PEGCompute_parallel::postProcessOneVertex(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m, Concurrent_Worklist<vertexid_t>* worklist){
	vertexid_t index = -1;
	while(worklist->pop_atomic(index)){
		postProcessOneVertex(index, compset, isDelete, m);
	}
}

void PEGCompute_parallel::computeOneVertex(ComputationSet *compset, Grammar *grammar, Concurrent_Worklist<vertexid_t>* worklist) {
	vertexid_t index = -1;
	while(worklist->pop_atomic(index)){
		PEGCompute::computeOneVertex(index, compset, grammar);
	}
}


// oldsV <- {oldsV + deltasV}, deltasV <- {newsV - oldsV}, newsV <- emptyset
void PEGCompute_parallel::postProcessOneVertex(vertexid_t id, ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m){
	{
		//oldsV <- {oldsV + deltasV}
		vertexid_t id_old = id;
		int n1 = compset->getOldsNumEdges(id_old);
		int n2 = compset->getDeltasNumEdges(id_old);
		vertexid_t *edges = new vertexid_t[n1 + n2];
		char *labels = new char[n1 + n2];
		int len = myalgo::unionTwoArray(edges, labels, n1,
				compset->getOldsEdges(id_old), compset->getOldsLabels(id_old), n2,
				compset->getDeltasEdges(id_old), compset->getDeltasLabels(id_old));
		compset->setOlds(id_old, len, edges, labels);
		delete[] edges;
		delete[] labels;
	}

	//erase deltas
	compset->getDeltas()[id].clear();

	{
		//deltasV <- {newsV - oldsV}, newsV <- emptyset
		vertexid_t i_new = id;
		int n1 = compset->getNewsNumEdges(i_new);
		int n2 = compset->getOldsNumEdges(i_new);
		vertexid_t *edges = new vertexid_t[n1];
		char * labels = new char[n1];
		int len = myalgo::minusTwoArray(edges, labels, n1,
				compset->getNewsEdges(i_new), compset->getNewsLabels(i_new), n2,
				compset->getOldsEdges(i_new), compset->getOldsLabels(i_new));
		if (len) {
			compset->setDeltas(i_new, len, edges, labels);

			if (isDelete) {
				mergeToDeletedGraph(i_new, m, compset);
			}
		}

		delete[] edges;
		delete[] labels;
	}

	//erase news
	compset->getNews()[id].clear();
}


void PEGCompute_parallel::mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet* compset) {
	if(!(m->at(i_new).isEmpty())){
		int n1 = m->at(i_new).getSize();
		int n2 = compset->getDeltasNumEdges(i_new);
		vertexid_t* edges = new vertexid_t[n1 + n2];
		char* labels = new char[n1 + n2];
		int len_union = myalgo::unionTwoArray(edges, labels,
				n1, m->at(i_new).getEdges(), m->at(i_new).getLabels(),
				n2, compset->getDeltasEdges(i_new), compset->getDeltasLabels(i_new));
		m->at(i_new).set(len_union, edges, labels);

		delete[] edges;
		delete[] labels;
	}
	else{
		m->at(i_new).set(compset->getDeltasNumEdges(i_new), compset->getDeltasEdges(i_new), compset->getDeltasLabels(i_new));
	}
}

//// oldsV <- {oldsV + deltasV}, deltasV <- {newsV - oldsV}, newsV <- emptyset
//void PEGCompute_parallel::postProcessOneVertex(vertexid_t id, ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m){
//	bool isOldEmpty = compset->oldEmpty(id);
//	bool isDeltaEmpty = compset->deltaEmpty(id);
//	bool isNewEmpty = compset->newEmpty(id);
//
//	//oldsV <- {oldsV + deltasV}
//	if(!isDeltaEmpty){
//		if(isOldEmpty){
//			vertexid_t id_delta = id;
//			assert(compset->getOlds().find(id_delta) == compset->getOlds().end());
//			compset->setOlds(id_delta, compset->getDeltasNumEdges(id_delta), compset->getDeltasEdges(id_delta), compset->getDeltasLabels(id_delta));
//		}
//		else {
//			vertexid_t id_old = id;
//			int n1 = compset->getOldsNumEdges(id_old);
//			int n2 = compset->getDeltasNumEdges(id_old);
//			vertexid_t *edges = new vertexid_t[n1 + n2];
//			char *labels = new char[n1 + n2];
//			int len = myalgo::unionTwoArray(edges, labels, n1,
//					compset->getOldsEdges(id_old), compset->getOldsLabels(id_old), n2,
//					compset->getDeltasEdges(id_old), compset->getDeltasLabels(id_old));
//			compset->setOlds(id_old, len, edges, labels);
//			delete[] edges;
//			delete[] labels;
//		}
//
//		//erase deltas
////		compset->getDeltas().erase(id);
//		compset->getDeltas()[id].clear();
//	}
//
//	//deltasV <- {newsV - oldsV}, newsV <- emptyset
//	if(!isNewEmpty){
//		vertexid_t i_new = id;
//        int n1 = compset->getNewsNumEdges(i_new);
//        int n2 = compset->getOldsNumEdges(i_new);
//        vertexid_t *edges = new vertexid_t[n1];
//        char * labels = new char[n1];
//        int len = myalgo::minusTwoArray(edges, labels,
//                                        n1, compset->getNewsEdges(i_new), compset->getNewsLabels(i_new),
//                                        n2, compset->getOldsEdges(i_new), compset->getOldsLabels(i_new));
//		if (len){
//			compset->setDeltas(i_new, len, edges, labels);
//
//            if (isDelete) {
//				mergeToDeletedGraph(i_new, m, compset);
//            }
//        }
//
//		delete[] edges;
//		delete[] labels;
//
//		//erase news
////		compset->getNews().erase(id);
//		compset->getNews()[id].clear();
//	}
//}

