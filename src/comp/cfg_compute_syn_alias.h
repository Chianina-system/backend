/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_SYN_ALIAS_H_
#define COMP_CFG_COMPUTE_SYN_ALIAS_H_


//#include "../utility/timer_diff.hpp"
//#include "../utility/timer_sum_sync.hpp"
//#include "../utility/Timer_wrapper.h"
//#include "stmt/stmt_callfptr.h"

#include "cfg_compute_syn.h"

#include "cfg_map_alias.h"
#include "graphstore/itemset_graphstore_alias.h"
#include "graphstore/delta_graphstore_alias.h"
#include "graphstore/naive_graphstore_alias.h"


using namespace std;

class CFGCompute_syn_alias {

public:

  

    static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode); //worklist algorithm in parallel

    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag,
    		Timer_wrapper_inmemory* timer = nullptr){
        switch(stmt->getType()){
            case TYPE::Assign:
                return transfer_copy(in, (AssignStmt*)stmt, grammar, singletons, flag, timer);
            case TYPE::Load:
                return transfer_load(in, (LoadStmt*)stmt, grammar, singletons, flag, timer);
            case TYPE::Store:
                return transfer_store(in, (StoreStmt*)stmt, grammar, singletons, flag, timer);
            case TYPE::Alloca:
                return transfer_address(in, (AllocStmt*)stmt, grammar, singletons, flag, timer);
            case TYPE::Phi:
            	return transfer_phi(in, (PhiStmt*)stmt, grammar, singletons, flag, timer);
            case TYPE::Call:
            	return transfer_call(in);
            case TYPE::Return:
            	return transfer_return(in);
            case TYPE::Ret:
            	return transfer_ret(in);
            case TYPE::Skip:
            	return transfer_skip(in);
            case TYPE::Callfptr:
            	return transfer_callfptr(in);
            case TYPE::Calleefptr:
                return transfer_calleefptr(in);
            default:
                return nullptr;
        }
    }




        
    static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>* preds, CFGNode* cfg_node, Grammar *grammar);


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
		//propagate
		if (cfg_node->getStmt()->getType() == TYPE::Callfptr) {
			//to deal with function pointer callsite
			std::vector<CFGNode*> *successors = cfg->getSuccessors(cfg_node);
			if (successors) {
				for (auto it = successors->cbegin(); it != successors->cend(); ++it) {
					CFGNode *suc = *it;
					if(suc->getStmt() && suc->getStmt()->getType() == TYPE::Calleefptr){
						if (CFGCompute_syn_alias::isFeasible(suc->getStmt(), cfg_node->getStmt(), out, grammar)) {
							worklist_2->push_atomic(*it);
						}
					}
					else{
						worklist_2->push_atomic(*it);
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

					worklist_2->push_atomic(*it);
				}
			}
		}
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


