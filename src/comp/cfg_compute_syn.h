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
#include "graphstore/delta_graphstore.h"
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

        /* TODO: load grammar from file */
        grammar->loadGrammar(file_grammar.c_str());

    	singletons->loadSingletonSet(file_singleton);

    	return true;
    }


    static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode); //worklist algorithm in parallel

    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag,
    		Timer_wrapper_inmemory* timer = nullptr){
                return nullptr;

    }

    static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds, CFGNode* cfg_node, Grammar* grammar);

	static bool isFeasible(Stmt* callee, Stmt* caller, PEGraph* out, Grammar* grammar){
		vertexid_t caller_variable = ((CallfptrStmt*)caller)->getDst();
		vertexid_t caller_deref_variable = ((CallfptrStmt*)caller)->getAux();

		vertexid_t callee_variable = ((CalleefptrStmt*)callee)->getDst();

		if(out->getGraph().find(callee_variable) != out->getGraph().end()){
			int num = out->getNumEdges(callee_variable);
			vertexid_t* edges = out->getEdges(callee_variable);
			label_t* labels = out->getLabels(callee_variable);
			for(int i = 0; i < num; i++){
				if(edges[i] == caller_variable || edges[i] == caller_deref_variable){
					if(grammar->isMemoryAlias(labels[i]) || grammar->isValueAlias(labels[i])){
						return true;
					}
				}
			}
		}

		return false;
	}

	static void propagate(CFGNode *cfg_node, CFG *cfg, PEGraph *out, Grammar *grammar, Concurrent_Worklist<CFGNode*> *worklist_2) {
        
	}

private:
    static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2,
    		Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, bool flag,
			Timer_wrapper_inmemory* timer);

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

    static PEGraph* transfer_callfptr(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_calleefptr(PEGraph* in){
    	return in;
    }

    static PEGraph* transfer_phi(PEGraph* in, PhiStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

    static PEGraph* transfer_copy(PEGraph* in, AssignStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

    static PEGraph* transfer_load(PEGraph* in, LoadStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

    static PEGraph* transfer_store(PEGraph* in, StoreStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

    static PEGraph* transfer_address(PEGraph* in, AllocStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

    static bool is_strong_update_dst(vertexid_t x,PEGraph *out,Grammar *grammar, Singletons* singletons);

    static void strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);
    static void strong_update_simplify(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias_store_dst(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static bool is_strong_update_aux(vertexid_t aux,PEGraph *out,Grammar *grammar, Singletons* singletons);

    static void strong_update_store_aux(vertexid_t aux, vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);
    static void strong_update_store_aux_simplify(vertexid_t aux, vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void strong_update_store_dst(vertexid_t x, PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);
    static void strong_update_store_dst_simplify(vertexid_t x, PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias_store_aux(vertexid_t aux, vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static bool isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar);

    static void peg_compute_add(PEGraph *out,Stmt* stmt,Grammar* grammar, bool flag, Timer_wrapper_inmemory* timer);

    static void peg_compute_delete(PEGraph *out,Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag);
	static void getDirectAddedEdges_phi(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m, bool flag);
//	static void removeExistingEdges(const EdgeArray& edges_src, vertexid_t src, PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>* m);
};


#endif /* COMP_CFG_COMPUTE_SYN_H_ */
