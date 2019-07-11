/*
 * cfg_compute_ooc.cpp
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#include "cfg_compute_ooc.h"


bool CFGCompute_ooc::load(Partition partition, CFG *cfg_, Singletons* singletons, GraphStore *graphstore) {
	const string filename_cfg = "cfg_" + partition;
	const string filename_mirrors_in = "mirrors_in_" + partition;
	const string filename_mirrors_out = "mirrors_out" + partition;
	const string foldername_actives = "actives_" + partition;
	const string filename_stmt = "stmt_" + partition;
	const string filename_singleton = "singleton_" + partition;
	const string filename_graphs = "graphstore_" + partition;
	const string foldername_graphs_in = "graphstore_in_" + partition;

	CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
	cfg->loadCFG_ooc(filename_cfg, filename_stmt, filename_mirrors_in,
			filename_mirrors_out, foldername_actives);
	cout << *cfg;

	graphstore->loadGraphStore(filename_graphs, foldername_graphs_in);
	cout << *graphstore << endl;

	singletons->loadSingletonSet(filename_singleton);
	cout << *singletons << endl;

	return true;
}

//append the activated nodes into the corresponding file
void store_actives(const string& file_actives, std::unordered_set<CFGNode*>& set){
	if(readable){
		ofstream myfile;
		myfile.open(file_actives, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			for(auto& it: set){
				myfile << it->getCfgNodeId() << "\t";
			}
			myfile.close();
		}
	}
	else{

	}
}

void write_readable(ofstream& myfile, PEGraph_Pointer pointer, PEGraph* graph) {
	myfile << pointer << "\t";
	graph->write_readable(myfile);
}

//append the updated in_mirrors into the corresponding file
void store_graphs_in(const string& file_graphs_in, CFG* cfg, GraphStore* graphstore, std::unordered_set<CFGNode*>& set){
	if(readable){
		ofstream myfile;
		myfile.open(file_graphs_in, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			for(auto& it: set){
				CFGNode* node_dst = it;
				auto nodes = cfg->getPredesessors(node_dst);
				for(auto& n: nodes){
					auto pointer = n->getOutPointer();
					PEGraph* graph = graphstore->retrieve(pointer);
					//write a pegraph into file
					write_readable(myfile, pointer, graph);
				}
			}

			myfile.close();
		}
	}
	else{

	}
}

void CFGCompute_ooc::pass(Partition partition, CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* actives){
	//store all the graphs into file
	const string filename_graphs = "graphstore_" + partition;
	NaiveGraphStore* graphstore_naive = dynamic_cast<NaiveGraphStore*>(graphstore);
	graphstore_naive->deserialize(filename_graphs);

	//divide all the activated nodes into multiple partitions
	std::unordered_map<Partition, std::unordered_set<CFGNode*>> map;
    CFGNode* cfg_node;
	while(actives->pop(cfg_node)){
		Partition partition = Preprocess::getPartition(cfg_node);
		if(map.find(partition) != map.end()){
			map[partition].insert(cfg_node);
		}
		else{
			map[partition] = std::unordered_set<CFGNode*>();
		}
	}

	//get the corresponding partition
	for(auto it = map.begin(); it != map.end(); ++it){
		Partition part = it->first;
		const string& file_actives = "actives_" + part;
		store_actives(file_actives, it->second);
		const string& file_graphs_in = "graphstore_in_" + std::to_string(part) + "/" + std::to_string(partition);
		store_graphs_in(file_graphs_in, cfg, graphstore, it->second);
	}

}


void CFGCompute_ooc::do_worklist_ooc(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives){
	Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

    //initiate concurrent worklist
    CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
    std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();

//    //for debugging
//    StaticPrinter::print_vector(nodes);

    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
        worklist_1->push_atomic(*it);
    }

    //initiate a temp graphstore to maintain all the updated graphs
    GraphStore* tmp_graphstore = new NaiveGraphStore();

    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
    while(!worklist_1->isEmpty()){
        //for debugging
        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

        std::vector<std::thread> comp_threads;
        for (unsigned int i = 0; i < NUM_THREADS_CFGCOMPUTE; i++)
            comp_threads.push_back(std::thread( [=] {compute_ooc(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphstore, singletons, actives);}));

        for (auto &t : comp_threads)
            t.join();

        //synchronize and communicate
        graphstore->update_graphs(tmp_graphstore);
        tmp_graphstore->clear();

        //update worklists
        assert(worklist_1->isEmpty());
        Concurrent_Worklist<CFGNode*>* worklist_tmp = worklist_1;
        worklist_1 = worklist_2;
        worklist_2 = worklist_tmp;
        assert(worklist_2->isEmpty());

        //for debugging
        Logger::print_thread_info_locked("--------------------------------------------------------------- finished ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);
    }

    //clean
    delete(worklist_1);
    delete(worklist_2);

    delete(tmp_graphstore);

    Logger::print_thread_info_locked("-------------------------------------------------------------- Done ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);
    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
}


void CFGCompute_ooc::compute_ooc(CFG_map_outcore* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives){
    CFGNode* cfg_node;
	while(worklist_1->pop_atomic(cfg_node)){
//    	//for debugging
//    	cout << "\nCFG Node under processing: " << *cfg_node << endl;
    	Logger::print_thread_info_locked("----------------------- CFG Node "
    			+ to_string(cfg_node->getCfgNodeId())
				+ " {" + cfg_node->getStmt()->toString()
				+ "} start processing -----------------------\n", LEVEL_LOG_CFGNODE);
    	Logger::print_thread_info_locked(to_string((long)graphstore) + "\n", LEVEL_LOG_CFGNODE);

        //merge
    	std::vector<CFGNode*> preds = cfg->getPredesessors(cfg_node);
//        //for debugging
//    	StaticPrinter::print_vector(preds);
        PEGraph* in = CFGCompute::combine_synchronous(graphstore, preds);

        //for debugging
        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString() + "\n", LEVEL_LOG_PEG);

        //transfer
        PEGraph* out = CFGCompute::transfer(in, cfg_node->getStmt(), grammar, singletons);

        //for debugging
        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString() + "\n", LEVEL_LOG_PEG);

        //update and propagate
        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
        PEGraph* old_out = graphstore->retrieve(out_pointer);
        bool isEqual = out->equals(old_out);
        //for debugging
        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);
        if(!isEqual){
            //propagate
            std::vector<CFGNode*> successors = cfg->getSuccessors(cfg_node);
            for(auto it = successors.cbegin(); it != successors.cend(); ++it){
                if(!cfg->isMirror(*it)){
					worklist_2->push_atomic(*it);
                }
//                else if(cfg->isInMirror(*it)){
//                	worklist_2->push_atomic(*it);
//                }
                else{
                	actives->push_atomic(*it);
                }
            }

            //store the new graph into tmp_graphstore
            tmp_graphstore->addOneGraph(out_pointer, out);
        }
        else{
			delete out;
        }

        //clean out
        delete old_out;

        //for debugging
//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);

        //for debugging
        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
    }
}




