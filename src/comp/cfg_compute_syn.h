/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_SYN_H_
#define COMP_CFG_COMPUTE_SYN_H_


#include "../preproc/grammar.h"
#include "../preproc/singletons.h"
#include "cfg_map.h"
#include "cfg_map_outcore.h"
#include "computationset.h"
#include "../utility/StaticPrinter.h"
#include "../utility/Logger.hpp"
#include "concurrent_worklist/concurrent_workqueue.h"
#include "concurrent_worklist/concurrent_workset.h"
#include "graphstore/graphstore.h"
#include "peg_compute.h"
#include "peg_compute_parallel.h"
#include "../utility/timer_sum.hpp"



using namespace std;

class CFGCompute_syn {

public:

    static bool load(const string& file_total, const string& file_cfg, const string& file_stmt, const string& file_entries, CFG *cfg_, 
                     const string& file_singleton, Singletons* singletons, GraphStore *graphstore, const string& file_grammar, Grammar * grammar){
        CFG_map* cfg = dynamic_cast<CFG_map*>(cfg_);
    	cfg->loadCFG(file_cfg, file_stmt, file_entries);

    	//graphstore->init(cfg_);
    	//graphstore->loadGraphStore(file_singleton);

        /* TODO: load grammar from file */
        grammar->loadGrammar(file_grammar.c_str());

    	singletons->loadSingletonSet(file_singleton);

    	return true;
    }
    
    static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode);
    
    //static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds, CFGNode* cfg_node, Grammar* grammar);
    
    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer = nullptr);
    
    static void propagate(CFGNode *cfg_node, CFG *cfg, PEGraph *out, Grammar *grammar, Concurrent_Worklist<CFGNode*> *worklist_2);


//protected: 

    static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
    		Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, bool flag,
			Timer_wrapper_inmemory* timer);
};


#endif /* COMP_CFG_COMPUTE_SYN_H_ */

