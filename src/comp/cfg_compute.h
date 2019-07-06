/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_H_
#define COMP_CFG_COMPUTE_H_

#include "graphstore.h"
#include "cfg_map.h"
#include "naive_graphstore.h"
#include "concurrent_workset.h"
#include "concurrent_workqueue.h"
#include "grammar.h"
#include "computationset.h"
#include "singletons.h"
#include "../utility/StaticPrinter.h"
#include "../utility/Logger.hpp"
#include "peg_compute.h"
#include "peg_compute_parallel.h"

using namespace std;

class CFGCompute {

public:
    static bool load(Partition* part, CFG* cfg, GraphStore* graphstore);

    static bool load(const string& file_cfg, const string& file_stmt, CFG *cfg, const string& file_singleton, Singletons* singletons, GraphStore *graphstore, const string& file_grammar, Grammar * grammar);

    static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons); //worklist algorithm in parallel

    static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons){
        switch(stmt->getType()){
            case TYPE::Assign:
                return transfer_copy(in, stmt, grammar, singletons);
            case TYPE::Load:
                return transfer_load(in, stmt, grammar, singletons);
            case TYPE::Store:
                return transfer_store(in, stmt, grammar, singletons);
            case TYPE::Alloca:
                return transfer_address(in, stmt, grammar, singletons);
            default:
                return nullptr;
        }
    }


private:
    static void compute_synchronous(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons);

//    static void update_GraphStore(GraphStore* graphstore, GraphStore* tmp_graphstore);

    static PEGraph* combine_synchronous(GraphStore* graphstore, std::vector<CFGNode*>& preds);


    static PEGraph* transfer_copy(PEGraph* in, Stmt* stmt,Grammar* grammar, Singletons* singletons);

    static PEGraph* transfer_load(PEGraph* in, Stmt* stmt,Grammar* grammar, Singletons* singletons);

    static PEGraph* transfer_store(PEGraph* in, Stmt* stmt,Grammar* grammar, Singletons* singletons);

    static PEGraph* transfer_address(PEGraph* in, Stmt* stmt,Grammar* grammar, Singletons* singletons);

    static bool is_strong_update(vertexid_t x,PEGraph *out,Grammar *grammar, Singletons* singletons);

    static void strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static void must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, Singletons* singletons);

    static bool isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar);

    static void peg_compute_add(PEGraph *out,Stmt* stmt,Grammar* grammar);

    static void peg_compute_delete(PEGraph *out,Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>* m);
	static void removeExistingEdges(const EdgeArray& edges_src, vertexid_t src, PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>* m);
};


#endif /* COMP_CFG_COMPUTE_H_ */
