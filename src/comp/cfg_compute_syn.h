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
#include "graphstore/naive_graphstore.h"
#include "graphstore/itemset_graphstore.h"
#include "peg_compute.h"
#include "peg_compute_parallel.h"
#include "../utility/timer_sum.hpp"
#include "../utility/timer_diff.hpp"
#include "../utility/timer_sum_sync.hpp"

using namespace std;

class CFGCompute_syn {

public:

    static bool load(const string& file_total, const string& file_cfg, const string& file_stmt, const string& file_entries, CFG *cfg_,
    		const string& file_singleton, Singletons* singletons, GraphStore *graphstore, const string& file_grammar, Grammar * grammar){
    	CFG_map* cfg = dynamic_cast<CFG_map*>(cfg_);
    	cfg->loadCFG(file_cfg, file_stmt, file_entries);

//    	graphstore->init(cfg_);
//    	graphstore->loadGraphStore(file_singleton);

        /* TODO: load grammar from file */
        grammar->loadGrammar(file_grammar.c_str());

    	singletons->loadSingletonSet(file_singleton);

    	return true;
    }


    static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode); //worklist algorithm in parallel

    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag){
        switch(stmt->getType()){
            case TYPE::Assign:
                return transfer_copy(in, (AssignStmt*)stmt, grammar, singletons, flag);
            case TYPE::Load:
                return transfer_load(in, (LoadStmt*)stmt, grammar, singletons, flag);
            case TYPE::Store:
                return transfer_store(in, (StoreStmt*)stmt, grammar, singletons, flag);
            case TYPE::Alloca:
                return transfer_address(in, (AllocStmt*)stmt, grammar, singletons, flag);
            case TYPE::Phi:
            	return transfer_phi(in, (PhiStmt*)stmt, grammar, singletons, flag);
            case TYPE::Call:
            	return transfer_call(in);
            case TYPE::Return:
            	return transfer_return(in);
            case TYPE::Ret:
            	return transfer_ret(in);
            case TYPE::Skip:
            	return transfer_skip(in);
            default:
                return nullptr;
        }
    }

    static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds);

private:
    static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
    		Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, bool flag, Timer_sum_sync* merge_sum, Timer_sum_sync* transfer_sum, Timer_sum_sync* propagate_sum);

    static PEGraph* transfer_call(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_return(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_ret(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_skip(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_phi(PEGraph* in, PhiStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag);

    static PEGraph* transfer_copy(PEGraph* in, AssignStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag);

    static PEGraph* transfer_load(PEGraph* in, LoadStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag);

    static PEGraph* transfer_store(PEGraph* in, StoreStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag);

    static PEGraph* transfer_address(PEGraph* in, AllocStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag);

    static bool is_strong_update_dst(vertexid_t x,PEGraph *out,Grammar *grammar, Singletons* singletons);

    static void strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias_store_dst(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static bool is_strong_update_aux(vertexid_t aux,PEGraph *out,Grammar *grammar, Singletons* singletons);

    static void strong_update_store_aux(vertexid_t aux, vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void strong_update_store_dst(vertexid_t x, PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias_store_aux(vertexid_t aux, vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static bool isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar);

    static void peg_compute_add(PEGraph *out,Stmt* stmt,Grammar* grammar, bool flag);

    static void peg_compute_delete(PEGraph *out,Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag);
	static void getDirectAddedEdges_phi(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag);
//	static void removeExistingEdges(const EdgeArray& edges_src, vertexid_t src, PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>* m);
};


#endif /* COMP_CFG_COMPUTE_SYN_H_ */
