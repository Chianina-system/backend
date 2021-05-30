/*
 * cfg_compute_ooc.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_ALIAS_H_
#define COMP_CFG_COMPUTE_OOC_SYN_ALIAS_H_


#include "cfg_map_outcore_alias.h"
#include "../preproc/preprocess.h"
#include "cfg_compute_syn_alias_itemset.h"
#include "cfg_compute_syn_alias_naive.h"
#include "cfg_compute_syn_alias_delta.h"
#include "cfg_compute_ooc_syn.h"

using namespace std;

class CFGCompute_ooc_syn_alias {

public:


	static void pass(Partition partition, CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* actives, Context* context, bool file_mode){
		//for debugging
		Logger::print_thread_info_locked("pass starting...\n", 1);

		//store all the graphs into file
		const string filename_graphs = Context::file_graphstore + to_string(partition);
//		NaiveGraphStore* graphstore_naive = dynamic_cast<NaiveGraphStore*>(graphstore);
		graphstore->serialize(filename_graphs);

		//divide all the activated nodes into multiple partitions
		std::unordered_map<Partition, std::unordered_set<CFGNode*>> map;
	    CFGNode* cfg_node;
		while(actives->pop(cfg_node)){
			Partition partition = context->getPartition(cfg_node);
			if(map.find(partition) != map.end()){
				map[partition].insert(cfg_node);
			}
			else{
				map[partition] = std::unordered_set<CFGNode*>();
				map[partition].insert(cfg_node);
			}
		}

		//get the corresponding partition
		for(auto it = map.begin(); it != map.end(); ++it){
			Partition part = it->first;

			//write actives
			const string file_actives = Context::folder_actives + to_string(part);
			store_actives(file_mode, file_actives, it->second);

			//write graphs_in
			const string file_graphs_in = Context::folder_graphs_in + std::to_string(part);
			CFGCompute_ooc_syn::store_in_graphs(file_graphs_in, cfg, graphstore, it->second);

//			if(!FileUtil::file_exists(folder_in)){
//				if(mkdir(folder_in.c_str(), 0777) == -1){
//			        cout << "can't create folder: " << folder_in << endl;
//			        exit (EXIT_FAILURE);
//				}
//			}
//			const string file_graphs_in = folder_in + "/" + std::to_string(partition);
//			store_graphs_in(file_graphs_in, cfg, graphstore, it->second);

			//update the priority set information
			context->update_priority(part, it->second.size());
		}

		//for debugging
		Logger::print_thread_info_locked("pass finished.\n", 1);
	}


	static void do_worklist_ooc_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
			Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
		Logger::print_thread_info_locked("-------------------------------------------------------------- Start ---------------------------------------------------------------\n\n\n", LEVEL_LOG_MAIN);

	    Concurrent_Worklist<CFGNode*>* worklist_1 = new Concurrent_Workset<CFGNode*>();

	    //initiate concurrent worklist
	    CFG_map_outcore* cfg = dynamic_cast<CFG_map_outcore*>(cfg_);
	    std::unordered_set<CFGNode*> nodes = cfg->getActiveNodes();
//	    std::vector<CFGNode*> nodes = cfg->getNodes();

	//    //for debugging
	//    StaticPrinter::print_vector(nodes);

	    for(auto it = nodes.cbegin(); it != nodes.cend(); ++it){
	        worklist_1->push_atomic(*it);
	    }

	    //initiate a temp graphstore to maintain all the updated graphs
	    NaiveGraphStore_alias* tmp_graphstore = new NaiveGraphStore_alias();

	    Concurrent_Worklist<CFGNode*>* worklist_2 = new Concurrent_Workset<CFGNode*>();
	    while(!worklist_1->isEmpty()){
	        //for debugging
	        Logger::print_thread_info_locked("--------------------------------------------------------------- superstep starting ---------------------------------------------------------------\n\n", LEVEL_LOG_MAIN);

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->start();

	        std::vector<std::thread> comp_threads;
	        for (unsigned int i = 0; i < NUM_THREADS; i++)
	            comp_threads.push_back(std::thread( [=] {compute_ooc(cfg, graphstore, worklist_1, worklist_2, grammar, tmp_graphstore, singletons, actives, flag, timer);}));

	        for (auto &t : comp_threads)
	            t.join();

	        //for tuning
	        timer_ooc->getEdgeComputeSum()->end();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->start();

	        //synchronize and communicate
	        dynamic_cast<GraphStore_alias*>(tmp_graphstore)->update_graphs(tmp_graphstore, update_mode);
	        tmp_graphstore->clear();

	        //for tuning
	        timer_ooc->getEdgeUpdateSum()->end();

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
//	    Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	    dynamic_cast<NaiveGraphStore*>(graphstore)->printOutInfo();
	}


	static void compute_ooc(CFG_map_outcore* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
			Timer_wrapper_inmemory* timer){
		//for performance tuning
		Timer_diff diff_merge;
		Timer_diff diff_transfer;
		Timer_diff diff_propagate;

	    CFGNode* cfg_node;
		while(worklist_1->pop_atomic(cfg_node)){
//	//    	//for debugging
//	    	Logger::print_thread_info_locked("----------------------- CFG Node "
//	    			+ to_string(cfg_node->getCfgNodeId())
//					+ " " + cfg_node->getStmt()->toString()
//					+ " start processing -----------------------\n", LEVEL_LOG_CFGNODE);

			//for tuning
			diff_merge.start();

	        //merge
	    	std::vector<CFGNode*>* preds = cfg->getPredesessors(cfg_node);
	//        //for debugging
	//    	StaticPrinter::print_vector(preds);
	        PEGraph* in = CFGCompute_syn_alias::combine_synchronous(graphstore, preds, cfg_node, grammar);

	        //for tuning
	        diff_merge.end();
	        timer->getMergeSum()->add_locked(diff_merge.getClockDiff(), diff_merge.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The in-PEG after combination:" + in->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_transfer.start();

	        //transfer
	        PEGraph* out = CFGCompute_syn_alias::transfer(in, cfg_node->getStmt(), grammar, singletons, flag, timer);

	        //for tuning
	        diff_transfer.end();
	        timer->getTransferSum()->add_locked(diff_transfer.getClockDiff(), diff_transfer.getTimeDiff());

//	        //for debugging
//	        Logger::print_thread_info_locked("The out-PEG after transformation:\n" + out->toString(grammar) + "\n", LEVEL_LOG_PEG);


	        //for tuning
	        diff_propagate.start();

	        //update and propagate
	        PEGraph_Pointer out_pointer = cfg_node->getOutPointer();
	        PEGraph* old_out = dynamic_cast<GraphStore_alias*>(graphstore)->retrieve(out_pointer);
	        bool isEqual = out->equals(old_out);

//	        //for debugging
//	        Logger::print_thread_info_locked("+++++++++++++++++++++++++ equality: " + to_string(isEqual) + " +++++++++++++++++++++++++\n", LEVEL_LOG_INFO);

	        if(!isEqual){
	            //propagate
				propagate(cfg_node, cfg, out, grammar, worklist_2, actives);

	            //store the new graph into tmp_graphstore
//	            tmp_graphstore->addOneGraph_atomic(out_pointer, out);
	            dynamic_cast<NaiveGraphStore_alias*>(tmp_graphstore)->addOneGraph_atomic(out_pointer, out);
	        }
	        else{
				delete out;
	        }

	        //clean out
	        if(old_out){
	        	delete old_out;
	        }

	        //for tuning
	        diff_propagate.end();
	        timer->getPropagateSum()->add_locked(diff_propagate.getClockDiff(), diff_propagate.getTimeDiff());

//	        //for debugging
//	//        Logger::print_thread_info_locked(graphstore->toString() + "\n", LEVEL_LOG_GRAPHSTORE);
//	        Logger::print_thread_info_locked("CFG Node " + to_string(cfg_node->getCfgNodeId()) + " finished processing.\n", LEVEL_LOG_CFGNODE);
//
//	        //for debugging
//	        Logger::print_thread_info_locked("1-> " + worklist_1->toString() + "\t2-> " + worklist_2->toString() + "\n\n\n", LEVEL_LOG_WORKLIST);
	    }
	}

	static void propagate(CFGNode *cfg_node, CFG_map_outcore *cfg, PEGraph *out,
			Grammar *grammar, Concurrent_Worklist<CFGNode*> *worklist_2,
			Concurrent_Worklist<CFGNode*> *actives) {
		//propagate
		if (cfg_node->getStmt()->getType() == TYPE::Callfptr) {
			//to deal with function pointer callsite
			std::vector<CFGNode*> *successors = cfg->getSuccessors(cfg_node);
			if (successors) {
				for (auto it = successors->cbegin(); it != successors->cend(); ++it) {
					CFGNode *suc = *it;
					if(suc->getStmt() && suc->getStmt()->getType() == TYPE::Calleefptr){
						if (CFGCompute_syn_alias::isFeasible(suc->getStmt(), cfg_node->getStmt(), out, grammar)) {
							if (!cfg->isMirror(*it)) {
								worklist_2->push_atomic(*it);
							}
							else{
								actives->push_atomic(*it);
							}
						}
					}
					else{
						if (!cfg->isMirror(*it)) {
							worklist_2->push_atomic(*it);
						}
						else{
							actives->push_atomic(*it);
						}
					}

				}
			}
		}
		else {
			//propagate
			std::vector<CFGNode*> *successors = cfg->getSuccessors(cfg_node);
			if (successors) {
				for (auto it = successors->cbegin(); it != successors->cend(); ++it) {
					CFGNode* suc = *it;
					if(suc->getStmt() && suc->getStmt()->getType() == TYPE::Return && ((ReturnStmt*)(suc->getStmt()))->getLength() == 0 &&
							(cfg_node->getStmt()->getType() == TYPE::Callfptr || cfg_node->getStmt()->getType() == TYPE::Call)){
						continue;
					}

					if (!cfg->isMirror(*it)) {
						worklist_2->push_atomic(*it);
					}
					else {
						actives->push_atomic(*it);
					}
				}
			}
		}
	}


protected:

	//append the activated nodes into the corresponding file
	static void store_actives(bool file_mode, const string& file_actives, std::unordered_set<CFGNode*>& set){
		if(file_mode){
			ofstream myfile;
			myfile.open(file_actives, std::ofstream::out | std::ofstream::app);
			if (myfile.is_open()) {
				for (auto &it : set) {
					myfile << it->getCfgNodeId() << "\n";
				}
				myfile.close();
			}
		}
		else{
			FILE *f_actives = fopen(file_actives.c_str(), "ab");
			if (f_actives == NULL) {
				cout << "can't write to actives_file: " << file_actives << endl;
				exit(-1);
			}
			else {
				char* buf = (char*) malloc(IO_SIZE);
				long real_io_size = get_real_io_size(IO_SIZE, sizeof(int));
				long offset = 0;
				for (auto &it : set) {
					int id = it->getCfgNodeId();
					memcpy(buf + offset, (char*)& id, sizeof(int));
					offset += sizeof(int);

			    	if(offset == real_io_size){
						fwrite(buf, real_io_size, 1, f_actives);
						offset = 0;
			    	}
				}
				//write the remaining part to file
				fwrite(buf, offset, 1, f_actives);

				free(buf);
				fclose(f_actives);
			}
		}
	}

};




#endif /* COMP_CFG_COMPUTE_OOC_SYN_ALIAS_H_ */


