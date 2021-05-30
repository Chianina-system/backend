/*
 * cfg_compute_ooc.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_H_
#define COMP_CFG_COMPUTE_OOC_SYN_H_


#include "cfg_map_outcore.h"
//#include "cfg_map_outcore_cache.h"
//#include "cfg_map_outcore_alias.h"
#include "../preproc/preprocess.h"
//#include "cfg_compute_syn_cache_naive.h"
#include "cfg_compute_syn.h"




using namespace std;

class CFGCompute_ooc_syn {
//
public:

    static bool
    load(Partition part, CFG *cfg_, GraphStore *graphstore, Context *context, bool file_mode, int mining_mode,
         int support, int length) {
        //for debugging
        Logger::print_thread_info_locked("load starting...\n", LEVEL_LOG_FUNCTION);

        string partition = to_string(part);
        const string filename_cfg = Context::file_cfg + partition;
        const string filename_mirrors_in = Context::folder_mirrors_call + partition;
        const string filename_mirrors_out = Context::folder_mirrors_shallow + partition;
        const string foldername_actives = Context::folder_actives + partition;
        const string filename_stmt = Context::file_stmts + partition;
        const string filename_singleton = context->getFileSingletons();
        const string filename_graphs = Context::file_graphstore + partition;
        const string foldername_graphs_in = Context::folder_graphs_in + partition;

        CFG_map_outcore *cfg = dynamic_cast<CFG_map_outcore *>(cfg_);
        cfg->loadCFG_ooc(file_mode, filename_cfg, filename_stmt, filename_mirrors_in, filename_mirrors_out,
                         foldername_actives);

        graphstore->loadGraphStore(filename_graphs, foldername_graphs_in, part, mining_mode, support, length);

        //for debugging
        Logger::print_thread_info_locked("load finished.\n", LEVEL_LOG_FUNCTION);

        return true;
    }

//
//

    static void do_worklist_ooc_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
                 Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer);
    

    
    static void compute_ooc(CFG_map_outcore* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			     Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
                 Timer_wrapper_inmemory* timer);

    
    static void propagate(CFGNode *cfg_node, CFG_map_outcore *cfg, PEGraph *out, Grammar *grammar, 
                 Concurrent_Worklist<CFGNode*> *worklist_2, Concurrent_Worklist<CFGNode*> *actives);

    static void pass(Partition partition, CFG *cfg, GraphStore *graphstore, Concurrent_Worklist<CFGNode *> *actives, Context *context, bool file_mode); 


    //append the activated nodes into the corresponding file
    static void store_actives(bool file_mode, const string& file_actives, std::unordered_set<CFGNode*>& set);

    //append the updated in_mirrors into the corresponding file
    static void store_in_graphs(const string &file_graphs_in, CFG *cfg, GraphStore *graphstore,
                                std::unordered_set<CFGNode *> &set) {
        //get all the predecessors
        std::unordered_set<CFGNode *> s;
        for (auto &it: set) {
            CFGNode *node_dst = it;
            auto nodes = cfg->getPredesessors(node_dst);
            if (nodes) {
                for (auto &n: *nodes) {
                    s.insert(n);
                }
            }
        }

		//write the corresponding peg into file
		graphstore->store_in_graphs(file_graphs_in, s);

    }

};


#endif /* COMP_CFG_COMPUTE_OOC_SYN_H_ */


